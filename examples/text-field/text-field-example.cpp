/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 * @file text-field-example.cpp
 * @brief Basic usage of TextField control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/text-abstraction/text-abstraction.h>

// INTERNAL INCLUDES
#include "edit-layout.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const float BORDER_WIDTH = 4.0f;

} // unnamed namespace

/**
 * @brief The main class of the demo.
 */
class TextFieldExample : public ConnectionTracker
{
public:

  TextFieldExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextFieldExample::Create );
  }

  ~TextFieldExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    stage.KeyEventSignal().Connect(this, &TextFieldExample::OnKeyEvent);

    Vector2 stageSize = stage.GetSize();

    EditLayout layout = EditLayout::New();
    layout.SetParentOrigin( ParentOrigin::CENTER );
    layout.SetAnchorPoint( AnchorPoint::CENTER );
    layout.SetSize( stageSize.width - BORDER_WIDTH*2.0f, stageSize.height*0.2f );
    stage.Add( layout );

    TextField field = TextField::New();
    field.SetParentOrigin( ParentOrigin::CENTER );
    field.SetBackgroundColor( Color::BLACK );
    layout.SetTopPanel( field );

    field.SetProperty( TextField::PROPERTY_TEXT, "A Quick Brown Fox Jumps Over The Lazy Dog" );

    // TODO
    //Property::Value fieldText = field.GetProperty( TextField::PROPERTY_TEXT );
    //std::cout << "Got text from field: " << fieldText.Get< std::string >() << std::endl;
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
};

void RunTest( Application& application )
{
  TextFieldExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
