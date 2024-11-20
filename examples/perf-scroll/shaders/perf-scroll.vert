//@version 100

INPUT mediump vec2 aPosition;
INPUT mediump vec2 aTexCoord;
OUTPUT mediump vec2 vTexCoord;

UNIFORM_BLOCK Vanilla
{
UNIFORM mediump mat4 uMvpMatrix;
UNIFORM mediump vec3 uSize;
};

void main()
{
  vec4 position = vec4(aPosition,0.0,1.0)*vec4(uSize,1.0);
  gl_Position = uMvpMatrix * position;
  vTexCoord = aTexCoord;
}
