//@version 100

precision mediump float;

// DALI uniform
UNIFORM_BLOCK FragBuffer
{
  UNIFORM vec4 uColor;
};

INPUT vec4 vPosition;
INPUT vec3 vNormal;

// These are our outputs.
OUTPUT vec3 oNormal;
layout(location=1) out vec4 oPosition;
layout(location=2) out vec3 oColor;

void main()
{
  oColor = uColor.rgb;
  oPosition = vPosition;
  oNormal = normalize(vNormal) * .5f + .5f;
}
