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

using namespace Dali;

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

Geometry CreateCuboidWireframeGeometry()
{
//
// 2---3
// |-  |-
// | 6---7
// | | | |
// 0-|-1 |
//  -|  -|
//   4---5
//
  Vector3 vertexData[] = {
    Vector3(-.5, -.5, -.5),
	Vector3( .5, -.5, -.5),
	Vector3(-.5,  .5, -.5),
	Vector3( .5,  .5, -.5),
	Vector3(-.5, -.5,  .5),
	Vector3( .5, -.5,  .5),
	Vector3(-.5,  .5,  .5),
	Vector3( .5,  .5,  .5),
  };

  uint16_t indices[] = {
    0, 1, 1, 3, 3, 2, 2, 0,
	0, 4, 1, 5, 3, 7, 2, 6,
	4, 5, 5, 7, 7, 6, 6, 4
  };
  VertexBuffer vertexBuffer = VertexBuffer::New( Property::Map()
    .Add( "aPosition", Property::VECTOR3));
  vertexBuffer.SetData(vertexData, std::extent<decltype(vertexData)>::value );

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer );
  geometry.SetIndexBuffer( indices, std::extent<decltype(indices)>::value );
  geometry.SetType(Geometry::LINES);
  return geometry;
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
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
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
    auto actualIndex = PropertyRanges::DEFAULT_RENDERER_PROPERTY_START_INDEX + i;
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
