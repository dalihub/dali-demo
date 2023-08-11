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

#include "cube-renderer.h"
#include <dali-toolkit/dali-toolkit.h>
#include "generated/rendering-textured-shape-frag.h"
#include "generated/rendering-textured-shape-vert.h"

using namespace Dali;

namespace
{
const char* TEXTURE_URL = DEMO_IMAGE_DIR "/wood.png";
} // namespace

Dali::Geometry   CubeRenderer::gCubeGeometry;
Dali::TextureSet CubeRenderer::gCubeTextureSet;

Dali::Shader CreateShader()
{
  static Dali::Shader gShapeShader;

  if(!gShapeShader)
  {
    gShapeShader = Shader::New(SHADER_RENDERING_TEXTURED_SHAPE_VERT, SHADER_RENDERING_TEXTURED_SHAPE_FRAG);
  }
  return gShapeShader;
}

/**
 * @brief CreateCubeGeometry
 * This function creates a cube geometry including texture coordinates.
 */
Geometry CubeRenderer::CreateCubeGeometry()
{
  if(!gCubeGeometry)
  {
    struct Vertex
    {
      Vector3 aPosition;
      Vector2 aTexCoord;
    };

    Vertex vertices[] = {
      {Vector3(1.0f, -1.0f, -1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, 1.0f, -1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, 1.0f, -1.0f), Vector2(0.0, 1.0)},
      {Vector3(-1.0f, 1.0f, 1.0f), Vector2(1.0, 1.0)},
      {Vector3(1.0f, -1.0f, 1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, 1.0f, 1.0f), Vector2(0.0, 1.0)},
      {Vector3(1.0f, 1.0f, 1.0f), Vector2(1.0, 1.0)},
      {Vector3(1.0f, -1.0f, -1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, 1.0f, -1.0f), Vector2(0.0, 1.0)},
      {Vector3(1.0f, -1.0f, 1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, -1.0f, -1.0f), Vector2(0.0, 1.0)},
      {Vector3(-1.0f, -1.0f, -1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, 1.0f, 1.0f), Vector2(0.0, 0.0)},
      {Vector3(-1.0f, 1.0f, -1.0f), Vector2(0.0, 1.0)},
      {Vector3(1.0f, 1.0f, -1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, 1.0f, 1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, 1.0f, 1.0f), Vector2(0.0, 1.0)},
      {Vector3(1.0f, -1.0f, -1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, -1.0f, -1.0f), Vector2(1.0, 0.0)},
      {Vector3(-1.0f, 1.0f, -1.0f), Vector2(0.0, 0.0)},
      {Vector3(-1.0f, 1.0f, 1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, -1.0f, 1.0f), Vector2(1.0, 0.0)},
      {Vector3(1.0f, -1.0f, 1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, 1.0f, 1.0f), Vector2(1.0, 1.0)},
      {Vector3(1.0f, -1.0f, 1.0f), Vector2(1.0, 0.0)},
      {Vector3(1.0f, -1.0f, -1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, -1.0f, 1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, -1.0f, 1.0f), Vector2(1.0, 0.0)},
      {Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0, 0.0)},
      {Vector3(-1.0f, -1.0f, -1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, -1.0f, 1.0f), Vector2(1.0, 0.0)},
      {Vector3(-1.0f, 1.0f, 1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, 1.0f, -1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, 1.0f, -1.0f), Vector2(1.0, 0.0)},
      {Vector3(-1.0f, 1.0f, 1.0f), Vector2(0.0, 0.0)},
    };

    VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map()
                                                    .Add("aPosition", Property::VECTOR3)
                                                    .Add("aTexCoord", Property::VECTOR2));
    vertexBuffer.SetData(vertices, sizeof(vertices) / sizeof(Vertex));

    // create indices
    const unsigned short INDEX_CUBE[] = {
      2, 1, 0, 5, 4, 3, 8, 7, 6, 11, 10, 9, 14, 13, 12, 17, 16, 15, 20, 19, 18, 23, 22, 21, 26, 25, 24, 29, 28, 27, 32, 31, 30, 35, 34, 33};

    gCubeGeometry = Geometry::New();
    gCubeGeometry.AddVertexBuffer(vertexBuffer);
    gCubeGeometry.SetIndexBuffer(INDEX_CUBE,
                                 sizeof(INDEX_CUBE) / sizeof(INDEX_CUBE[0]));
    gCubeGeometry.SetType(Geometry::TRIANGLES);
  }
  return gCubeGeometry;
}

/**
 * This function loads a pixel data from a file. In order to load it we use SyncImageLoader utility.
 * If loading succeeds returned PixelData object can be used to create a texture.
 * Texture must be uploaded. In the end the texture must be set on the TextureSet object.
 */
TextureSet CubeRenderer::CreateTexture(std::string url)
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
Dali::Renderer CubeRenderer::CreateRenderer(TextureSet textures)
{
  CreateCubeGeometry();
  Dali::Shader shader   = CreateShader();
  Renderer     renderer = Renderer::New(gCubeGeometry, shader);
  renderer.SetTextures(textures);

  // Face culling is enabled to hide the backwards facing sides of the cube
  // This is sufficient to render a single object; for more complex scenes depth-testing might be required
  renderer.SetProperty(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK);
  return renderer;
}

/**
 * Creates new actor and renderer.
 */
Actor CubeRenderer::CreateActor(Vector3 size, Vector4 color)
{
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  // Mesh is 2x2x2, so halve the size
  actor.SetProperty(Actor::Property::SIZE, Vector3(size.x, size.y, size.z) * 0.5f);
  actor.SetProperty(Actor::Property::COLOR, color);
  if(!gCubeTextureSet)
  {
    gCubeTextureSet = CreateTexture(TEXTURE_URL);
  }
  Renderer renderer = CreateRenderer(gCubeTextureSet);
  actor.AddRenderer(renderer);
  return actor;
}

Actor CubeRenderer::CreateActor(Vector3 size, std::string url)
{
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(size.x, size.y, size.z) * 0.5f);

  TextureSet textures = CreateTexture(url);
  Renderer   renderer = CreateRenderer(textures);
  actor.AddRenderer(renderer);
  return actor;
}
