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

#include "image-effect-source.h"
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <random>

namespace Dali::ParticleEffect
{
namespace
{
Vector4 GetColorAt(uint32_t x, uint32_t y, Devel::PixelBuffer& buffer)
{
  if(buffer.GetPixelFormat() == Pixel::Format::RGBA8888)
  {
    const auto ptr   = reinterpret_cast<uint32_t*>(buffer.GetBuffer());
    auto       value = *(ptr + x + (y * buffer.GetHeight()));
    auto       rgba  = reinterpret_cast<uint8_t*>(&value);
    return Vector4(float(rgba[0]) / 255.0f, float(rgba[1]) / 255.0f, float(rgba[2]) / 255.0f, 1.0f);
  }
  else
  {
    auto       rgba  = reinterpret_cast<uint8_t*>(buffer.GetBuffer() + (y*buffer.GetWidth()*3) + (x*3));
    return Vector4(float(rgba[0]) / 255.0f, float(rgba[1]) / 255.0f, float(rgba[2]) / 255.0f, 1.0f);
  }
}
}

static float LIFETIME = 50000.0f; // we need infinite lifetime?
ImageExplodeEffectSource::ImageExplodeEffectSource(ParticleEmitter& emitter)
: mEmitter(emitter)
{
}

ImageExplodeEffectSource::ImageExplodeEffectSource(ParticleEmitter& emitter, const std::string& imageFileName, uint32_t width, uint32_t height) :
mEmitter(emitter)
{

  // Create texture
  std::string filePath(DEMO_IMAGE_DIR);
  filePath += imageFileName;
  ImageDimensions dimensions(width, height);
  // Pixel buffer will be used as a source of pixels (populating colors of particles based on image pixels)
  Devel::PixelBuffer pixelBuffer = Dali::LoadImageFromFile(filePath, dimensions, FittingMode::SHRINK_TO_FIT, SamplingMode::DEFAULT, false);
  mImageWidth = pixelBuffer.GetWidth();
  mImageHeight = pixelBuffer.GetHeight();
  mPixelBuffer = pixelBuffer;
}

void ImageExplodeEffectSource::Init()
{
  mStreamBasePos = mEmitter.GetParticleList().AddLocalStream<Vector3>(Vector3::ZERO);
}

uint32_t ImageExplodeEffectSource::Update(ParticleList& particleList, uint32_t count)
{
  if(!mShouldEmit)
  {
    return 0;
  }

  if(mPixelBuffer.GetPixelFormat() != Dali::Pixel::RGBA8888 &&
     mPixelBuffer.GetPixelFormat() != Dali::Pixel::RGB888)
  {
    return 0;
  }

  auto i = 0u;
  float particleScale = 4.0f;
  float pixelSize = 2.0f;

  auto halfWidth = (float(mImageWidth)*0.5f) * particleScale;
  auto halfHeight = (float(mImageHeight)*0.5f) * particleScale;

  for(auto y = 0u ; y < mImageHeight; ++y)
  {
    for(auto x = 0u; x < mImageWidth; ++x)
    {
      if(i < particleList.GetCapacity())
      {
        // Ignore count, populating all pixels instantly (emitter must account for all the points)
        auto particle = particleList.NewParticle(LIFETIME);

        auto& basePosition = particle.GetByIndex<Vector3>(mStreamBasePos);
        auto& position     = particle.Get<Vector3>(ParticleStream::POSITION_STREAM_BIT);
        auto& color        = particle.Get<Vector4>(ParticleStream::COLOR_STREAM_BIT);
        auto& velocity     = particle.Get<Vector3>(ParticleStream::VELOCITY_STREAM_BIT);
        auto& scale        = particle.Get<Vector3>(ParticleStream::SCALE_STREAM_BIT);
        color              = GetColorAt(x, y, mPixelBuffer);
        // Set basePosition
        position = basePosition = Vector3(float(x) * particleScale - halfWidth, float(y)* particleScale - halfHeight, 0);
        scale                   = Vector3(pixelSize, pixelSize, 1);
        velocity                = Vector3::ZERO;
      }
      ++i;
    }
  }

  mShouldEmit = false;
  return mImageWidth * mImageHeight;
}


} // namespace Dali::ParticleEffect