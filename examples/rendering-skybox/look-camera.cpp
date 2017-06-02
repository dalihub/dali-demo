/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include "look-camera.h"

#include <dali/public-api/common/stage.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/events/touch-data.h>
#include <dali/public-api/events/touch-event.h>

using namespace Dali;

namespace
{
// Input sensitivity, the larger value, the more sensitive input
// Default value has been chosen empirically
const float   CAMERA_SENSITIVITY        ( 90.0f );

// Vertical angle limit of the camera
const float   CAMERA_VERTICAL_LIMIT     ( 80.0f );

// Position where camera is instantiated by default
const Vector3 CAMERA_DEFAULT_POSITION   ( 0.0f, 0.0f, 3.0f );

// Field-of-View in degrees
const float   CAMERA_DEFAULT_FOV        ( 60.0f );

// Near plane
const float   CAMERA_DEFAULT_NEAR       ( 0.1f );

// Far plane
const float   CAMERA_DEFAULT_FAR        ( 100.0f );

// Default forward vector
const Vector3 CAMERA_FORWARD            ( 0.0f, 0.0f, 1.0f );

// Default up vector
const Vector3 CAMERA_UP                 ( Vector3::YAXIS );
}

LookCamera::LookCamera()
: mCameraYawPitch( 0.0f, 180.0f ),
  mFovY( CAMERA_DEFAULT_FOV ),
  mNear( CAMERA_DEFAULT_NEAR ),
  mFar( CAMERA_DEFAULT_FAR ),
  mCameraPosition( CAMERA_DEFAULT_POSITION )
{
}

LookCamera::~LookCamera()
{
  mTimer.Stop();
  mCameraActor.Remove( mInterceptorActor );
}

void LookCamera::Initialise( const Vector3& position, float fovY, float near, float far )
{
  mFovY = fovY;
  mNear = near;
  mFar = far;

  // Camera position is shadowed in order to avoid using GetCurrentPosition()
  mCameraPosition = position;

  // Initialise default camera
  InitialiseDefaultCamera();

  // Create input interceptor actor
  CreateInterceptorActor();

  // Start timer
  mTimer = Timer::New( 16 );
  mTimer.TickSignal().Connect( this, &LookCamera::OnTick );
  mTimer.Start();
}

bool LookCamera::OnTick()
{
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  // ---------------------------------------------------------------------
  // update rotation
  Vector2 tmp( mScreenLookDelta );
  mScreenLookDelta = Vector2::ZERO;

  float yaw = ( (tmp.y / stageSize.x ) * CAMERA_SENSITIVITY );
  float pitch = ( (tmp.x / stageSize.y ) * CAMERA_SENSITIVITY );
  mCameraYawPitch.x -= yaw;
  mCameraYawPitch.y -= pitch;
  if( abs( mCameraYawPitch.x ) > CAMERA_VERTICAL_LIMIT )
  {
    mCameraYawPitch.x = CAMERA_VERTICAL_LIMIT * ((mCameraYawPitch.x < 0) ? -1.0f : 1.0f );
  }

  Quaternion rotation;
  Quaternion rotX( Degree( mCameraYawPitch.x), Vector3( 1.0f, 0.0f, 0.0f ) );
  Quaternion rotY( Degree( mCameraYawPitch.y), Vector3( 0.0f, 1.0f, 0.0f ) );
  rotation = ( rotY * rotX );

  mCameraActor.SetOrientation( rotation );

  return true;
}

void LookCamera::InitialiseDefaultCamera()
{
  Stage stage = Stage::GetCurrent();
  mCameraActor = stage.GetRenderTaskList().GetTask(0).GetCameraActor();
  mCameraActor.SetName( "LookCamera" );
  mCameraActor.SetAnchorPoint( AnchorPoint::CENTER );
  mCameraActor.SetParentOrigin( ParentOrigin::CENTER );
  mCameraActor.SetFieldOfView( Radian( Degree( mFovY ) ) );

  // should be read from file
  mCameraActor.SetNearClippingPlane( mNear );
  mCameraActor.SetFarClippingPlane( mFar );
  mCameraActor.SetPosition( mCameraPosition );
}

void LookCamera::CreateInterceptorActor()
{
  Stage stage = Stage::GetCurrent();

  mInterceptorActor = Actor::New();
  mInterceptorActor.SetName( "InputInterceptor" );
  mInterceptorActor.SetSize( Vector3( stage.GetSize().x, stage.GetSize().y, 1 ) );
  mInterceptorActor.SetPosition( Vector3( 0.0, 0.0, 1.0  ) );
  mInterceptorActor.SetAnchorPoint( AnchorPoint::CENTER );
  mInterceptorActor.SetParentOrigin( ParentOrigin::CENTER );
  mCameraActor.Add( mInterceptorActor );

  // Connect TouchSignal to interceptor actor
  mInterceptorActor.TouchSignal().Connect( this, &LookCamera::OnTouch );
}

bool LookCamera::OnTouch( Actor actor, const TouchData& touch )
{
  Stage stage = Stage::GetCurrent();

  for( int i = 0; i < (int)touch.GetPointCount() && i < 3; ++i )
  {
    Vector2 position( touch.GetScreenPosition( i ) );

    // touch started
    if( touch.GetState( i ) == PointState::STARTED )
    {
      mOldTouchLookPosition = position;
    }
    else if( touch.GetState( i ) == PointState::FINISHED ||
             touch.GetState( i ) == PointState::LEAVE ||
             touch.GetState( i ) == PointState::INTERRUPTED
             )
    {
      mScreenLookDelta = Vector2::ZERO;
      mOldTouchLookPosition = Vector2::ZERO;
    }
    else // on motion
    {
      mScreenLookDelta.x += ( position.x - mOldTouchLookPosition.x );
      mScreenLookDelta.y += ( position.y - mOldTouchLookPosition.y );
      mOldTouchLookPosition = position;
    }
  }

  return true;
}
