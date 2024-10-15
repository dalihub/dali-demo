//@version 100
// Vertex shader for textured quad

INPUT mediump vec2 aPosition;

UNIFORM_BLOCK Vanilla
{
UNIFORM   mediump mat4 uMvpMatrix; // DALi shader builtin
UNIFORM   mediump vec3 uSize;      // DALi shader builtin
};

OUTPUT mediump vec2 vTexCoord;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vTexCoord = vec2(1.0, 1.0)*(aPosition + vec2(0.5) );
  gl_Position = uMvpMatrix * vertexPosition;
}
