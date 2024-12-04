//@version 100

INPUT mediump vec3  vVertexColor;
INPUT mediump float vHue;

UNIFORM sampler2D   sTexture1;
UNIFORM sampler2D   sTexture2;

UNIFORM_BLOCK FragUniforms
{
UNIFORM lowp vec4     uColor;
UNIFORM lowp vec4     uFadeColor;
};

void main()
{
  mediump vec4 texCol1 = TEXTURE(sTexture1, gl_PointCoord);
  mediump vec4 texCol2 = TEXTURE(sTexture2, gl_PointCoord);
  gl_FragColor         = vec4(vVertexColor, 1.0) * ((texCol1 * vHue) + (texCol2 * (1.0 - vHue)));
}
