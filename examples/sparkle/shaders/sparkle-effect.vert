precision highp float;

attribute vec2  aTexCoord;
uniform   mat4  uMvpMatrix;
varying   vec2  vTexCoord;

attribute vec2  aParticlePath0;
attribute vec2  aParticlePath1;
attribute vec2  aParticlePath2;
attribute vec2  aParticlePath3;
attribute vec2  aParticlePath4;
attribute vec2  aParticlePath5;

uniform float uPercentage;
uniform float uPercentageMarked;
uniform vec3  uParticleColors[NUM_COLOR];
uniform float uOpacity[NUM_PARTICLE];
uniform vec2  uTapIndices;
uniform float uTapOffset[MAXIMUM_ANIMATION_COUNT];
uniform vec2  uTapPoint[MAXIMUM_ANIMATION_COUNT];
uniform float uAcceleration;
uniform float uRadius;
uniform float uScale;
uniform float uBreak;

varying lowp vec4 vColor;

void main()
{
  // we store the particle index inside texCoord attribute
  float idx = abs(aTexCoord.y)-1.0;

  // early out if the particle is invisible
  if(uOpacity[int(idx)]<1e-5)
  {
    gl_Position = vec4(0.0);
    vColor = vec4(0.0);
    return;
  }

  // As the movement along the b-curve has nonuniform speed with a uniform increasing parameter 'uPercentage'
  // we give different particles the different 'percentage' to make them looks more random
  float increment = idx / float(NUM_PARTICLE)*5.0;
  float percentage = mod(uPercentage +uAcceleration+increment, 1.0);

  vec2 p0; vec2 p1; vec2 p2; vec2 p3;
  // calculate the particle position by using the cubic b-curve equation
  if(percentage<0.5) // particle on the first b-curve
  {
    p0 = aParticlePath0;
    p1 = aParticlePath1;
    p2 = aParticlePath2;
    p3 = aParticlePath3;
  }
  else
  {
    p0 = aParticlePath3;
    p1 = aParticlePath4;
    p2 = aParticlePath5;
    p3 = aParticlePath0;
  }
  float t = mod( percentage*2.0, 1.0);
  vec2 position = (1.0-t)*(1.0-t)*(1.0-t)*p0 + 3.0*(1.0-t)*(1.0-t)*t*p1+3.0*(1.0-t)*t*t*p2 + t*t*t*p3;

  vec2 referencePoint = mix(p0,p3,0.5);
  float maxAnimationCount = float(MAXIMUM_ANIMATION_COUNT);

  for( float i=uTapIndices.x; i<uTapIndices.y; i+=1.0 )
  {
    int id = int( mod(i+0.5,maxAnimationCount ) );
    vec2 edgePoint = normalize(referencePoint-uTapPoint[id])*uRadius+vec2(uRadius);
    position = mix( position, edgePoint, uTapOffset[id] ) ;
  }

  position = mix( position, vec2( 250.0,250.0 ),uBreak*(1.0-uOpacity[int(idx)]) ) ;

  // vertex position on the mesh: (sign(aTexCoord.x), sign(aTexCoord.y))*PARTICLE_HALF_SIZE
  gl_Position = uMvpMatrix * vec4( position.x+sign(aTexCoord.x)*PARTICLE_HALF_SIZE/uScale,
                                   position.y+sign(aTexCoord.y)*PARTICLE_HALF_SIZE/uScale,
                                   0.0, 1.0 );

  // we store the color index inside texCoord attribute
  float colorIndex = abs(aTexCoord.x);
  vColor.rgb = uParticleColors[int(colorIndex)-1];
  vColor.a = fract(colorIndex) * uOpacity[int(idx)];

  // produce a 'seemingly' random fade in/out
  percentage = mod(uPercentage+increment+0.15, 1.0);
  float ramdomOpacity = (min(percentage, 0.25)-0.15+0.9-max(percentage,0.9))*10.0;
  vColor.a *=ramdomOpacity;

  vTexCoord = clamp(aTexCoord, 0.0, 1.0);
}
