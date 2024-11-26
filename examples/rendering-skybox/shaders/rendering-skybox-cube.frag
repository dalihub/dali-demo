//@version 100

// Fragment shader for a textured cube

UNIFORM sampler2D uTexture;

INPUT mediump vec2 vTexCoord;

void main()
{
  mediump vec4 texColor = TEXTURE( uTexture, vTexCoord );
  gl_FragColor = texColor;
}
