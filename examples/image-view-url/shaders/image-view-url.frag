precision highp float;
INPUT mediump vec2 vTexCoord;
uniform sampler2D sTexture;
uniform mediump float uDelta;

void main()
{
  vec4 color = vec4(0.0);
  vec2 texCoord = vTexCoord * 2. - 1.;
  mat2 rotation = mat2(cos(uDelta), -sin(uDelta), sin(uDelta), cos(uDelta));
  texCoord = (rotation * texCoord) * .5 + .5;
  color += TEXTURE( sTexture, texCoord );
  OUT_COLOR = color;
}
