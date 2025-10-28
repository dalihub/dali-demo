//@version 100

INPUT vec3 aPosition;
INPUT vec2 aTexCoord;
INPUT vec3 aNormal;
INPUT vec3 aTangent;

OUTPUT vec2 vUV;
OUTPUT vec3 vNormal;
OUTPUT vec3 vTangent;
OUTPUT vec3 vBitangent;
OUTPUT vec3 vViewVec;
OUTPUT mat4 uCubeMatrix;

UNIFORM_BLOCK VertBlock
{
UNIFORM vec3 uSize;
UNIFORM mat4 uMvpMatrix;
UNIFORM mat3 uNormalMatrix;
UNIFORM mat4 uModelMatrix;
UNIFORM mat4 uModelView;
UNIFORM mat4 uViewMatrix;
};

void main()
{
  vec4 vPosition = vec4( aPosition * uSize , 1.0);
  gl_Position = uMvpMatrix * vPosition;
  vUV = aTexCoord;

  vNormal = normalize(uNormalMatrix * aNormal);

  vTangent = normalize(uNormalMatrix * aTangent);

  vBitangent = cross(vNormal, vTangent);

  vViewVec = (uModelView * vPosition).xyz;

  uCubeMatrix  = transpose(uModelView);

}
