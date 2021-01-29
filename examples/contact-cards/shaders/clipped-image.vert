// This vertex-shader mixes in the quad and circle geometry depending on the value of uDelta.
//
// uDelta is used to mix in the Circle and the Quad positions.
// If uDelta is 0.0f, then the circle position is adopted and if it is 1.0f, then the quad position is adopted.

attribute mediump vec2  aPositionCircle;
attribute mediump vec2  aPositionQuad;
uniform   mediump float uDelta;
uniform mediump mat4    uMvpMatrix;
uniform mediump vec3    uSize;

void main()
{
  mediump vec4 vertexPosition = vec4(mix(aPositionCircle,aPositionQuad,uDelta), 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  gl_Position = uMvpMatrix * vertexPosition;
}
