/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

/**
 * @file simple-text-label-example.cpp
 * @brief Basic usage of SimpleTextLabel control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

/**
 * @brief The main class of the demo.
 */
class SimpleTextLabelExample : public ConnectionTracker
{
public:

  SimpleTextLabelExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &SimpleTextLabelExample::Create );
  }

  ~SimpleTextLabelExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    stage.KeyEventSignal().Connect(this, &SimpleTextLabelExample::OnKeyEvent);

    mLabel = TextLabel::New( "A Quick Brown Fox Jumps Over The Lazy Dog" );
    mLabel.SetProperty( Dali::Actor::Property::NAME, "SimpleTextLabel" );
    mLabel.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    mLabel.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mLabel.SetSize( 400.f, 400.f );
    mLabel.SetProperty( TextLabel::Property::MULTI_LINE, true );
    mLabel.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLACK );
    mLabel.SetBackgroundColor( Color::WHITE );

    stage.Add( mLabel );
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  Application& mApplication;

  TextLabel mLabel;
};

void RunTest( Application& application )
{
  SimpleTextLabelExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
