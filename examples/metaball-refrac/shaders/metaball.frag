//@version 100

// Fragment shader for metaballs

precision mediump float;
INPUT vec2 vTexCoord;

UNIFORM_BLOCK VanillaFrag
{
UNIFORM vec2 uPositionMetaball;
UNIFORM vec2 uPositionVar;
UNIFORM vec2 uGravityVector;
UNIFORM float uRadius;
UNIFORM float uRadiusVar;
UNIFORM float uAspect;
};

void main()
{
  vec2 adjustedCoords = vTexCoord * 2.0 - 1.0;
  vec2 finalMetaballPosition = uPositionMetaball + uGravityVector + uPositionVar;

  float distance = (adjustedCoords.x - finalMetaballPosition.x) * (adjustedCoords.x - finalMetaballPosition.x) +
                   (adjustedCoords.y - finalMetaballPosition.y) * (adjustedCoords.y - finalMetaballPosition.y);
  float finalRadius = uRadius + uRadiusVar;
  float color = finalRadius / sqrt( distance );
  vec2 bordercolor = vec2(0.0,0.0);
  if (vTexCoord.x < 0.1)
  {
    bordercolor.x = (0.1 - vTexCoord.x) * 0.8;
  }
  if (vTexCoord.x > 0.9)
  {
    bordercolor.x = (vTexCoord.x - 0.9) * 0.8;
  }
  if (vTexCoord.y < 0.1)
  {
    bordercolor.y = (0.1 - vTexCoord.y) * 0.8;
  }
  if (vTexCoord.y > (0.9 * uAspect))
  {
    bordercolor.y = (vTexCoord.y - (0.9 * uAspect)) * 0.8;
  }
  float border = (bordercolor.x + bordercolor.y) * 0.5;
  gl_FragColor = vec4(color + border,color + border,color + border,1.0);
}
