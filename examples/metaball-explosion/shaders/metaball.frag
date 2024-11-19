// Fragment shader code for metaball

precision mediump float;
varying vec2 vTexCoord;
uniform vec2 uPositionMetaball;
uniform vec2 uPositionVar;
uniform vec2 uGravityVector;
uniform float uRadius;
uniform float uRadiusVar;

void main()
{
  vec2 adjustedCoords = vTexCoord * 2.0 - 1.0;
  vec2 finalMetaballPosition = uPositionMetaball + uGravityVector + uPositionVar;

  float finalRadius = uRadius + uRadiusVar;
  vec2 distanceVec = adjustedCoords - finalMetaballPosition;
  float result = dot(distanceVec, distanceVec);
  float color = inversesqrt(result) * finalRadius;

  gl_FragColor = vec4(color,color,color,1.0);
}
