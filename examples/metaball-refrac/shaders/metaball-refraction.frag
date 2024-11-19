// Fragment shader code for metaball and background composition with refraction effect

precision mediump float;
varying vec2 vTexCoord;
uniform sampler2D sTexture;
uniform sampler2D sEffect;

void main()
{
  vec4 metaColor = texture2D(sEffect, vTexCoord);
  vec2 zoomCoords;
  float bright = 1.0;
  if (metaColor.r > 0.85)
  {
    zoomCoords = ((vTexCoord - 0.5) * 0.95) + 0.5;
  }
  else if (metaColor.r > 0.78)
  {
    float interpolation = mix(0.95, 1.05, (0.85 - metaColor.r) * 50.0);
    zoomCoords = ((vTexCoord - 0.5) * interpolation) + 0.5;
    bright = 1.2;
  }
  else
  {
    zoomCoords = vTexCoord;
  }

  gl_FragColor = texture2D(sTexture, zoomCoords) * bright;
}
