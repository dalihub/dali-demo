attribute highp vec3 aPosition;
attribute highp vec3 aNormal;
attribute highp vec2 aTexCoord;
uniform highp mat4 uMvpMatrix;
varying highp vec2 vTexCoord;

void main()
{
  gl_Position = uMvpMatrix * vec4(aPosition, 1.0 );
  vTexCoord = aTexCoord;
  vTexCoord.y = 1.0 - vTexCoord.y;
}
