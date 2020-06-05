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
    mStartAngle( 0.0f ),
    mSweepAngle( 0.0f )
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
    mControl.SetProperty( Actor::Property::SIZE, Vector2( 200.0f, 200.0f ) );
    mControl.SetProperty( Control::Property::BACKGROUND, BACKGROUND );

    stage.Add( mControl );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &ArcVisualExample::OnTouch );

    // Respond to key events
    stage.KeyEventSignal().Connect( this, &ArcVisualExample::OnKeyEvent );
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
    if( event.state == KeyEvent::Down )
    {
      if ( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;
  Control mControl;
  float mStartAngle;
  float mSweepAngle;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  ArcVisualExample test( application );
  application.MainLoop();
  return 0;
}
