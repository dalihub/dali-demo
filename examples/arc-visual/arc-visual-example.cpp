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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/arc-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/arc-visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const float START_ANGLE_INITIAL_VALUE( 0.0f );
const float START_ANGLE_TARGET_VALUE( 360.0f );
const float SWEEP_ANGLE_INITIAL_VALUE( 90.0f );
const float SWEEP_ANGLE_TARGET_VALUE( 360.0f );
const float ANIMATION_DURATION( 3.0f );

const Property::Value BACKGROUND
{
  { Visual::Property::TYPE, DevelVisual::ARC },
  { Visual::Property::MIX_COLOR, Color::RED },
  { DevelArcVisual::Property::START_ANGLE, 0.0f },
  { DevelArcVisual::Property::SWEEP_ANGLE, 90.0f },
  { DevelArcVisual::Property::CAP, DevelArcVisual::Cap::ROUND },
  { DevelArcVisual::Property::THICKNESS, 20.0f }
};

const Property::Value TEXT_BACKGROUND
{
  { Visual::Property::TYPE, Visual::COLOR },
  { ColorVisual::Property::MIX_COLOR, Vector4( 0.8f, 0.8f, 0.8f, 1.0f ) },
  { DevelVisual::Property::CORNER_RADIUS, 10.0f }
};

const Property::Value TRANSITION_ANIMATOR
{
  { "timePeriod", Property::Map().Add( "duration", ANIMATION_DURATION ) }
};

const Property::Value TRANSITION_START_ANGLE
{
  { "target", "background" },
  { "property", "startAngle" },
  { "initialValue", START_ANGLE_INITIAL_VALUE },
  { "targetValue", START_ANGLE_TARGET_VALUE },
  { "animator", TRANSITION_ANIMATOR }
};

const Property::Value TRANSITION_SWEEP_ANGLE
{
  { "target", "background" },
  { "property", "sweepAngle" },
  { "initialValue", SWEEP_ANGLE_INITIAL_VALUE },
  { "targetValue", SWEEP_ANGLE_TARGET_VALUE },
  { "animator", TRANSITION_ANIMATOR }
};

} // namespace

// This example shows the properties of the arc visual - thickness, startAngle and sweepAngle and animates them.
//
class ArcVisualExample : public ConnectionTracker
{
public:

  ArcVisualExample( Application& application )
  : mApplication( application ),
    mSelectedPoperty( DevelArcVisual::Property::START_ANGLE )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ArcVisualExample::Create );
  }

  ~ArcVisualExample() = default;

