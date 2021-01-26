uniform samplerCube uSkyBoxTexture;
varying mediump vec3 vTexCoord;

void main()
{
  mediump vec4 texColor = textureCube( uSkyBoxTexture, vTexCoord, 0.0);
  gl_FragColor = texColor;
}
