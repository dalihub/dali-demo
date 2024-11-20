//@version 100

INPUT mediump vec2 aPosition;

UNIFORM_BLOCK Vanilla
{
UNIFORM mediump mat4 uMvpMatrix;
UNIFORM vec3 uSize;
};

void main()
{
  gl_Position = uMvpMatrix * vec4(aPosition*uSize.xy, 0.0, 1.0);
}
