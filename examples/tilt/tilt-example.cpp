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

#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/adaptor-framework/tilt-sensor.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

// This example shows how to use sensor using a simple TextLabel
//
class TiltController : public ConnectionTracker
{
public:
  TiltController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TiltController::Create );
  }

  ~TiltController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::BLUE);

    mTextLabel = TextLabel::New( "Tilt Sensor Demo" );
    mTextLabel.SetParentOrigin( ParentOrigin::CENTER );
    mTextLabel.SetAnchorPoint( AnchorPoint::CENTER );
    mTextLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mTextLabel.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mTextLabel.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE );
    mTextLabel.SetProperty( TextLabel::Property::POINT_SIZE, 15.0f );
    mTextLabel.SetName( "tiltLabel" );
    stage.Add( mTextLabel );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &TiltController::OnTouch );

    CreateSensor();

    // Connect signals to allow Back and Escape to exit.
    stage.KeyEventSignal().Connect( this, &TiltController::OnKeyEvent );
  }

  void CreateSensor()
  {
    mTiltSensor = TiltSensor::Get();
    if ( mTiltSensor.Start() )
    {
      // Get notifications when the device is tilted
      mTiltSensor.TiltedSignal().Connect( this, &TiltController::OnTilted );
    }
  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  void OnTilted(const TiltSensor& sensor)
  {
    Quaternion pitchRot(Radian(Degree(sensor.GetPitch() * 90.0f)), Vector3(1, 0, 0));
    Quaternion rollRot(Radian(Degree(sensor.GetRoll() * 90.0f)), Vector3(0, 1, 0));

    mTextLabel.SetOrientation(Quaternion());
    mTextLabel.RotateBy(rollRot);
    mTextLabel.RotateBy(pitchRot);;
  }

  /**
   * @brief OnKeyEvent signal handler.
   * @param[in] event The key event information
   */
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
  TiltSensor mTiltSensor;
  TextLabel mTextLabel;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  TiltController test( application );

  application.MainLoop();
  return 0;
}
