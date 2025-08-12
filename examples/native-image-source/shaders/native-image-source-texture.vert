//@version 100

precision highp float;
INPUT mediump vec2 aPosition;
INPUT mediump vec2 aTexCoord;
UNIFORM_BLOCK VertBlock
{
  UNIFORM mediump mat4 uMvpMatrix;
  UNIFORM mediump vec3 uSize;
};
OUTPUT mediump vec2 vTexCoord;

void main()
{
  vec4 position = vec4(aPosition,0.0,1.0)*vec4(uSize,1.0);
  gl_Position = uMvpMatrix * position;
  vTexCoord = aTexCoord;
}
