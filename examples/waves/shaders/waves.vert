//@version 100

#define FMA(a, b, c) ((a) * (b) + (c))  // fused multiply-add

precision highp float;

const float kTile = 1.;
const float kPi = 3.1415926535;
const float kEpsilon = 1. / 32.;

// DALI uniforms
UNIFORM_BLOCK Vanilla
{
UNIFORM vec3 uSize;
UNIFORM mat4 uModelView;
UNIFORM mat4 uProjection;
UNIFORM mat3 uNormalMatrix;
};

// our UNIFORMs
UNIFORM_BLOCK CustomVert
{
UNIFORM float uTime;
UNIFORM vec2  uScrollScale;
UNIFORM float uWaveRate;
UNIFORM float uWaveAmplitude;
UNIFORM float uParallaxAmount;
};

INPUT vec2 aPosition;
INPUT vec2 aTexCoord;

OUTPUT vec2 vUv;
OUTPUT vec3 vViewPos;
OUTPUT vec3 vNormal;
OUTPUT float vHeight;

float CubicHermite(float B, float C, float t)
{
  float dCB = (C - B) * .5;
  float A = B - dCB;
  float D = B + dCB;
  vec3 p = vec3(D + .5 * (((B - C) * 3.) - A), A - 2.5 * B + 2. * C - D,
    .5 * (C - A));
  return FMA(FMA(FMA(p.x, t, p.y), t, p.z), t, B);
}

float Hash(float n)
{
  return fract(sin(n) * 43751.5453123);
}

float HeightAtTile(vec2 pos)
{
  float rate = Hash(Hash(pos.x) * Hash(pos.y));

  return (sin(uTime * rate * uWaveRate) * .5 + .5) * uWaveAmplitude;
}

float CalculateHeight(vec2 position)
{
  vec2 tile = floor(position);
  position = fract(position);

  vec2 cp = vec2(
    CubicHermite(
      HeightAtTile(tile + vec2( kTile * -0.5, kTile * -0.5)),
      HeightAtTile(tile + vec2( kTile * +0.5, kTile * -0.5)),
      position.x),
    CubicHermite(
      HeightAtTile(tile + vec2( kTile * -0.5, kTile * +0.5)),
      HeightAtTile(tile + vec2( kTile * +0.5, kTile * +0.5)),
      position.x)
  );

  return CubicHermite(cp.x, cp.y, position.y);
}

vec3 CalculateNormal(vec2 position)
{
  vec3 normal = vec3(
    CalculateHeight(vec2(position.x - kEpsilon, position.y)) -
      CalculateHeight(vec2(position.x + kEpsilon, position.y)),
    .25,
    CalculateHeight(vec2(position.x, position.y - kEpsilon)) -
      CalculateHeight(vec2(position.x, position.y + kEpsilon))
  );
  return normal;
}

void main()
{
  vUv = aTexCoord;

  vec2 scrollPosition = aPosition * uScrollScale + vec2(0., uTime * -kPi);
  vNormal = uNormalMatrix * CalculateNormal(scrollPosition);

  float h = CalculateHeight(scrollPosition);
  vHeight = h * uParallaxAmount;
  vec3 position = vec3(aPosition.x, h, aPosition.y);

  vec4 viewPosition = uModelView * vec4(position * uSize, 1.);
  vViewPos = -viewPosition.xyz;

  gl_Position = uProjection * viewPosition;
}
