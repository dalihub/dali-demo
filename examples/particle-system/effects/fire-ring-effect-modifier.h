#ifndef DALI_PROJECT_FIRE_RING_EFFECT_MODIFIER_H
#define DALI_PROJECT_FIRE_RING_EFFECT_MODIFIER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/public-api/particle-system/particle-emitter.h>
#include <dali-toolkit/public-api/particle-system/particle-list.h>
#include <dali-toolkit/public-api/particle-system/particle-modifier.h>
#include <dali-toolkit/public-api/particle-system/particle-source.h>
#include <dali-toolkit/public-api/particle-system/particle.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/weak-handle.h>
#include <ctime>

namespace Dali::ParticleEffect
{
using namespace Dali::Toolkit::ParticleSystem;

class FireModifier : public ParticleModifierInterface
{
public:
  struct ColorGradient
  {
    std::vector<Vector4> colors;
    std::vector<float>   position;

    void PushColor(const Vector4& color, float pos)
    {
      colors.emplace_back(color);
      position.emplace_back(pos);
    }

    Vector4 GetColorAt(float pos)
    {
      if(pos >= 1.0f)
      {
        return colors.back();
      }
      else if(pos <= 0.0f)
      {
        return colors[0];
      }
      for(auto i = 0u; i < position.size() - 1; ++i)
      {
        if(pos >= position[i] && pos < position[i + 1])
        {
          auto colorDiff = colors[i + 1] - colors[i];
          return colors[i] + (colorDiff * ((pos - position[i]) / (position[i + 1] - position[i])));
        }
      }
      return colors[0];
    }
  };

  explicit FireModifier(ParticleEmitter& emitter);

  bool IsMultiThreaded() override;

  void Update(ParticleList& particleList, uint32_t first, uint32_t count) override;

  ColorGradient                     mFireGradient;
  Dali::WeakHandle<ParticleEmitter> mEmitter;
  uint32_t                          mStreamBasePos{0u};
  uint32_t                          mAngle{0u};
};

} //namespace Dali::ParticleEffect

#endif // DALI_PROJECT_FIRE_RING_EFFECT_MODIFIER_H
