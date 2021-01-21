// Vertex shader for textured quad

attribute mediump vec2 aPosition;
uniform   mediump mat4 uMvpMatrix; // DALi shader builtin
uniform   mediump vec3 uSize;      // DALi shader builtin

varying mediump vec2 vTexCoord;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vTexCoord = vec2(1.0, 1.0)*(aPosition + vec2(0.5) );
  gl_Position = uMvpMatrix * vertexPosition;
}
