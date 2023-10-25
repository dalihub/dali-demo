/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
 */

#include "ball-renderer.h"
#include <dali-toolkit/dali-toolkit.h>
#include "cube-renderer.h"
#include "generated/rendering-textured-shape-frag.h"
#include "generated/rendering-textured-shape-vert.h"

using namespace Dali;

namespace
{
const char* TEXTURE_URL = DEMO_IMAGE_DIR "/background-3.jpg";
} // namespace

Dali::Geometry   BallRenderer::gBallGeometry;
Dali::TextureSet BallRenderer::gBallTextureSet;

struct Vertex
{
  Vector3 aPosition;
  Vector2 aTexCoord;
};

void SubDivide(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices)
{
  uint16_t triangleCount = indices.size() / 3;
  for(uint16_t i = 0; i < triangleCount; ++i)
  {
    auto v1 = vertices[indices[i * 3]].aPosition;
    auto v2 = vertices[indices[i * 3 + 1]].aPosition;
    auto v3 = vertices[indices[i * 3 + 2]].aPosition;
    // Triangle subdivision adds pts halfway along each edge.
    auto     v4 = v1 + (v2 - v1) * 0.5f;
    auto     v5 = v2 + (v3 - v2) * 0.5f;
    auto     v6 = v3 + (v1 - v3) * 0.5f;
    uint16_t j  = vertices.size();
    vertices.emplace_back(Vertex{v4, Vector2::ZERO});
    vertices.emplace_back(Vertex{v5, Vector2::ZERO});
    vertices.emplace_back(Vertex{v6, Vector2::ZERO});
    // Now, original tri breaks into 4, so replace this tri, and add 3 more
    uint16_t i1        = indices[i * 3 + 1];
    uint16_t i2        = indices[i * 3 + 2];
    indices[i * 3 + 1] = j;
    indices[i * 3 + 2] = j + 2;

    std::vector<uint16_t> newTris = {j, i1, uint16_t(j + 1), j, uint16_t(j + 1), uint16_t(j + 2), uint16_t(j + 1), i2, uint16_t(j + 2)};
    indices.insert(indices.end(), newTris.begin(), newTris.end());
  }
  for(auto& vertex : vertices)
  {
    vertex.aPosition.Normalize();
  }
}

void MapUVsToSphere(std::vector<Vertex>& vertices)
{
  // Convert world coords to long-lat
  // Assume radius=1;
  // V=(cos(long)cos(lat), sin(long)cos(lat), sin(lat))
  // => lat=arcsin(z), range (-PI/2, PI/2); => 0.5+(asin(z)/PI) range(0,1)
  // => y/x = sin(long)/cos(long) => long = atan2(y/x), range(-pi, pi)
  // But, rotate 90 deg for portrait texture!
  for(auto& vertex : vertices)
  {
    vertex.aTexCoord.y = 1.0f + (atan2f(vertex.aPosition.y, vertex.aPosition.x) / (2.0f * Math::PI));
    vertex.aTexCoord.x = 1.0f - (0.5f + (asinf(vertex.aPosition.z) / Math::PI));
  }
}

/**
 * @brief CreateBallGeometry
 * This function creates a ball geometry including texture coordinates.
 */
