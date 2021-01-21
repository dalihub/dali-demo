attribute mediump vec3 aPosition;
attribute mediump vec3 aNormal;
attribute mediump vec2 aTexCoord;
uniform mediump mat4 uMvpMatrix;
uniform mediump mat3 uNormalMatrix;
uniform mediump vec3 uSize;

varying mediump vec2 vTexCoord;
varying mediump vec3 vNormal;
varying mediump vec3 vPosition;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 1.0);
  vertexPosition.xyz *= uSize;
  vTexCoord = aTexCoord;
  vNormal = normalize(uNormalMatrix * aNormal);
  vPosition = aPosition;
  gl_Position = uMvpMatrix * vertexPosition;
}