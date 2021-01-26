attribute mediump vec3 aPosition; // DALi shader builtin
attribute mediump vec3 aColor; // DALi shader builtin
uniform   mediump mat4 uMvpMatrix; // DALi shader builtin
uniform   mediump vec3 uSize; // DALi shader builtin

varying mediump vec4 vColor;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 1.0);
  vertexPosition.xyz *= uSize;
  vColor = vec4( aColor, 1.0 );
  gl_Position = uMvpMatrix * vertexPosition;
}
