//@version 100

INPUT highp vec2 aPosition;
FLAT OUTPUT mediump vec4 vColor;

UNIFORM_BLOCK Vanilla
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uSize;
  UNIFORM highp int  uFirstInstance;
  UNIFORM highp int  uInstanceIndex;
};

UNIFORM_BLOCK SharedVertexBlock
{
  UNIFORM mediump vec2 uOffsetArray[ARRAY_COUNT];
};

highp float nrand(const vec2 uv)
{
  const highp float a = 12.9898, b = 78.233, c = 43758.5453, pi = 3.141592653589793;
  highp float dt = dot(uv, vec2(a, b)), sn = mod(dt, pi);
  return fract(sin(sn) * c);
}

void main()
{
  vec3 position = vec3(aPosition, 1.0) * uSize;

  highp int index = (uFirstInstance + uInstanceIndex) % ARRAY_COUNT;
  position.xy += uOffsetArray[index];
  vColor.r = nrand(vec2(float(index), 0.0));
  vColor.g = nrand(vec2(float(index), 10.0));
  vColor.b = nrand(vec2(float(index), -10.0));
  vColor.a = 1.0;

  gl_Position = uMvpMatrix * vec4(position, 1);
}
