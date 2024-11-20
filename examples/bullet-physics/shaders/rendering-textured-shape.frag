//@version 100

UNIFORM sampler2D uTexture;

UNIFORM_BLOCK VanFrag
{
UNIFORM mediump float uBrightness;
};

INPUT mediump vec2 vTexCoord;
INPUT mediump vec3 vIllumination;

mediump vec3 redistribute_rgb(mediump vec3 color)
{
    mediump float threshold = 0.9999999;
    mediump float m = max(max(color.r, color.g), color.b);
    if(m <= threshold)
    {
        return color;
    }
    mediump float total = color.r + color.g + color.b;
    if( total >= 3.0 * threshold)
    {
        return vec3(threshold);
    }
    mediump float x = (3.0 * threshold - total) / (3.0 * m - total);
    mediump float gray = threshold - x * m;
    return vec3(gray) + vec3(x)*color;
}

void main()
{
  mediump vec4 texColor = TEXTURE( uTexture, vTexCoord );
  //mediump vec4 texColor = vec4(0.5,0.5,0.5,1.0);
  //gl_FragColor = vec4(texColor.rgb, 1.0);

  mediump vec3 pcol=vec3(vIllumination.rgb * texColor.rgb)*(1.0+uBrightness);
  gl_FragColor = vec4( redistribute_rgb(pcol), 1.0);
}
