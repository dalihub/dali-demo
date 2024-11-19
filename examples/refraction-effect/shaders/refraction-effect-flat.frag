//@version 100

UNIFORM_BLOCK VanillaFrag
{
UNIFORM lowp vec4 uColor;
};

UNIFORM sampler2D sTexture;

INPUT mediump vec2 vTexCoord;

void main()
{
  gl_FragColor = TEXTURE( sTexture, vTexCoord ) * uColor;
}
