//@version 100

// Shader for simple textured geometry.

precision mediump float;

UNIFORM_BLOCK VertexBlock
{
  UNIFORM mat4 uMvpMatrix; //by DALi
  UNIFORM vec3 uSize;  // by DALi
};

INPUT vec3 aPosition;

void main()
{
  gl_Position = uMvpMatrix * vec4(aPosition * uSize, 1.0);
}
