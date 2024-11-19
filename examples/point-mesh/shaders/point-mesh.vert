//@version 100

INPUT mediump vec2 aPosition;
INPUT highp float  aHue;
OUTPUT mediump vec2   vTexCoord;
OUTPUT mediump vec3   vVertexColor;
OUTPUT mediump float  vHue;

UNIFORM_BLOCK Vanilla
{
UNIFORM mediump mat4   uMvpMatrix;
UNIFORM mediump vec3   uSize;
};

UNIFORM_BLOCK Custom
{
UNIFORM mediump float  uPointSize;
};

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
