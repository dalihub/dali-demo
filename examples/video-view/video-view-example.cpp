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

  const char* const PLAY_FILE = DEMO_VIDEO_DIR "demoVideo.mp4";
  const char* const PLAY_IMAGE = DEMO_IMAGE_DIR "icon-play.png";
  const char* const PAUSE_IMAGE = DEMO_IMAGE_DIR "Pause.png";
  const char* const CHANGE_IMAGE = DEMO_IMAGE_DIR "icon-change.png";
  const char* const FORWARD_IMAGE = DEMO_IMAGE_DIR "Forward.png";
  const char* const BACKWARD_IMAGE = DEMO_IMAGE_DIR "Backward.png";

}  // namespace

class VideoViewController: public ConnectionTracker
{
 public:

  VideoViewController( Application& application )
    : mApplication( application ),
      mIsPlay( false ),
      mIsFullScreen( false ),
      mScale( 1.f ),
      mPinchStartScale( 1.0f )
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
    Window window = application.GetWindow();
    mWindowSize = window.GetSize();

    mVideoView = Toolkit::VideoView::New();
    window.Add( mVideoView );
    mVideoView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mVideoView.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    mVideoView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    mVideoView.SetProperty( Actor::Property::SIZE, Vector2( INIT_WIDTH, INIT_HEIGHT ) );
    mVideoView.SetProperty( VideoView::Property::LOOPING, true );
    mVideoView.SetProperty( VideoView::Property::MUTED, false );
    mVideoView.SetProperty( VideoView::Property::VIDEO, PLAY_FILE );

