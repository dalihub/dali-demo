//@version 100

// Fragment shader code when there's no effect

precision mediump float;

INPUT vec2 vTexCoord;
UNIFORM sampler2D sTexture;

void main()
{
  gl_FragColor = TEXTURE(sTexture, vTexCoord);
}
