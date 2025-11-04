//@version 100


#ifdef HIGHP
  precision highp float;
#else
  precision mediump float;
#endif

UNIFORM sampler2D sAlbedo;
UNIFORM_BLOCK FragBlock
{
UNIFORM vec4 uColor;

#ifdef ALPHA_TEST
  uniform float uAlphaThreshold;
#endif	//ALPHA_TEST
};

INPUT vec2 vUV;
OUTPUT mediump vec4 FragColor;

void main()
{
  vec4 color = TEXTURE(sAlbedo, vUV.st);
#ifdef ALPHA_TEST
  if (color.a <= uAlphaThreshold)
  {
    discard;
  }
#endif
  FragColor = color * uColor;
}
