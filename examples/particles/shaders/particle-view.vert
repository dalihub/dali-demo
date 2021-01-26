// Shader for billboarded particles, where the vertices of the particles
// are supplied as vec3 position (particle position) + vec2 sub-position.

#version 300 es

precision lowp float;
uniform mat4 uModelView; // DALi
uniform mat4 uProjection; // DALi
uniform vec3 uSize; // DALi
uniform vec4 uColor; // DALi

uniform vec3 uSecondaryColor;
uniform vec2 uDepthRange; // x is zNear, y is 1.f / (zFar - zNear)
uniform float uTwinkleFrequency;
uniform float uTwinkleSizeScale;
uniform float uTwinkleOpacityWeight;
uniform float uTime;
uniform float uFocalLength;
uniform float uAperture;
uniform float uPopulation;

struct Scatter
{
  float radiusSqr;
  float amount;
  vec3 ray;
};

const int SCATTER_VARS = 6; // Must match ParticleView::mScatterProps' size.
uniform Scatter uScatter[SCATTER_VARS];

const int POPULATION_GRANULARITY = 128;
uniform float uOrderLookUp[POPULATION_GRANULARITY];

in vec3 aPosition;
in float aSeed;
in vec4 aPath;
in vec2 aSubPosition;
in float aSize;

flat out float vDepth;
flat out float vFocalDistance;
out vec2 vUvUnit;
flat out float vOpacity;
flat out vec3 vColor; // ignore alpha

float bezier(vec3 control, float alpha)
{
  return mix(mix(control.x, control.y, alpha), mix(control.y, control.z, alpha), alpha);
}

void main() {
  // Get random order from the look-up table, based on particle ID.
  int particleId = gl_VertexID / 6;
  float order = uOrderLookUp[particleId & (POPULATION_GRANULARITY - 1)];

  // Get twinkle scalar
  float twinkle = sin(uTime * floor(uTwinkleFrequency * aSeed) + fract(aSeed * 1.17137));

  // Add Motion
  float s = sin(uTime + aSeed) * .5f + .5f;	// different phase for all
  // NOTE: you'd think that taking the bezier() calls apart would save 4 mix() calls, since
  // the mix()es (of xy / yz / zw / wx) are all calculated twice. It turns out that the MALI
  // compiler is already doing this; leaving it as is for readability.
  float bx0 = bezier(aPath.xyz, s);
  float bx1 = bezier(aPath.zwx, s);
  float by0 = bezier(aPath.yzw, s);
  float by1 = bezier(aPath.wxy, s);
  vec3 motion = vec3(mix(bx0, bx1, s), mix(by0, by1, s), 0.f);

  // Model to view position
  vec3 position3 = aPosition * uSize + motion;

  vec4 position = uModelView * vec4(position3, 1.f);

  // Add scatter - calculated in view space, using view ray
  vec3 normalizedPos = position.xyz / uSize;
  for (int i = 0; i < SCATTER_VARS; ++i)
  {
    vec2 scatterDist = (normalizedPos - uScatter[i].ray * dot(uScatter[i].ray, normalizedPos)).xy;

    // NOTE: replacing the division with a multiplication (by inverse) oddly results in more instructions (MALI).
    float scatter = max(0.f, uScatter[i].radiusSqr - dot(scatterDist, scatterDist)) *
      uScatter[i].amount / aSize;
    position.xy += scatter * normalize(scatterDist) * uSize.xy;
  }

  // Calculate normalised depth and distance from focal plane
  float depth = (position.z - uDepthRange.x) * uDepthRange.y;
  vDepth = depth;

  float focalDist = (uFocalLength - depth) * uAperture;
  focalDist *= focalDist;
  vFocalDistance = max(focalDist, 1e-6f);	// NOTE: was clamp(..., 1.f); side effect: out of focus particles get squashed at higher aperture values.

  // Calculate expiring scale - for size and opacity.
  float expiringScale = smoothstep(order + 1.f, order, uPopulation);

  // Calculate billboard position and size
  vec2 subPosition = aSubPosition * aSize *
    (1.f + twinkle * aSeed * uTwinkleSizeScale) *
    expiringScale;

  // Insist on hacking the size? Do it here...
  float sizeHack = depth + .5f;
  // NOTE: sizeHack *= sizeHack looked slightly better.
  subPosition *= sizeHack;

  vec3 subPositionView = vec3(subPosition, 0.);

  // Add billboards to view position.
  position += vec4(subPositionView, 0.f);

  // subPosition doubles as normalized (-1..1) UV.
  vUvUnit = aSubPosition;

  // Vary opacity (actor alpha) by time as well as expiring scale.
  vOpacity = uColor.a * expiringScale *
    (1.0f + aSeed + twinkle * uTwinkleOpacityWeight) / (2.0f + uTwinkleOpacityWeight);

  // Randomize RGB using seed.
  vec3 mixColor = vec3(fract(aSeed), fract(aSeed * 16.f), fract(aSeed * 256.f));
  vColor = mix(uColor.rgb, uSecondaryColor, mixColor);

  gl_Position = uProjection * position;
}
