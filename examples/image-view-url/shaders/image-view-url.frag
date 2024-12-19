//@version 100

precision highp float;
INPUT mediump vec2 vTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM_BLOCK FragBlock
{
UNIFORM mediump float uDelta;
};

void main()
{
  vec4 color = vec4(0.0);
  vec2 texCoord = vTexCoord * 2. - 1.;
  mat2 rotation = mat2(cos(uDelta), -sin(uDelta), sin(uDelta), cos(uDelta));
  texCoord = (rotation * texCoord) * .5 + .5;
  color += TEXTURE( sTexture, texCoord );
  gl_FragColor = color;
}
