/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include "dali-toolkit/dali-toolkit.h"
#include <fstream>

using namespace Dali;
using namespace Dali::Toolkit;

Vector3 ToHueSaturationLightness(Vector3 rgb)
{
  float min = std::min(rgb.r, std::min(rgb.g, rgb.b));
  float max = std::max(rgb.r, std::max(rgb.g, rgb.b));

  Vector3 hsl(max - min, 0.f, (max + min) * .5f);
  if (hsl.x * hsl.x > .0f)
  {
    hsl.y = hsl.x / max;
    if (max == rgb.r)
    {
      hsl.x = (rgb.g - rgb.b) / hsl.x;
    }
    else if(max == rgb.g)
    {
      hsl.x = 2.f + (rgb.b - rgb.r) / hsl.x;
    }
    else
    {
      hsl.x = 4.f + (rgb.r - rgb.g) / hsl.x;
    }
    hsl.x *= 60.f;
    if (hsl.x < 0.f)
    {
      hsl.x += 360.f;
    }
  }
  else
  {
    hsl.y = 0.f;
  }

  return hsl;
}

Vector3 FromHueSaturationLightness(Vector3 hsl)
{
  Vector3 rgb;
  if (hsl.y * hsl.y > 0.f)
  {
    if(hsl.x >= 360.f)
    {
      hsl.x -= 360.f;
    }
    hsl.x /= 60.f;

    int i = FastFloor(hsl.x);
    float ff = hsl.x - i;
    float p = hsl.z * (1.0 - hsl.y);
    float q = hsl.z * (1.0 - (hsl.y * ff));
    float t = hsl.z * (1.0 - (hsl.y * (1.f - ff)));

    switch (i)
    {
    case 0:
      rgb.r = hsl.z;
      rgb.g = t;
      rgb.b = p;
      break;

    case 1:
      rgb.r = q;
      rgb.g = hsl.z;
      rgb.b = p;
      break;

    case 2:
      rgb.r = p;
      rgb.g = hsl.z;
      rgb.b = t;
      break;

    case 3:
      rgb.r = p;
      rgb.g = q;
      rgb.b = hsl.z;
      break;

    case 4:
      rgb.r = t;
      rgb.g = p;
      rgb.b = hsl.z;
      break;

    case 5:
    default:
      rgb.r = hsl.z;
      rgb.g = p;
      rgb.b = q;
      break;
    }
  }
  else
  {
    rgb = Vector3::ONE * hsl.z;
  }

  return rgb;
}

Geometry CreateTesselatedQuad(unsigned int xVerts, unsigned int yVerts,
  Vector2 scale, VertexFn positionFn, VertexFn texCoordFn)
{
  DALI_ASSERT_DEBUG(xVerts > 1 && yVerts > 1);
  int numVerts = xVerts * yVerts;
  struct Vertex
  {
    Vector2 aPosition;
    Vector2 aTexCoord;
  };
  std::vector<Vertex> vertices;
  vertices.reserve( numVerts);

  float dx = 1.f / (xVerts - 1);
  float dz = 1.f / (yVerts - 1);

  Vector2 pos{ 0.f, 0.f };
  for (unsigned int i = 0; i < yVerts; ++i)
  {
    pos.x = float(int((i & 1) * 2) - 1) * dx * .25f;
    for (unsigned int j = 0; j < xVerts; ++j)
    {
      auto vPos = pos + Vector2{ -.5f, -.5f };
      vertices.push_back(Vertex{ (positionFn ? positionFn(vPos) : vPos) * scale,
        texCoordFn ? texCoordFn(pos) : pos });
      pos.x += dx;
    }

    pos.y += dz;
  }

  VertexBuffer vertexBuffer = VertexBuffer::New( Property::Map()
    .Add( "aPosition", Property::VECTOR2 )
    .Add( "aTexCoord", Property::VECTOR2 ));
  vertexBuffer.SetData(vertices.data(), vertices.size());

  int numInds = (xVerts - 1) * (yVerts - 1) * 6;
  std::vector<uint16_t> indices;
  indices.reserve(numInds);

  for (unsigned int i = 1; i < yVerts; ++i)
  {
    if ((i & 1) == 0)
    {
      for (unsigned int j = 1; j < xVerts; ++j)
      {
        int iBase = i * xVerts + j;
        indices.push_back(iBase);
        indices.push_back(iBase - 1);
        indices.push_back(iBase - xVerts - 1);
        indices.push_back(indices.back());
        indices.push_back(iBase - xVerts);
        indices.push_back(iBase);
      }
    }
    else
    {
      for (unsigned int j = 1; j < xVerts; ++j)
      {
        int iBase = i * xVerts + j;
        indices.push_back(iBase);
        indices.push_back(iBase - 1);
        indices.push_back(iBase - xVerts);
        indices.push_back(indices.back());
        indices.push_back(iBase - 1);
        indices.push_back(iBase - xVerts - 1);
      }
    }
  }

  Geometry geom = Geometry::New();
  geom.AddVertexBuffer(vertexBuffer);
  geom.SetIndexBuffer(indices.data(), indices.size());
  return geom;
}

