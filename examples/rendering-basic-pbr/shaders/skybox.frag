//@version 100

UNIFORM samplerCube uSkyBoxTexture;

INPUT mediump vec3 vTexCoord;

void main()
{
  mediump vec4 texColor = textureCube( uSkyBoxTexture, vTexCoord, 0.0);
  gl_FragColor = texColor;
}
