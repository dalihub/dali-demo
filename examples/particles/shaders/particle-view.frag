#version 300 es
// Fragment shader for particles, which simulates depth of field using
// a combination of procedural texturing, alpha testing and alpha blending.

precision lowp float;
uniform float uAlphaTestRefValue;
uniform vec2 uFadeRange; // near, far
in vec2 vUvUnit;
flat in float vDepth;
flat in float vFocalDistance;
flat in float vOpacity;
flat in vec3 vColor;
out vec4 oFragColor;

const float REF_VALUE_THRESHOLD = 1. / 64.;

void main()
{
  // Softened disc pattern from normalized UVs
  float value = 1.f - dot(vUvUnit, vUvUnit);

  // Decrease area of particles 'in-focus'.
  float refValue = (1.f - vFocalDistance) * .5f;
  float threshold = REF_VALUE_THRESHOLD * (1.f + vDepth);
  float alpha = pow(value, vFocalDistance) * smoothstep(refValue - threshold, refValue + threshold, value);
  if (alpha < uAlphaTestRefValue)
  {
    discard;
  }

  // Apply opacity
  alpha *= vOpacity;
  alpha *= alpha;

  // Fade particles out as they get close to the near and far clipping planes
  alpha *= smoothstep(.0f, uFadeRange.x, vDepth) * smoothstep(1.f, uFadeRange.y, vDepth);

  oFragColor = vec4(vColor, alpha);
}
