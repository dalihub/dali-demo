#ifndef GAME_SCENE_H
#define GAME_SCENE_H

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

#include <vector>
#include <stdint.h>
#include <inttypes.h>

#include "game-container.h"
#include "game-utils.h"
#include "game-camera.h"

#include <dali/public-api/actors/actor.h>

class GameCamera;
class GameEntity;
class GameTexture;
class GameModel;

/**
 * Container based types owning heap allocated data of specifed types
 */
typedef GameContainer< GameEntity* > EntityArray;
typedef GameContainer< GameTexture* > TextureArray;
typedef GameContainer< GameModel* > ModelArray;

class GameScene
{
public:

  /**
   * Creates an instance of the GameScene
   */
  GameScene();

  /**
   * Destroys an instance of the GameScene
   */
  ~GameScene();

  /**
   * Loads scene from formatted JSON file, returns true on success
   *
   * @param[in] filename Path to the scene file
   * @return true if suceess
   */
  bool Load( const char* filename );

  /**
   * Loads resource ( model or texture ) or gets if from cache if already loaded
   * @param[in] filename Path to the resource file
   * @param[in] cache Reference to the cache array to be used
   * @return Pointer to the resource or NULL otherwise
   */
  template <typename T>
  T* GetResource( const char* filename, GameContainer<T*>& cache );

  /**
   * Returns scene root actor
   * @return Parent actor of the whole game scene
   */
  Dali::Actor& GetRootActor();

private:

  EntityArray     mEntities;
  GameCamera      mCamera;

  // internal scene cache
  ModelArray      mModelCache;
  TextureArray    mTextureCache;

  Dali::Actor     mRootActor;
};


template<typename T>
T* GameScene::GetResource( const char* filename, GameContainer<T*>& cache )
{
  std::string path( DEMO_GAME_DIR );
  path += "/";
  path += filename;

  uint32_t hash( GameUtils::HashString( path.c_str() ) );

  for( typename GameContainer<T*>::Iterator iter = cache.Begin(); iter != cache.End(); ++iter )
  {
    if( (*iter)->GetUniqueId() == hash )
    {
      return (*iter);
    }
  }

  // load resource
  T* resource = new T( path.c_str() );
  if( !resource->IsReady() )
  {
    return NULL;
  }

  cache.PushBack( resource );

  return resource;
}


#endif
