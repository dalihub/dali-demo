#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

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

#include <dali/public-api/rendering/renderer.h>

class GameModel;
class GameTexture;

/**
 * @brief The GameRenderer class
 * GameRenderer binds the main texture with model. Can be used by multiple entities. It wraps
 * Dali::Renderer.
 */
class GameRenderer
{
public:

  /**
   * Creates an instance of the GameRenderer
   */
  GameRenderer();

  /**
   * Destroys an instance of the GameRenderer
   */
  ~GameRenderer();

  /**
   * Sets current model on the renderer
   * Resets the Dali::Renderer or creates new one on first time setup
   * @param[in] model Pointer to the GameModel object
   */
  void SetModel( GameModel* model );

  /**
   * Sets main texture on the renderer
   * Resets the Dali::Renderer or creates new one on first time setup
   * @param[in] texture Pointer to the GameTexture object
   */
  void SetMainTexture( GameTexture* texture );

  /**
   * Retrieves DALi renderer object
   */
  Dali::Renderer& GetRenderer();

private:

  /**
   * Initialises rendering data
   */
  void Setup();

private:

  Dali::Renderer  mRenderer;
  GameModel*      mModel;
  GameTexture*    mTexture;
};

#endif
