// Fragment shader for a textured cube

uniform sampler2D uTexture;

varying mediump vec2 vTexCoord;

void main()
{
  mediump vec4 texColor = texture2D( uTexture, vTexCoord );
  gl_FragColor = texColor;
}
