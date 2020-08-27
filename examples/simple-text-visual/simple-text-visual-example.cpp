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

/**
 * @file text-visual-example.cpp
 * @brief Basic usage of Text Visual.
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

/**
 * @brief The main class of the demo.
 */
class TextVisualExample : public ConnectionTracker
{
public:

  TextVisualExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextVisualExample::Create );
  }

  ~TextVisualExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Window window = application.GetWindow();

    window.KeyEventSignal().Connect(this, &TextVisualExample::OnKeyEvent);
    window.SetBackgroundColor( Color::WHITE );

    Dali::Toolkit::Control control = Dali::Toolkit::ImageView::New();
    control.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );

    const std::string markupText( "<color value='blue'><font size='50'>H</font></color>ello <color value='blue'><font size='50'>w</font></color>orld" );

    Dali::Property::Map map;
    map.Add( Dali::Toolkit::Visual::Property::TYPE, Dali::Toolkit::Visual::TEXT ).
      Add( Dali::Toolkit::TextVisual::Property::ENABLE_MARKUP, true ).
      Add( Dali::Toolkit::TextVisual::Property::TEXT, markupText ).
      Add( Dali::Toolkit::TextVisual::Property::TEXT_COLOR, Dali::Vector4( 0.25f, 0.25f, 0.5f, 1.f ) ).
      Add( Dali::Toolkit::TextVisual::Property::FONT_FAMILY, "TizenSansRegular" ).
      Add( Dali::Toolkit::TextVisual::Property::POINT_SIZE, 30.f ).
      Add( Dali::Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT, "END" ).
      Add( Dali::Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT, "CENTER" );

    control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );

    window.Add( control );
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
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
  TextVisualExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
