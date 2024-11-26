//@version 100

INPUT mediump vec2 vTexCoord;
INPUT mediump vec3 vIllumination;

UNIFORM_BLOCK color
{
  UNIFORM lowp vec4 uColor;
};

UNIFORM sampler2D    sTexture;

void main()
{
  gl_FragColor = vec4( vIllumination.rgb * uColor.rgb, uColor.a );
}
