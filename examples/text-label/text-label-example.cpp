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
 * @file text-label-example.cpp
 * @brief Basic usage of TextLabel control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/text-abstraction/text-abstraction.h>

// INTERNAL INCLUDES
#include "center-layout.h"
#include "shared/multi-language-strings.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace MultiLanguageStrings;

namespace
{
  const unsigned int KEY_ZERO = 10;
  const unsigned int KEY_ONE = 11;
  const unsigned int KEY_A = 38;
  const unsigned int KEY_M = 58;
  const unsigned int KEY_L = 46;

  const char* ALIGNMENT_STRING_TABLE[] =
  {
    "BEGIN",
    "CENTER",
    "END"
  };

  const unsigned int ALIGNMENT_STRING_COUNT = sizeof( ALIGNMENT_STRING_TABLE ) / sizeof( ALIGNMENT_STRING_TABLE[0u] );
}

/**
 * @brief The main class of the demo.
 */
class TextLabelExample : public ConnectionTracker
{
public:

  TextLabelExample( Application& application )
  : mApplication( application ),
    mLanguageId( 0u ),
    mAlignment( 0u )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextLabelExample::Create );
  }

  ~TextLabelExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    stage.SetBackgroundColor( Color::BLUE );
    stage.KeyEventSignal().Connect(this, &TextLabelExample::OnKeyEvent);
    Vector2 stageSize = stage.GetSize();

    CenterLayout centerLayout = CenterLayout::New();
    centerLayout.SetParentOrigin( ParentOrigin::CENTER );
    centerLayout.SetSize( stageSize.width*0.6f, stageSize.width*0.6f );
    stage.Add( centerLayout );

    mLabel = TextLabel::New();
    mLabel.SetBackgroundColor( Color::BLACK );
    centerLayout.Add( mLabel );

    mLabel.SetProperty( TextLabel::Property::MULTI_LINE, true );
    mLabel.SetProperty( TextLabel::Property::TEXT, "A Quick Brown Fox Jumps Over The Lazy Dog" );

    Property::Value labelText = mLabel.GetProperty( TextLabel::Property::TEXT );
    std::cout << "Displaying text: \"" << labelText.Get< std::string >() << "\"" << std::endl;
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
          case KEY_ZERO: // fall through
          case KEY_ONE:
          {
            mLabel.SetProperty( TextLabel::Property::RENDERING_BACKEND, event.keyCode - 10 );
            break;
          }
          case KEY_A:
          {
            if( ++mAlignment >= ALIGNMENT_STRING_COUNT )
            {
              mAlignment = 0u;
            }

            mLabel.SetProperty( TextLabel::Property::ALIGNMENT, ALIGNMENT_STRING_TABLE[ mAlignment ] );
            break;
          }
          case KEY_M:
          {
            bool multiLine = mLabel.GetProperty<bool>( TextLabel::Property::MULTI_LINE );
            mLabel.SetProperty( TextLabel::Property::MULTI_LINE, !multiLine );
            break;
          }
          case KEY_L:
          {
            const Language& language = LANGUAGES[ mLanguageId ];

            mLabel.SetProperty( TextLabel::Property::TEXT, language.text );

            if( ++mLanguageId >= NUMBER_OF_LANGUAGES )
            {
              mLanguageId = 0u;
            }
            break;
          }
        }
      }
    }
  }

private:

  Application& mApplication;

  TextLabel mLabel;

  unsigned int mLanguageId;
  unsigned int mAlignment;
};

void RunTest( Application& application )
{
  TextLabelExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
