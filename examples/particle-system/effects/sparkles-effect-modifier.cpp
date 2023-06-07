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

#include "sparkles-effect-modifier.h"
#include "sparkles-effect-source.h"

namespace Dali::ParticleEffect
{
static float LIFETIME = 3.0f;
SparklesModifier::SparklesModifier(ParticleEmitter& emitter)
: mEmitter(emitter)
{
}

bool SparklesModifier::IsMultiThreaded()
{
  return true;
}

void SparklesModifier::Update(ParticleList& particleList, uint32_t first, uint32_t count)
{
  // If no acive particles return
  if(!particleList.GetActiveParticleCount())
  {
    return;
  }

  mAngle = ((mAngle + 2) % 360);

  // Retrieve the Source and get the stream
  auto handle = mEmitter.GetHandle();
  if(!handle)
  {
    return;
  }
  if(!mStreamBasePos)
  {
    mStreamBasePos = static_cast<SparklesSource*>(&handle.GetSource().GetSourceCallback())->mStreamBasePos;
  }
  if(!mStreamBaseAngle)
  {
    mStreamBaseAngle = static_cast<SparklesSource*>(&handle.GetSource().GetSourceCallback())->mStreamBaseAngle;
  }

  // Missing stream, return!
  if(!mStreamBasePos)
  {
    return;
  }

  auto& activeParticles = particleList.GetActiveParticles();

  auto it = activeParticles.begin();
  std::advance(it, first);

  for(; count; ++it, count--)
  {
    // Acquire stream data
    auto&                  particle = *it;
    auto&                  position = particle.Get<Vector3>(ParticleStream::POSITION_STREAM_BIT);
    auto&                  velocity = particle.Get<Vector3>(ParticleStream::VELOCITY_STREAM_BIT);
    auto&                  color    = particle.Get<Vector4>(ParticleStream::COLOR_STREAM_BIT);
    auto&                  scale    = particle.Get<Vector3>(ParticleStream::SCALE_STREAM_BIT);

    // Get base positions
    [[maybe_unused]] auto& basePos = particle.GetByIndex<Vector3>(mStreamBasePos);

    auto angle = particle.GetByIndex<float>(mStreamBaseAngle);
    auto                   radians  = ((angle * M_PI)/180.f);
    float                  lifetime = particle.Get<float>(ParticleStream::LIFETIME_STREAM_BIT);
    position.y += velocity.y * sin(radians);
    position.x += velocity.x * cos(radians);

    velocity *= 0.990f;
    float normalizedTime = (lifetime / LIFETIME);
    color.a = normalizedTime;
    scale      = Vector3(64.0f*(normalizedTime * normalizedTime * normalizedTime * normalizedTime), 64.0f*(normalizedTime * normalizedTime * normalizedTime * normalizedTime), 1.0);
  }
}
}