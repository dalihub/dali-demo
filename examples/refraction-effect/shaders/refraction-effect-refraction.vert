//@version 100

INPUT mediump vec3 aPosition;
INPUT mediump vec3 aNormal;
INPUT highp   vec2 aTexCoord;

UNIFORM_BLOCK Vanilla
{
UNIFORM   mediump mat4 uMvpMatrix;
};

OUTPUT   mediump vec4 vVertex;
OUTPUT   mediump vec3 vNormal;
OUTPUT   mediump vec2 vTexCoord;
OUTPUT   mediump vec2 vTextureOffset;

void main()
{
  gl_Position = uMvpMatrix * vec4( aPosition.xy, 0.0, 1.0 );
  vTexCoord = aTexCoord.xy;

  vNormal = aNormal;
  vVertex = vec4( aPosition, 1.0 );
  float length = max(0.01, length(aNormal.xy)) * 40.0;
  vTextureOffset = aNormal.xy / length;
}
