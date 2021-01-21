attribute mediump vec2 aPosition1;
attribute mediump vec2 aPosition2;
attribute lowp vec3    aColor;
uniform mediump mat4   uMvpMatrix;
uniform mediump vec3   uSize;
uniform mediump float  uMorphAmount;

varying lowp vec3 vColor;

void main()
{
  mediump vec2 morphPosition  = mix(aPosition1, aPosition2, uMorphAmount);
  mediump vec4 vertexPosition = vec4(morphPosition, 0.0, 1.0);
  vColor                      = aColor;
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  gl_Position    = vertexPosition;
}
