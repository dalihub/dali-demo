#ifndef DALI_DEMO_UTILITY_H
#define DALI_DEMO_UTILITY_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>

namespace DemoHelper
{

Dali::Texture LoadTexture( const char* imagePath,
                           Dali::ImageDimensions size = Dali::ImageDimensions(),
                           Dali::FittingMode::Type fittingMode = Dali::FittingMode::DEFAULT,
                           Dali::SamplingMode::Type samplingMode = Dali::SamplingMode::DEFAULT,
                           bool orientationCorrection = true )
{
  Dali::Devel::PixelBuffer pixelBuffer = LoadImageFromFile(imagePath, size, fittingMode, samplingMode, orientationCorrection );
  Dali::Texture texture  = Dali::Texture::New( Dali::TextureType::TEXTURE_2D,
                                               pixelBuffer.GetPixelFormat(),
                                               pixelBuffer.GetWidth(),
                                               pixelBuffer.GetHeight() );

  Dali::PixelData pixelData = Dali::Devel::PixelBuffer::Convert(pixelBuffer);
  texture.Upload( pixelData );

  return texture;
}

Dali::Texture LoadStageFillingTexture( const char* imagePath )
{
  Dali::Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
  return LoadTexture( imagePath, Dali::ImageDimensions( stageSize.x, stageSize.y ), Dali::FittingMode::SCALE_TO_FILL, Dali::SamplingMode::BOX_THEN_LINEAR );
}

Dali::Geometry CreateTexturedQuad()
{
  struct Vertex
  {
    Dali::Vector2 position;
    Dali::Vector2 texCoord;
  };

  static const Vertex data[] = {{ Dali::Vector2( -0.5f, -0.5f ), Dali::Vector2( 0.0f, 0.0f ) },
                                { Dali::Vector2(  0.5f, -0.5f ), Dali::Vector2( 1.0f, 0.0f ) },
                                { Dali::Vector2( -0.5f,  0.5f ), Dali::Vector2( 0.0f, 1.0f ) },
                                { Dali::Vector2(  0.5f,  0.5f ), Dali::Vector2( 1.0f, 1.0f ) }};

  Dali::PropertyBuffer vertexBuffer;
  Dali::Property::Map vertexFormat;
  vertexFormat["aPosition"] = Dali::Property::VECTOR2;
  vertexFormat["aTexCoord"] = Dali::Property::VECTOR2;

  //Create a vertex buffer for vertex positions and texture coordinates
  vertexBuffer = Dali::PropertyBuffer::New( vertexFormat );
  vertexBuffer.SetData( data, 4u );

  //Create the geometry
  Dali::Geometry geometry = Dali::Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer );
  geometry.SetType(Dali::Geometry::TRIANGLE_STRIP );

  return geometry;
}
} // DemoHelper

#endif // DALI_DEMO_UTILITY_H
