#ifndef DALI_IMAGE_EFFECT_SOURCE_H
#define DALI_IMAGE_EFFECT_SOURCE_H

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
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/public-api/object/weak-handle.h>
#include <ctime>

namespace Dali::ParticleEffect
{
using namespace Dali::Toolkit::ParticleSystem;

/**
 * Image source will use 2D image to populate points for the emitter
 * This particular implementation populates points only once.
 */
class ImageExplodeEffectSource : public Toolkit::ParticleSystem::ParticleSourceInterface
{
public:

  explicit ImageExplodeEffectSource(ParticleEmitter& emitter);

  explicit ImageExplodeEffectSource(ParticleEmitter& emitter, const std::string& imageFileName, uint32_t width, uint32_t height);

  uint32_t Update(ParticleList& particleList, uint32_t count) override;

  void Init() override;

  WeakHandle<ParticleEmitter> mEmitter;

  uint32_t mImageWidth{0u};
  uint32_t mImageHeight{0u};

  uint32_t mStreamBasePos{0u};

  Devel::PixelBuffer mPixelBuffer;

  bool mShouldEmit {true};
};

}
#endif // DALI_IMAGE_EFFECT_SOURCE_H
