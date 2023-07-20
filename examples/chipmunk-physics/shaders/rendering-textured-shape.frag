uniform sampler2D uTexture;
uniform mediump float uBrightness;
varying mediump vec2 vTexCoord;
varying mediump vec3 vIllumination;

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
  mediump vec4 texColor = texture2D( uTexture, vTexCoord );
  mediump vec3 pcol=texColor.rgb*(1.0+uBrightness);
  gl_FragColor = vec4( redistribute_rgb(pcol), texColor.a);
}