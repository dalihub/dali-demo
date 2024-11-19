uniform lowp vec4 uColor;
varying lowp vec2 vCoefficient;

void main()
{
  lowp float C = (vCoefficient.x*vCoefficient.x-vCoefficient.y);
  lowp float Cdx = dFdx(C);
  lowp float Cdy = dFdy(C);

  lowp float distance = float(C / sqrt(Cdx*Cdx + Cdy*Cdy));
  lowp float alpha = 0.5 - distance;
  gl_FragColor = vec4( uColor.rgb, uColor.a * alpha );
}
