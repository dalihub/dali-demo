#ifndef UTILS_H
#define UTILS_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/dali-toolkit.h>
#include <string>
#include <vector>

namespace Utils
{

enum class Direction
{
  HORIZONTAL,
  VERTICAL
};

std::string CreateEffectPassTexture( std::string inputUrl, std::vector<uint32_t>& fragShader,
                                     Dali::Vector2 targetSize, Direction direction );
Dali::Actor CreateEffectPassTexture( std::string inputUrl, std::vector<uint32_t>& fragShader,
                                     Dali::Vector2 targetSize, Direction direction, Dali::FrameBuffer fb );
Dali::Actor CreatePreview( const std::string& url, Dali::Vector2 targetSize, Dali::Vector3 parentOrigin,
                           Dali::Vector3 labelOrigin, const std::string& label, bool copy );
Dali::Actor CreatePreviewRenderTask( std::string url, Dali::Vector2 targetSize );
Dali::Texture CreateRenderTask(Dali::Actor inputActor, Dali::Vector2 targetSize);
void CreateRenderTask(Dali::Actor inputActor, Dali::Vector2 targetSize, Dali::FrameBuffer fbo);

void SetShaderConstants(Dali::Actor actor, Direction direction, uint32_t numSamples,
                        float downsampledWidth, float downsampledHeight);


}

#endif // UTILS_H
