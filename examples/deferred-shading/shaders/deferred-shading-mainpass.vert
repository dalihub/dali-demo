//@version 100

precision mediump float;

// DALI uniforms
UNIFORM_BLOCK VertBuffer
{
UNIFORM mat4 uMvpMatrix;
UNIFORM vec3 uSize;
};

INPUT vec3 aPosition;
INPUT vec2 aTexCoord;

OUTPUT vec2 vUv;

void main()
{
  vec4 position = uMvpMatrix * vec4(aPosition * uSize, 1.f);
  vUv = aTexCoord;

  gl_Position = position;
}
