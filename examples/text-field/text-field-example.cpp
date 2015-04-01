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

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* const BACKGROUND_IMAGE = DALI_IMAGE_DIR "button-up.9.png";

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

    mContainer = Control::New();
    mContainer.SetName( "Container" );
    mContainer.SetParentOrigin( ParentOrigin::CENTER );
    mContainer.SetResizePolicy( FIXED, ALL_DIMENSIONS );
    mContainer.SetPreferredSize( Vector2(stageSize.width*0.6f, stageSize.width*0.6f) );
    mContainer.SetBackgroundImage( ResourceImage::New( BACKGROUND_IMAGE ) );
    mContainer.GetChildAt(0).SetZ(-1.0f);
    stage.Add( mContainer );

    TextField field = TextField::New();
    field.SetParentOrigin( ParentOrigin::CENTER );
    field.SetResizePolicy( FIXED, ALL_DIMENSIONS );
    field.SetPreferredSize( Vector2(stageSize.width*0.6f, stageSize.width*0.6f) );

    mContainer.Add( field );

    field.SetProperty( TextField::Property::TEXT, "Hello" );

    Property::Value fieldText = field.GetProperty( TextField::Property::TEXT );
    std::cout << "Displaying text: " << fieldText.Get< std::string >() << std::endl;
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

  Control mContainer;
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
