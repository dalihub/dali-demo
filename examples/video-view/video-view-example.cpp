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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
  const int SEEK_POS( 5000 );
  const int INIT_WIDTH( 600 );
  const int INIT_HEIGHT( 400 );
  const int BUTTON_SIZE( 80 );

  const char* const PLAY_FILE = DEMO_VIDEO_DIR "big_buck_bunny.mp4";
  const char* const PLAY_IMAGE = DEMO_IMAGE_DIR "icon-play.png";
  const char* const PAUSE_IMAGE = DEMO_IMAGE_DIR "Pause.png";
  const char* const STOP_IMAGE = DEMO_IMAGE_DIR "icon-stop.png";
  const char* const RESET_IMAGE = DEMO_IMAGE_DIR "icon-reset.png";
  const char* const FORWARD_IMAGE = DEMO_IMAGE_DIR "Forward.png";
  const char* const BACKWARD_IMAGE = DEMO_IMAGE_DIR "Backward.png";

const char* DEFAULT_FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
  }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  precision mediump float;
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  \n
  void main()\n
  {\n
    vec2 st = vTexCoord.st;\n
    vec3 irgb = texture2D( sTexture, st ).rgb;\n
    vec3 negative = vec3( 1., 1., 1. ) - irgb;\n
    gl_FragColor = vec4( mix( irgb, negative, 1.0), 1. ) * uColor;\n
  }\n
);

}  // namespace

class VideoViewController: public ConnectionTracker
{
 public:

