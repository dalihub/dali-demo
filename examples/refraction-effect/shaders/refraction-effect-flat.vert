// The shader source is used when the MeshActor is not touched

attribute mediump vec3    aPosition;
attribute mediump vec3    aNormal;
attribute highp   vec2    aTexCoord;
uniform   mediump mat4    uMvpMatrix;
varying   mediump vec2    vTexCoord;

void main()
{
  gl_Position = uMvpMatrix * vec4( aPosition.xy, 0.0, 1.0 );
  vTexCoord = aTexCoord.xy;
}
