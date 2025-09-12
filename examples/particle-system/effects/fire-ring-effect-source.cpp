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

#include "fire-ring-effect-source.h"
#include <random>

namespace Dali::ParticleEffect
{
static float LIFETIME = 5.0f;
FireSource::FireSource(ParticleEmitter& emitter)
: mEmitter(emitter)
{
  std::time_t result = std::time(nullptr);
  srand(result);
  mRadius = Vector2::ONE;
}

FireSource::FireSource(ParticleEmitter& emitter, Dali::Vector2 ringRadius)
: mEmitter(emitter)
{
  std::time_t result = std::time(nullptr);
  srand(result);
  mRadius = ringRadius;
}

void FireSource::Init()
{
  auto handle = mEmitter.GetHandle();
  if(handle)
  {
    mStreamBasePos = handle.GetParticleList().AddLocalStream<Vector3>(Vector3::ZERO);
  }
}

uint32_t FireSource::Update(ParticleList& particleList, uint32_t count)
{
  while(count--)
  {
    auto particle = particleList.NewParticle(LIFETIME * (float(std::rand() % 1000) / 1000.0f) + 1.0f);
    if(!particle)
    {
      return 0u;
    }

    auto& basePosition = particle.GetByIndex<Vector3>(mStreamBasePos);

    auto& position = particle.Get<Vector3>(ParticleStream::POSITION_STREAM_BIT);
    auto& color    = particle.Get<Vector4>(ParticleStream::COLOR_STREAM_BIT);
    auto& velocity = particle.Get<Vector3>(ParticleStream::VELOCITY_STREAM_BIT);
    auto& scale    = particle.Get<Vector3>(ParticleStream::SCALE_STREAM_BIT);
    UpdateParticle(position, basePosition, color, velocity, scale);
  }

  return 0;
}

void FireSource::UpdateParticle(Vector3& position, Vector3& basePosition, Vector4& color, Vector3& velocity, Vector3& scale)
{
  float posRadians = ((rand() % 360) * M_PI) / 180.0f;

  basePosition.x = position.x = mRadius.x * sin(posRadians);
  basePosition.y = position.y = mRadius.y * cos(posRadians);
  color                       = Dali::Color::WHITE; // white color when emitted

  // angle of motion
  float radians = ((rand() % 360) * M_PI) / 180.0f;
  float speed   = ((rand() % 5) + 5);
  velocity.x    = sin(radians) * speed;
  velocity.y    = cos(radians) * speed;

  // Random initial scale
  float currentScale = float(rand() % 32) + 32;
  scale              = Vector3(currentScale, currentScale, 1);
}

} // namespace Dali::ParticleEffect