  VideoViewController( Application& application )
    : mApplication( application ),
      mIsPlay( false ),
      mIsStop( false ),
      mIsFullScreen( false ),
      mSetCustomShader( false ),
      mScale( 1.f )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &VideoViewController::Create );
  }

  ~VideoViewController()
  {
    mVideoView.Stop();
  }

  void Create( Application& application )
  {
    mStageSize = Stage::GetCurrent().GetSize();

    mVideoView = Toolkit::VideoView::New();
    Stage::GetCurrent().Add( mVideoView );
    mVideoView.SetParentOrigin( ParentOrigin::CENTER );
    mVideoView.SetAnchorPoint( AnchorPoint::CENTER );
    mVideoView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    mVideoView.SetSize( INIT_WIDTH, INIT_HEIGHT );
    mVideoView.SetProperty( VideoView::Property::LOOPING, true );
    mVideoView.SetProperty( VideoView::Property::MUTED, false );
    mVideoView.SetProperty( VideoView::Property::VIDEO, PLAY_FILE );

    mMenu = Layer::New();
    mMenu.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    mMenu.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    mMenu.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mMenu.SetSize( INIT_WIDTH, 120 );
    mVideoView.Add( mMenu );

    mPlayButton = PushButton::New();
    mPlayButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mPlayButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mPlayButton.SetName( "Play" );
    mPlayButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mPlayButton.SetSize( BUTTON_SIZE, BUTTON_SIZE );
    mPlayButton.SetPosition( 40, 10 );
    mPlayButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mPauseButton = PushButton::New();
    mPauseButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mPauseButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mPauseButton.SetName( "Pause" );
    mPauseButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mPauseButton.SetSize( BUTTON_SIZE, BUTTON_SIZE );
    mPauseButton.SetPosition( 40, 10 );
    mPauseButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mStopButton = PushButton::New();
    mStopButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mStopButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mStopButton.SetName( "Stop" );
    mStopButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mStopButton.SetSize( BUTTON_SIZE, BUTTON_SIZE );
    mStopButton.SetPosition( 140, 10 );
    mStopButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mResetButton = PushButton::New();
    mResetButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mResetButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mResetButton.SetName( "Reset" );
    mResetButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mResetButton.SetSize( BUTTON_SIZE, BUTTON_SIZE );
    mResetButton.SetPosition( 140, 10 );
    mResetButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mBackwardButton = PushButton::New();
    mBackwardButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mBackwardButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mBackwardButton.SetName( "Backward" );
    mBackwardButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mBackwardButton.SetSize( BUTTON_SIZE, BUTTON_SIZE );
    mBackwardButton.SetPosition( 240, 10 );
    mBackwardButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mForwardButton = PushButton::New();
    mForwardButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mForwardButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mForwardButton.SetName( "Forward" );
    mForwardButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mForwardButton.SetSize( BUTTON_SIZE, BUTTON_SIZE );
    mForwardButton.SetPosition( 340, 10 );
    mForwardButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mMenu.Add( mPlayButton );
    mMenu.Add( mPauseButton );
    mMenu.Add( mStopButton );
    mMenu.Add( mResetButton );
    mMenu.Add( mBackwardButton );
    mMenu.Add( mForwardButton );

    mPauseButton.SetVisible( false );
    mPauseButton.SetDisabled( true );
    mPlayButton.SetVisible( true );
    mPlayButton.SetDisabled( false );
    mStopButton.SetVisible( true );
    mStopButton.SetDisabled( false );
    mResetButton.SetVisible( false );
    mResetButton.SetDisabled( true );

    mPlayButton.SetUnselectedImage( PLAY_IMAGE );
    mPlayButton.SetSelectedImage( PLAY_IMAGE );
    mPauseButton.SetUnselectedImage( PAUSE_IMAGE );
    mPauseButton.SetSelectedImage( PAUSE_IMAGE );

    mStopButton.SetUnselectedImage( STOP_IMAGE );
    mStopButton.SetSelectedImage( STOP_IMAGE );
    mResetButton.SetUnselectedImage( RESET_IMAGE );
    mResetButton.SetSelectedImage( RESET_IMAGE );

    mBackwardButton.SetUnselectedImage( BACKWARD_IMAGE );
    mBackwardButton.SetSelectedImage( BACKWARD_IMAGE );
    mForwardButton.SetUnselectedImage( FORWARD_IMAGE );
    mForwardButton.SetSelectedImage( FORWARD_IMAGE );

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( mVideoView );
    mPanGestureDetector.DetectedSignal().Connect( this, &VideoViewController::OnPan );

    mPinchGestureDetector = PinchGestureDetector::New();
    mPinchGestureDetector.Attach( mVideoView );
    mPinchGestureDetector.DetectedSignal().Connect( this, &VideoViewController::OnPinch );

    mTapGestureDetector = TapGestureDetector::New();
    mTapGestureDetector.Attach( mVideoView );
    mTapGestureDetector.DetectedSignal().Connect( this, &VideoViewController::OnTap );

    mRotationAnimation = Animation::New(2.f);
    mRotationAnimation.AnimateBy( Property(mVideoView, Actor::Property::ORIENTATION), Quaternion(Degree(0.f), Degree(360.f), Degree(0.f)) );
    mRotationAnimation.SetLooping(false);

    Stage::GetCurrent().KeyEventSignal().Connect( this, &VideoViewController::OnKeyEvent );

    Property::Map customShader;
    customShader.Insert( Visual::Shader::Property::FRAGMENT_SHADER, FRAGMENT_SHADER );
    mCustomShader.Insert( Visual::Property::TYPE, Visual::IMAGE );
    mCustomShader.Insert( Visual::Property::SHADER, customShader );

    Property::Map defaultShader;
    customShader.Insert( Visual::Shader::Property::FRAGMENT_SHADER, DEFAULT_FRAGMENT_SHADER );
    mDefaultShader.Insert( Visual::Property::TYPE, Visual::IMAGE );
    mDefaultShader.Insert( Visual::Property::SHADER, customShader );

    mWindowSurfaceTarget.Insert( "RENDERING_TARGET", "windowSurfaceTarget" );
    mNativeImageTarget.Insert( "RENDERING_TARGET", "nativeImageTarget" );

    mVideoView.FinishedSignal().Connect( this, &VideoViewController::OnFinished );
  }

  bool OnButtonClicked( Button button )
  {

    if( mPauseButton.GetId() == button.GetId())
    {
       if( mIsPlay )
      {
        mPauseButton.SetVisible( false );
        mPauseButton.SetDisabled( true );
        mPlayButton.SetVisible( true );
        mPlayButton.SetDisabled( false );

        mIsPlay = false;
        mVideoView.Pause();
      }
    }
    else if( mPlayButton.GetId() == button.GetId())
    {
      if( mIsStop )
      {
        return false;
      }

      mPauseButton.SetVisible( true );
      mPauseButton.SetDisabled( false );
      mPlayButton.SetVisible( false );
      mPlayButton.SetDisabled( true );

      mIsPlay = true;
      mVideoView.Play();
    }
    else if( mResetButton.GetId() == button.GetId())
    {
      if( mIsStop )
      {
        mPauseButton.SetVisible( true );
        mPauseButton.SetDisabled( false );
        mPlayButton.SetVisible( false );
        mPlayButton.SetDisabled( true );
        mResetButton.SetVisible( false );
        mResetButton.SetDisabled( true );
        mStopButton.SetVisible( true );
        mStopButton.SetDisabled( false );

        mIsStop = false;
        mIsPlay = true;
        mVideoView.SetProperty( VideoView::Property::VIDEO, PLAY_FILE );
        mVideoView.Play();
      }
    }
    else if( mStopButton.GetId() == button.GetId())
    {
      mPauseButton.SetVisible( false );
      mPauseButton.SetDisabled( true );
      mPlayButton.SetVisible( true );
      mPlayButton.SetDisabled( false );
      mResetButton.SetVisible( true );
      mResetButton.SetDisabled( false );
      mStopButton.SetVisible( false );
      mStopButton.SetDisabled( true );

      mIsStop = true;
      mVideoView.Stop();
    }
    else if( mBackwardButton.GetId() == button.GetId())
    {
      mVideoView.Backward( SEEK_POS );
    }
    else if( mForwardButton.GetId() == button.GetId())
    {
      mVideoView.Forward( SEEK_POS );
    }

    return true;
  }

  void OnFinished( VideoView& view )
  {
    if( !mIsFullScreen )
    {
      mRotationAnimation.Play();
      mIsStop = true;
    }
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    if( !mIsFullScreen && gesture.state == Gesture::Continuing )
    {
      mVideoView.TranslateBy( Vector3( gesture.displacement ) );
    }
  }

  void OnPinch( Actor actor, const PinchGesture& gesture )
  {
    if( gesture.state == Gesture::Started )
    {
      mPinchStartScale = mScale;
    }

    if( gesture.state == Gesture::Finished )
    {
      mScale = mPinchStartScale * gesture.scale;
      if( mScale > 1.f )
      {
        mVideoView.SetSize( mStageSize.x, mStageSize.y );
        mVideoView.SetProperty( VideoView::Property::VIDEO, mWindowSurfaceTarget );
        mMenu.SetSize( mStageSize.x, 120 );
        mIsFullScreen = true;
      }
      else
      {
        mVideoView.SetSize( INIT_WIDTH, INIT_HEIGHT );
        mVideoView.SetProperty( VideoView::Property::VIDEO, mNativeImageTarget );
        mMenu.SetSize( INIT_WIDTH, 120 );
        mIsFullScreen = false;
      }
    }
  }

  void OnTap( Actor actor, const TapGesture& tapGesture )
  {
    if( !mIsFullScreen )
    {
      mRotationAnimation.Play();

      if( mSetCustomShader )
      {
        mSetCustomShader = false;
        mVideoView.SetProperty( VideoView::Property::VIDEO, mDefaultShader );
      }
      else
      {
        mSetCustomShader = true;
        mVideoView.SetProperty( VideoView::Property::VIDEO, mCustomShader );
      }
    }
  }

private:

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
  VideoView mVideoView;
  Layer mMenu;
  Vector2 mStageSize;

  bool mIsPlay;
  bool mIsStop;
  bool mIsFullScreen;
  bool mSetCustomShader;

  PushButton mPlayButton;
  PushButton mPauseButton;
  PushButton mStopButton;
  PushButton mResetButton;
  PushButton mBackwardButton;
  PushButton mForwardButton;

  PanGestureDetector mPanGestureDetector;
  PinchGestureDetector mPinchGestureDetector;
  TapGestureDetector mTapGestureDetector;
  float mScale;
  float mPinchStartScale;

  Animation mRotationAnimation;
  Property::Map mCustomShader;
  Property::Map mDefaultShader;
  Property::Map mWindowSurfaceTarget;
  Property::Map mNativeImageTarget;
};

void RunTest( Application& application )
{
  VideoViewController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
