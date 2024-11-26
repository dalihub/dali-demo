//@version 100

UNIFORM sampler2D sTexture0;
UNIFORM sampler2D sTexture1;

UNIFORM_BLOCK Custom
{
UNIFORM lowp vec4 uColor;
UNIFORM mediump vec3 eyePos;
UNIFORM mediump vec3 lightDir;
UNIFORM mediump vec2 uScreenSize;
};

INPUT mediump vec3 vNormal;
INPUT mediump vec3 vPosition;
INPUT mediump vec2 vTexCoord;

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
  gl_FragColor = ((TEXTURE(sTexture0, vTexCoord) * factor ) +
                  (TEXTURE(sTexture1, uv))) * intensity;
}
