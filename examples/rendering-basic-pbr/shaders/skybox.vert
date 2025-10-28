//@version 100

INPUT mediump vec3 aPosition;  // DALi shader builtin

UNIFORM_BLOCK Vanilla
{
UNIFORM   mediump mat4 uMvpMatrix; // DALi shader builtin
};

OUTPUT mediump vec3 vTexCoord;

void main()
{
  vTexCoord =  aPosition;

  mediump vec4 vertexPosition = vec4(aPosition, 1.0);
  vec4 clipSpacePosition = uMvpMatrix * vertexPosition;
  gl_Position = clipSpacePosition.xyww; // Writes 1.0, the maximum depth value, into the depth buffer.
                                        // This is an optimization to avoid running the fragment shader
                                        // for the pixels hidden by the scene's objects.
}
