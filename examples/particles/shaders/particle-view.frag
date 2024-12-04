//@version 100

// Fragment shader for particles, which simulates depth of field using
// a combination of procedural texturing, alpha testing and alpha blending.

precision lowp float;

UNIFORM_BLOCK VanillaFrag
{
UNIFORM float uAlphaTestRefValue;
UNIFORM vec2 uFadeRange; // near, far
};

INPUT vec2 vUvUnit;
INPUT flat float vDepth;
INPUT flat float vFocalDistance;
INPUT flat float vOpacity;
INPUT flat vec3 vColor;

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

  gl_FragColor = vec4(vColor, alpha);
}
