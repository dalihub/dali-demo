INPUT vec2 aPosition;

layout(std140) uniform VertexBlock
{
  highp mat4 uMvpMatrix;
  highp vec3 uSize;
};

void main()
{
  vec3 position = vec3(aPosition, 1.0) * uSize;
  gl_Position = uMvpMatrix * vec4(position, 1);
}
