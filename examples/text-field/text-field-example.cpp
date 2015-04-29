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
#include "shared/multi-language-strings.h"
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace MultiLanguageStrings;

namespace
{

  const char* const BACKGROUND_IMAGE = DALI_IMAGE_DIR "button-up.9.png";

  const float BORDER_WIDTH = 4.0f;

  const unsigned int KEY_ZERO = 10;
  const unsigned int KEY_ONE = 11;
  const unsigned int KEY_F = 41;
  const unsigned int KEY_H = 43;
  const unsigned int KEY_V = 55;
  const unsigned int KEY_M = 58;
  const unsigned int KEY_L = 46;
  const unsigned int KEY_S = 39;
  const unsigned int KEY_PLUS = 21;
  const unsigned int KEY_MINUS = 20;

  const char* H_ALIGNMENT_STRING_TABLE[] =
  {
    "BEGIN",
    "CENTER",
    "END"
  };

  const unsigned int H_ALIGNMENT_STRING_COUNT = sizeof( H_ALIGNMENT_STRING_TABLE ) / sizeof( H_ALIGNMENT_STRING_TABLE[0u] );

  const char* V_ALIGNMENT_STRING_TABLE[] =
  {
    "TOP",
    "CENTER",
    "BOTTOM"
  };

  const unsigned int V_ALIGNMENT_STRING_COUNT = sizeof( V_ALIGNMENT_STRING_TABLE ) / sizeof( V_ALIGNMENT_STRING_TABLE[0u] );

} // unnamed namespace

/**
 * @brief The main class of the demo.
 */
class TextFieldExample : public ConnectionTracker
{
public:

  TextFieldExample( Application& application )
  : mApplication( application ),
    mLanguageId( 0u ),
    mAlignment( 0u )
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
    DemoHelper::RequestThemeChange();

    Stage stage = Stage::GetCurrent();

    mTapGestureDetector = TapGestureDetector::New();
    mTapGestureDetector.Attach( stage.GetRootLayer() );
    mTapGestureDetector.DetectedSignal().Connect( this, &TextFieldExample::OnTap );

    stage.KeyEventSignal().Connect(this, &TextFieldExample::OnKeyEvent);

    Vector2 stageSize = stage.GetSize();

    Control container = Control::New();
    container.SetName( "Container" );
    container.SetParentOrigin( ParentOrigin::CENTER );
    container.SetSize( Vector2(stageSize.width*0.6f, stageSize.width*0.6f) );
    container.SetBackgroundColor( Color::WHITE );
    container.GetChildAt(0).SetZ(-1.0f);
    stage.Add( container );

    mField = TextField::New();
    mField.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mField.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mField.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
    mField.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Unnamed folder" );
    mField.SetProperty( TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>( BORDER_WIDTH, BORDER_WIDTH, stageSize.width - BORDER_WIDTH*2, stageSize.height - BORDER_WIDTH*2 ) );

    container.Add( mField );

    Property::Value fieldText = mField.GetProperty( TextField::Property::TEXT );

    std::cout << "Displaying text: " << fieldText.Get< std::string >() << std::endl;
  }

  void OnTap( Actor actor, const TapGesture& tapGesture )
  {
    mField.ClearKeyInputFocus();
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
      else if( event.IsCtrlModifier() )
      {
        switch( event.keyCode )
        {
          // Select rendering back-end
          case KEY_ZERO: // fall through
          case KEY_ONE:
          {
            mField.SetProperty( TextField::Property::RENDERING_BACKEND, event.keyCode - 10 );
            break;
          }
          case KEY_H: // Horizontal alignment
          {
            if( ++mAlignment >= H_ALIGNMENT_STRING_COUNT )
            {
              mAlignment = 0u;
            }

            mField.SetProperty( TextField::Property::HORIZONTAL_ALIGNMENT, H_ALIGNMENT_STRING_TABLE[ mAlignment ] );
            break;
          }
          case KEY_V: // Vertical alignment
          {
            if( ++mAlignment >= V_ALIGNMENT_STRING_COUNT )
            {
              mAlignment = 0u;
            }

            mField.SetProperty( TextField::Property::VERTICAL_ALIGNMENT, V_ALIGNMENT_STRING_TABLE[ mAlignment ] );
            break;
          }
          case KEY_L: // Language
          {
            const Language& language = LANGUAGES[ mLanguageId ];

            mField.SetProperty( TextField::Property::TEXT, language.text );

            if( ++mLanguageId >= NUMBER_OF_LANGUAGES )
            {
              mLanguageId = 0u;
            }
            break;
          }
          case KEY_S: // Shadow color
          {
            if( Color::BLACK == mField.GetProperty<Vector4>( TextField::Property::SHADOW_COLOR ) )
            {
              mField.SetProperty( TextField::Property::SHADOW_COLOR, Color::RED );
            }
            else
            {
              mField.SetProperty( TextField::Property::SHADOW_COLOR, Color::BLACK );
            }
            break;
          }
          case KEY_PLUS: // Increase shadow offset
          {
            mField.SetProperty( TextField::Property::SHADOW_OFFSET, mField.GetProperty<Vector2>( TextField::Property::SHADOW_OFFSET ) + Vector2( 1.0f, 1.0f ) );
            break;
          }
          case KEY_MINUS: // Decrease shadow offset
          {
            mField.SetProperty( TextField::Property::SHADOW_OFFSET, mField.GetProperty<Vector2>( TextField::Property::SHADOW_OFFSET ) - Vector2( 1.0f, 1.0f ) );
            break;
          }
        }
      }
    }
  }

private:

  Application& mApplication;

  TextField mField;

  TapGestureDetector mTapGestureDetector;

  unsigned int mLanguageId;
  unsigned int mAlignment;
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
