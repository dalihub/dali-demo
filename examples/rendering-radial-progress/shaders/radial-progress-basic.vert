// Vertex shader for polygon

attribute mediump vec3  aPosition;
uniform   mediump mat4  uMvpMatrix; // DALi shader builtin
uniform   mediump vec3  uSize;      // DALi shader builtin
uniform   mediump float uProgress;

varying mediump vec2 vTexCoord;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition.x, aPosition.y, 0.0, 1.0);

  float index = aPosition.z;
  if( uProgress < index )
  {
    vertexPosition = vec4(0.0, 0.0, 0.0, 1.0);
  }

  vertexPosition.xyz *= uSize;
  gl_Position = uMvpMatrix * vertexPosition;
}
