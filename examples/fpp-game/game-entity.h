#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

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

#include "game-renderer.h"

#include <dali/public-api/actors/actor.h>

/**
 * @brief The GameEntity class
 * GameEntity wraps the Dali::Actor class and binds the GameRenderer with
 * loaded entity. Entities are owned by the GameScene and populated when
 * scene is being deserialized.
 */
class GameEntity
{
public:

  /**
   * Creates an instance of GameEntity with given name
   * @param[in] name Name of created entity
   */
  GameEntity( const char* name );

  /**
   * Destroys an instance of GameEntity
   */
  ~GameEntity();

  /**
   * Returns the GameRenderer object
   */
  GameRenderer& GetGameRenderer();

  /**
   * Returns associated DALi actor
   * @return Returns actor associated with this GameEntity
   */
  Dali::Actor& GetActor();

  /**
   * Sets location of entity
   * @param[in] location Local position of entity
   */
  void SetLocation( const Dali::Vector3& location );

  /**
   * Sets rotation of entity
   * @param[in] rotation Local rotation of entity
   */
  void SetRotation( const Dali::Quaternion& rotation );

  /**
   * Sets scale of entity
   * @param[in] scale Local scale of entity
   */
  void SetScale( const Dali::Vector3& scale );

  /**
   * Sets size of entity
   * @param[in] size Bounding box of entity
   */
  void SetSize( const Dali::Vector3& size );

  /**
   * Updates Dali::Renderer in case if anything changed ( geometry, texture, etc. )
   */
  void UpdateRenderer();

private:

  Dali::Actor   mActor;
  GameRenderer  mGameRenderer;
};

#endif
