attribute mediump vec3 aCoefficient;
uniform mediump mat4 uMvpMatrix;
uniform mediump vec3 uPosition[MAX_POINT_COUNT];
varying lowp vec2 vCoefficient;

void main()
{
  int vertexId = int(aCoefficient.z);
  gl_Position = uMvpMatrix * vec4(uPosition[vertexId], 1.0);

  vCoefficient = aCoefficient.xy;
}
