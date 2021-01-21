precision mediump float;

// <DALI>
uniform mat4 uMvpMatrix;
uniform vec3 uSize;
// </DALI>

uniform float uFlow;
uniform vec4 uUvTransform; // rotation, scale (initial, target))

attribute vec2 aPosition;

varying vec2 vUv;
varying vec2 vUvFlow;

void main()
{
  vec4 position = uMvpMatrix * vec4(aPosition * uSize.xy, 0., 1.);

  gl_Position = position;

  vec2 uv = position.xy / (position.ww * 2.);
  vUv = uv + vec2(.5);

  float alpha = uFlow * .5 + .5;
  vec2 uvRotationScale = mix(uUvTransform.xy, uUvTransform.zw, alpha);
  float c = cos(uvRotationScale.x) * uvRotationScale.y;
  float s = sin(uvRotationScale.x) * uvRotationScale.y;
  vec4 uvMatrix = vec4(c, -s, s, c);
  uv = vec2(dot(uvMatrix.xy, uv), dot(uvMatrix.zw, uv));

  // N.B. +y is down which is well aligned with the inverted y of the off-screen render,
  // however we need to flip the y of the uvs for the flow map.
  vUvFlow = vec2(uv.x + .5, .5 - uv.y);
}
