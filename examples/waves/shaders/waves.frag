precision highp float;

uniform vec4 uColor; // DALi
uniform sampler2D uNormalMap; // DALi

uniform vec3 uInvLightDir;
uniform vec3 uLightColorSqr;
uniform vec3 uAmbientColor;

uniform float uNormalMapWeight;
uniform float uSpecularity;

varying vec2 vUv;
varying vec3 vNormal;
varying vec3 vViewPos;
varying float vHeight;

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

  vec3 perturbNormal = texture2D(uNormalMap, vUv).rgb * 2. - 1.;
  vec3 perturbNormal2 = texture2D(uNormalMap, uv2).rgb * 2. - 1.;
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
