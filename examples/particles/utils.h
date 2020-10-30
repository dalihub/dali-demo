#ifndef PARTICLES_UTILS_H_
#define PARTICLES_UTILS_H_
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

#include "dali/public-api/actors/actor.h"
#include "dali/public-api/actors/actor.h"
#include "dali/public-api/rendering/geometry.h"
#include "dali/public-api/rendering/renderer.h"
#include "dali/public-api/rendering/shader.h"
#include "dali/public-api/rendering/texture.h"
#include "dali/public-api/math/vector3.h"
#include <cmath>

//
// Maths
//
inline
float FastFloor(float x)
{
  return static_cast<int>(x) - static_cast<int>(x < 0);
}

///@brief Converts RGB values (in the 0..1 range) to HSL, where hue is in degrees,
/// in the 0..360 range, and saturation and lightness are in the 0..1 range.
Dali::Vector3 ToHueSaturationLightness(Dali::Vector3 rgb);

///@brief Converts HSL values, where hue is in degrees, in the 0..360 range, and
/// saturation and lightness are in 0..1  to RGB (in the 0..1 range)
Dali::Vector3 FromHueSaturationLightness(Dali::Vector3 hsl);

//
// Dali entities
//
///@brief Creates geometry for a unit cube wireframe (i.e. vertex positions between
/// -.5 and .5)..
Dali::Geometry CreateCuboidWireframeGeometry();

enum RendererOptions
{
  OPTION_NONE = 0x0,
  OPTION_BLEND = 0x01,
  OPTION_DEPTH_TEST = 0x02,
  OPTION_DEPTH_WRITE = 0x04
};

///@brief Creates a renderer with the given @a textures set, @a geometry, @a shader
/// and @a options from above.
///@note Back face culling is on.
///@note If textures is not a valid handle, an empty texture set will be created.
Dali::Renderer CreateRenderer(Dali::TextureSet textures, Dali::Geometry geometry,
  Dali::Shader shader, uint32_t options = OPTION_NONE);

///@brief Sets @a actor's anchor point and parent origin to center.
void CenterActor(Dali::Actor actor);

///@brief Creates an empty and centered actor.
Dali::Actor CreateActor();

///@brief Creates a copy of @a original, sharing the same geometry and shader and
/// copying each properties.
///@note Breaks if @a original has any custom properties. TODO: fix.
Dali::Renderer CloneRenderer(Dali::Renderer original);

///@brief Creates a copy of @a original, cloning each renderer, and a select set
/// of properties: parent origin, anchor point, size, position, orientation, scale,
/// visible, color and name.
///@note Does not copy resize policy related properties, as setting those, even if
/// default, will break the ability to specify a size for the actor in Z.
Dali::Actor CloneActor(Dali::Actor original);

#endif //PARTICLES_UTILS_H_
