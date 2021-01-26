#version 300 es

precision mediump float;

in vec2 aPosition;
in vec2 aTexCoord;

out vec2 vUV;

uniform vec3 uSize;
uniform mat4 uMvpMatrix;

void main()
{
  vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  gl_Position = uMvpMatrix * vertexPosition;

  vUV = aTexCoord;
}
