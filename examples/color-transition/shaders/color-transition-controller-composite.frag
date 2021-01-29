precision mediump float;

const float kStepsilon = 1e-2;

uniform sampler2D sColor;
uniform sampler2D sFlowMap;

uniform float uFlow;
uniform vec3 uRgb[2];

varying vec2 vUv;
varying vec2 vUvFlow;

void main()
{
  vec4 colorAlpha = texture2D(sColor, vUv);
  float flow = smoothstep(.5 - kStepsilon, .5 + kStepsilon, clamp(uFlow + texture2D(sFlowMap, vUvFlow).r, 0., 1.));

  gl_FragColor = vec4(mix(colorAlpha.rgb, mix(uRgb[0], uRgb[1], flow), colorAlpha.a), 1.);
}
