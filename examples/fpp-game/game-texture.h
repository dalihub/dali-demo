#ifndef GAME_TEXTURE_H
#define GAME_TEXTURE_H

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

#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/rendering/sampler.h>

#include <inttypes.h>

class GameTexture
{
public:

  /**
   * Creates an instance of the GameTexture
   */
  GameTexture();

  /**
   * Creates an instance of the GameTexture with given filename
   */
  GameTexture( const char* filename );

  /**
   * Destroys an instance of the GameTexture
   */
  ~GameTexture();

  /**
   * @brief Loads texture from file
   * @return Returns true if success
   */
  bool Load( const char* filename );

  /**
   * Checks status of texture, returns false if failed to load
   * @return true if texture has been loaded, false otherwise
   */
  bool IsReady();

  /**
   * @brief Returns DALi texture set associated with the GameTexture
   * @return Initialised TextureSet
   */
  Dali::TextureSet& GetTextureSet();

  /**
   * Returns unique Id of the texture
   * @return Value of unique Id
   */
  uint32_t GetUniqueId();

private:

  Dali::Texture     mTexture;
  Dali::Sampler     mSampler;
  Dali::TextureSet  mTextureSet;

  uint32_t          mUniqueId;

  bool              mIsReady;
};

#endif
