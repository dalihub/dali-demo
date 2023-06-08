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

#include "particle-effect.h"
#include "fire-ring-effect-source.h"
#include "sparkles-effect-source.h"
#include "image-effect-source.h"
#include "fire-ring-effect-modifier.h"
#include "sparkles-effect-modifier.h"
#include "image-effect-modifier.h"

#include <dali-toolkit/public-api/particle-system/particle-domain.h>
#include <dali-toolkit/public-api/particle-system/particle-renderer.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <functional>

#ifndef DEMO_IMAGE_DIR
#define DEMO_IMAGE_DIR ""
#endif

namespace Dali::ParticleEffect
{
using ParticleEmitter = Dali::Toolkit::ParticleSystem::ParticleEmitter;
using ParticleSource = Dali::Toolkit::ParticleSystem::ParticleSource;
using ParticleModifier = Dali::Toolkit::ParticleSystem::ParticleModifier;

struct FunctorReturn
{
  ParticleEmitter emitter;
  ParticleSource source;
  ParticleModifier modifier;
};

static std::vector<FunctorReturn(*)(const ParticleEffectParams&)> gEffectInitializers =
{
  [](const ParticleEffectParams& params){
      ParticleEmitter emitter = ParticleEmitter::New();
      return FunctorReturn{emitter, ParticleSource::New<FireSource>(emitter, params.sourceSize), ParticleModifier::New<FireModifier>(emitter) };
    },
  [](const ParticleEffectParams& params){
      ParticleEmitter emitter = ParticleEmitter::New();
      return FunctorReturn{emitter, ParticleSource::New<SparklesSource>(emitter), ParticleModifier::New<SparklesModifier>(emitter) };
    },
  [](const ParticleEffectParams& params){
      ParticleEmitter emitter = ParticleEmitter::New();
      return FunctorReturn{emitter, ParticleSource::New<ImageExplodeEffectSource>(emitter,
                                                                                  params.strImageSourceName,
                                                                                  uint32_t(params.sourceSize.width),
                                                                                  uint32_t(params.sourceSize.height)
                                                                                  ), ParticleModifier::New<ImageExplodeEffectModifier>(emitter) };
    },
};

ParticleEffect::ParticleEffect() = default;

ParticleEffect::~ParticleEffect() = default;

Dali::Toolkit::ParticleSystem::ParticleEmitter ParticleEffect::CreateEffectEmitter( EffectType effectType, Actor parentActor, const ParticleEffectParams& params )
{
  auto retval = gEffectInitializers[int(effectType)](params);
  auto emitter = retval.emitter;

  ParticleRenderer renderer = ParticleRenderer::New();

  if(!params.strTexture.empty())
  {
    // Create texture
    std::string filename(DEMO_IMAGE_DIR);
    filename += params.strTexture;
    Dali::PixelData pixelData = Dali::Toolkit::SyncImageLoader::Load(filename);
    auto texture = Texture::New(Dali::TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight());
    texture.Upload(pixelData);
    renderer.SetTexture(texture);
  }

  emitter.AttachTo(std::move(parentActor));
  emitter.SetEmissionRate( params.emissionRate ); // 20 particles emitted per second
  emitter.SetParticleCount( params.particleCount );
  emitter.SetSource( retval.source );
  emitter.SetDomain( ParticleDomain::New() );
  emitter.AddModifier(retval.modifier);
  emitter.SetRenderer( renderer );
  renderer.SetBlendingMode(Dali::Toolkit::ParticleSystem::BlendingMode::SCREEN);
  emitter.SetInitialParticleCount( params.initialParticleCount );

  return emitter;
}


}

