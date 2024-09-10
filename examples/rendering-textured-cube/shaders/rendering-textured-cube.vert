//@version 100
INPUT mediump vec3 aPosition;  // DALi shader builtin
INPUT mediump vec2 aTexCoord;  // DALi shader builtin
UNIFORM_BLOCK Vert
{
UNIFORM mediump mat4 uMvpMatrix; // DALi shader builtin
UNIFORM mediump vec3 uSize;      // DALi shader builtin
};

OUTPUT mediump vec2 vTexCoord;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 1.0);
  vertexPosition.xyz *= uSize;
  vTexCoord = aTexCoord;
  gl_Position = uMvpMatrix * vertexPosition;
}
