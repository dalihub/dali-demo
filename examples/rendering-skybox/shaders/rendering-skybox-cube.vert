// Vertex shader for a textured cube

attribute mediump vec3 aPosition; // DALi shader builtin
attribute mediump vec2 aTexCoord; // DALi shader builtin
uniform   mediump mat4 uMvpMatrix; // DALi shader builtin
uniform   mediump vec3 uSize; // DALi shader builtin

varying mediump vec2 vTexCoord;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 1.0);
  vertexPosition.xyz *= uSize;
  vTexCoord = aTexCoord;
  gl_Position = uMvpMatrix * vertexPosition;
}
