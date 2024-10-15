//@version 100

INPUT mediump vec2 vTexCoord;

UNIFORM sampler2D    sTexture;

UNIFORM_BLOCK CustomColor
{
UNIFORM lowp vec4    uColor;
UNIFORM lowp vec4    uFadeColor;
};

void main()
{
  gl_FragColor = TEXTURE(sTexture, vTexCoord)* uColor * uFadeColor;
}