    mMenu = Layer::New();
    mMenu.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT );
    mMenu.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_LEFT );
    mMenu.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mMenu.SetProperty( Actor::Property::SIZE, Vector2( INIT_WIDTH, 120 ) );
    mVideoView.Add( mMenu );

    mPlayButton = PushButton::New();
    mPlayButton.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
    mPlayButton.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    mPlayButton.SetProperty( Dali::Actor::Property::NAME, "Play" );
    mPlayButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mPlayButton.SetProperty( Actor::Property::SIZE, Vector2( BUTTON_SIZE, BUTTON_SIZE ) );
    mPlayButton.SetProperty( Actor::Property::POSITION, Vector2( 40, 10 ));
    mPlayButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mPauseButton = PushButton::New();
    mPauseButton.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
    mPauseButton.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    mPauseButton.SetProperty( Dali::Actor::Property::NAME, "Pause" );
    mPauseButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mPauseButton.SetProperty( Actor::Property::SIZE, Vector2( BUTTON_SIZE, BUTTON_SIZE ) );
    mPauseButton.SetProperty( Actor::Property::POSITION, Vector2( 40, 10 ));
    mPauseButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mChangeButton = PushButton::New();
    mChangeButton.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
    mChangeButton.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    mChangeButton.SetProperty( Dali::Actor::Property::NAME, "Change" );
    mChangeButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mChangeButton.SetProperty( Actor::Property::SIZE, Vector2( BUTTON_SIZE, BUTTON_SIZE ) );
    mChangeButton.SetProperty( Actor::Property::POSITION, Vector2( 140, 10 ));
    mChangeButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mBackwardButton = PushButton::New();
    mBackwardButton.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
    mBackwardButton.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    mBackwardButton.SetProperty( Dali::Actor::Property::NAME, "Backward" );
    mBackwardButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mBackwardButton.SetProperty( Actor::Property::SIZE, Vector2( BUTTON_SIZE, BUTTON_SIZE ) );
    mBackwardButton.SetProperty( Actor::Property::POSITION, Vector2( 240, 10 ));
    mBackwardButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mForwardButton = PushButton::New();
    mForwardButton.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
    mForwardButton.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    mForwardButton.SetProperty( Dali::Actor::Property::NAME, "Forward" );
    mForwardButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    mForwardButton.SetProperty( Actor::Property::SIZE, Vector2( BUTTON_SIZE, BUTTON_SIZE ) );
    mForwardButton.SetProperty( Actor::Property::POSITION, Vector2( 340, 10 ));
    mForwardButton.ClickedSignal().Connect( this, &VideoViewController::OnButtonClicked );

    mMenu.Add( mPlayButton );
    mMenu.Add( mPauseButton );
    mMenu.Add( mChangeButton );
    mMenu.Add( mBackwardButton );
    mMenu.Add( mForwardButton );

    mPauseButton.SetProperty( Actor::Property::VISIBLE, false );
    mPauseButton.SetProperty( Button::Property::DISABLED, true );
    mPlayButton.SetProperty( Actor::Property::VISIBLE, true );
    mPlayButton.SetProperty( Button::Property::DISABLED, false );
    mChangeButton.SetProperty( Actor::Property::VISIBLE, true );
    mChangeButton.SetProperty( Button::Property::DISABLED, false );

    mPlayButton.SetProperty( Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, PLAY_IMAGE );
    mPlayButton.SetProperty( Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, PLAY_IMAGE );
    mPauseButton.SetProperty( Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, PAUSE_IMAGE );
    mPauseButton.SetProperty( Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, PAUSE_IMAGE );

    mChangeButton.SetProperty( Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, CHANGE_IMAGE );
    mChangeButton.SetProperty( Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, CHANGE_IMAGE );

    mBackwardButton.SetProperty( Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, BACKWARD_IMAGE );
    mBackwardButton.SetProperty( Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, BACKWARD_IMAGE );
    mForwardButton.SetProperty( Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, FORWARD_IMAGE );
    mForwardButton.SetProperty( Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, FORWARD_IMAGE );

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

    window.KeyEventSignal().Connect( this, &VideoViewController::OnKeyEvent );
  }

  bool OnButtonClicked( Button button )
  {
    if( mPauseButton.GetProperty< int >( Actor::Property::ID ) == button.GetProperty< int >( Actor::Property::ID ))
    {
       if( mIsPlay )
      {
        mPauseButton.SetProperty( Actor::Property::VISIBLE, false );
        mPauseButton.SetProperty( Button::Property::DISABLED, true );
        mPlayButton.SetProperty( Actor::Property::VISIBLE, true );
        mPlayButton.SetProperty( Button::Property::DISABLED, false );

        mIsPlay = false;
        mVideoView.Pause();
      }
    }
    else if( mPlayButton.GetProperty< int >( Actor::Property::ID ) == button.GetProperty< int >( Actor::Property::ID ))
    {
      mPauseButton.SetProperty( Actor::Property::VISIBLE, true );
      mPauseButton.SetProperty( Button::Property::DISABLED, false );
      mPlayButton.SetProperty( Actor::Property::VISIBLE, false );
      mPlayButton.SetProperty( Button::Property::DISABLED, true );

      mIsPlay = true;
      mVideoView.Play();
    }
    else if( mChangeButton.GetProperty< int >( Actor::Property::ID ) == button.GetProperty< int >( Actor::Property::ID ))
    {
      bool underlay = false;
      underlay = mVideoView.GetProperty( Toolkit::VideoView::Property::UNDERLAY ).Get< bool >();
      if( underlay )
      {
        mVideoView.SetProperty( Toolkit::VideoView::Property::UNDERLAY, false );
      }
      else
      {
        mVideoView.SetProperty( Toolkit::VideoView::Property::UNDERLAY, true );
      }
    }
    else if( mBackwardButton.GetProperty< int >( Actor::Property::ID ) == button.GetProperty< int >( Actor::Property::ID ))
    {
      mVideoView.Backward( SEEK_POS );
    }
    else if( mForwardButton.GetProperty< int >( Actor::Property::ID ) == button.GetProperty< int >( Actor::Property::ID ))
    {
      mVideoView.Forward( SEEK_POS );
    }

    return true;
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    if( !mIsFullScreen && gesture.GetState() == Gesture::Continuing )
    {
      mVideoView.TranslateBy( Vector3( gesture.GetDisplacement() ) );
    }
  }

  void OnPinch( Actor actor, const PinchGesture& gesture )
  {
    Gesture::State state = gesture.GetState();
    if( state == Gesture::Started )
    {
      mPinchStartScale = mScale;
    }

    if( state == Gesture::Finished )
    {
      mScale = mPinchStartScale * gesture.GetScale();
      mVideoView.SetProperty( Actor::Property::SCALE, mScale );
    }
  }

  void OnTap( Actor actor, const TapGesture& tapGesture )
  {
    if( !mIsFullScreen )
    {
      mVideoView.SetProperty( Actor::Property::SIZE, mWindowSize );
      mIsFullScreen = true;
    }
    else
    {
      mVideoView.SetProperty( Actor::Property::SIZE, Vector2( INIT_WIDTH, INIT_HEIGHT ) );
      mIsFullScreen = false;
    }
  }

private:

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
  Application&  mApplication;
  VideoView mVideoView;
  Layer mMenu;
  Vector2 mWindowSize;

  bool mIsPlay;
  bool mIsFullScreen;

  PushButton mPlayButton;
  PushButton mPauseButton;
  PushButton mChangeButton;
  PushButton mResetButton;
  PushButton mBackwardButton;
  PushButton mForwardButton;

  PanGestureDetector mPanGestureDetector;
  PinchGestureDetector mPinchGestureDetector;
  TapGestureDetector mTapGestureDetector;
  float mScale;
  float mPinchStartScale;

  Animation mRotationAnimation;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  VideoViewController test( application );
  application.MainLoop();
  return 0;
}
