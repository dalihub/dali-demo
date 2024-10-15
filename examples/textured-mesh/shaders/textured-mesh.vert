//@version 100

INPUT mediump vec2 aPosition;
INPUT highp vec2   aTexCoord;

OUTPUT mediump vec2   vTexCoord;

UNIFORM_BLOCK Vanilla
{
UNIFORM mediump mat4   uMvpMatrix;
UNIFORM mediump vec3   uSize;
};

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  vTexCoord      = aTexCoord;
  gl_Position    = vertexPosition;
}
