/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include "model-skybox.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include <cstdio>
#include <string.h>

// INTERNAL INCLUDES
#include "obj-loader.h"

namespace
{
/*
 * Vertex shader for a skybox
 */
const char* VERTEX_SHADER_SKYBOX = DALI_COMPOSE_SHADER(
attribute mediump vec3 aPosition;\n // DALi shader builtin
uniform   mediump mat4 uMvpMatrix;\n // DALi shader builtin
\n
varying mediump vec3 vTexCoord;\n
void main()\n
{\n
  vTexCoord =  aPosition;\n

  mediump vec4 vertexPosition = vec4(aPosition, 1.0);\n
  vec4 clipSpacePosition = uMvpMatrix * vertexPosition;\n
  gl_Position = clipSpacePosition.xyww;\n // Writes 1.0, the maximum depth value, into the depth buffer.
                                          // This is an optimization to avoid running the fragment shader
                                          // for the pixels hidden by the scene's objects.
}\n
);

/*
 * Fragment shader for a skybox
 */
const char* FRAGMENT_SHADER_SKYBOX = DALI_COMPOSE_SHADER(
uniform samplerCube uSkyBoxTexture;\n
\n
varying mediump vec3 vTexCoord;\n
void main()\n
{\n
  mediump vec4 texColor = textureCube( uSkyBoxTexture, vTexCoord, 0.0);\n
  gl_FragColor = texColor;\n
}\n
);

} // namespace


ModelSkybox::ModelSkybox()
{
}

ModelSkybox::~ModelSkybox()
{
}

void ModelSkybox::Init( const Vector3& size )
{

  Geometry geometry = CreateGeometry();
  Shader shader = Shader::New( VERTEX_SHADER_SKYBOX, FRAGMENT_SHADER_SKYBOX );

  Renderer renderer = Renderer::New( geometry, shader );

  if( mTextureSet )
  {
    renderer.SetTextures( mTextureSet );
  }

  // Face culling is enabled to hide the backwards facing sides of the model
  // This is sufficient to render a single object; for more complex scenes depth-testing might be required
  renderer.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::ON );
  renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::ON );
  renderer.SetProperty( Renderer::Property::DEPTH_FUNCTION, DepthFunction::LESS_EQUAL );
  renderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK );

  mActor = Actor::New();
  mActor.SetAnchorPoint( AnchorPoint::CENTER );
  mActor.SetParentOrigin( ParentOrigin::CENTER );
  mActor.SetSize( size );
  mActor.AddRenderer( renderer );
}

/**
 * Set texture and sampler
 */
void ModelSkybox::InitTexture(Texture texSkybox)
{
  mTextureSet = TextureSet::New();
  mTextureSet.SetTexture( 0u, texSkybox );

  Sampler sampler = Sampler::New();
  sampler.SetWrapMode(WrapMode::CLAMP_TO_EDGE,WrapMode::CLAMP_TO_EDGE,WrapMode::CLAMP_TO_EDGE);
  sampler.SetFilterMode(FilterMode::LINEAR_MIPMAP_LINEAR,FilterMode::LINEAR);
  mTextureSet.SetSampler(0,sampler);
}

Actor& ModelSkybox::GetActor()
{
  return mActor;
}


/**
 * @brief CreateGeometry
 * This function creates a cube geometry including texture coordinates.
 */
Geometry ModelSkybox::CreateGeometry()
{
  Geometry geometry;

  struct Vertex
  {
    Vector3 aPosition;
  };

  Vertex skyboxVertices[] = {
      // back
      { Vector3(  -1.0f,  1.0f, -1.0f ) },
      { Vector3(  -1.0f, -1.0f, -1.0f ) },
      { Vector3(   1.0f, -1.0f, -1.0f ) },
      { Vector3(   1.0f, -1.0f, -1.0f ) },
      { Vector3(   1.0f,  1.0f, -1.0f ) },
      { Vector3(  -1.0f,  1.0f, -1.0f ) },

      // left
      { Vector3(  -1.0f, -1.0f,  1.0f ) },
      { Vector3(  -1.0f, -1.0f, -1.0f ) },
      { Vector3(  -1.0f,  1.0f, -1.0f ) },
      { Vector3(  -1.0f,  1.0f, -1.0f ) },
      { Vector3(  -1.0f,  1.0f,  1.0f ) },
      { Vector3(  -1.0f, -1.0f,  1.0f ) },

      // right
      { Vector3(   1.0f, -1.0f, -1.0f ) },
      { Vector3(   1.0f, -1.0f,  1.0f ) },
      { Vector3(   1.0f,  1.0f,  1.0f ) },
      { Vector3(   1.0f,  1.0f,  1.0f ) },
      { Vector3(   1.0f,  1.0f, -1.0f ) },
      { Vector3(   1.0f, -1.0f, -1.0f ) },

      // front
      { Vector3(  -1.0f, -1.0f,  1.0f ) },
      { Vector3(  -1.0f,  1.0f,  1.0f ) },
      { Vector3(   1.0f,  1.0f,  1.0f ) },
      { Vector3(   1.0f,  1.0f,  1.0f ) },
      { Vector3(   1.0f, -1.0f,  1.0f ) },
      { Vector3(  -1.0f, -1.0f,  1.0f ) },

      // botton
      { Vector3(  -1.0f,  1.0f, -1.0f ) },
      { Vector3(   1.0f,  1.0f, -1.0f ) },
      { Vector3(   1.0f,  1.0f,  1.0f ) },
      { Vector3(   1.0f,  1.0f,  1.0f ) },
      { Vector3(  -1.0f,  1.0f,  1.0f ) },
      { Vector3(  -1.0f,  1.0f, -1.0f ) },

      // top
      { Vector3(  -1.0f, -1.0f, -1.0f ) },
      { Vector3(  -1.0f, -1.0f,  1.0f ) },
      { Vector3(   1.0f, -1.0f, -1.0f ) },
      { Vector3(   1.0f, -1.0f, -1.0f ) },
      { Vector3(  -1.0f, -1.0f,  1.0f ) },
      { Vector3(   1.0f, -1.0f,  1.0f ) }
  };

  PropertyBuffer vertexBuffer = PropertyBuffer::New( Property::Map()
                                                     .Add( "aPosition", Property::VECTOR3 ) );
  vertexBuffer.SetData( skyboxVertices, sizeof(skyboxVertices) / sizeof(Vertex) );

  geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer );
  geometry.SetType( Geometry::TRIANGLES );
  return geometry;
}
