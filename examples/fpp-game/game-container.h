#ifndef GAME_CONTAINER_H
#define GAME_CONTAINER_H

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

#include <dali/public-api/common/dali-vector.h>


/**
 * GameContainer is a vector which owns heap-allocated objects.
 * Unlike vector this will call delete on the stored pointers during destruction.
 * For example, you can define a vector of heap-allocated Node objects:
 * @code
 *   typedef GameContainer< GameEntity* > EntityArray;
 *
 *   EntityArray entites;
 *   entites.PushBack( new GameEntity() );
 *   // container is now responsible for calling delete on GameEntity
 *
 * @endcode
 */
template< class T > class GameContainer : public Dali::Vector< T >
{
public:

  typedef typename Dali::Vector< T >::Iterator Iterator;
  typedef typename Dali::Vector< T >::ConstIterator ConstIterator;

  /**
   * Create a owner container.
   */
  GameContainer()
  {
  }

  /**
   * Non-virtual destructor; GameCache<T> is not suitable as base class.
   */
  ~GameContainer()
  {
    Clear();
    Dali::VectorBase::Release();
  }

  /**
   * Destroy all of the elements in the container.
   */
  void Clear()
  {
    ConstIterator end = Dali::Vector< T >::End();
    for( Iterator iter = Dali::Vector< T >::Begin(); iter != end; ++iter )
    {
      delete (*iter);
    }
    Dali::Vector< T >::Clear();
  }

private:

  // Undefined copy constructor.
  GameContainer( const GameContainer& );

  // Undefined assignment operator.
  GameContainer& operator=( const GameContainer& );
};

#endif // GAMECACHE_H
