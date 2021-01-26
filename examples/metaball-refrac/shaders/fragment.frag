// Fragment shader code when there's no effect

precision mediump float;
varying vec2 vTexCoord;
uniform sampler2D sTexture;

void main()
{
  gl_FragColor = texture2D(sTexture, vTexCoord);
}
