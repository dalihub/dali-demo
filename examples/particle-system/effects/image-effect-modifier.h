#ifndef DALI_IMAGE_EFFECT_MODIFIER_H
#define DALI_IMAGE_EFFECT_MODIFIER_H

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
#include <dali/public-api/object/weak-handle.h>
#include <ctime>

namespace Dali::ParticleEffect
{
using namespace Dali::Toolkit::ParticleSystem;

class ImageExplodeEffectModifier : public ParticleModifierInterface
{
public:
  explicit ImageExplodeEffectModifier(ParticleEmitter& emitter);

  bool IsMultiThreaded() override;

  void Update(ParticleList& particleList, uint32_t first, uint32_t count) override;

  WeakHandle<ParticleEmitter> mEmitter;
  uint32_t                    mStreamBasePos{0u};
  float                       mAngle{0.0f};
};

} //namespace Dali::ParticleEffect

#endif // DALI_IMAGE_EFFECT_MODIFIER_H
