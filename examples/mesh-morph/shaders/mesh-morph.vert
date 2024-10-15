//@version 100

INPUT mediump vec2 aInitPos;
INPUT mediump vec2 aFinalPos;
INPUT mediump vec3 aColor;

OUTPUT lowp vec4      vColor;

UNIFORM_BLOCK Vanilla
{
UNIFORM mediump mat4   uMvpMatrix;
UNIFORM mediump vec3   uSize;
UNIFORM mediump float  uDelta;
UNIFORM lowp vec4      uColor;
};

void main()
{
  mediump vec4 vertexPosition = vec4(mix(aInitPos, aFinalPos, uDelta), 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  gl_Position    = vertexPosition;
  vColor         = vec4(aColor, 0.) * uColor;
}
