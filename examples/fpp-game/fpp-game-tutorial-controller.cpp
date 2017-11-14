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

#include "fpp-game-tutorial-controller.h"

#include <dali/public-api/events/touch-data.h>
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

void FppGameTutorialController::OnTouch( const TouchData& touchEvent )
{
  Vector2 size( mStage.GetSize() );

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

void FppGameTutorialController::DisplayTutorial()
{
  mStage = Stage::GetCurrent();

  Vector2 stageSize( mStage.GetSize() );
  bool isLandscape( stageSize.x > stageSize.y );
  if( !isLandscape )
  {
    std::swap( stageSize.x, stageSize.y );
  }

  mUiRoot = Actor::New();
  mStage.Add( mUiRoot );

  // left tutorial text label
  mLeftLabel = Toolkit::TextLabel::New("Touch here to walk");
  mLeftLabel.SetParentOrigin( ParentOrigin::CENTER );
  mLeftLabel.SetAnchorPoint( AnchorPoint::CENTER );
  mLeftLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  mLeftLabel.SetSize( Vector3( stageSize.x*0.5, stageSize.y, 1.0f ) );
  mLeftLabel.SetProperty( Toolkit::Control::Property::BACKGROUND,
                          Property::Map().Add( Toolkit::Visual::Property::TYPE, Visual::COLOR )
                                         .Add( ColorVisual::Property::MIX_COLOR, Vector4( 0.0, 0.0, 0.7, 0.2 ) ) );
  mLeftLabel.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) ); // White.
  mLeftLabel.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  mLeftLabel.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

  // right tutorial text label
  mRightLabel = Toolkit::TextLabel::New("Touch here to look around");
  mRightLabel.SetParentOrigin( ParentOrigin::CENTER );
  mRightLabel.SetAnchorPoint( AnchorPoint::CENTER );
  mRightLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  mRightLabel.SetSize( Vector3( stageSize.x*0.5, stageSize.y, 1.0f ) );
  mRightLabel.SetProperty( Toolkit::Control::Property::BACKGROUND,
                           Property::Map().Add( Toolkit::Visual::Property::TYPE, Visual::COLOR )
                                          .Add( ColorVisual::Property::MIX_COLOR, Vector4( 0.5, 0.0, 0.0, 0.2 ) ) );
  mRightLabel.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) ); // White.
  mRightLabel.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  mRightLabel.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

  // create camera dedicated to be used with UI controls
  CameraActor uiCamera = CameraActor::New();
  mTutorialRenderTask = mStage.GetRenderTaskList().CreateTask();
  mTutorialRenderTask.SetCameraActor( uiCamera );
  mTutorialRenderTask.SetClearEnabled( false );
  mTutorialRenderTask.SetSourceActor( mUiRoot );
  mTutorialRenderTask.SetExclusive( true );

  if( !isLandscape )
  {
    uiCamera.RotateBy( Degree(90.0f), Vector3( 0.0f, 0.0f, 1.0f ));
  }

  mLeftLabel.SetPosition( Vector3( -stageSize.x*0.25f, 0.0, 0.0 ) );
  mRightLabel.SetPosition( Vector3( stageSize.x*0.25f, 0.0, 0.0 ) );

  mUiRoot.Add( mLeftLabel );
  mUiRoot.Add( mRightLabel );
  mStage.Add( uiCamera );

  Animation animation = Animation::New( 1.0f );
  animation.AnimateTo( Property( mLeftLabel, Actor::Property::COLOR_ALPHA ), 1.0f, AlphaFunction::EASE_OUT );
  animation.AnimateTo( Property( mRightLabel, Actor::Property::COLOR_ALPHA ), 1.0f, AlphaFunction::EASE_OUT );

  animation.FinishedSignal().Connect( this, &FppGameTutorialController::OnTutorialAnimationFinished );

  animation.Play();
}

void FppGameTutorialController::OnTutorialAnimationFinished( Animation& animation )
{
  // touch signal will wait for a single touch on each side of screen
  mStage.TouchSignal().Connect( this, &FppGameTutorialController::OnTouch );
}

void FppGameTutorialController::OnTutorialComplete( Animation& animation )
{
  mStage.Remove( mUiRoot );
  mUiRoot.Reset();
  mStage.GetRenderTaskList().RemoveTask( mTutorialRenderTask );
}

