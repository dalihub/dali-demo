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

#include "fire-ring-effect-modifier.h"
#include "fire-ring-effect-source.h"

namespace Dali::ParticleEffect
{
FireModifier::FireModifier(ParticleEmitter& emitter)
: mEmitter(emitter)
{
  // initialize gradient with flame colors
  mFireGradient.PushColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f - 1.0f);
  mFireGradient.PushColor(Vector4(0.975, 0.955, 0.476, 1.0f), 1.0f - 0.947f);
  mFireGradient.PushColor(Vector4(0.999, 0.550, 0.194, 1.0f), 1.0f - 0.800f);
  mFireGradient.PushColor(Vector4(0.861, 0.277, 0.094, 1.0f), 1.0f - 0.670f);
  mFireGradient.PushColor(Vector4(0.367, 0.0, 0.0, 1.0f), 1.0f - 0.456f);
  mFireGradient.PushColor(Vector4(0.3, 0.3, 0.3, 1.0f), 1.0f - 0.400f);
  mFireGradient.PushColor(Vector4(0.3, 0.2, 0.2, 1.0f), 1.0f - 0.200f);
  mFireGradient.PushColor(Vector4(0.2, 0.1, 0.1, 1.0f), 1.0f - 0.150f);
  mFireGradient.PushColor(Vector4(0.1, 0.0, 0.0, 1.0f), 1.0f - 0.100f);
  mFireGradient.PushColor(Vector4(0.0, 0.0, 0.0, 0.5f), 1.0f - 0.050f);
  mFireGradient.PushColor(Vector4(0.0, 0.0, 0.0, 0.2f), 1.0f);
}

bool FireModifier::IsMultiThreaded()
{
  return false;
}

void FireModifier::Update(ParticleList& particleList, uint32_t first, uint32_t count)
{
  // If no acive particles return
  if(!particleList.GetActiveParticleCount())
  {
    return;
  }

  mAngle = ((mAngle + 2) % 360);

  // Retrieve the Source and get the stream
  if(!mStreamBasePos)
  {
    auto handle = mEmitter.GetHandle();
    if(!handle)
    {
      return;
    }
    mStreamBasePos = static_cast<FireSource*>(&handle.GetSource().GetSourceCallback())->mStreamBasePos;
  }

  // Missing stream, return!
  if(!mStreamBasePos)
  {
    return;
  }

  auto& activeParticles = particleList.GetActiveParticles();

  auto it = activeParticles.begin();
  std::advance(it, first);

  int i = 0;
  for(; count; ++it, count--)
  {
    i += 1;

    // Acquire stream data
    auto& particle     = *it;
    auto& position     = particle.Get<Vector3>(ParticleStream::POSITION_STREAM_BIT);
    auto& velocity     = particle.Get<Vector3>(ParticleStream::VELOCITY_STREAM_BIT);
    auto& color        = particle.Get<Vector4>(ParticleStream::COLOR_STREAM_BIT);
    auto& baseLifetime = particle.Get<float>(ParticleStream::LIFETIME_BASE_STREAM_BIT);
    auto& scale        = particle.Get<Vector3>(ParticleStream::SCALE_STREAM_BIT);

    // Get base positions
    auto& basePos = particle.GetByIndex<Vector3>(mStreamBasePos);

    float lifetime = particle.Get<float>(ParticleStream::LIFETIME_STREAM_BIT);
    position.y += -fabs(velocity.y);
    position.x = basePos.x + 5.0f * sin((((mAngle + i) % 360) * M_PI) / 180.f);

    velocity *= 0.990f;
    auto  newColor       = mFireGradient.GetColorAt((baseLifetime - lifetime) / baseLifetime);
    float normalizedTime = (lifetime / baseLifetime);
    newColor.a           = normalizedTime * normalizedTime;

    scale = Vector3(64.0f * (normalizedTime * normalizedTime * normalizedTime * normalizedTime), 64.0f * (normalizedTime * normalizedTime * normalizedTime * normalizedTime), 1.0);

    color = newColor;
  }
}
} //namespace Dali::ParticleEffect