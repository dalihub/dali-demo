//@version 100

UNIFORM_BLOCK SharedFragmentBlock
{
  UNIFORM  mediump vec4 uColorArray[1024];
};

UNIFORM_BLOCK FragmentBlock
{
  UNIFORM  mediump int uColorIndex;
};

void main()
{
  gl_FragColor = uColorArray[uColorIndex];
}
