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

#include <string.h>
#include <stdio.h>

#include "game-scene.h"
#include "game-model.h"
#include "game-texture.h"
#include "game-entity.h"
#include "game-renderer.h"
#include "game-camera.h"

#include "third-party/picojson.h"

#include <dali/dali.h>

using namespace Dali;
using namespace picojson;

using std::vector;

using namespace GameUtils;

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

bool GameScene::Load(const char *filename)
{
  ByteArray bytes;
  if( !LoadFile( filename, bytes ) )
  {
    return false;
  }

  // add EOL
  bytes.push_back( '\0');

  picojson::value root;
  picojson::parse( root, bytes.data() );

  bool failed( false );

  if( root.is<object>() )
  {
    object rootObject = root.get<object>();
    for( object::iterator it = rootObject.begin(); it != rootObject.end(); ++it )
    {
      std::string entityName( (*it).first );

      GameEntity* entity = new GameEntity( entityName.c_str() );
      mEntities.PushBack( entity );

      value& val( (*it).second );
      value& vLocation = val.get( "location" );
      value& vRotation = val.get( "rotation" );
      value& vScale = val.get( "scale" );
      value& vSize = val.get( "size" );
      value& vModel = val.get( "model" );
      value& vTexture = val.get( "texture" );

      if( !vLocation.is<null>() )
      {
        array& location = vLocation.get<array>();
        entity->SetLocation( Vector3(
                              location.at(0).get<double>(),
                              location.at(1).get<double>(),
                              location.at(2).get<double>()
                              ));
      }

      if( !vRotation.is<null>() )
      {
        array& rotation = vRotation.get<array>();
        entity->SetRotation( Quaternion( Vector4(
                              -rotation.at(0).get<double>(),
                              rotation.at(1).get<double>(),
                              -rotation.at(2).get<double>(),
                              rotation.at(3).get<double>()
                              )) );
      }

      if( !vScale.is<null>() )
      {
        array& scale = vScale.get<array>();
        entity->SetScale( Vector3(
                              scale.at(0).get<double>(),
                              scale.at(1).get<double>(),
                              scale.at(2).get<double>()
                              ));
      }

      if( !vSize.is<null>() )
      {
        array& size = vSize.get<array>();
        entity->SetSize( Vector3(
                              size.at(0).get<double>(),
                              size.at(1).get<double>(),
                              size.at(2).get<double>()
                              ));
      }

      GameModel* model( NULL );
      GameTexture* texture( NULL );

      if( !vModel.is<null>() )
      {
        std::string& strModel = vModel.get<std::string>();
        model = GetResource( strModel.c_str(), mModelCache );
      }

      if( !vTexture.is<null>() )
      {
        std::string& strTexture = vTexture.get<std::string>();
        texture = GetResource( strTexture.c_str(), mTextureCache );
      }

      if( !model || !texture )
      {
        failed = true;
        break;
      }

      entity->GetGameRenderer().SetModel( model );
      entity->GetGameRenderer().SetMainTexture( texture );
    }
  }

  if( failed )
  {
    return false;
  }

  // add all to the stage
  Stage stage = Stage::GetCurrent();
  mRootActor = Actor::New();
  mRootActor.SetAnchorPoint( AnchorPoint::CENTER );
  mRootActor.SetParentOrigin( ParentOrigin::CENTER );
  stage.GetRootLayer().Add( mRootActor );
  mRootActor.SetScale( -1.0, 1.0, 1.0 );
  mRootActor.SetPosition( 0.0, 0.0, 0.0 );
  mRootActor.SetOrientation( Degree( 90 ), Vector3( 1.0, 0.0, 0.0 ));
  for( size_t i = 0; i < mEntities.Size(); ++i )
  {
    Actor actor( mEntities[i]->GetActor() );
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetParentOrigin( ParentOrigin::CENTER );
    mRootActor.Add( actor );
    mEntities[i]->UpdateRenderer();
  }

  // update camera
  mCamera.Initialise( 60.0f, 0.1f, 100.0f );

  return true;
}

Dali::Actor& GameScene::GetRootActor()
{
  return mRootActor;
}
