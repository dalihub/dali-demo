//@version 100

precision mediump float;

const float kStepsilon = 1e-2;

UNIFORM sampler2D sColor;
UNIFORM sampler2D sFlowMap;

UNIFORM_BLOCK FragBlock
{
  UNIFORM float uFlow;
  UNIFORM vec3 uRgb[2];
};

INPUT vec2 vUv;
INPUT vec2 vUvFlow;

void main()
{
  vec4 colorAlpha = TEXTURE(sColor, vUv);
  float flow = smoothstep(.5 - kStepsilon, .5 + kStepsilon, clamp(uFlow + texture2D(sFlowMap, vUvFlow).r, 0., 1.));

  gl_FragColor = vec4(mix(colorAlpha.rgb, mix(uRgb[0], uRgb[1], flow), colorAlpha.a), 1.);
}
