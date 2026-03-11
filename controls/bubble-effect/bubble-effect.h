#ifndef DALI_DEMO_CONTROLS_INTERNAL_BUBBLE_EFFECT_H
#define DALI_DEMO_CONTROLS_INTERNAL_BUBBLE_EFFECT_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/rendering/shader.h>
#include <sstream>

// INTERNAL INCLUDES
#include <controls/shaders/bubble-effect-frag.h>
#include <controls/shaders/bubble-effect-vert.h>

namespace Dali::Demo::Internal
{
/**
 * Create the shader to be used by the renderer
 * @param[in] numberOfBubble How many groups of uniforms are used to control the bubble movement.
 * @return A handle to the newly created shader.
 */
inline Shader CreateBubbleShader(unsigned int numBubble)
{
  std::ostringstream vertexShaderStringStream;
  vertexShaderStringStream << "#define NUMBER_OF_BUBBLE " << numBubble << "\n"
                           << SHADER_BUBBLE_EFFECT_VERT;
  Shader shader = Shader::New(vertexShaderStringStream.str(), SHADER_BUBBLE_EFFECT_FRAG, static_cast<Shader::Hint::Value>(Shader::Hint::FILE_CACHE_SUPPORT | Shader::Hint::INTERNAL), "BUBBLE_EFFECT");

  return shader;
}

} // namespace Dali::Demo::Internal

#endif // DALI_DEMO_CONTROLS_INTERNAL_BUBBLE_EFFECT_H
