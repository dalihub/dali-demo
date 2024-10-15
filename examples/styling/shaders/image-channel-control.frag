//@version 100

INPUT mediump vec2 vTexCoord;

UNIFORM sampler2D sTexture;

UNIFORM_BLOCK Custom
{
UNIFORM mediump vec4 uColor;
UNIFORM mediump vec3 uChannels;
};

void main()
{
  gl_FragColor = TEXTURE( sTexture, vTexCoord ) * uColor * vec4(uChannels, 1.0) ;
}
