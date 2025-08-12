//@version 100

precision highp float;
INPUT highp vec3 aPosition;
INPUT highp vec3 aNormal;
INPUT highp vec2 aTexCoord;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
};

OUTPUT highp vec2 vTexCoord;

void main()
{
  gl_Position = uMvpMatrix * vec4(aPosition, 1.0 );
  vTexCoord = aTexCoord;
  vTexCoord.y = 1.0 - vTexCoord.y;
}
