uniform lowp vec4 uColor;
uniform sampler2D sTexture;

varying lowp vec3 vColor;

void main()
{
  gl_FragColor = uColor * vec4(vColor, 1.0);
}
