varying mediump vec2 vTexCoord;
uniform lowp vec4    uColor;
uniform sampler2D    sTexture;
uniform lowp vec4    uFadeColor;

void main()
{
  gl_FragColor = texture2D(sTexture, vTexCoord) * uColor * uFadeColor;
}
