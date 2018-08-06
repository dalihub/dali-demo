/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

using namespace Dali;
using Dali::Toolkit::TextLabel;

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    TextLabel textLabel = TextLabel::New( "Hello World" );
    textLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    textLabel.SetName( "helloWorldLabel" );
    textLabel.SetParentOrigin( ParentOrigin::CENTER );
    textLabel.SetAnchorPoint( AnchorPoint::CENTER );
    textLabel.SetBackgroundColor( Color::RED );
    textLabel.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX );
    stage.Add( textLabel );

    TextLabel textLabel2 = TextLabel::New( "2nd One" );
    textLabel2.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    textLabel2.SetParentOrigin( ParentOrigin::TOP_CENTER );
    textLabel2.SetBackgroundColor( Color::YELLOW );
    textLabel2.SetPosition( 100.0f, 10.0f );
    textLabel.Add( textLabel2 );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &HelloWorldController::OnTouch );

    // Respond to key events
    stage.KeyEventSignal().Connect( this, &HelloWorldController::OnKeyEvent );
  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // quit the application
    mApplication.Quit();
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
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  HelloWorldController test( application );
  application.MainLoop();
  return 0;
}
