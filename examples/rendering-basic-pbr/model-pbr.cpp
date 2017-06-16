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
#include "model-pbr.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include <cstdio>
#include <string.h>

// INTERNAL INCLUDES
#include "obj-loader.h"

namespace
{
const int SEGMENTS = 16;

struct Vertices
{
  Vector<Vector3> positions;
  Vector<Vector3> normals;
  Vector<Vector3> tangents;
  Vector<Vector2> texCoords;
};

} // namespace


ModelPbr::ModelPbr()
{
}

ModelPbr::~ModelPbr()
{
}

void ModelPbr::Init( Shader shader, const std::string& modelUrl, const Vector3& position, const Vector3& size )
{
  Geometry geometry;

  geometry = CreateGeometry( modelUrl );

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
  mActor.SetPosition( position );
  mActor.SetSize( size );
  mActor.AddRenderer( renderer );
}

/**
 * Set texture and sampler
 */
void ModelPbr::InitTexture(Texture albedoM, Texture normalR, Texture texDiffuse, Texture texSpecular)
{
  mTextureSet = TextureSet::New();
  mTextureSet.SetTexture( 0u, albedoM );
  mTextureSet.SetTexture( 1u, normalR );
  mTextureSet.SetTexture( 2u, texDiffuse );
  mTextureSet.SetTexture( 3u, texSpecular );

  Sampler sampler = Sampler::New();
  sampler.SetWrapMode(WrapMode::CLAMP_TO_EDGE,WrapMode::CLAMP_TO_EDGE,WrapMode::CLAMP_TO_EDGE);
  sampler.SetFilterMode(FilterMode::LINEAR_MIPMAP_LINEAR,FilterMode::LINEAR);
  mTextureSet.SetSampler(3,sampler);
}

Actor& ModelPbr::GetActor()
{
  return mActor;
}

/**
 * Create geometry from OBJ path file
 */
Geometry ModelPbr::CreateGeometry( const std::string& url )
{
  std::streampos fileSize;
  Dali::Vector<char> fileContent;

  Geometry geometry;

  if( FileLoader::ReadFile( url, fileSize, fileContent, FileLoader::TEXT ) )
  {
    PbrDemo::ObjLoader objLoader;

    objLoader.ClearArrays();
    objLoader.LoadObject( fileContent.Begin(), fileSize );

    geometry = objLoader.CreateGeometry( PbrDemo::ObjLoader::TEXTURE_COORDINATES | PbrDemo::ObjLoader::TANGENTS, true );
  }

  return geometry;
}
