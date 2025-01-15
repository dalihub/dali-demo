//@version 100

UNIFORM_BLOCK SharedFragmentBlock
{
  UNIFORM  mediump vec4 uColorArray[1024];
};

UNIFORM_BLOCK FragmentBlock
{
  UNIFORM  lowp vec4 uColor;
  UNIFORM  mediump int uColorIndex;
};

void main()
{
  // Test that the array color is the same as the actor color
  gl_FragColor = vec4(vec3(0.5, 0.5, 0.5) + vec3(uColorArray[uColorIndex].xyz-uColor.xyz)*0.5, 1.0);
}
