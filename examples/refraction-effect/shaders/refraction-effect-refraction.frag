//@version 100

precision mediump float;
UNIFORM_BLOCK Custom
{
UNIFORM   mediump float  uEffectStrength;
UNIFORM   mediump vec3   uLightPosition;
UNIFORM   mediump vec2   uLightXYOffset;
UNIFORM   mediump vec2   uLightSpinOffset;
UNIFORM   mediump float  uLightIntensity;
UNIFORM   lowp    vec4   uColor;
};

UNIFORM   sampler2D      sTexture;

INPUT   mediump vec4   vVertex;
INPUT   mediump vec3   vNormal;
INPUT   mediump vec2   vTexCoord;
INPUT   mediump vec2   vTextureOffset;

vec3 rgb2hsl(vec3 rgb)
{
  float epsilon = 1.0e-10;
  vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
  vec4 P = mix(vec4(rgb.bg, K.wz), vec4(rgb.gb, K.xy), step(rgb.b, rgb.g));
  vec4 Q = mix(vec4(P.xyw, rgb.r), vec4(rgb.r, P.yzx), step(P.x, rgb.r));

  // RGB -> HCV
  float value = Q.x;
  float chroma = Q.x - min(Q.w, Q.y);
  float hue = abs(Q.z + (Q.w-Q.y) / (6.0*chroma+epsilon));
  // HCV -> HSL
  float lightness = value - chroma*0.5;
  return vec3( hue, chroma/max( 1.0-abs(lightness*2.0-1.0), 1.0e-1 ), lightness );
}

vec3 hsl2rgb( vec3 hsl )
{
  // pure hue->RGB
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(hsl.xxx + K.xyz) * 6.0 - K.www);
  vec3 RGB = clamp(p - K.xxx, 0.0, 1.0);

  float chroma = ( 1.0 - abs( hsl.z*2.0-1.0 ) ) * hsl.y;
  return ( RGB - 0.5 ) * chroma + hsl.z;
}

void main()
{
  vec3 normal = normalize( vNormal);

  vec3 lightPosition = uLightPosition + vec3(uLightXYOffset+uLightSpinOffset, 0.0);
  mediump vec3 vecToLight = normalize( (lightPosition - vVertex.xyz) * 0.01 );
  mediump float spotEffect = pow( max(0.05, vecToLight.z ) - 0.05, 8.0);

  spotEffect = spotEffect * uEffectStrength;
  mediump float lightDiffuse = ( ( dot( vecToLight, normal )-0.75 ) *uLightIntensity  ) * spotEffect;

  lowp vec4 color = TEXTURE( sTexture, vTexCoord + vTextureOffset * spotEffect );
  vec3 lightedColor =  hsl2rgb( rgb2hsl(color.rgb) + vec3(0.0,0.0,lightDiffuse) );

  gl_FragColor = vec4( lightedColor, color.a ) * uColor;
}
