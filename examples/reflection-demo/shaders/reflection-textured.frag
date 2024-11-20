//@version 100

UNIFORM sampler2D sTexture;

UNIFORM_BLOCK Custom
{
UNIFORM lowp vec4 uColor;
UNIFORM mediump vec2 uScreenSize;
UNIFORM mediump vec3 eyePos;
UNIFORM mediump vec3 lightDir;
};

INPUT mediump vec3 vNormal;
INPUT mediump vec3 vPosition;
INPUT mediump vec2 vTexCoord;

void main()
{
  mediump vec3 n = normalize( vNormal );
  mediump vec3 l = normalize( lightDir );
  mediump vec3 e = normalize( eyePos );
  mediump float intensity = max(dot(n,l), 0.0);
  gl_FragColor = TEXTURE(sTexture, vTexCoord) * intensity;
}
