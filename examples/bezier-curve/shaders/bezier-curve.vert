attribute mediump vec2 aPosition;
uniform mediump mat4 uMvpMatrix;
uniform vec3 uSize;

void main()
{
  gl_Position = uMvpMatrix * vec4(aPosition*uSize.xy, 0.0, 1.0);
}
