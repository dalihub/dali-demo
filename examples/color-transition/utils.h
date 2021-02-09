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
#ifndef DALI_DEMO_COLOR_TRANSITION_UTILS_H_
#define DALI_DEMO_COLOR_TRANSITION_UTILS_H_

#include "dali/public-api/actors/actor.h"
#include "dali/public-api/rendering/geometry.h"
#include "dali/public-api/rendering/renderer.h"
#include "dali/public-api/rendering/texture.h"

#define ACTOR_DECL(x)          \
  auto x = Dali::Actor::New(); \
  x.SetProperty(Actor::Property::NAME, #x);

/**
 * @brief Creates a unit quad centered on the origin (i.e. vertex positions
 * between -.5 and .5).
 */
Dali::Geometry CreateQuadGeometry();

struct RendererOptions
{
  enum
  {
    NONE        = 0x0,
    BLEND       = 0x01,
    DEPTH_TEST  = 0x02,
    DEPTH_WRITE = 0x04,
    CULL_BACK   = 0x08,
    CULL_FRONT  = 0x10,
  };
};

///@brief Creates a renderer with the given @a textures set, @a geometry, @a shader
/// and @a options from above.
///@note Back face culling is on.
///@note If textures is not a valid handle, an empty texture set will be created.
Dali::Renderer CreateRenderer(
  Dali::TextureSet textures,
  Dali::Geometry   geometry,
  Dali::Shader     shader,
  uint32_t         options = RendererOptions::NONE);

///@brief Sets @a actor's anchor point and parent origin to center.
void CenterActor(Dali::Actor actor);

#endif //DALI_DEMO_COLOR_TRANSITION_UTILS_H_
