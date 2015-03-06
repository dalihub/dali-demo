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

// INTERNAL INCLUDES
#include "vertical-layout.h"
#include "shared/multi-language-strings.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/text-abstraction/text-abstraction.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace MultiLanguageStrings;

/**
 * @brief The main class of the demo.
 */
class TextLabelMultiLanguageExample : public ConnectionTracker
{
public:

  TextLabelMultiLanguageExample( Application& application )
  : mApplication( application ),
    mLastPoint( 0.f )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextLabelMultiLanguageExample::Create );
  }

  ~TextLabelMultiLanguageExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    stage.KeyEventSignal().Connect(this, &TextLabelMultiLanguageExample::OnKeyEvent);

    mLayout = VerticalLayout::New();
    mLayout.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mLayout.SetAnchorPoint( AnchorPoint::TOP_LEFT );

    stage.Add( mLayout );

    for( unsigned int index = 0u; index < NUMBER_OF_LANGUAGES; ++index )
    {
      const Language& language = LANGUAGES[index];

      TextLabel label = TextLabel::New();
      label.SetParentOrigin( ParentOrigin::TOP_CENTER );
      label.SetAnchorPoint( AnchorPoint::TOP_CENTER );

      label.SetProperty( TextLabel::Property::MULTI_LINE, true );

      const std::string text = language.languageName + " " + language.languageRomanName + " " + language.text;

      label.SetProperty( TextLabel::Property::TEXT, text );
      mLayout.AddLabel( label );

      mLayout.TouchedSignal().Connect( this, &TextLabelMultiLanguageExample::OnTouchEvent );
    }

    const Vector2& size = Stage::GetCurrent().GetSize();
    const float height = mLayout.GetHeightForWidth( size.width );
    mLayout.SetSize( Size( size.width, height ) );
  }

  bool OnTouchEvent( Actor actor, const TouchEvent& event )
  {
    if( 1u == event.GetPointCount() )
    {
      const TouchPoint::State state = event.GetPoint(0u).state;

      // Clamp to integer values; this is to reduce flicking due to pixel misalignment
      const float localPoint = static_cast<float>( static_cast<int>( event.GetPoint( 0 ).local.y ) );

      if( TouchPoint::Down == state )
      {
        mLastPoint = localPoint;
        mAnimation = Animation::New( 0.25f );
      }
      else if( TouchPoint::Motion == state )
      {
        if( mAnimation )
        {
          mAnimation.MoveBy( mLayout, Vector3( 0.f, localPoint - mLastPoint, 0.f ), AlphaFunctions::Linear );
          mAnimation.Play();
          mLastPoint = localPoint;
        }
      }
    }

    return true;
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

  Application&   mApplication;
  VerticalLayout mLayout;
  Animation      mAnimation;
  float          mLastPoint;
};

void RunTest( Application& application )
{
  TextLabelMultiLanguageExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
