#version 300 es

precision mediump float;

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D sAlbedo;
uniform vec4      uColor;

void main()
{
  vec4 color = texture(sAlbedo, vUV);
  FragColor  = vec4(color.rgb, uColor.a * color.a);
}
