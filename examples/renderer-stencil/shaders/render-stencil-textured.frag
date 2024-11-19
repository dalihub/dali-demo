//@version 100

INPUT mediump vec2 vTexCoord;
INPUT mediump vec3 vIllumination;
UNIFORM_BLOCK Color
{
  UNIFORM lowp    vec4 uColor;
};

UNIFORM sampler2D    sTexture;

void main()
{
  gl_FragColor = vec4( TEXTURE( sTexture, vTexCoord ).rgb * vIllumination.rgb * uColor.rgb, uColor.a );
}