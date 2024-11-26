//@version 100

// Shader for basic, per-vertex lighting (vertex):

INPUT mediump vec3 aPosition;
INPUT highp   vec3 aNormal;
INPUT highp   vec2 aTexCoord;

OUTPUT   mediump vec2 vTexCoord;
OUTPUT   mediump vec3 vIllumination;

UNIFORM_BLOCK Vanilla
{
UNIFORM   mediump mat4 uMvpMatrix;
UNIFORM   mediump vec3 uSize;
UNIFORM   mediump vec3 uObjectDimensions;
UNIFORM   mediump mat4 uModelView;
UNIFORM   mediump mat4 uViewMatrix;
UNIFORM   mediump mat3 uNormalMatrix;
UNIFORM   mediump vec3 uLightPosition;
};

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
