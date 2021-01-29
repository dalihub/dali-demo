INPUT mediump vec2 vTexCoord;
uniform sampler2D sTexture;
uniform mediump vec4 uColor;
uniform mediump vec3 mixColor;
uniform mediump vec3 uChannels;

void main()
{
  OUT_COLOR = TEXTURE( sTexture, vTexCoord ) * vec4(mixColor,1.0) * uColor * vec4(uChannels, 1.0) ;
}
