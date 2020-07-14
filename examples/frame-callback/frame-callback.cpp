/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include "frame-callback.h"

using namespace Dali;
using namespace std;

FrameCallback::FrameCallback()
: mActorIdContainer(),
  windowHalfWidth( 0.0f )
{
}

void FrameCallback::SetWindowWidth( float windowWidth )
{
  windowHalfWidth = windowWidth * 0.5f;
}

void FrameCallback::AddId( uint32_t id )
{
  mActorIdContainer.PushBack( id );
}

void FrameCallback::Update( Dali::UpdateProxy& updateProxy, float /* elapsedSeconds */ )
{
  // Go through Actor ID container and check if we've hit the sides.
  for( auto&& i : mActorIdContainer )
  {
    Vector3 position;
    Vector3 size;
    if( updateProxy.GetPositionAndSize( i, position, size ) ) // Retrieve the position and size using the Actor ID.
    {
      float halfWidthPoint = windowHalfWidth - size.width * 0.5f;
      float xTranslation = abs( position.x );
      if( xTranslation > halfWidthPoint )
      {
        // Actor has hit the edge, adjust the size accordingly.
        float adjustment = xTranslation - halfWidthPoint;
        size.width += adjustment * SIZE_MULTIPLIER;
        size.height += adjustment * SIZE_MULTIPLIER;

        updateProxy.SetSize( i, size ); // Set the size using the UpdateProxy.
      }

      // Retrieve the actor's position and set make it more transparent the closer it is to the middle.
      Vector4 color;
      if( updateProxy.GetColor( i, color ) )
      {
        color.a = xTranslation / halfWidthPoint;
        updateProxy.SetColor( i, color );
      }
    }
  }
}
