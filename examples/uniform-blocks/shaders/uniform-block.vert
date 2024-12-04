//@version 100

INPUT vec2 aPosition;

UNIFORM_BLOCK Vanilla
{
UNIFORM  highp mat4 uMvpMatrix;
UNIFORM  highp vec3 uSize;
};

void main()
{
  vec3 position = vec3(aPosition, 1.0) * uSize;
  gl_Position = uMvpMatrix * vec4(position, 1);
}
