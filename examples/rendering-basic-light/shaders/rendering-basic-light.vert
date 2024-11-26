attribute mediump vec3 aPosition; // DALi shader builtin
attribute mediump vec3 aNormal; // DALi shader builtin
uniform   mediump mat4 uMvpMatrix; // DALi shader builtin
uniform   mediump vec3 uSize; // DALi shader builtin
uniform   mediump mat4 uModelView; // DALi shader builtin
uniform   mediump mat3 uNormalMatrix; // DALi shader builtin

varying mediump vec3 vNormal;
varying mediump vec3 vFragPos;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 1.0);
  vertexPosition.xyz *= uSize;
  vFragPos = vec3(uModelView * vertexPosition);
  vNormal = uNormalMatrix * aNormal;

  gl_Position = uMvpMatrix * vertexPosition;
}
