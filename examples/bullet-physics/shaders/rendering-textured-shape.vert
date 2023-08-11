attribute mediump vec3 aPosition;  // DALi shader builtin
attribute mediump vec2 aTexCoord;  // DALi shader builtin
uniform   mediump mat4 uMvpMatrix; // DALi shader builtin
uniform   mediump mat4 uViewMatrix; // DALi shader builtin
uniform   mediump mat4 uModelView; // DALi shader builtin
uniform   mediump vec3 uSize;      // DALi shader builtin
varying mediump vec3 vIllumination;
varying mediump vec2 vTexCoord;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 1.0);
  mediump vec3 normal = normalize(vertexPosition.xyz);

  vertexPosition.xyz *= uSize;
  vec4 pos = uModelView * vertexPosition;

  vec4 lightPosition = vec4(400.0, 0.0, 100.0, 1.0);
  vec4 mvLightPos = uViewMatrix * lightPosition;
  vec3 vectorToLight = normalize(mvLightPos.xyz - pos.xyz);
  float lightDiffuse = max(dot(vectorToLight, normal), 0.0);

  vIllumination = vec3(lightDiffuse * 0.5 + 0.5);
  vTexCoord = aTexCoord;
  gl_Position = uMvpMatrix * vertexPosition;
}
