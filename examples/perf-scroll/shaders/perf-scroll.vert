attribute mediump vec2 aPosition;
attribute mediump vec2 aTexCoord;
uniform mediump mat4 uMvpMatrix;
uniform mediump vec3 uSize;
varying mediump vec2 vTexCoord;

void main()
{
  vec4 position = vec4(aPosition,0.0,1.0)*vec4(uSize,1.0);
  gl_Position = uMvpMatrix * position;
  vTexCoord = aTexCoord;
}
