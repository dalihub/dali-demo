#version 300 es
precision mediump float;

// DALI uniform
uniform vec4 uColor;

in vec4 vPosition;
in vec3 vNormal;

// These are our outputs.
layout(location = 0) out vec3 oNormal;
layout(location = 1) out vec4 oPosition;
layout(location = 2) out vec3 oColor;

void main()
{
  oColor = uColor.rgb;
  oPosition = vPosition;
  oNormal = normalize(vNormal) * .5f + .5f;
}
