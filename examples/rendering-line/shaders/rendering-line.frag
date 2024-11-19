//@version 100

UNIFORM_BLOCK Custom
{
UNIFORM mediump vec4 uColor;
};

void main()
{
  gl_FragColor = uColor;
}
