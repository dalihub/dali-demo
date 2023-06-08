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

#include "image-effect-modifier.h"
#include "image-effect-source.h"
namespace Dali::ParticleEffect
{

#define RAD(x) (float(x)*M_PI/180.0f)

ImageExplodeEffectModifier::ImageExplodeEffectModifier(ParticleEmitter& emitter)
: mEmitter(emitter)
{

}

bool ImageExplodeEffectModifier::IsMultiThreaded()
{
  return false;
}

void ImageExplodeEffectModifier::Update(ParticleList& particleList, uint32_t first, uint32_t count)
{
  // If no acive particles return
  if(!particleList.GetActiveParticleCount())
  {
    return;
  }

  // Retrieve the Source and get the stream
  if(!mStreamBasePos)
  {
    mStreamBasePos = static_cast<ImageExplodeEffectSource*>(&mEmitter.GetSource().GetSourceCallback())->mStreamBasePos;
  }

  // Missing stream, return!
  if(!mStreamBasePos)
  {
    return;
  }

  auto& activeParticles = particleList.GetActiveParticles();

  auto it = activeParticles.begin();
  std::advance(it, first);

  mAngle += 5.0f;

  for(; count; ++it, count--)
  {
    // Acquire stream data
    auto&                  particle = *it;
    auto&                  position = particle.Get<Vector3>(ParticleStream::POSITION_STREAM_BIT);
    auto&                  color    = particle.Get<Vector4>(ParticleStream::COLOR_STREAM_BIT);

    // Get base positions
    auto& basePos = particle.GetByIndex<Vector3>(mStreamBasePos);
    position.z = 200.f * sin(RAD(mAngle+basePos.x));
    color.a = position.z < 0.0f ? 1.0f : 1.0f - position.z/500.0f;
    position.z = 500 + position.z;
  }
}
}