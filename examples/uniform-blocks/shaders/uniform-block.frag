layout(std140) uniform FragmentBlock
{
  lowp vec4 uColor;
  mediump vec4 uColorArray[1024];
  mediump int uColorIndex;
};

void main()
{
  fragColor = uColorArray[uColorIndex];
}
