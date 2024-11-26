//@version 100

INPUT mediump vec3 aPosition;
INPUT mediump vec3 aNormal;
INPUT mediump vec2 aTexCoord;

UNIFORM_BLOCK Vanilla
{
UNIFORM mediump mat4 uMvpMatrix;
UNIFORM mediump mat3 uNormalMatrix;
UNIFORM mediump vec3 uSize;
};

OUTPUT mediump vec2 vTexCoord;
OUTPUT mediump vec3 vNormal;
OUTPUT mediump vec3 vPosition;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 1.0);
  vertexPosition.xyz *= uSize;
  vTexCoord = aTexCoord;
  vNormal = normalize(uNormalMatrix * aNormal);
  vPosition = aPosition;
  gl_Position = uMvpMatrix * vertexPosition;
}