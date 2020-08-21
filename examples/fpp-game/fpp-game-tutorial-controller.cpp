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

#include "fpp-game-tutorial-controller.h"

#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>

using namespace Dali;
using namespace Dali::Toolkit;

FppGameTutorialController::FppGameTutorialController()
: mLeftTutorialComplete( false ),
  mRightTutorialComplete( false )
{

}

FppGameTutorialController::~FppGameTutorialController()
{

}

void FppGameTutorialController::OnTouch( const TouchEvent& touchEvent )
{
  Vector2 size( mWindow.GetSize() );

  bool isLandscape( size.x > size.y );

  if( !isLandscape )
  {
    std::swap( size.x, size.y );
  }

  Vector2 sizeHalf( size * 0.5f );

  for( size_t i = 0; i < touchEvent.GetPointCount(); ++i )
  {
    Vector2 pos = touchEvent.GetScreenPosition( i );
    if( !isLandscape )
    {
      std::swap( pos.x, pos.y );
    }

    // left label touched
    if( touchEvent.GetState( i ) == PointState::STARTED )
    {
      if( pos.x < sizeHalf.x && !mLeftTutorialComplete )
      {
        mLeftTutorialComplete = true;
        Animation animation = Animation::New( 1.0f );
        animation.AnimateTo( Property( mLeftLabel, Actor::Property::COLOR_ALPHA ), 0.0f );

        // connect complete signal
        if( mRightTutorialComplete )
        {
          animation.FinishedSignal().Connect( this, &FppGameTutorialController::OnTutorialComplete );
        }
        animation.Play();
      }
      // right label touched
      else if( !mRightTutorialComplete )
      {
        mRightTutorialComplete = true;
        Animation animation = Animation::New( 1.0f );
        animation.AnimateTo( Property( mRightLabel, Actor::Property::COLOR_ALPHA ), 0.0f );
        // connect complete signal
        if( mLeftTutorialComplete )
        {
          animation.FinishedSignal().Connect( this, &FppGameTutorialController::OnTutorialComplete );
        }
        animation.Play();
      }
    }
  }
}

void FppGameTutorialController::DisplayTutorial( Dali::Window window )
{
  mWindow = window;

  Vector2 windowSize( mWindow.GetSize() );
  bool isLandscape( windowSize.x > windowSize.y );
  if( !isLandscape )
  {
    std::swap( windowSize.x, windowSize.y );
  }

  mUiRoot = Actor::New();
  mWindow.Add( mUiRoot );

  // left tutorial text label
  mLeftLabel = Toolkit::TextLabel::New("Touch here to walk");
  mLeftLabel.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  mLeftLabel.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
  mLeftLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  mLeftLabel.SetProperty( Actor::Property::SIZE, Vector3( windowSize.x*0.5, windowSize.y, 1.0f ) );
  mLeftLabel.SetProperty( Toolkit::Control::Property::BACKGROUND,
                          Property::Map().Add( Toolkit::Visual::Property::TYPE, Visual::COLOR )
                                         .Add( ColorVisual::Property::MIX_COLOR, Vector4( 0.0, 0.0, 0.7, 0.2 ) ) );
  mLeftLabel.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) ); // White.
  mLeftLabel.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  mLeftLabel.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

  // right tutorial text label
  mRightLabel = Toolkit::TextLabel::New("Touch here to look around");
  mRightLabel.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  mRightLabel.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
  mRightLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  mRightLabel.SetProperty( Actor::Property::SIZE, Vector3( windowSize.x*0.5, windowSize.y, 1.0f ) );
  mRightLabel.SetProperty( Toolkit::Control::Property::BACKGROUND,
                           Property::Map().Add( Toolkit::Visual::Property::TYPE, Visual::COLOR )
                                          .Add( ColorVisual::Property::MIX_COLOR, Vector4( 0.5, 0.0, 0.0, 0.2 ) ) );
  mRightLabel.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) ); // White.
  mRightLabel.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  mRightLabel.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

  // create camera dedicated to be used with UI controls
  CameraActor uiCamera = CameraActor::New();
  mTutorialRenderTask = mWindow.GetRenderTaskList().CreateTask();
  mTutorialRenderTask.SetCameraActor( uiCamera );
  mTutorialRenderTask.SetClearEnabled( false );
  mTutorialRenderTask.SetSourceActor( mUiRoot );
  mTutorialRenderTask.SetExclusive( true );

  if( !isLandscape )
  {
    uiCamera.RotateBy( Degree(90.0f), Vector3( 0.0f, 0.0f, 1.0f ));
  }

  mLeftLabel.SetProperty( Actor::Property::POSITION, Vector3( -windowSize.x*0.25f, 0.0, 0.0 ) );
  mRightLabel.SetProperty( Actor::Property::POSITION, Vector3( windowSize.x*0.25f, 0.0, 0.0 ) );

  mUiRoot.Add( mLeftLabel );
  mUiRoot.Add( mRightLabel );
  mWindow.Add( uiCamera );

  Animation animation = Animation::New( 1.0f );
  animation.AnimateTo( Property( mLeftLabel, Actor::Property::COLOR_ALPHA ), 1.0f, AlphaFunction::EASE_OUT );
  animation.AnimateTo( Property( mRightLabel, Actor::Property::COLOR_ALPHA ), 1.0f, AlphaFunction::EASE_OUT );

  animation.FinishedSignal().Connect( this, &FppGameTutorialController::OnTutorialAnimationFinished );

  animation.Play();
}

void FppGameTutorialController::OnTutorialAnimationFinished( Animation& animation )
{
  // touch signal will wait for a single touch on each side of screen
  mWindow.TouchSignal().Connect( this, &FppGameTutorialController::OnTouch );
}

void FppGameTutorialController::OnTutorialComplete( Animation& animation )
{
  mWindow.Remove( mUiRoot );
  mUiRoot.Reset();
  mWindow.GetRenderTaskList().RemoveTask( mTutorialRenderTask );
}

