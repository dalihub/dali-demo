precision mediump float;
uniform sampler2D sTexture;

uniform float uTime;
uniform float uKFactor;
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

  const mediump float PI = 3.1415926535897932384626433832795;
  mediump float v = 0.0;
  mediump vec2 c = vTexCoord * uKFactor - uKFactor/2.0;
  v += sin((c.x+uTime));
  v += sin((c.y+uTime)/2.0);
  v += sin((c.x+c.y+uTime)/2.0);
  c += uKFactor/2.0 * vec2(sin(uTime/3.0), cos(uTime/2.0));
  v += sin(sqrt(c.x*c.x+c.y*c.y+1.0)+uTime);
  v = v/2.0;
  mediump vec3 col = vec3(1, sin(PI*v), cos(PI*v));
  gl_FragColor = (texture2D(sTexture, vTexCoord)) * (((col.r+col.g+col.b)/3.0)+1.0+intensity);
}
