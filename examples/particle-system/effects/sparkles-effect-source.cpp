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

#include "sparkles-effect-source.h"

namespace Dali::ParticleEffect
{
static float LIFETIME = 3.0f;
SparklesSource::SparklesSource(ParticleEmitter& emitter)
: mEmitter(emitter)
{
  std::time_t result = std::time(nullptr);
  srand(result);
  mRadius = Vector2::ONE;
}

SparklesSource::SparklesSource(ParticleEmitter& emitter, Dali::Vector2 ringRadius) :
mEmitter(emitter)
{
  std::time_t result = std::time(nullptr);
  srand(result);
  mRadius = ringRadius;

}

void SparklesSource::Init()
{
  mStreamBasePos = mEmitter.GetParticleList().AddLocalStream<Vector3>(Vector3::ZERO);
  mStreamBaseAngle = mEmitter.GetParticleList().AddLocalStream<float>(0.0f);
}

uint32_t SparklesSource::Update(ParticleList& particleList, uint32_t count)
{
  while(count--)
  {
    auto particle = particleList.NewParticle(LIFETIME);
    if(!particle)
    {
      return 0u;
    }

    auto& basePosition = particle.GetByIndex<Vector3>(mStreamBasePos);
    auto& angle = particle.GetByIndex<float>(mStreamBaseAngle);
    auto& position     = particle.Get<Vector3>(ParticleStream::POSITION_STREAM_BIT);
    auto& color        = particle.Get<Vector4>(ParticleStream::COLOR_STREAM_BIT);
    auto& velocity     = particle.Get<Vector3>(ParticleStream::VELOCITY_STREAM_BIT);
    auto& scale        = particle.Get<Vector3>(ParticleStream::SCALE_STREAM_BIT);


    UpdateParticle(position, basePosition, color, velocity, scale, angle);
  }

  return 0;
}

void SparklesSource::UpdateParticle(Vector3& position, Vector3& basePosition, Vector4& color, Vector3& velocity, Vector3& scale, float& angle)
{
  static uint32_t a = 0.0f;
  float posRadians   = ((rand() % 360) * M_PI) / 180.0f;

  basePosition.x = position.x = mRadius.x * sin(posRadians);
  basePosition.y = position.y = mRadius.y * cos(posRadians);
  color                       = Dali::Color::WHITE;

  angle = float(a);
  a = ((a+5)%360);
  float rad   = ((rand() % 360) * M_PI) / 180.0f;
  float speed = ((rand() % 5) + 5);
  velocity.x       = sin(rad) * speed;
  velocity.y       = cos(rad) * speed;

  // Random initial scale
  float initialScale = float(rand() % 32) + 32;
  scale   = Vector3(initialScale, initialScale, 1);
}

} // namespace Dali::ParticleEffect