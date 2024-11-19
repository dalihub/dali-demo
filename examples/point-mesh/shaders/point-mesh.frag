varying mediump vec3  vVertexColor;
varying mediump float vHue;
uniform lowp vec4     uColor;
uniform sampler2D     sTexture1;
uniform sampler2D     sTexture2;
uniform lowp vec4     uFadeColor;

void main()
{
  mediump vec4 texCol1 = texture2D(sTexture1, gl_PointCoord);
  mediump vec4 texCol2 = texture2D(sTexture2, gl_PointCoord);
  gl_FragColor         = vec4(vVertexColor, 1.0) * ((texCol1 * vHue) + (texCol2 * (1.0 - vHue)));
}
