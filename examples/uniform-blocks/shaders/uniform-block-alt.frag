layout(std140) uniform FragmentBlock
{
  lowp vec4 uColor;
  mediump vec4 uColorArray[1024];
  mediump int uColorIndex;
};

void main()
{
  // Test that the array color is the same as the actor color
  fragColor = vec4(vec3(0.5, 0.5, 0.5) + vec3(uColorArray[uColorIndex].xyz-uColor.xyz)*0.5, 1.0);
}
