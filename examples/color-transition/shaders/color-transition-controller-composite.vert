//@version 100

precision mediump float;

// <DALI>
UNIFORM_BLOCK Vanilla
{
  uniform mat4 uMvpMatrix;
  uniform vec3 uSize;
};
// </DALI>

UNIFORM_BLOCK VertBlock
{
UNIFORM vec4 uUvTransform; // rotation, scale (initial, target))
};

UNIFORM_BLOCK SharedBlock
{
UNIFORM float uFlow;
};

INPUT vec2 aPosition;
OUTPUT vec2 vUv;
OUTPUT vec2 vUvFlow;

void main()
{
  vec4 position = uMvpMatrix * vec4(aPosition * uSize.xy, 0., 1.);

  gl_Position = position;

  vec2 uv = position.xy / (position.ww * 2.);
  vUv = vec2(.5) + aPosition.xy;

  float alpha = uFlow * .5 + .5;
  vec2 uvRotationScale = mix(uUvTransform.xy, uUvTransform.zw, alpha);
  float c = cos(uvRotationScale.x) * uvRotationScale.y;
  float s = sin(uvRotationScale.x) * uvRotationScale.y;
  vec4 uvMatrix = vec4(c, -s, s, c);
  uv = vec2(dot(uvMatrix.xy, uv), dot(uvMatrix.zw, uv));

  vUvFlow = uv + vec2(.5);
}

