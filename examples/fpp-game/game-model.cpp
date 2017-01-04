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

#include "game-model.h"
#include "game-utils.h"

using namespace GameUtils;

namespace
{
// 'MODV' tag stored in the big-endian (network) order
const uint32_t MODV_TAG( 0x4D4F4456 );
}

GameModel::GameModel( const char *filename )
  : mUniqueId( false ),
    mIsReady( false )
{
  ByteArray bytes;
  if( !LoadFile( filename, bytes ) )
  {
    return;
  }

  mHeader = *(reinterpret_cast<ModelHeader*>( bytes.data() ));

  // expect big-endian
  if( MODV_TAG != mHeader.tag )
  {
    // jump to little-endian variant
    mHeader = *(reinterpret_cast<ModelHeader*>( bytes.data() + bytes.size()/2 ));
  }

  mVertexBuffer = Dali::PropertyBuffer::New( Dali::Property::Map().
                                             Add( "aPosition", Dali::Property::VECTOR3 ).
                                             Add( "aNormal", Dali::Property::VECTOR3 ).
                                             Add( "aTexCoord", Dali::Property::VECTOR2 )
                                             );

  mVertexBuffer.SetData( bytes.data() + mHeader.dataBeginOffset, mHeader.vertexBufferSize/mHeader.vertexStride );

  mGeometry = Dali::Geometry::New();
  mGeometry.AddVertexBuffer( mVertexBuffer );
  mGeometry.SetType( Dali::Geometry::TRIANGLES );

  mUniqueId = HashString( filename );

  mIsReady = true;
}

GameModel::~GameModel()
{
}

Dali::Geometry& GameModel::GetGeometry()
{
  return mGeometry;
}

bool GameModel::IsReady()
{
  return mIsReady;
}

uint32_t GameModel::GetUniqueId()
{
  return mUniqueId;
}
