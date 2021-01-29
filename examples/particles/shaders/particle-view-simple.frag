#version 300 es
// Shader for an unlit, unfogged, textured mesh.

precision mediump float;
uniform vec4 uColor;
out vec4 oFragColor;

void main()
{
  oFragColor = uColor;
}
