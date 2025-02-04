//@version 100

precision mediump float;

// DALI uniforms
UNIFORM_BLOCK VertBuffer
{
UNIFORM mat4 uMvpMatrix;
UNIFORM mat3 uNormalMatrix;
UNIFORM vec3 uSize;

UNIFORM vec3 uDepth_InvDepth_Near;
};
#define DEPTH uDepth_InvDepth_Near.x
#define INV_DEPTH uDepth_InvDepth_Near.y
#define NEAR uDepth_InvDepth_Near.z

INPUT vec3 aPosition;
INPUT vec3 aNormal;

OUTPUT vec4 vPosition;
OUTPUT vec3 vNormal;

vec4 Map(vec4 v) // projection space -> texture
{
  return vec4(v.xyz / (2.f * v.w) + vec3(.5f), (v.w - NEAR) * INV_DEPTH);
}

void main()
{
  vec4 position = uMvpMatrix * vec4(aPosition * uSize, 1.f);
  vPosition = Map(position);
  gl_Position = position;

  vNormal = normalize(uNormalMatrix * aNormal);
}