private:

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    mControl = Control::New();
    mControl.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mControl.SetProperty( Actor::Property::SIZE, Vector2( 300.0f, 300.0f ) );
    mControl.SetProperty( Control::Property::BACKGROUND, BACKGROUND );
    stage.Add( mControl );

    mStartAngleLabel = TextLabel::New( "1" );
    mStartAngleLabel.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mStartAngleLabel.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_RIGHT );
    mStartAngleLabel.SetProperty( Actor::Property::POSITION, Vector2( -30.0f, -10.0f ) );
    mStartAngleLabel.SetProperty( Control::Property::BACKGROUND, TEXT_BACKGROUND );
    mStartAngleLabel.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::USE_NATURAL_SIZE );
    mStartAngleLabel.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::USE_NATURAL_SIZE );
    mStartAngleLabel.SetProperty( Control::Property::PADDING, Extents( 20.0f, 20.0f, 10.0f, 10.0f ) );
    mStartAngleLabel.TouchSignal().Connect( this, &ArcVisualExample::OnButtonTouch );
    stage.Add( mStartAngleLabel );

    mSweepAngleLabel = TextLabel::New( "2" );
    mSweepAngleLabel.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mSweepAngleLabel.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER );
    mSweepAngleLabel.SetProperty( Actor::Property::POSITION, Vector2( 0.0f, -10.0f ) );
    mSweepAngleLabel.SetProperty( Control::Property::BACKGROUND, TEXT_BACKGROUND );
    mSweepAngleLabel.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::USE_NATURAL_SIZE );
    mSweepAngleLabel.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::USE_NATURAL_SIZE );
    mSweepAngleLabel.SetProperty( Control::Property::PADDING, Extents( 20.0f, 20.0f, 10.0f, 10.0f ) );
    mSweepAngleLabel.TouchSignal().Connect( this, &ArcVisualExample::OnButtonTouch );
    stage.Add( mSweepAngleLabel );

    mThicknessLabel = TextLabel::New( "3" );
    mThicknessLabel.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mThicknessLabel.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_LEFT );
    mThicknessLabel.SetProperty( Actor::Property::POSITION, Vector2( 30.0f, -10.0f ) );
    mThicknessLabel.SetProperty( Control::Property::BACKGROUND, TEXT_BACKGROUND );
    mThicknessLabel.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::USE_NATURAL_SIZE );
    mThicknessLabel.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::USE_NATURAL_SIZE );
    mThicknessLabel.SetProperty( Control::Property::PADDING, Extents( 20.0f, 20.0f, 10.0f, 10.0f ) );
    mThicknessLabel.TouchSignal().Connect( this, &ArcVisualExample::OnButtonTouch );
    stage.Add( mThicknessLabel );

    mPlusTextLabel = TextLabel::New( "+" );
    mPlusTextLabel.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mPlusTextLabel.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    mPlusTextLabel.SetProperty( Actor::Property::POSITION, Vector2( 20.0f, 10.0f ) );
    mPlusTextLabel.SetProperty( Control::Property::BACKGROUND, TEXT_BACKGROUND );
    mPlusTextLabel.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::USE_NATURAL_SIZE );
    mPlusTextLabel.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::USE_NATURAL_SIZE );
    mPlusTextLabel.SetProperty( Control::Property::PADDING, Extents( 20.0f, 20.0f, 10.0f, 10.0f ) );
    mPlusTextLabel.TouchSignal().Connect( this, &ArcVisualExample::OnButtonTouch );
    stage.Add( mPlusTextLabel );

    mMinusTextLabel = TextLabel::New( "-" );
    mMinusTextLabel.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mMinusTextLabel.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_RIGHT );
    mMinusTextLabel.SetProperty( Actor::Property::POSITION, Vector2( -20.0f, 10.0f ) );
    mMinusTextLabel.SetProperty( Control::Property::BACKGROUND, TEXT_BACKGROUND );
    mMinusTextLabel.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::USE_NATURAL_SIZE );
    mMinusTextLabel.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::USE_NATURAL_SIZE );
    mMinusTextLabel.SetProperty( Control::Property::PADDING, Extents( 25.0f, 25.0f, 10.0f, 10.0f ) );
    mMinusTextLabel.TouchSignal().Connect( this, &ArcVisualExample::OnButtonTouch );
    stage.Add( mMinusTextLabel );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &ArcVisualExample::OnTouch );

    // Respond to key events
    stage.KeyEventSignal().Connect( this, &ArcVisualExample::OnKeyEvent );
  }

  bool OnButtonTouch( Actor actor, const TouchData& touch )
  {
    if( touch.GetState( 0 ) == PointState::UP )
    {
      Control control = Control::DownCast( actor );
      if( control == mStartAngleLabel )
      {
        mSelectedPoperty = DevelArcVisual::Property::START_ANGLE;
      }
      else if( control == mSweepAngleLabel )
      {
        mSelectedPoperty = DevelArcVisual::Property::SWEEP_ANGLE;
      }
      else if( control == mThicknessLabel )
      {
        mSelectedPoperty = DevelArcVisual::Property::THICKNESS;
      }
      else if( control == mPlusTextLabel )
      {
        Property::Map map = mControl.GetProperty< Property::Map >( Control::Property::BACKGROUND );
        Property::Value* value = map.Find( mSelectedPoperty );
        if( value )
        {
          DevelControl::DoAction( mControl, Control::Property::BACKGROUND, DevelArcVisual::Action::UPDATE_PROPERTY,
                                  Property::Map().Add( mSelectedPoperty, value->Get< float >() + 5.0f ) );
        }
      }
      else
      {
        Property::Map map = mControl.GetProperty< Property::Map >( Control::Property::BACKGROUND );
        Property::Value* value = map.Find( mSelectedPoperty );
        if( value )
        {
          DevelControl::DoAction( mControl, Control::Property::BACKGROUND, DevelArcVisual::Action::UPDATE_PROPERTY,
                                  Property::Map().Add( mSelectedPoperty, value->Get< float >() - 5.0f ) );
        }
      }
    }
    return true;
  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    if( touch.GetState( 0 ) == PointState::UP )
    {
      Property::Array array;
      array.PushBack( TRANSITION_START_ANGLE );
      array.PushBack( TRANSITION_SWEEP_ANGLE );

      TransitionData transitionData = TransitionData::New( array );
      Animation animation = DevelControl::CreateTransition( Toolkit::Internal::GetImplementation( mControl ), transitionData );
      animation.Play();
    }
    return true;
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Up )
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;
  Control mControl;
  TextLabel mStartAngleLabel;
  TextLabel mSweepAngleLabel;
  TextLabel mThicknessLabel;
  TextLabel mPlusTextLabel;
  TextLabel mMinusTextLabel;
  Property::Index mSelectedPoperty;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  ArcVisualExample test( application );
  application.MainLoop();
  return 0;
}