Geometry BallRenderer::CreateBallGeometry()
{
  if(!gBallGeometry)
  {
    float phi = (1.0f + sqrt(5.0f)) * 0.5f; // golden ratio
    float a   = 1.0f;
    float b   = 1.0f / phi;

    // add vertices
    std::vector<Vertex> vertices;
    vertices.emplace_back(Vertex{Vector3{0, b, -a}, Vector2::ZERO});
    vertices.emplace_back(Vertex{Vector3{b, a, 0}, Vector2::ZERO});
    vertices.emplace_back(Vertex{Vector3{-b, a, -a}, Vector2::ZERO});

    vertices.emplace_back(Vertex{Vector3{0, b, a}, Vector2::ZERO});
    vertices.emplace_back(Vertex{Vector3{0, -b, a}, Vector2::ZERO});
    vertices.emplace_back(Vertex{Vector3{-a, 0, b}, Vector2::ZERO});

    vertices.emplace_back(Vertex{Vector3{0, -b, -a}, Vector2::ZERO});
    vertices.emplace_back(Vertex{Vector3{a, 0, -b}, Vector2::ZERO});
    vertices.emplace_back(Vertex{Vector3{a, 0, b}, Vector2::ZERO});

    vertices.emplace_back(Vertex{Vector3{-a, 0, -b}, Vector2::ZERO});
    vertices.emplace_back(Vertex{Vector3{b, -a, 0}, Vector2::ZERO});
    vertices.emplace_back(Vertex{Vector3{-b, -a, 0}, Vector2::ZERO});

    for(auto& vertex : vertices)
    {
      vertex.aPosition.Normalize();
    }

    // this dodgy code is not zero indexed but starts at 1.
    std::vector<uint16_t> indices = {
      3, 2, 1, 2, 3, 4, 6, 5, 4, 5, 9, 4, 8, 7, 1, 7, 10, 1, 12, 11, 5, 11, 12, 7, 10, 6, 3, 6, 10, 12, 9, 8, 2, 8, 9, 11, 3, 6, 4, 9, 2, 4, 10, 3, 1, 2, 8, 1, 12, 10, 7, 8, 11, 7, 6, 12, 5, 11, 9, 5};
    // fix offset indices
    for(auto& index : indices)
    {
      --index;
    }

    // 2 subdivisions gives a reasonably nice sphere
    SubDivide(vertices, indices);
    SubDivide(vertices, indices);

    MapUVsToSphere(vertices);

    VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map()
                                                    .Add("aPosition", Property::VECTOR3)
                                                    .Add("aTexCoord", Property::VECTOR2));
    vertexBuffer.SetData(&vertices[0], vertices.size());

    gBallGeometry = Geometry::New();
    gBallGeometry.AddVertexBuffer(vertexBuffer);
    gBallGeometry.SetIndexBuffer(&indices[0], indices.size());
    gBallGeometry.SetType(Geometry::TRIANGLES);
  }
  return gBallGeometry;
}

TextureSet BallRenderer::CreateTexture(std::string url)
{
  // Load image from file
  PixelData pixels = Dali::Toolkit::SyncImageLoader::Load(url);

  Texture texture = Texture::New(TextureType::TEXTURE_2D, pixels.GetPixelFormat(), pixels.GetWidth(), pixels.GetHeight());
  texture.Upload(pixels, 0, 0, 0, 0, pixels.GetWidth(), pixels.GetHeight());

  // create TextureSet
  auto textureSet = TextureSet::New();
  textureSet.SetTexture(0, texture);

  return textureSet;
}

/**
 * Function creates renderer. It turns on depth test and depth write.
 */
Dali::Renderer BallRenderer::CreateRenderer(TextureSet textures)
{
  CreateBallGeometry();
  Dali::Shader shader   = CreateShader();
  Renderer     renderer = Renderer::New(gBallGeometry, shader);
  renderer.SetTextures(textures);

  // Face culling is enabled to hide the backwards facing sides of the ball
  // This is sufficient to render a single object; for more complex scenes depth-testing might be required
  renderer.SetProperty(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK);
  return renderer;
}

/**
 * Creates new actor and renderer.
 */
Actor BallRenderer::CreateActor(Vector3 size, Vector4 color)
{
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(size.x, size.y, size.z) * 0.5f);
  actor.SetProperty(Actor::Property::COLOR, color);
  if(!gBallTextureSet)
  {
    gBallTextureSet = CreateTexture(TEXTURE_URL);
  }
  Renderer renderer = CreateRenderer(gBallTextureSet);
  actor.AddRenderer(renderer);
  return actor;
}

Actor BallRenderer::CreateActor(Vector3 size, std::string url)
{
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  // Sphere is radius 1; so has natural diameter 2, so halve the size
  actor.SetProperty(Actor::Property::SIZE, Vector3(size.x, size.y, size.z) * 0.5f);

  TextureSet textures = CreateTexture(url);
  Renderer   renderer = CreateRenderer(textures);
  actor.AddRenderer(renderer);
  return actor;
}
