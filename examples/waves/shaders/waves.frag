//@version 100

precision highp float;

UNIFORM sampler2D uNormalMap; // DALi

UNIFORM_BLOCK CustomFrag
{
UNIFORM vec4 uColor; // DALi

UNIFORM vec3 uInvLightDir;
UNIFORM vec3 uLightColorSqr;
UNIFORM vec3 uAmbientColor;

UNIFORM float uNormalMapWeight;
UNIFORM float uSpecularity;
};

INPUT vec2 vUv;
INPUT vec3 vNormal;
INPUT vec3 vViewPos;
INPUT float vHeight;

float Rand(vec2 co)
{
  return fract(sin(dot(co.xy, vec2(12.98981, 78.2331))) * 43758.5453);
}

float Sum(vec3 v)
{
  return v.x + v.y + v.z;
}

void main()
{
  vec3 viewPos = normalize(vViewPos);
  vec2 uv2 = vUv + vViewPos.xy / vViewPos.z * vHeight + vec2(.5, 0.);

  vec3 perturbNormal = TEXTURE(uNormalMap, vUv).rgb * 2. - 1.;
  vec3 perturbNormal2 = TEXTURE(uNormalMap, uv2).rgb * 2. - 1.;
  vec3 normal = normalize(vNormal + perturbNormal * uNormalMapWeight);
  vec3 normal2 = normalize(vNormal + perturbNormal2 * uNormalMapWeight);

  vec3 color = uAmbientColor;
  float d = max(0., dot(normal, -uInvLightDir));
  color += uColor.rgb * d;

  vec3 reflected = reflect(uInvLightDir, normal);
  d = max(0., dot(reflected, viewPos));
  color += pow(d, uSpecularity) * uLightColorSqr;

  reflected = reflect(uInvLightDir, normal2);
  d = max(0., dot(reflected, viewPos));
  color += pow(d, uSpecularity) * uLightColorSqr;

  gl_FragColor = vec4(color, 1.);
}
