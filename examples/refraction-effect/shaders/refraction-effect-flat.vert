//@version 100

// The shader source is used when the MeshActor is not touched

INPUT mediump vec3    aPosition;
INPUT mediump vec3    aNormal;
INPUT highp   vec2    aTexCoord;
UNIFORM_BLOCK Vanilla
{
UNIFORM   mediump mat4    uMvpMatrix;
};

OUTPUT   mediump vec2    vTexCoord;

void main()
{
  gl_Position = uMvpMatrix * vec4( aPosition.xy, 0.0, 1.0 );
  vTexCoord = aTexCoord.xy;
}
