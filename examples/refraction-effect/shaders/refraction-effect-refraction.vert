attribute mediump vec3 aPosition;
attribute mediump vec3 aNormal;
attribute highp   vec2 aTexCoord;
uniform   mediump mat4 uMvpMatrix;
varying   mediump vec4 vVertex;
varying   mediump vec3 vNormal;
varying   mediump vec2 vTexCoord;
varying   mediump vec2 vTextureOffset;

void main()
{
  gl_Position = uMvpMatrix * vec4( aPosition.xy, 0.0, 1.0 );
  vTexCoord = aTexCoord.xy;

  vNormal = aNormal;
  vVertex = vec4( aPosition, 1.0 );
  float length = max(0.01, length(aNormal.xy)) * 40.0;
  vTextureOffset = aNormal.xy / length;
}
