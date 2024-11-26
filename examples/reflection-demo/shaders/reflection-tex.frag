uniform lowp vec4 uColor;
uniform sampler2D sTexture0;
uniform sampler2D sTexture1;
uniform mediump vec3 eyePos;
uniform mediump vec3 lightDir;
uniform mediump vec2 uScreenSize;
varying mediump vec3 vNormal;
varying mediump vec3 vPosition;
varying mediump vec2 vTexCoord;

mediump float rand(mediump vec2 co)
{
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
  mediump vec2 tx = (gl_FragCoord.xy / uScreenSize.xy);
  mediump vec3 n = normalize( vNormal );
  mediump vec3 l = normalize( lightDir );
  mediump vec3 e = normalize( eyePos );
  mediump float factor = gl_FragCoord.y / uScreenSize.y;
  mediump float intensity = max(dot(n,l), 0.0);
  mediump vec2 uv = tx;
  gl_FragColor = ((texture2D(sTexture0, vTexCoord) * factor ) +
                 (texture2D(sTexture1, uv))) * intensity;
}
