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
 * @file message-field-example.cpp
 * @brief Basic usage of TextField control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  const char* DESKTOP_IMAGE( DALI_IMAGE_DIR "woodEffect.jpg" );
  const Vector2 DESKTOP_SIZE( Vector2( 1440, 1600 ) );
  const Vector2 PHOTOBOX_SIZE( Vector2(330.0f, 80.0f ) );
  const float MAX_OFFSCREEN_RENDERING_SIZE = 2048.f;
  const float SCREEN_BORDER = 5.0f; // Border around screen that Popups and handles will not exceed
}
/**
 * @brief The main class of the demo.
 */
class TextMessageFieldExample : public ConnectionTracker
{
public:

  TextMessageFieldExample( Application& application )
  : mApplication( application ),
    mTargetActorPosition(),
    mTargetActorSize()
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextMessageFieldExample::Create );
  }

  ~TextMessageFieldExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    mStageSize = stage.GetSize();

    stage.KeyEventSignal().Connect(this, &TextMessageFieldExample::OnKeyEvent);

    // Create Root actor
    Actor rootActor = Actor::New();
    rootActor.SetName("rootActor");
    rootActor.SetResizePolicy( ResizePolicy::FIXED,  Dimension::ALL_DIMENSIONS );
    rootActor.SetSize( mStageSize );
    rootActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );

    stage.Add( rootActor );

    const Size mTargetActorSize( mStageSize.width, DESKTOP_SIZE.height );

    // Create Desktop
    ImageView desktop = ImageView::New( DESKTOP_IMAGE );
    desktop.SetName("desktopActor");
    desktop.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    desktop.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    desktop.SetSize( mTargetActorSize );

    rootActor.Add( desktop ); // Add desktop (content) to offscreen actor

    // Create Photo Box A
    Control photoBoxA = Control::New();

    Dali::Property::Map border;
    border.Insert( "renderer-type", "border-renderer" );
    border.Insert( "border-color", Color::WHITE );
    border.Insert( "border-size", 1.f );
    photoBoxA.SetProperty( Control::Property::BACKGROUND, border );

    photoBoxA.SetName("photoBoxA");
    photoBoxA.SetAnchorPoint( AnchorPoint::CENTER );
    photoBoxA.SetParentOrigin( ParentOrigin::CENTER );
    photoBoxA.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    photoBoxA.SetSize( PHOTOBOX_SIZE );
    photoBoxA.SetPosition( 0.0f, -500.0f, 1.0f );
    desktop.Add( photoBoxA );

    // Create TextField
    TextField field = TextField::New();
    field.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    field.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    field.SetPadding( Padding( 1.0f, 1.0f, 1.0f, 1.0f ) );
    field.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    field.SetProperty( TextField::Property::TEXT, "Enter Title name"  );
    field.SetProperty( TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>( SCREEN_BORDER, SCREEN_BORDER, mStageSize.width - SCREEN_BORDER*2, mStageSize.height - SCREEN_BORDER*2 ) );
    photoBoxA.Add( field );

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect(this, &TextMessageFieldExample::OnPanGesture );
    mPanGestureDetector.Attach( desktop );
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

  void OnPanGesture( Actor actor, const PanGesture& gesture )
  {
    if( gesture.state == Gesture::Continuing )
    {
      Vector2 position = Vector2( gesture.displacement );
      mTargetActorPosition.y = mTargetActorPosition.y + position.y;
      mTargetActorPosition.y = std::min( mTargetActorPosition.y, -mTargetActorSize.height );
      mTargetActorPosition.y = std::max( mTargetActorPosition.y, ( mTargetActorSize.height - mStageSize.height*0.25f ) );
      actor.SetPosition( 0.0f, mTargetActorPosition.y );
    }
  }

private:

  Application& mApplication;
  PanGestureDetector mPanGestureDetector;

  Vector2 mTargetActorPosition;
  Vector2 mTargetActorSize;
  Vector2 mStageSize;
};

void RunTest( Application& application )
{
  TextMessageFieldExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DALI_DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