Texture LoadTexture(const std::string& path)
{
  PixelData pixelData = SyncImageLoader::Load(path);

  Texture texture = Texture::New(TextureType::TEXTURE_2D, pixelData.GetPixelFormat(),
    pixelData.GetWidth(), pixelData.GetHeight());
  texture.Upload(pixelData);
  return texture;
}

Renderer CreateRenderer(TextureSet textures, Geometry geometry, Shader shader, uint32_t options)
{
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetProperty(Renderer::Property::BLEND_MODE,
    (options & OPTION_BLEND) ? BlendMode::ON : BlendMode::OFF);
  renderer.SetProperty(Renderer::Property::DEPTH_TEST_MODE,
    (options & OPTION_DEPTH_TEST) ? DepthTestMode::ON : DepthTestMode::OFF);
  renderer.SetProperty(Renderer::Property::DEPTH_WRITE_MODE,
    (options & OPTION_DEPTH_WRITE) ? DepthWriteMode::ON : DepthWriteMode::OFF);
  renderer.SetProperty(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK);

  if (!textures)
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

Actor CreateActor()
{
  auto actor = Actor::New();
  CenterActor(actor);
  return actor;
}

Renderer CloneRenderer(Renderer original)
{
  Geometry geom = original.GetGeometry();
  Shader shader = original.GetShader();
  Renderer clone = Renderer::New(geom, shader);

  // Copy properties.
  Property::IndexContainer indices;
  original.GetPropertyIndices(indices);

  for (auto& i: indices)
  {
    auto actualIndex = Dali::PropertyRanges::DEFAULT_RENDERER_PROPERTY_START_INDEX + i;
    clone.SetProperty(actualIndex, original.GetProperty(actualIndex));
  }

  // Copy texture references (and create TextureSet, if there's any textures).
  TextureSet ts = original.GetTextures();
  clone.SetTextures(ts);

  return clone;
}

Actor CloneActor(Actor original)
{
  using namespace Dali;

  auto clone = Actor::New();
  clone.SetProperty(Actor::Property::NAME, original.GetProperty(Actor::Property::NAME));

  // Copy properties.
  // Don't copy every single one of them.
  // Definitely don't copy resize policy related things, which will internally enable
  // relayout, which in turn will result in losing the ability to set Z size.
  for (auto i : {
    Actor::Property::PARENT_ORIGIN,
    Actor::Property::ANCHOR_POINT,
    Actor::Property::SIZE,
    Actor::Property::POSITION,
    Actor::Property::ORIENTATION,
    Actor::Property::SCALE,
    Actor::Property::VISIBLE,
    Actor::Property::COLOR,
    Actor::Property::NAME,
  })
  {
    clone.SetProperty(i, original.GetProperty(i));
  }

  // Clone renderers.
  for(unsigned int i = 0; i < original.GetRendererCount(); ++i)
  {
    auto rClone = CloneRenderer(original.GetRendererAt(i));
    clone.AddRenderer(rClone);
  }

  // Recurse into children.
  for(unsigned int i = 0; i < original.GetChildCount(); ++i)
  {
    Actor newChild = CloneActor(original.GetChildAt(i));
    clone.Add(newChild);
  }

  return clone;
}
