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

// HEADER
#include "clipped-image.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>

// INTERNAL INCLUDES
#include "generated/clipped-image-vert.h"
#include "generated/clipped-image-frag.h"

namespace ClippedImage
{
using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* const DELTA_PROPERTY_NAME("uDelta"); ///< Name of uniform used to mix the Circle and Quad geometries.

/**
 * @brief Creates the shader required for the clipped image
 * @return A reference to a static handle to a shader object (only created when first called).
 */
Shader& CreateShader()
{
  // Only need to create it once
  // The issue with using a static is that the shader will only get destroyed at application destruction.
  // This is OK for a simple use cases such as this example, but for more polished applications, the shader creation/destruction needs to
  // be managed by the application writer.
  static Shader shader;

  if(!shader)
  {
    shader = Shader::New(SHADER_CLIPPED_IMAGE_VERT, SHADER_CLIPPED_IMAGE_FRAG);
  }

  return shader;
}

/**
 * @brief Creates the geometry required for the clipped image
 * @return A reference to a static handle to a geometry object (only created when first called).
 */
Geometry& CreateGeometry()
{
  // Only need to create it once
  // The issue with using a static is that the geometry will only get destroyed at application destruction.
  // This is OK for a simple use cases such as this example, but for more polished applications, the geometry creation/destruction needs to
  // be managed by the application writer.
  static Geometry geometry;

  if(!geometry)
  {
    const int vertexCount = 34; // Needs to be 4n plus 2 where n is a positive integer above 4

    // Create the circle geometry

    // Radius is bound to actor's dimensions so this should not be increased.
    // If a bigger circle is required then the actor size should be increased.
    const float   radius = 0.5f;
    const Vector2 center = Vector2::ZERO;

    // Create a buffer for vertex data
    Vector2 circleBuffer[vertexCount];
    int     idx = 0;

    // Center vertex for triangle fan
    circleBuffer[idx++] = center;

    // Outer vertices of the circle
    const int outerVertexCount = vertexCount - 1;

    for(int i = 0; i < outerVertexCount; ++i)
    {
      const float percent = (i / static_cast<float>(outerVertexCount - 1));
      const float rad     = percent * 2.0f * Math::PI;

      // Vertex position
      Vector2 outer;
      outer.x = center.x + radius * cos(rad);
      outer.y = center.y + radius * sin(rad);

      circleBuffer[idx++] = outer;
    }

    Property::Map circleVertexFormat;
    circleVertexFormat["aPositionCircle"] = Property::VECTOR2;
    VertexBuffer circleVertices           = VertexBuffer::New(circleVertexFormat);
    circleVertices.SetData(circleBuffer, vertexCount);

    // Create the Quad Geometry
    Vector2 quadBuffer[vertexCount];
    idx               = 0;
    quadBuffer[idx++] = center;

    const size_t vertsPerSide = (vertexCount - 2) / 4;
    Vector2      outer(0.5f, 0.0f);
    quadBuffer[idx++]        = outer;
    float incrementPerBuffer = 1.0f / static_cast<float>(vertsPerSide);

    for(size_t i = 0; i < vertsPerSide && outer.y < 0.5f; ++i)
    {
      outer.y += incrementPerBuffer;
      quadBuffer[idx++] = outer;
    }

    for(size_t i = 0; i < vertsPerSide && outer.x > -0.5f; ++i)
    {
      outer.x -= incrementPerBuffer;
      quadBuffer[idx++] = outer;
    }

    for(size_t i = 0; i < vertsPerSide && outer.y > -0.5f; ++i)
    {
      outer.y -= incrementPerBuffer;
      quadBuffer[idx++] = outer;
    }

    for(size_t i = 0; i < vertsPerSide && outer.x < 0.5f; ++i)
    {
      outer.x += incrementPerBuffer;
      quadBuffer[idx++] = outer;
    }

    for(size_t i = 0; i < vertsPerSide && outer.y < 0.0f; ++i)
    {
      outer.y += incrementPerBuffer;
      quadBuffer[idx++] = outer;
    }

    Property::Map vertexFormat;
    vertexFormat["aPositionQuad"] = Property::VECTOR2;
    VertexBuffer quadVertices2    = VertexBuffer::New(vertexFormat);
    quadVertices2.SetData(quadBuffer, vertexCount);

    // Create the geometry object itself
    geometry = Geometry::New();
    geometry.AddVertexBuffer(circleVertices);
    geometry.AddVertexBuffer(quadVertices2);
    geometry.SetType(Geometry::TRIANGLE_FAN);
  }

  return geometry;
}

} // unnamed namespace

const float CIRCLE_GEOMETRY = 0.0f;
const float QUAD_GEOMETRY   = 1.0f;

Dali::Toolkit::Control Create(const std::string& imagePath, Property::Index& propertyIndex)
{
  // Create a control which whose geometry will be morphed between a circle and a quad
  Control clippedImage = Control::New();
  clippedImage.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);

  // Create the required renderer and add to the clipped image control
  Renderer renderer = Renderer::New(CreateGeometry(), CreateShader());
  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
  clippedImage.AddRenderer(renderer);

  // Register the property on the clipped image control which will allow animations between a circle and a quad
  propertyIndex = clippedImage.RegisterProperty(DELTA_PROPERTY_NAME, 0.0f);

  // Add the actual image to the control
  Control image = ImageView::New(imagePath);
  image.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  image.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  image.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  clippedImage.Add(image);

  return clippedImage;
}

} // namespace ClippedImage
