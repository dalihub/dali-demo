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

// INTERNAL INCLUDES

#include "vertical-layout.h"
#include "emoji-strings.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/text-abstraction/text-abstraction.h>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace EmojiStrings;

 // TODO Need to expose Text::RENDER.....
const int ATLAS_RENDERER = 0;

// This example shows how to create and display Hello World! using a simple TextActor
//
class EmojiExample : public ConnectionTracker
{
public:

  typedef uint32_t SizeType;

  EmojiExample( Application& application )
  : mApplication( application )
  {
    std::cout << "EmoticonController::EmoticonController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &EmojiExample::Create );
  }

  ~EmojiExample()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {

    mLayout = VerticalLayout::New();
    mLayout.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mLayout.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    Stage stage = Stage::GetCurrent();
    stage.Add( mLayout );
    stage.KeyEventSignal().Connect(this, &EmojiExample::OnKeyEvent);

    for( unsigned int index = 0u; index < NUMBER_OF_EMOJIS; ++index )
    {
      const Emoji& emoji = EMOJIS[index];
      const std::string text = emoji.mUTF8 + " " + emoji.mDescription;
      TextLabel label = TextLabel::New();
      label.SetParentOrigin( ParentOrigin::TOP_CENTER );
      label.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      label.SetProperty( TextLabel::Property::MULTI_LINE, true );
      label.SetProperty( TextLabel::Property::RENDERING_BACKEND, 0 );
      label.SetProperty( TextLabel::Property::TEXT, text );
      mLayout.AddLabel( label );
      mLayout.TouchedSignal().Connect( this, &EmojiExample::OnTouchEvent );
    }

    const Vector2& size = stage.GetSize();
    const float height = mLayout.GetHeightForWidth( size.width );
    mLayout.SetSize( Size( size.width, height ) );
    mLayout.TouchedSignal().Connect( this, &EmojiExample::OnTouchEvent );

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
  Application&  mApplication;
  VerticalLayout mLayout;
  Animation      mAnimation;
  float          mLastPoint;
};

void RunTest( Application& application )
{
  EmojiExample test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
