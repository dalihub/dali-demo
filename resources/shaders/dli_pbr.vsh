//@version 100

#ifdef HIGHP
  precision highp float;
#else
  precision mediump float;
#endif

INPUT vec3 aPosition;
INPUT vec2 aTexCoord;
INPUT vec3 aNormal;
INPUT vec3 aTangent;
INPUT vec4 aVertexColor;

#ifdef MORPH
  UNIFORM sampler2D sBlendShapeGeometry;
#endif

OUTPUT vec2 vUV;
OUTPUT vec3 vNormal;
OUTPUT vec3 vTangent;
OUTPUT vec3 vViewVec;
OUTPUT vec4 vColor;

UNIFORM_BLOCK VertBlock
{
UNIFORM highp mat4 uMvpMatrix;
UNIFORM highp mat4 uViewMatrix;
UNIFORM mat3 uNormalMatrix;
UNIFORM mat4 uModelMatrix;
UNIFORM mat4 uModelView;
UNIFORM mat4 uProjection;
};

#ifdef SKINNING
INPUT vec4 aJoints0;
INPUT vec4 aWeights0;
#define MAX_BONES 64

UNIFORM_BLOCK SkinningBlock
{
UNIFORM highp mat4 uBone[MAX_BONES];
};
#endif

#ifdef MORPH
UNIFORM_BLOCK MorphBlock
{
#define MAX_BLEND_SHAPE_NUMBER 128
UNIFORM int uNumberOfBlendShapes;                                   ///< Total number of blend shapes loaded.
UNIFORM float uBlendShapeWeight[MAX_BLEND_SHAPE_NUMBER];            ///< The weight of each blend shape.
#ifdef MORPH_VERSION_2_0
UNIFORM float uBlendShapeUnnormalizeFactor;                         ///< Factor used to unnormalize the geometry of the blend shape.
#else
UNIFORM float uBlendShapeUnnormalizeFactor[MAX_BLEND_SHAPE_NUMBER]; ///< Factor used to unnormalize the geometry of the blend shape.
#endif
UNIFORM int uBlendShapeComponentSize;                               ///< The size in the texture of either the vertices, normals or tangents. Used to calculate the offset to address them.
};
#endif

void main()
{
  vec4 position = vec4(aPosition, 1.0);
  vec3 normal = aNormal;
  vec3 tangent = aTangent;

#ifdef MORPH
  int width = textureSize( sBlendShapeGeometry, 0 ).x;

  int blendShapeBufferOffset = 0;
  for( int index = 0; index < uNumberOfBlendShapes; ++index )
  {
#ifdef MORPH_POSITION
    // Calculate the index to retrieve the geometry from the texture.
    int vertexId = gl_VertexID + blendShapeBufferOffset;
    int x = vertexId % width;
    int y = vertexId / width;

    vec3 diff = vec3(0.0);
    // Retrieves the blend shape geometry from the texture, unnormalizes it and multiply by the weight.
    if( 0.0 != uBlendShapeWeight[index] )
    {
#ifdef MORPH_VERSION_2_0
       float unnormalizeFactor = uBlendShapeUnnormalizeFactor;
#else
       float unnormalizeFactor = uBlendShapeUnnormalizeFactor[index];
#endif

      diff = uBlendShapeWeight[index] * unnormalizeFactor * ( texelFetch( sBlendShapeGeometry, ivec2(x, y), 0 ).xyz - 0.5 );
    }

    position.xyz += diff;

    blendShapeBufferOffset += uBlendShapeComponentSize;
#endif

#ifdef MORPH_NORMAL
    // Calculate the index to retrieve the normal from the texture.
    vertexId = gl_VertexID + blendShapeBufferOffset;
    x = vertexId % width;
    y = vertexId / width;

    // Retrieves the blend shape normal from the texture, unnormalizes it and multiply by the weight.
    if( 0.0 != uBlendShapeWeight[index] )
    {
      diff = uBlendShapeWeight[index] * 2.0 * ( texelFetch( sBlendShapeGeometry, ivec2(x, y), 0 ).xyz - 0.5 );
    }

    normal += diff.xyz;

    blendShapeBufferOffset += uBlendShapeComponentSize;
#endif

#ifdef MORPH_TANGENT
    // Calculate the index to retrieve the tangent from the texture.
    vertexId = gl_VertexID + blendShapeBufferOffset;
    x = vertexId % width;
    y = vertexId / width;

    // Retrieves the blend shape tangent from the texture, unnormalizes it and multiply by the weight.
    if( 0.0 != uBlendShapeWeight[index] )
    {
      diff = uBlendShapeWeight[index] * 2.0 * ( texelFetch( sBlendShapeGeometry, ivec2(x, y), 0 ).xyz - 0.5 );
    }

    tangent += diff.xyz;

    blendShapeBufferOffset += uBlendShapeComponentSize;
#endif
  }

#endif

#ifdef SKINNING
  mat4 bone = uBone[int(aJoints0.x)] * aWeights0.x +
    uBone[int(aJoints0.y)] * aWeights0.y +
    uBone[int(aJoints0.z)] * aWeights0.z +
    uBone[int(aJoints0.w)] * aWeights0.w;
  position = bone * position;
  normal = (bone * vec4(normal, 0.0)).xyz;
  tangent = (bone * vec4(tangent, 0.0)).xyz;

  normal = normalize(normal);
  tangent = normalize(tangent);
  vec4 vPosition = position;
#else
  vec4 vPosition = uModelMatrix * position;
#endif

  vNormal = normalize(uNormalMatrix * normal);

  vTangent = normalize(uNormalMatrix * tangent);


  vec4 viewPosition = uViewMatrix * vPosition;
  gl_Position = uProjection * viewPosition;

#ifdef FLIP_V
  vUV = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
#else
  vUV = aTexCoord;
#endif

  vColor = aVertexColor;

  vViewVec = viewPosition.xyz;
}
