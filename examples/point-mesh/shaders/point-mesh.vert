attribute mediump vec2 aPosition;
attribute highp float  aHue;
varying mediump vec2   vTexCoord;
uniform mediump mat4   uMvpMatrix;
uniform mediump vec3   uSize;
uniform mediump float  uPointSize;
uniform lowp vec4      uFadeColor;
varying mediump vec3   vVertexColor;
varying mediump float  vHue;

vec3 hsv2rgb(vec3 c)
{
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= (uSize - uPointSize);
  vertexPosition = uMvpMatrix * vertexPosition;
  vVertexColor   = hsv2rgb(vec3(aHue, 0.7, 1.0));
  vHue           = aHue;
  gl_PointSize   = uPointSize;
  gl_Position    = vertexPosition;
}
