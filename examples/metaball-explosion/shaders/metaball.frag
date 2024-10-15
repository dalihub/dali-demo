//@version 100

// Fragment shader code for metaball

precision mediump float;

INPUT vec2 vTexCoord;

UNIFORM_BLOCK VanillaFrag
{
UNIFORM vec2 uPositionMetaball;
UNIFORM vec2 uPositionVar;
UNIFORM vec2 uGravityVector;
UNIFORM float uRadius;
UNIFORM float uRadiusVar;
};

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
