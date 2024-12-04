//@version 100

// Fragment shader for a skybox

UNIFORM samplerCube uSkyBoxTexture;

INPUT mediump vec3 vTexCoord;

void main()
{
  mediump vec4 texColor = TEXTURE( uSkyBoxTexture, vTexCoord );
  gl_FragColor = texColor;
}
