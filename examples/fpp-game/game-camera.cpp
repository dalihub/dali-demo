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

#include "game-camera.h"

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
const Vector3 CAMERA_DEFAULT_POSITION   ( 1.0f, -1.5f, -3.0f );

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

GameCamera::GameCamera()
: mFovY( CAMERA_DEFAULT_FOV ),
  mNear( CAMERA_DEFAULT_NEAR ),
  mFar( CAMERA_DEFAULT_FAR ),
  mWalkingTouchId( -1 ),
  mLookingTouchId( -1 ),
  mPortraitMode( false )
{
}

GameCamera::~GameCamera()
{
  mTimer.Stop();
  mCameraActor.Remove( mInterceptorActor );
}

void GameCamera::Initialise( float fovY, float near, float far )
{
  mFovY = fovY;
  mNear = near;
  mFar = far;

  Vector2 stageSize = Stage::GetCurrent().GetSize();
  mPortraitMode = stageSize.x < stageSize.y ? true : false;

  // Initialise default camera
  InitialiseDefaultCamera();

  // Create input interceptor actor
  CreateInterceptorActor();

  // Start timer
  mTimer = Timer::New( 16 );
  mTimer.TickSignal().Connect( this, &GameCamera::OnTick );
  mTimer.Start();
}

bool GameCamera::OnTick()
{
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  // ---------------------------------------------------------------------
  // update rotation
  Vector2 tmp( mScreenLookDelta );
  mScreenLookDelta = Vector2::ZERO;

  if( mPortraitMode )
  {
    float yaw = ( (tmp.y / stageSize.y ) * CAMERA_SENSITIVITY );
    float pitch = ( (tmp.x / stageSize.x ) * CAMERA_SENSITIVITY );
    mCameraYawPitch.y -= yaw;
    mCameraYawPitch.x -= pitch;
    if( abs( mCameraYawPitch.y ) > CAMERA_VERTICAL_LIMIT )
    {
      mCameraYawPitch.y = CAMERA_VERTICAL_LIMIT * ((mCameraYawPitch.y < 0) ? -1.0f : 1.0f );
    }
  }
  else
  {
    float yaw = ( (tmp.y / stageSize.x ) * CAMERA_SENSITIVITY );
    float pitch = ( (tmp.x / stageSize.y ) * CAMERA_SENSITIVITY );
    mCameraYawPitch.x -= yaw;
    mCameraYawPitch.y -= pitch;
    if( abs( mCameraYawPitch.x ) > CAMERA_VERTICAL_LIMIT )
    {
      mCameraYawPitch.x = CAMERA_VERTICAL_LIMIT * ((mCameraYawPitch.x < 0) ? -1.0f : 1.0f );
    }
  }

  Quaternion rotation;
  Quaternion rotX( Degree( mCameraYawPitch.x), Vector3( 1.0f, 0.0f, 0.0f ) );
  Quaternion rotY( Degree( mCameraYawPitch.y), Vector3( 0.0f, 1.0f, 0.0f ) );
  if (mPortraitMode )
  {
    Quaternion rotZ( Degree( mPortraitMode ? 90.0 : 0.0f), Vector3( 0.0f, 0.0f, 1.0f ) );
    rotation = ( rotZ * rotX * rotY );
  }
  else
  {
    rotation = ( rotY * rotX );
  }
  mCameraActor.SetOrientation( rotation );

  // ---------------------------------------------------------------------
  // update position
  Vector3 position( mCameraPosition );

  // Rotate CAMERA_FORWARD vector
  Vector3 forwardVector = rotation.Rotate( CAMERA_FORWARD );

  // Cancel vertical movement
  forwardVector.y = 0.0f;

  // Normalize
  forwardVector.Normalize();

  // compute sideways vector
  Vector3 sidewaysVector = forwardVector.Cross( CAMERA_UP );

  sidewaysVector.Normalize();

  const float forwardSpeed( mScreenWalkDelta.y / stageSize.y );
  const float sidewaysSpeed( mScreenWalkDelta.x / stageSize.x );

  // Adjust walking speed
  if ( mPortraitMode )
  {
    position += forwardVector * (forwardSpeed * 0.5f);
  }
  else
  {
    position += forwardVector * (-forwardSpeed * 0.5f);
  }

  position += sidewaysVector * (sidewaysSpeed * 0.5f);

  mCameraActor.SetPosition( position );

  mCameraPosition = position;

  return true;
}

