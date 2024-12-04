//@version 100

UNIFORM_BLOCK FragmentBlock
{
UNIFORM  lowp vec4 uColor;
UNIFORM  mediump vec4 uColorArray[1024];
UNIFORM  mediump int uColorIndex;
};

void main()
{
  gl_FragColor = uColorArray[uColorIndex];
}
