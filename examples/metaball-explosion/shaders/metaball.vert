//@version 100

//Vertex shader code for metaball

INPUT mediump vec2    aPosition;
INPUT mediump vec2    aTexture;

OUTPUT   mediump vec2    vTexCoord;

UNIFORM_BLOCK Vanilla
{
UNIFORM   mediump mat4    uMvpMatrix;
UNIFORM   mediump vec3    uSize;
UNIFORM   lowp    vec4    uColor;
};

void main()
{
  vTexCoord = aTexture;
  mediump vec4 vertexPosition = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
  gl_Position = uMvpMatrix * vertexPosition;
}
