// Shader for an unlit, unfogged, textured mesh.

#version 300 es

precision mediump float;
uniform vec4 uColor;
out vec4 oFragColor;

void main()
{
  oFragColor = uColor;
}
