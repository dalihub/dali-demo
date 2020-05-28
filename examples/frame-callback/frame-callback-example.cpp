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

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/common/stage-devel.h>

// INTERNAL INCLUDES
#include "frame-callback.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * IMAGE_NAME = DEMO_IMAGE_DIR "application-icon-1.png";

const char * TEXT_ENABLED( "FrameCallback: ON" );
const char * TEXT_DISABLED( "FrameCallback: OFF" );
Vector4 TEXT_COLOR_ENABLED( Color::BLACK );
Vector4 TEXT_COLOR_DISABLED( Color::RED );

float ANIMATION_TIME( 4.0f );
float ANIMATION_PROGRESS_MULTIPLIER( 0.02f );
} // unnamed namespace

/**
 * @brief An example of how to set/unset the FrameCallbackInterface in DALi.
 *
 * Creates a scene with several image-views which are animated from side-to-side.
 * With the frame-callback enabled, the image-views' sizes expand as they hits the sides and the opacity
 * changes to transparent as they go to the middle.
 */
class FrameCallbackController : public ConnectionTracker
{
public:

  /**
   * @brief Constructor.
   * @param[in]  application  The application.
   */
  FrameCallbackController( Application& application )
  : mApplication( application ),
    mStage(),
    mFrameCallback(),
    mTextLabel(),
    mTapDetector(),
    mFrameCallbackEnabled( false )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &FrameCallbackController::Create );
  }

private:

  /**
   * @brief Creates the scene.
   *
   * Creates several image-views and places them appropriately.
   * Animate all image-views.
   * Set the FrameCallbackInterface on the stage.
   * Tapping on the stage enables/disables the FrameCallback.
   */
  void Create( Application& /* application */ )
  {
    // Hide the indicator bar.
    mApplication.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    // Set the stage background color and connect to the stage's key signal to allow Back and Escape to exit.
    mStage = Stage::GetCurrent();
    mStage.SetBackgroundColor( Color::WHITE );
    mStage.KeyEventSignal().Connect( this, &FrameCallbackController::OnKeyEvent );

    // Notify mFrameCallback about the stage width.
    // Can call methods in mFrameCallback directly as we have not set it on the stage yet.
    Vector2 stageSize = mStage.GetSize();
    mFrameCallback.SetStageWidth( stageSize.width );

    // Detect taps on the root layer.
    mTapDetector = TapGestureDetector::New();
    mTapDetector.Attach( mStage.GetRootLayer() );
    mTapDetector.DetectedSignal().Connect( this, &FrameCallbackController::OnTap );

    // Create some key-frames to be used by all animations.
    KeyFrames keyFrames = KeyFrames::New();
    keyFrames.Add( 0.0f,   0.0f );
    keyFrames.Add( 0.25f,  stageSize.width * 0.5f );
    keyFrames.Add( 0.75f, -stageSize.width * 0.5f );
    keyFrames.Add( 1.0f,   0.0f );

    float yPos = 0.0f;
    for( int i = 0; yPos < stageSize.height; ++i )
    {
      ImageView imageView = ImageView::New( IMAGE_NAME );
      imageView.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER );
      imageView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER );
      imageView.SetY( yPos );
      yPos += imageView.GetNaturalSize().height;

      // Add the ID of the created ImageView to mFrameCallback.
      // Again, can call methods in mFrameCallback directly as we have not set it on the stage yet.
      mFrameCallback.AddId( imageView.GetId() );

      mStage.Add( imageView );

      // Create an animation and set the progress so that each image starts at a different point.
      Animation animation = Animation::New( ANIMATION_TIME );
      animation.SetLooping( true );
      animation.AnimateBetween( Property( imageView, Actor::Property::POSITION_X ), keyFrames );
      animation.SetCurrentProgress( std::min( 1.0f, ANIMATION_PROGRESS_MULTIPLIER * i ) );
      animation.Play();
    }

    // Create a text-label to display whether the FrameCallback is enabled/disabled.
    mTextLabel = TextLabel::New( TEXT_ENABLED );
    mTextLabel.SetProperty( TextLabel::Property::TEXT_COLOR, TEXT_COLOR_ENABLED );
    mTextLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mTextLabel.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    mTextLabel.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mStage.Add( mTextLabel );

    // Set the FrameCallbackInterface on the root layer.
    DevelStage::AddFrameCallback( mStage, mFrameCallback, mStage.GetRootLayer() );
    mFrameCallbackEnabled = true;
  }

  /**
   * @brief Called when a tap on the stage occurs.
   *
   * Toggle enabling/disabling of the FrameCallbackInterface
   */
  void OnTap( Actor /* actor */, const TapGesture& /* tap */ )
  {
    if( mFrameCallbackEnabled )
    {
      DevelStage::RemoveFrameCallback( mStage, mFrameCallback );
      mTextLabel.SetProperty( TextLabel::Property::TEXT, TEXT_DISABLED );
      mTextLabel.SetProperty( TextLabel::Property::TEXT_COLOR, TEXT_COLOR_DISABLED );
    }
    else
    {
      DevelStage::AddFrameCallback( mStage, mFrameCallback, mStage.GetRootLayer() );
      mTextLabel.SetProperty( TextLabel::Property::TEXT, TEXT_ENABLED );
      mTextLabel.SetProperty( TextLabel::Property::TEXT_COLOR, TEXT_COLOR_ENABLED );
    }

    mFrameCallbackEnabled = !mFrameCallbackEnabled;
  }

  /**
   * @brief Called when any key event is received
   *
   * Will use this to quit the application if Back or the Escape key is received
   * @param[in] event The key event information
   */
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
  Application&        mApplication;          ///< A reference to the application instance.
  Stage               mStage;                ///< The stage we enable the FrameCallback on.
  FrameCallback       mFrameCallback;        ///< An instance of our implementation of the FrameCallbackInterface.
  TextLabel           mTextLabel;            ///< Text label which shows whether the frame-callback is enabled/disabled.
  TapGestureDetector  mTapDetector;          ///< Tap detector to enable/disable the FrameCallbackInterface.
  bool                mFrameCallbackEnabled; ///< Stores whether the FrameCallbackInterface is enabled/disabled.
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  FrameCallbackController controller( application );
  application.MainLoop();
  return 0;
}
