//@version 100

INPUT mediump vec3 aPosition; // DALi shader builtin
INPUT mediump vec3 aNormal; // DALi shader builtin

UNIFORM_BLOCK Vanilla
{
UNIFORM   mediump mat4 uMvpMatrix; // DALi shader builtin
UNIFORM   mediump vec3 uSize; // DALi shader builtin
UNIFORM   mediump mat4 uModelView; // DALi shader builtin
UNIFORM   mediump mat3 uNormalMatrix; // DALi shader builtin
};

OUTPUT mediump vec3 vNormal;
OUTPUT mediump vec3 vFragPos;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 1.0);
  vertexPosition.xyz *= uSize;
  vFragPos = vec3(uModelView * vertexPosition);
  vNormal = uNormalMatrix * aNormal;

  gl_Position = uMvpMatrix * vertexPosition;
}
