//@version 100

UNIFORM sampler2D sTexture;

UNIFORM_BLOCK VanillaFrag
{
UNIFORM lowp vec4 uColor;
};

INPUT lowp vec3 vColor;

void main()
{
  gl_FragColor = uColor * vec4(vColor, 1.0);
}
