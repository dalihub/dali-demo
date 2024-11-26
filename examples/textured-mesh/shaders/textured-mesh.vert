attribute mediump vec2 aPosition;
attribute highp vec2   aTexCoord;
varying mediump vec2   vTexCoord;
uniform mediump mat4   uMvpMatrix;
uniform mediump vec3   uSize;
uniform lowp vec4      uFadeColor;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  vTexCoord      = aTexCoord;
  gl_Position    = vertexPosition;
}
