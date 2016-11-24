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

#include <stdio.h>

#include "game-texture.h"
#include "game-utils.h"

#include <dali-toolkit/public-api/image-loader/sync-image-loader.h>

GameTexture::GameTexture()
: mUniqueId( 0 ),
  mIsReady( false )
{
}

GameTexture::~GameTexture()
{
}

GameTexture::GameTexture( const char* filename )
: mUniqueId( 0 ),
  mIsReady( false )
{
  Load( filename );
}

bool GameTexture::Load( const char* filename )
{
  Dali::PixelData pixelData = Dali::Toolkit::SyncImageLoader::Load( filename );

  if( !pixelData )
  {
    return false;
  }

  Dali::Texture texture = Dali::Texture::New( Dali::TextureType::TEXTURE_2D,
                                  pixelData.GetPixelFormat(),
                                  pixelData.GetWidth(),
                                  pixelData.GetHeight() );
  texture.Upload( pixelData );
  texture.GenerateMipmaps();
  Dali::TextureSet textureSet = Dali::TextureSet::New();
  textureSet.SetTexture( 0, texture );
  Dali::Sampler sampler = Dali::Sampler::New();
  sampler.SetWrapMode( Dali::WrapMode::REPEAT, Dali::WrapMode::REPEAT, Dali::WrapMode::REPEAT );
  sampler.SetFilterMode( Dali::FilterMode::LINEAR_MIPMAP_LINEAR, Dali::FilterMode::LINEAR );
  textureSet.SetSampler( 0, sampler );

  mTexture = texture;
  mSampler = sampler;
  mTextureSet = textureSet;

  mUniqueId = GameUtils::HashString( filename );

  mIsReady = true;

  return true;
}

Dali::TextureSet& GameTexture::GetTextureSet()
{
  return mTextureSet;
}

uint32_t GameTexture::GetUniqueId()
{
  return mUniqueId;
}

bool GameTexture::IsReady()
{
  return mIsReady;
}
