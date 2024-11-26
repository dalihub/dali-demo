//@version 100

INPUT mediump vec3 aPosition; // DALi shader builtin
INPUT mediump vec3 aColor; // DALi shader builtin

UNIFORM_BLOCK Vanilla
{
UNIFORM mediump mat4 uMvpMatrix; // DALi shader builtin
UNIFORM mediump vec3 uSize; // DALi shader builtin
};

OUTPUT mediump vec4 vColor;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 1.0);
  vertexPosition.xyz *= uSize;
  vColor = vec4( aColor, 1.0 );
  gl_Position = uMvpMatrix * vertexPosition;
}
