//@version 100

// This vertex-shader mixes in the quad and circle geometry depending on the value of uDelta.
//
// uDelta is used to mix in the Circle and the Quad positions.
// If uDelta is 0.0f, then the circle position is adopted and if it is 1.0f, then the quad position is adopted.

INPUT mediump vec2  aPositionCircle;
INPUT mediump vec2  aPositionQuad;

UNIFORM_BLOCK Vanilla
{
UNIFORM mediump float uDelta;
UNIFORM mediump mat4  uMvpMatrix;
UNIFORM mediump vec3  uSize;
};

void main()
{
  mediump vec4 vertexPosition = vec4(mix(aPositionCircle,aPositionQuad,uDelta), 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  gl_Position = uMvpMatrix * vertexPosition;
}
