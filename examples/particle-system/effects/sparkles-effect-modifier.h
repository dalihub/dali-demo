#ifndef DALI_PARTICLES_SPARKLES_EFFECT_MODIFIER_H
#define DALI_PARTICLES_SPARKLES_EFFECT_MODIFIER_H

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
 *
 */


#include <dali-toolkit/public-api/particle-system/particle-emitter.h>
#include <dali-toolkit/public-api/particle-system/particle-source.h>
#include <dali-toolkit/public-api/particle-system/particle-modifier.h>
#include <dali-toolkit/public-api/particle-system/particle-list.h>
#include <dali-toolkit/public-api/particle-system/particle.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <ctime>

namespace Dali::ParticleEffect
{
using namespace Dali::Toolkit::ParticleSystem;

class SparklesModifier : public ParticleModifierInterface
{
public:

  explicit SparklesModifier(ParticleEmitter& emitter);

  bool IsMultiThreaded() override;

  void Update(ParticleList& particleList, uint32_t first, uint32_t count) override;

  ParticleEmitter mEmitter;
  uint32_t mStreamBasePos{0u};
  uint32_t mStreamBaseAngle{0u};
  uint32_t mAngle{0u};
};



}

#endif // DALI_PARTICLES_SPARKLES_EFFECT_MODIFIER_H
