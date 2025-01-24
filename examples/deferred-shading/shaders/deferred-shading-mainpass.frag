//@version 100

precision mediump float;

const int kMaxLights = 32;

const float kAttenuationConst = .05f;
const float kAttenuationLinear = .1f;
const float kAttenuationQuadratic = .15f;

// G-buffer
UNIFORM sampler2D uTextureNormal;
UNIFORM sampler2D uTexturePosition;
UNIFORM sampler2D uTextureColor;

UNIFORM_BLOCK FragBuffer
{
  UNIFORM mat4 uInvProjection;
  UNIFORM vec3 uDepth_InvDepth_Near;
};

#define DEPTH uDepth_InvDepth_Near.x
#define INV_DEPTH uDepth_InvDepth_Near.y
#define NEAR uDepth_InvDepth_Near.z

// Light source uniforms
struct Light
{
  vec3 position;    // view space
  float radius;
  vec3 color;
};

UNIFORM_BLOCK LightsBuffer
{
UNIFORM Light uLights[kMaxLights];
};

INPUT vec2 vUv;

vec4 Unmap(vec4 m)  // texture -> projection
{
  m.w = m.w * DEPTH + NEAR;
  m.xyz = (m.xyz - vec3(.5)) * (2.f * m.w);
  return m;
}

vec3 CalculateLighting(vec3 pos, vec3 normal)
{
  vec3 viewDir = normalize(pos);
  vec3 viewDirRefl = -reflect(viewDir, normal);

  vec3 light = vec3(0.04f); // fake ambient term
  for (int i = 0; i < kMaxLights; ++i)
  {
    vec3 rel = pos - uLights[i].position;
    float distance = length(rel);
    rel /= distance;

    float a = uLights[i].radius / (kAttenuationConst + kAttenuationLinear * distance +
      kAttenuationQuadratic * distance * distance);     // attenuation

    float l = max(0.f, dot(normal, rel));   // lambertian
    float s = pow(max(0.f, dot(viewDirRefl, rel)), 256.f);  // specular

    light += (uLights[i].color * (l + s)) * a;
  }

  return light;
}

void main()
{
  vec3 normSample = TEXTURE(uTextureNormal, vUv).xyz;
  if (dot(normSample, normSample) == 0.f)
  {
    discard;  // if we didn't write this texel, don't bother lighting it.
  }

  vec3 normal = normalize(normSample - .5f);

  vec4 posSample = TEXTURE(uTexturePosition, vUv);
  vec3 pos = (uInvProjection * Unmap(posSample)).xyz;

  vec3 color = TEXTURE(uTextureColor, vUv).rgb;
  vec3 finalColor = color * CalculateLighting(pos, normal);

  gl_FragColor = vec4(finalColor, 1.f);
}
