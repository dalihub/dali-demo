#pragma once

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

#include <dali/dali.h>
using Dali::Actor;
using Dali::Geometry;
using Dali::Renderer;
using Dali::Shader;
using Dali::TextureSet;

class BallRenderer
{
public:
  static Dali::Geometry   CreateBallGeometry();
  static Dali::Renderer   CreateRenderer(TextureSet textures);
  static Dali::TextureSet CreateTexture(std::string url);

  /**
   * The size can control whether this is a ball or a cuboid. Note, textures
   * will be stretched on non-square sides.
   */
  static Dali::Actor CreateActor(Dali::Vector3 size, Dali::Vector4 color);
  static Dali::Actor CreateActor(Dali::Vector3 size, std::string url);

  static Geometry   gBallGeometry;
  static Shader     gBallShader;
  static TextureSet gBallTextureSet;
};
