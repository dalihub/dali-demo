#ifdef HIGHP
  precision highp float;
#else
  precision mediump float;
#endif

uniform sampler2D sAlbedo;

uniform vec4 uColor;

#ifdef ALPHA_TEST
  uniform float uAlphaThreshold;
#endif	//ALPHA_TEST

varying vec2 vUV;

void main()
{
  vec4 color = texture2D(sAlbedo, vUV.st);
#ifdef ALPHA_TEST
  if (color.a <= uAlphaThreshold)
  {
    discard;
  }
#endif
  gl_FragColor = color * uColor;
}
