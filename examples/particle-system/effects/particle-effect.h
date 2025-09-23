#ifndef DALI_PARTICLE_EFFECT_H
#define DALI_PARTICLE_EFFECT_H

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
#include <memory>

namespace Dali::ParticleEffect
{

using EffectIndex = uint32_t;

enum class EffectType
{
  FIRE_RING,
  SPARKLES,
  IMAGE_EXPLOSION
};

struct ParticleEffectParams
{
  uint32_t    emissionRate;
  uint32_t    particleCount;
  uint32_t    initialParticleCount;
  Vector2     sourceSize;
  std::string strTexture;
  std::string strImageSourceName;
};

/**
 * Simple manager to spawn and control partcile emitters
 */
class ParticleEffect
{
public:
  ParticleEffect();

  ~ParticleEffect();

  Dali::Toolkit::ParticleSystem::ParticleEmitter CreateEffectEmitter(EffectType effectType, Actor parentActor, const ParticleEffectParams& params);
};
} //namespace Dali::ParticleEffect

#endif // DALI_PARTICLE_MANAGER_H