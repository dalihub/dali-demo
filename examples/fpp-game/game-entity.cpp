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

#include "game-entity.h"
#include "game-renderer.h"

GameEntity::GameEntity( const char* name )
{
  mActor = Dali::Actor::New();
  mActor.SetName( name );
}

GameEntity::~GameEntity()
{
}

void GameEntity::UpdateRenderer()
{
  if( mActor.GetRendererCount() )
  {
    Dali::Renderer currentRenderer = mActor.GetRendererAt( 0 );
    if( currentRenderer == mGameRenderer.GetRenderer() )
    {
      return;
    }
    mActor.RemoveRenderer( currentRenderer );
  }
  if( mGameRenderer.GetRenderer() )
  {
    mActor.AddRenderer( mGameRenderer.GetRenderer() );
  }
}

GameRenderer& GameEntity::GetGameRenderer()
{
  return mGameRenderer;
}

Dali::Actor& GameEntity::GetActor()
{
  return mActor;
}

void GameEntity::SetLocation( const Dali::Vector3& loc )
{
  mActor.SetPosition( loc );
}

void GameEntity::SetRotation( const Dali::Quaternion& rot )
{
  mActor.SetOrientation( rot );
}

void GameEntity::SetScale( const Dali::Vector3& scale )
{
  mActor.SetScale( scale );
}

void GameEntity::SetSize( const Dali::Vector3& size )
{
  mActor.SetSize( size );
}
