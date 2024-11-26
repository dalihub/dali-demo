// Shader for basic, per-vertex lighting (vertex):

attribute mediump vec3 aPosition;
attribute highp   vec3 aNormal;
attribute highp   vec2 aTexCoord;

varying   mediump vec2 vTexCoord;
uniform   mediump mat4 uMvpMatrix;
uniform   mediump vec3 uSize;
uniform   mediump vec3 uObjectDimensions;
varying   mediump vec3 vIllumination;
uniform   mediump mat4 uModelView;
uniform   mediump mat4 uViewMatrix;
uniform   mediump mat3 uNormalMatrix;
uniform   mediump vec3 uLightPosition;

void main()
{
  mediump vec4 vertexPosition = vec4( aPosition * uObjectDimensions, 1.0 );
  vertexPosition = uMvpMatrix * vertexPosition;

  vec4 mvVertexPosition = uModelView * vertexPosition;

  vec3 vectorToLight = normalize( mat3( uViewMatrix ) * uLightPosition - mvVertexPosition.xyz );

  vec3 normal = uNormalMatrix * aNormal;
  float lightDiffuse = max( dot( vectorToLight, normal ), 0.0 );
  vIllumination = vec3( lightDiffuse * 0.5 + 0.5 );

  gl_Position = vertexPosition;
}
