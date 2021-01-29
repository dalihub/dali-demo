//Vertex shader code for metaball

attribute mediump vec2    aPosition;
attribute mediump vec2    aTexture;
uniform   mediump mat4    uMvpMatrix;
uniform   mediump vec3    uSize;
uniform   lowp    vec4    uColor;
varying   mediump vec2    vTexCoord;

void main()
{
  vTexCoord = aTexture;
  mediump vec4 vertexPosition = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
  gl_Position = uMvpMatrix * vertexPosition;
}