void GameCamera::InitialiseDefaultCamera()
{
  Stage stage = Stage::GetCurrent();
  mCameraActor = stage.GetRenderTaskList().GetTask(0).GetCameraActor();
  mCameraActor.SetName( "GameCamera" );
  mCameraActor.SetAnchorPoint( AnchorPoint::CENTER );
  mCameraActor.SetParentOrigin( ParentOrigin::CENTER );
  mCameraActor.SetFieldOfView( Radian( Degree( mFovY ) ) );

  // should be read from file
  mCameraActor.SetNearClippingPlane( mNear );
  mCameraActor.SetFarClippingPlane( mFar );
  mCameraActor.SetPosition( CAMERA_DEFAULT_POSITION );

  // Camera position is shadowed in order to avoid using GetCurrentPosition()
  mCameraPosition = CAMERA_DEFAULT_POSITION;
}

void GameCamera::CreateInterceptorActor()
{
  Stage stage = Stage::GetCurrent();

  mInterceptorActor = Actor::New();
  mInterceptorActor.SetName( "GameInputInterceptor" );
  mInterceptorActor.SetSize( Vector3( stage.GetSize().x, stage.GetSize().y, 1 ) );
  mInterceptorActor.SetPosition( Vector3( 0.0, 0.0, 1.0  ) );
  mInterceptorActor.SetAnchorPoint( AnchorPoint::CENTER );
  mInterceptorActor.SetParentOrigin( ParentOrigin::CENTER );
  mCameraActor.Add( mInterceptorActor );

  // Connect TouchSignal to interceptor actor
  mInterceptorActor.TouchSignal().Connect( this, &GameCamera::OnTouch );
}

bool GameCamera::OnTouch( Actor actor, const TouchData& touch )
{
  Stage stage = Stage::GetCurrent();

  for( int i = 0; i < (int)touch.GetPointCount() && i < 3; ++i )
  {
    int id = touch.GetDeviceId( i );
    Vector2 tmp( touch.GetScreenPosition( i ) );
    Vector2 position;
    float halfWindowSize;
    if( mPortraitMode )
    {
      position.x = tmp.y;
      position.y = tmp.x;
      halfWindowSize = stage.GetSize().y / 2;
    }
    else
    {
      position.x = tmp.x;
      position.y = tmp.y;
      halfWindowSize = stage.GetSize().x / 2;
    }

    // touch started
    if( touch.GetState( i ) == PointState::STARTED )
    {
      // start looking
      if( position.x > halfWindowSize && mLookingTouchId < 0 )
      {
        mLookingTouchId = id;
        mOldTouchLookPosition = position;
      }
      // start walking
      else if( position.x < halfWindowSize && mWalkingTouchId < 0 )
      {
        mWalkingTouchId = id;
        mOldTouchWalkPosition = position;
        mScreenWalkDelta = Vector2::ZERO;
      }
    }
    else if( touch.GetState( i ) == PointState::FINISHED ||
             touch.GetState( i ) == PointState::LEAVE ||
             touch.GetState( i ) == PointState::INTERRUPTED
             )
    {
      // terminate look
      if( mLookingTouchId == id )
      {
        mScreenLookDelta = Vector2::ZERO;
        mOldTouchLookPosition = Vector2::ZERO;
        mLookingTouchId = -1;
      }
      // terminate walking
      else if( mWalkingTouchId == id )
      {
        mScreenWalkDelta = Vector2::ZERO;
        mOldTouchWalkPosition = Vector2::ZERO;
        mWalkingTouchId = -1;
      }
    }
    else // on motion
    {
      // update looking
      if( mLookingTouchId == id )
      {
        mScreenLookDelta.x += ( position.x - mOldTouchLookPosition.x );
        mScreenLookDelta.y += ( position.y - mOldTouchLookPosition.y );
        mOldTouchLookPosition = position;
      }
      // update walking
      else if ( mWalkingTouchId == id )
      {
        mScreenWalkDelta.x += ( position.x - mOldTouchWalkPosition.x );
        mScreenWalkDelta.y += ( position.y - mOldTouchWalkPosition.y );
        mOldTouchWalkPosition = position;
      }
    }
  }
  return true;
}
