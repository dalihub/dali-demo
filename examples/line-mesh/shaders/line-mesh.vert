//@version 100

INPUT mediump vec2 aPosition1;
INPUT mediump vec2 aPosition2;
INPUT lowp vec3    aColor;

UNIFORM_BLOCK Vanilla
{
UNIFORM mediump mat4   uMvpMatrix;
UNIFORM mediump vec3   uSize;
UNIFORM mediump float  uMorphAmount;
};

OUTPUT lowp vec3 vColor;

void main()
{
  mediump vec2 morphPosition  = mix(aPosition1, aPosition2, uMorphAmount);
  mediump vec4 vertexPosition = vec4(morphPosition, 0.0, 1.0);
  vColor                      = aColor;
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  gl_Position    = vertexPosition;
}
