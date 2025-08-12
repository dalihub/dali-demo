//@version 100

precision highp float;
UNIFORM sampler2D sTexture;
INPUT highp vec2 vTexCoord;

void main()
{
  gl_FragColor = TEXTURE( sTexture, vTexCoord ) * vec4(1.2, 1.2, 1.2, 1.0);
}
