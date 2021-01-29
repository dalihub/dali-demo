#version 300 es
precision mediump float;

// DALI uniforms
uniform mat4 uMvpMatrix;
uniform vec3 uSize;

in vec3 aPosition;
in vec2 aTexCoord;

out vec2 vUv;

void main()
{
  vec4 position = uMvpMatrix * vec4(aPosition * uSize, 1.f);
  vUv = aTexCoord;

  gl_Position = position;
}
