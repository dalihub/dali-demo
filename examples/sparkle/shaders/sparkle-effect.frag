//@version 100

precision highp float;
UNIFORM sampler2D sTexture;

INPUT vec2 vTexCoord;
INPUT lowp vec4 vColor;

void main()
{
  gl_FragColor = vColor;
  gl_FragColor.a *= TEXTURE(sTexture, vTexCoord).a;
}
