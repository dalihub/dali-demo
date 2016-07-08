#ifndef __DALI_DEMO_UTILITY_H__
#define __DALI_DEMO_UTILITY_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <dali/dali.h>
#include <dali/devel-api/images/atlas.h>
#include <dali/devel-api/rendering/texture.h>
#include <dali/devel-api/adaptor-framework/bitmap-loader.h>

namespace DemoHelper
{

Dali::PixelData LoadPixelData( const char* imagePath,
                               Dali::ImageDimensions size,
                               Dali::FittingMode::Type fittingMode,
                               Dali::SamplingMode::Type samplingMode )
{
  Dali::BitmapLoader loader = Dali::BitmapLoader::New( imagePath, size, fittingMode, samplingMode );
  loader.Load();
  return loader.GetPixelData();
}


Dali::Atlas LoadImage( const char* imagePath,
                       Dali::ImageDimensions size = Dali::ImageDimensions(),
                       Dali::FittingMode::Type fittingMode = Dali::FittingMode::DEFAULT,
                       Dali::SamplingMode::Type samplingMode = Dali::SamplingMode::DEFAULT )
{
  Dali::PixelData pixelData = LoadPixelData(imagePath, size, fittingMode, samplingMode);
  Dali::Atlas image  =Dali:: Atlas::New( pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetPixelFormat() );
  image.Upload( pixelData, 0u, 0u );

  return image;
}

Dali::Texture LoadTexture( const char* imagePath,
                           Dali::ImageDimensions size = Dali::ImageDimensions(),
                           Dali::FittingMode::Type fittingMode = Dali::FittingMode::DEFAULT,
                           Dali::SamplingMode::Type samplingMode = Dali::SamplingMode::DEFAULT )
{
  Dali::PixelData pixelData = LoadPixelData(imagePath, size, fittingMode, samplingMode);
  Dali::Texture texture  = Dali::Texture::New( Dali::TextureType::TEXTURE_2D,
                                               pixelData.GetPixelFormat(),
                                               pixelData.GetWidth(),
                                               pixelData.GetHeight() );
  texture.Upload( pixelData );

  return texture;
}

/**
 * @brief Load an bitmap resource.
 *
 * If it is required to scaled-down to no more than the stage dimensions,
 * uses image scaling mode FittingMode::SCALE_TO_FILL to resize the image at
 * load time to cover the entire stage with pixels with no borders,
 * and filter mode BOX_THEN_LINEAR to sample the image with
 * maximum quality.
 */

Dali::Atlas LoadStageFillingImage( const char* imagePath )
{
  Dali::Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
  return LoadImage( imagePath, Dali::ImageDimensions( stageSize.x, stageSize.y ), Dali::FittingMode::SCALE_TO_FILL, Dali::SamplingMode::BOX_THEN_LINEAR );
}

Dali::Texture LoadStageFillingTexture( const char* imagePath )
{
  Dali::Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
  return LoadTexture( imagePath, Dali::ImageDimensions( stageSize.x, stageSize.y ), Dali::FittingMode::SCALE_TO_FILL, Dali::SamplingMode::BOX_THEN_LINEAR );
}

} // DemoHelper

#endif // __DALI_DEMO_HELPER_VIEW_H__
