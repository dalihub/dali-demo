//Fragment shader code for metaball and background composition with refraction effect

precision highp float;
varying vec2 vTexCoord;
uniform sampler2D sTexture;
uniform sampler2D sEffect;
uniform vec2 uPositionMetaball;
void main()
{
  vec2 zoomCoords;
  vec3 normal = vec3(0.0,0.0,1.0);
  vec2 fakePos = vec2(0.0,0.0);
  vec3 color = vec3(1.0, 1.0, 1.0);
  float ambient = 0.2;

  vec4 metaColor = texture2D(sEffect, vTexCoord);

  vec2 adjustedCoords = vTexCoord.xy * vec2(2.0) - vec2(1.0);
  fakePos = adjustedCoords.xy - vec2(uPositionMetaball.x, -uPositionMetaball.y);
  float len = length(fakePos) + 0.01;
  vec3 colorPos = vec3(0,0,1);

  if (metaColor.r > 0.85)
  {
    zoomCoords = ((vTexCoord - 0.5) * 0.9);
    zoomCoords = zoomCoords + 0.5;

    float interpNormal = mix(0.7, 1.0, (metaColor.r - 0.85) * 4.);
    normal.xyz = vec3(fakePos.x * (1.0 - interpNormal) / len, fakePos.y * (1.0 - interpNormal) / len, interpNormal);
    normal.xyz = normalize(normal.xyz);
    color = vec3(0.65, 1.0, 0);
    colorPos = vec3(fakePos.x,fakePos.y,0);
  }
  else if (metaColor.r > 0.75)
  {
    float interpolation = mix(0.9, 1.15, (0.85 - metaColor.r) * 10.0);
    zoomCoords = ((vTexCoord - 0.5) * interpolation);
    zoomCoords = zoomCoords + 0.5;

    float interpNormal = mix(0.7, 0.0, (0.85 - metaColor.r) * 10.0);
    normal.xyz = vec3(fakePos.x * (1.0 - interpNormal) / len, fakePos.y * (1.0 - interpNormal) / len, interpNormal);
    normal.xyz = normalize(normal.xyz);
    color = vec3(0.65, 1.0, 0);
    colorPos = vec3(fakePos.x,fakePos.y,0);
  }
  else
  {
    zoomCoords = vTexCoord;
    normal = vec3(0,0,0);
    ambient = 0.5;
  }

  vec3 lightPosition = vec3(-750.0,-1000.0,2000.0);
  vec3 vertex = vec3(adjustedCoords.x,adjustedCoords.y,0.0);

  vec3 vecToLight = normalize( lightPosition - vertex );

  float lightDiffuse = dot( vecToLight, normal );
  lightDiffuse = max(0.0,lightDiffuse);
  lightDiffuse = lightDiffuse * 0.5 + 0.5;

  vec3 vertexToEye = vec3(0,0,1) - vertex;
  vertexToEye = normalize(vertexToEye);
  vec3 lightReflect = normalize(reflect(-vecToLight, normal));
  float specularFactor = max(0.0,dot(vertexToEye, lightReflect));
  specularFactor = pow(specularFactor, 32.0) * 0.7;

  vec4 texColor = texture2D(sTexture, zoomCoords);
  gl_FragColor.rgb = texColor.rgb * ambient + color.rgb * texColor.rgb * lightDiffuse + vec3(specularFactor);
  gl_FragColor.a = 1.0;
}
