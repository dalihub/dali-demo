/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <string.h>
#include <cstdio>

// INTERNAL INCLUDES
#include "generated/skybox-frag.h"
#include "generated/skybox-vert.h"
#include "obj-loader.h"

ModelSkybox::ModelSkybox()
{
}

ModelSkybox::~ModelSkybox()
{
}

void ModelSkybox::Init(const Vector3& size)
{
  Geometry geometry = CreateGeometry();
  Shader   shader   = Shader::New(SHADER_SKYBOX_VERT, SHADER_SKYBOX_FRAG);

  Renderer renderer = Renderer::New(geometry, shader);

  if(mTextureSet)
  {
    renderer.SetTextures(mTextureSet);
  }

  // Face culling is enabled to hide the backwards facing sides of the model
  // This is sufficient to render a single object; for more complex scenes depth-testing might be required
  renderer.SetProperty(Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::ON);
  renderer.SetProperty(Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::ON);
  renderer.SetProperty(Renderer::Property::DEPTH_FUNCTION, DepthFunction::LESS_EQUAL);
  renderer.SetProperty(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK);

  mActor = Actor::New();
  mActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  mActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mActor.SetProperty(Actor::Property::SIZE, size);
  mActor.AddRenderer(renderer);
}

/**
 * Set texture and sampler
 */
void ModelSkybox::InitTexture(Texture texSkybox)
{
  mTextureSet = TextureSet::New();
  mTextureSet.SetTexture(0u, texSkybox);

  Sampler sampler = Sampler::New();
  sampler.SetWrapMode(WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE);
  sampler.SetFilterMode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR);
  mTextureSet.SetSampler(0, sampler);
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
    {Vector3(-1.0f, 1.0f, -1.0f)},
    {Vector3(-1.0f, -1.0f, -1.0f)},
    {Vector3(1.0f, -1.0f, -1.0f)},
    {Vector3(1.0f, -1.0f, -1.0f)},
    {Vector3(1.0f, 1.0f, -1.0f)},
    {Vector3(-1.0f, 1.0f, -1.0f)},

    // left
    {Vector3(-1.0f, -1.0f, 1.0f)},
    {Vector3(-1.0f, -1.0f, -1.0f)},
    {Vector3(-1.0f, 1.0f, -1.0f)},
    {Vector3(-1.0f, 1.0f, -1.0f)},
    {Vector3(-1.0f, 1.0f, 1.0f)},
    {Vector3(-1.0f, -1.0f, 1.0f)},

    // right
    {Vector3(1.0f, -1.0f, -1.0f)},
    {Vector3(1.0f, -1.0f, 1.0f)},
    {Vector3(1.0f, 1.0f, 1.0f)},
    {Vector3(1.0f, 1.0f, 1.0f)},
    {Vector3(1.0f, 1.0f, -1.0f)},
    {Vector3(1.0f, -1.0f, -1.0f)},

    // front
    {Vector3(-1.0f, -1.0f, 1.0f)},
    {Vector3(-1.0f, 1.0f, 1.0f)},
    {Vector3(1.0f, 1.0f, 1.0f)},
    {Vector3(1.0f, 1.0f, 1.0f)},
    {Vector3(1.0f, -1.0f, 1.0f)},
    {Vector3(-1.0f, -1.0f, 1.0f)},

    // botton
    {Vector3(-1.0f, 1.0f, -1.0f)},
    {Vector3(1.0f, 1.0f, -1.0f)},
    {Vector3(1.0f, 1.0f, 1.0f)},
    {Vector3(1.0f, 1.0f, 1.0f)},
    {Vector3(-1.0f, 1.0f, 1.0f)},
    {Vector3(-1.0f, 1.0f, -1.0f)},

    // top
    {Vector3(-1.0f, -1.0f, -1.0f)},
    {Vector3(-1.0f, -1.0f, 1.0f)},
    {Vector3(1.0f, -1.0f, -1.0f)},
    {Vector3(1.0f, -1.0f, -1.0f)},
    {Vector3(-1.0f, -1.0f, 1.0f)},
    {Vector3(1.0f, -1.0f, 1.0f)}};

  VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map()
                                                  .Add("aPosition", Property::VECTOR3));
  vertexBuffer.SetData(skyboxVertices, sizeof(skyboxVertices) / sizeof(Vertex));

  geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  geometry.SetType(Geometry::TRIANGLES);
  return geometry;
}
