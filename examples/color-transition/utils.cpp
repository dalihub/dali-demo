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
#include "utils.h"

#include <cstdarg>
#include <cstdio>

#include <cmath>

using namespace Dali;

Geometry CreateQuadGeometry()
{
  // Create geometry -- unit square with whole of the texture mapped to it.
  struct Vertex
  {
    Vector3 aPosition;
  };

  Vertex vertexData[] = {
    {Vector3(-.5f, .5f, .0f)},
    {Vector3(.5f, .5f, .0f)},
    {Vector3(-.5f, -.5f, .0f)},
    {Vector3(.5f, -.5f, .0f)},
  };

  VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map()
                                                  .Add("aPosition", Property::VECTOR3));
  vertexBuffer.SetData(vertexData, std::extent<decltype(vertexData)>::value);

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  geometry.SetType(Geometry::TRIANGLE_STRIP);
  return geometry;
}

Renderer CreateRenderer(TextureSet textures, Geometry geometry, Shader shader, uint32_t options)
{
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetProperty(Renderer::Property::BLEND_MODE,
                       (options & RendererOptions::BLEND) ? BlendMode::ON : BlendMode::OFF);
  renderer.SetProperty(Renderer::Property::DEPTH_TEST_MODE,
                       (options & RendererOptions::DEPTH_TEST) ? DepthTestMode::ON : DepthTestMode::OFF);
  renderer.SetProperty(Renderer::Property::DEPTH_WRITE_MODE,
                       (options & RendererOptions::DEPTH_WRITE) ? DepthWriteMode::ON : DepthWriteMode::OFF);

  int faceCulling = (((options & RendererOptions::CULL_BACK) != 0) << 1) |
                    ((options & RendererOptions::CULL_FRONT) != 0);
  renderer.SetProperty(Renderer::Property::FACE_CULLING_MODE, faceCulling);

  if(!textures)
  {
    textures = TextureSet::New();
  }

  renderer.SetTextures(textures);
  return renderer;
}

void CenterActor(Actor actor)
{
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
}
