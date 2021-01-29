// Fragment shader for a skybox

uniform samplerCube uSkyBoxTexture;

varying mediump vec3 vTexCoord;

void main()
{
  mediump vec4 texColor = textureCube( uSkyBoxTexture, vTexCoord );
  gl_FragColor = texColor;
}
