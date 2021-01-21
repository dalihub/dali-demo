precision highp float;
uniform sampler2D sTexture;
varying vec2 vTexCoord;

varying lowp vec4 vColor;

void main()
{
  gl_FragColor = vColor;
  gl_FragColor.a *= texture2D(sTexture, vTexCoord).a;
}
