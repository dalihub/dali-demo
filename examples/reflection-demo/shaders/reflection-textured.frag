uniform lowp vec4 uColor;
uniform sampler2D sTexture;
uniform mediump vec2 uScreenSize;

uniform mediump vec3 eyePos;
uniform mediump vec3 lightDir;

varying mediump vec3 vNormal;
varying mediump vec3 vPosition;
varying mediump vec2 vTexCoord;

void main()
{
  mediump vec3 n = normalize( vNormal );
  mediump vec3 l = normalize( lightDir );
  mediump vec3 e = normalize( eyePos );
  mediump float intensity = max(dot(n,l), 0.0);
  gl_FragColor = texture2D(sTexture, vTexCoord) * intensity;
}
