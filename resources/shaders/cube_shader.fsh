//@version 100

INPUT mediump vec3 vTexCoord;
UNIFORM samplerCube uSkyBoxTexture;
OUTPUT mediump vec4 FragColor;

void main()
{
  mediump vec4 texColor = TEXTURE_CUBE(uSkyBoxTexture, vTexCoord);
  FragColor = texColor;
}
