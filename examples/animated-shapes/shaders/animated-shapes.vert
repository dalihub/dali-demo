//@version 100

INPUT mediump vec3 aCoefficient;

UNIFORM_BLOCK Vanilla
{
UNIFORM mediump mat4 uMvpMatrix;
UNIFORM mediump vec3 uPosition[MAX_POINT_COUNT];
};

OUTPUT lowp vec2 vCoefficient;

void main()
{
  int vertexId = int(aCoefficient.z);
  gl_Position = uMvpMatrix * vec4(uPosition[vertexId], 1.0);

  vCoefficient = aCoefficient.xy;
}
