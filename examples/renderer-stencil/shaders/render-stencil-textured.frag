varying mediump vec2 vTexCoord;
varying mediump vec3 vIllumination;
uniform lowp    vec4 uColor;
uniform sampler2D    sTexture;

void main()
{
  gl_FragColor = vec4( texture2D( sTexture, vTexCoord ).rgb * vIllumination.rgb * uColor.rgb, uColor.a );
}
