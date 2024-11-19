#define FMA(a, b, c) ((a) * (b) + (c))  // fused multiply-add

precision highp float;

const float kTile = 1.;

const float kPi = 3.1415926535;
const float kEpsilon = 1. / 32.;

// DALI uniforms
uniform vec3 uSize;
uniform mat4 uModelView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

// our uniforms
uniform float uTime;
uniform vec2 uScrollScale;
uniform float uWaveRate;
uniform float uWaveAmplitude;
uniform float uParallaxAmount;

attribute vec2 aPosition;
attribute vec2 aTexCoord;

varying vec2 vUv;
varying vec3 vViewPos;
varying vec3 vNormal;
varying float vHeight;

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
