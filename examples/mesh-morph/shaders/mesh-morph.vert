attribute mediump vec2 aInitPos;
attribute mediump vec2 aFinalPos;
attribute mediump vec3 aColor;
uniform mediump mat4   uMvpMatrix;
uniform mediump vec3   uSize;
uniform mediump float  uDelta;
uniform lowp vec4      uColor;
varying lowp vec4      vColor;

void main()
{
  mediump vec4 vertexPosition = vec4(mix(aInitPos, aFinalPos, uDelta), 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  gl_Position    = vertexPosition;
  vColor         = vec4(aColor, 0.) * uColor;
}
