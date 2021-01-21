#version 300 es

precision mediump float;

// DALI uniforms
uniform mat4 uMvpMatrix;
uniform mat3 uNormalMatrix;
uniform vec3 uSize;

uniform vec3 uDepth_InvDepth_Near;

#define DEPTH uDepth_InvDepth_Near.x
#define INV_DEPTH uDepth_InvDepth_Near.y
#define NEAR uDepth_InvDepth_Near.z

in vec3 aPosition;
in vec3 aNormal;

out vec4 vPosition;
out vec3 vNormal;

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
