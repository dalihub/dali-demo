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
#include <iostream>
#include <dali-toolkit/devel-api/visuals/animated-gradient-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Toolkit;

// This example shows how to create and display animated-gradient-effect
//
namespace
{
// The value for scale-change between wearable-mobile
// Can be changed on App-Create time
Vector2 WINDOW_SIZE = Vector2( 360.0f, 360.0f );
Vector2 SCALED_WINDOW_SIZE = Vector2( 1.0f, 1.0f );
Vector3 SCALED_WINDOW_SIZE_3 = Vector3( 1.0f, 1.0f, 0.0f );
float SCALED_WIDTH = 1.0f;
float SCALED_HEIGHT = 1.0f;
float FONT_SCALE = 0.25f;

// const parameters for animations
const float CHANGE_DURATION = 0.2f;

// const parameters for icon position and size when resolution is 360x360
const Vector2 ICON_CALL_SIZE = Vector2( 54.0f, 54.0f );
const Vector3 ICON_CALL_POSITION = Vector3( 0.0f, 0.0f, 0.0f );
const Vector2 ICON_DECALL_SIZE = Vector2( 54.0f, 54.0f );
const Vector3 ICON_DECALL_POSITION = Vector3( 141.0f, 0.0f, 0.0f );
const Vector2 ICON_BATTERY_SIZE = Vector2( 14.0f, 23.0f );
const Vector3 ICON_BATTERY_POSITION = Vector3( 0.0f, 25.5f, 0.0f );
const Vector2 BUTTON_CALL_START_SIZE = Vector2( 54.0f, 54.0f );
const Vector3 BUTTON_CALL_START_POSITION = Vector3( -141.0f, 0.0f, 0.0f );

const Vector2 BUTTON_DECALL_ICON_SIZE = Vector2( 62.0f, 62.0f );
const Vector3 BUTTON_DECALL_ICON_POSITION = Vector3( 0.0f, 0.0f, 0.0f );
const Vector2 BUTTON_DECALL_SIZE = Vector2( 360.0f, 82.0f );
const Vector3 BUTTON_DECALL_CLIP_POSITION = Vector3( 0.0f, 82.0f, 0.0f );
const Vector3 BUTTON_DECALL_POSITION = Vector3( 0.0f, 0.0f, 0.0f );

// icon image name
const char * const ICON_CALL_IMAGE( DEMO_IMAGE_DIR "Call_Accept.png" );
const char * const ICON_DECALL_IMAGE( DEMO_IMAGE_DIR "Call_Decline.png" );
const char * const ICON_BATTERY_IMAGE( DEMO_IMAGE_DIR "Call_Battery.png" );
const char * const BUTTON_DECALL_ICON_IMAGE( DEMO_IMAGE_DIR "Call_Decline_wh.png" );

// const parameters for string position and size and font-size when resolution is 360x360
const Vector2 LABEL_INCOMING_SIZE = Vector2( 156.0f, 26.0f );
const Vector3 LABEL_INCOMING_POSITION = Vector3( 0.0f, -47.0f, 0.0f );
const Vector4 LABEL_INCOMING_FONT_COLOR = Vector4( 0.98f, 0.98f, 0.98f, 1.0f );
const float   LABEL_INCOMING_FONT_SIZE = 21.39f;
const Vector2 LABEL_NAME_SIZE = Vector2( 230.0f, 45.0f );
const Vector3 LABEL_NAME_POSITION = Vector3( 0.0f, -2.5f, 0.0f );
const Vector4 LABEL_NAME_FONT_COLOR = Vector4( 0.98f, 0.98f, 0.98f, 1.0f );
const float   LABEL_NAME_FONT_SIZE = 37.71f;
const Vector2 LABEL_NUMBER_SIZE = Vector2( 196.0f, 25.0f );
const Vector3 LABEL_NUMBER_POSITION = Vector3( 0.0f, 31.0f, 0.0f );
const Vector4 LABEL_NUMBER_FONT_COLOR = Vector4( 0.98f, 0.98f, 0.98f, 1.0f );
const float   LABEL_NUMBER_FONT_SIZE = 20.55f;
const Vector2 LABEL_DECLINE_SIZE = Vector2( 203.0f, 25.0f );
const Vector3 LABEL_DECLINE_POSITION = Vector3( 0.0f, -50.5f, 0.0f );
const Vector4 LABEL_DECLINE_FONT_COLOR = Vector4( 0.98f, 0.98f, 0.98f, 1.0f );
const float   LABEL_DECLINE_FONT_SIZE = 20.55f;
const Vector2 LABEL_TIME_SIZE = Vector2( 91.0f, 26.0f );
const Vector3 LABEL_TIME_POSITION = Vector3( 0.0f, -47.0f, 0.0f );
const Vector4 LABEL_TIME_FONT_COLOR = Vector4( 0.98f, 0.98f, 0.98f, 1.0f );
const float   LABEL_TIME_FONT_SIZE = 21.39f;

// string string
const char * const LABEL_INCOMING_STR( "Incoming Call" );
const char * const LABEL_NAME_STR( "Sam Smith" );
const char * const LABEL_NUMBER_STR( "+1 908-247-1695" );
const char * const LABEL_DECLINE_STR( "Decline Message" );
const char * const LABEL_TIME_STR( "1:03" );

// Set style from json
const char * const BACKGROUND_STYLE_JSON( DEMO_STYLE_DIR "animated-gradient-call-active-style.json" );
const char * const BACKGROUND_INCOME_STYLE_STR( "IncomeBackground" );
const char * const BACKGROUND_ACTIVE_STYLE_STR( "ActiveBackground" );
const char * const DECLINE_BUTTON_STYLE_STR( "DeclineButton" );

} // unnamed namespace

// This example shows how to render animated gradients
//
class CallController : public ConnectionTracker
{
public:
  CallController(Application& application)
    : mApplication( application ),
      mColorStart( 0.0f, 0.0f, 0.0f, 0.0f ),
      mColorEnd( 0.0f, 0.0f, 0.0f, 0.0f ),
      mColorReduce( 0.0f, 0.0f, 0.0f, 0.0f ),
      mButtonColorStart( 0.0f, 0.0f, 0.0f, 0.0f ),
      mButtonColorEnd( 0.0f, 0.0f, 0.0f, 0.0f ),
      mDuration( 0.0f ),
      mBackgroundDurationIncoming( 0.0f ),
      mBackgroundDurationActive( 0.0f ),
      mButtonDuration( 0.0f ),
      mButtonDelay( 0.0f )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &CallController::Create );
  }

  ~CallController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the main window
    mWindow = application.GetWindow();
    mWindow.KeyEventSignal().Connect( this, &CallController::OnKeyEvent );

    // Apply custom style for background and button.
    StyleManager::Get().ApplyTheme( BACKGROUND_STYLE_JSON );

    // Get current device's width and height.
    const Window::WindowSize windowSize = mWindow.GetSize();
    WINDOW_SIZE = Vector2(windowSize.GetWidth(), windowSize.GetHeight());
    SCALED_WINDOW_SIZE = WINDOW_SIZE / 360.0f;
    SCALED_WINDOW_SIZE_3 = Vector3( SCALED_WINDOW_SIZE.x, SCALED_WINDOW_SIZE.y, 0.0f );
    SCALED_WIDTH = SCALED_WINDOW_SIZE.x < SCALED_WINDOW_SIZE.y ? SCALED_WINDOW_SIZE.x : SCALED_WINDOW_SIZE.y;
    SCALED_HEIGHT = SCALED_WIDTH;

    // Note that this is heuristic value
    FONT_SCALE = 0.25f * WINDOW_SIZE.y / WINDOW_SIZE.x;

    mBackground = Control::New();
    mBackground.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mBackground.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    mBackground.SetProperty( Actor::Property::SIZE, WINDOW_SIZE );

    mWindow.Add( mBackground );

    BuildParameter();
    SetupActors();
    SetupAnimation();

    Reset();
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if( event.GetState() == KeyEvent::DOWN )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  // Setup const parameter values
  void BuildParameter()
  {
    mDuration = CHANGE_DURATION;
  }

  void SetupActors()
  {
    SetupComingActors();
    SetupActiveActors();
  }

  // Create and Add to window when visible at call incomming
  void SetupComingActors()
  {
    mButtonIconDecall = ImageView::New();
    mButtonIconDecall.SetImage( ICON_DECALL_IMAGE );
    mButtonIconDecall.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mButtonIconDecall.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    mButtonIconDecall.SetProperty( Actor::Property::SIZE, ICON_DECALL_SIZE * SCALED_WIDTH );
    mButtonIconDecall.SetProperty( Actor::Property::POSITION, ICON_DECALL_POSITION * SCALED_WIDTH );

    mButtonIconBattery = ImageView::New();
    mButtonIconBattery.SetImage( ICON_BATTERY_IMAGE );
    mButtonIconBattery.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER );
    mButtonIconBattery.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER );
    mButtonIconBattery.SetProperty( Actor::Property::SIZE, ICON_BATTERY_SIZE * SCALED_WIDTH );
    mButtonIconBattery.SetProperty( Actor::Property::POSITION, ICON_BATTERY_POSITION * SCALED_WIDTH );

    mCallStartButton = PushButton::New();
    mCallStartButton.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mCallStartButton.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    mCallStartButton.SetProperty( Actor::Property::SIZE, BUTTON_CALL_START_SIZE * SCALED_WIDTH );
    mCallStartButton.SetProperty( Actor::Property::POSITION, BUTTON_CALL_START_POSITION * SCALED_WIDTH );
    mCallStartButton.ClickedSignal().Connect( this, &CallController::OnButtonClicked );
    mCallStartButton.SetProperty( Button::Property::SELECTED_BACKGROUND_VISUAL, ICON_CALL_IMAGE );
    mCallStartButton.SetProperty( Button::Property::UNSELECTED_BACKGROUND_VISUAL, ICON_CALL_IMAGE );
    mCallStartButton.SetProperty( Control::Property::BACKGROUND, ICON_CALL_IMAGE );
    mCallStartButton.SetProperty( Button::Property::LABEL, "" );

    mWindow.Add( mCallStartButton );
    mWindow.Add( mButtonIconDecall );
    mWindow.Add( mButtonIconBattery );

    mLabelIncoming = TextLabel::New( LABEL_INCOMING_STR );
    mLabelIncoming.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mLabelIncoming.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    mLabelIncoming.SetProperty( Actor::Property::SIZE, LABEL_INCOMING_SIZE * SCALED_WINDOW_SIZE );
    mLabelIncoming.SetProperty( Actor::Property::POSITION, LABEL_INCOMING_POSITION * SCALED_WINDOW_SIZE_3 );
    mLabelIncoming.SetProperty( Actor::Property::VISIBLE, true );
    mLabelIncoming.SetProperty( TextLabel::Property::TEXT_COLOR, LABEL_INCOMING_FONT_COLOR );
    mLabelIncoming.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_INCOMING_FONT_SIZE * FONT_SCALE );
    mLabelIncoming.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mLabelIncoming.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

    mLabelName = TextLabel::New( LABEL_NAME_STR );
    mLabelName.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mLabelName.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    mLabelName.SetProperty( Actor::Property::SIZE, LABEL_NAME_SIZE * SCALED_WINDOW_SIZE );
    mLabelName.SetProperty( Actor::Property::POSITION, LABEL_NAME_POSITION * SCALED_WINDOW_SIZE_3 );
    mLabelName.SetProperty( Actor::Property::VISIBLE, true );
    mLabelName.SetProperty( TextLabel::Property::TEXT_COLOR, LABEL_NAME_FONT_COLOR );
    mLabelName.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_NAME_FONT_SIZE * FONT_SCALE );
    mLabelName.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mLabelName.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

    mLabelNumber = TextLabel::New( LABEL_NUMBER_STR );
    mLabelNumber.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mLabelNumber.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    mLabelNumber.SetProperty( Actor::Property::SIZE, LABEL_NUMBER_SIZE * SCALED_WINDOW_SIZE );
    mLabelNumber.SetProperty( Actor::Property::POSITION, LABEL_NUMBER_POSITION * SCALED_WINDOW_SIZE_3 );
    mLabelNumber.SetProperty( Actor::Property::VISIBLE, true );
    mLabelNumber.SetProperty( TextLabel::Property::TEXT_COLOR, LABEL_NUMBER_FONT_COLOR );
    mLabelNumber.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_NUMBER_FONT_SIZE * FONT_SCALE );
    mLabelNumber.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mLabelNumber.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

    mLabelDecline = TextLabel::New( LABEL_DECLINE_STR );
    mLabelDecline.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER );
    mLabelDecline.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER );
    mLabelDecline.SetProperty( Actor::Property::SIZE, LABEL_DECLINE_SIZE * SCALED_WINDOW_SIZE );
    mLabelDecline.SetProperty( Actor::Property::POSITION, LABEL_DECLINE_POSITION * SCALED_WIDTH );
    mLabelDecline.SetProperty( Actor::Property::VISIBLE, true );
    mLabelDecline.SetProperty( TextLabel::Property::TEXT_COLOR, LABEL_DECLINE_FONT_COLOR );
    mLabelDecline.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_DECLINE_FONT_SIZE * FONT_SCALE );
    mLabelDecline.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mLabelDecline.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

    mWindow.Add( mLabelIncoming );
    mWindow.Add( mLabelName );
    mWindow.Add( mLabelNumber );
    mWindow.Add( mLabelDecline );
  }

  // Create and Add to window when visible at call active
  void SetupActiveActors()
  {
    mButtonClip = Control::New();
    mButtonClip.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER );
    mButtonClip.SetProperty( Actor::Property::ANCHOR_POINT, ParentOrigin::BOTTOM_CENTER );
    mButtonClip.SetProperty( Actor::Property::SIZE, BUTTON_DECALL_SIZE * SCALED_WIDTH );
    mButtonClip.SetProperty( Actor::Property::POSITION, BUTTON_DECALL_CLIP_POSITION * SCALED_WIDTH );

    mButtonIcon = ImageView::New();
    mButtonIcon.SetImage( BUTTON_DECALL_ICON_IMAGE );
    mButtonIcon.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mButtonIcon.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    mButtonIcon.SetProperty( Actor::Property::SIZE, BUTTON_DECALL_ICON_SIZE * SCALED_WIDTH );
    mButtonIcon.SetProperty( Actor::Property::POSITION, BUTTON_DECALL_ICON_POSITION * SCALED_WIDTH );

    mCallEndButton = PushButton::New();
    mCallEndButton.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mCallEndButton.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    mCallEndButton.SetProperty( Actor::Property::SIZE, BUTTON_DECALL_SIZE * SCALED_WIDTH );
    mCallEndButton.SetProperty( Actor::Property::POSITION, BUTTON_DECALL_POSITION * SCALED_WIDTH );
    mCallEndButton.ClickedSignal().Connect( this, &CallController::OnButtonClicked );
    mCallEndButton.SetProperty( Button::Property::SELECTED_BACKGROUND_VISUAL, "" );
    mCallEndButton.SetProperty( Button::Property::UNSELECTED_BACKGROUND_VISUAL, "" );
    mCallEndButton.SetStyleName( DECLINE_BUTTON_STYLE_STR );
    mCallEndButton.SetProperty( Button::Property::LABEL, "" );

    mButtonClip.Add( mCallEndButton );
    mButtonClip.Add( mButtonIcon );

    mLabelTime = TextLabel::New( LABEL_TIME_STR );
    mLabelTime.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mLabelTime.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    mLabelTime.SetProperty( Actor::Property::SIZE, LABEL_TIME_SIZE * SCALED_WINDOW_SIZE );
    mLabelTime.SetProperty( Actor::Property::POSITION, LABEL_TIME_POSITION * SCALED_WINDOW_SIZE_3 );
    mLabelTime.SetProperty( Actor::Property::VISIBLE, false );
    mLabelTime.SetProperty( TextLabel::Property::TEXT_COLOR, LABEL_TIME_FONT_COLOR );
    mLabelTime.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_TIME_FONT_SIZE * FONT_SCALE );
    mLabelTime.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mLabelTime.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

    mWindow.Add( mLabelTime );
  }

  void SetupAnimation()
  {
    mMoveFront = Animation::New( mDuration );
    mMoveBack = Animation::New( mDuration );

    mMoveFront.AnimateTo( Property( mButtonClip, Actor::Property::POSITION_Y ), 0.0f * SCALED_HEIGHT );
    mMoveBack.AnimateTo( Property( mButtonClip, Actor::Property::POSITION_Y ), BUTTON_DECALL_CLIP_POSITION.y * SCALED_HEIGHT );

    mMoveFront.AnimateTo( Property( mCallStartButton, Actor::Property::VISIBLE ), false );
    mMoveFront.AnimateTo( Property( mButtonIconDecall, Actor::Property::VISIBLE ), false );
    mMoveBack.AnimateTo( Property( mCallStartButton, Actor::Property::VISIBLE ), true );
    mMoveBack.AnimateTo( Property( mButtonIconDecall, Actor::Property::VISIBLE ), true );

    mMoveFront.AnimateTo( Property( mLabelIncoming, Actor::Property::VISIBLE ), false );
    mMoveFront.AnimateTo( Property( mLabelNumber, Actor::Property::VISIBLE ), false );
    mMoveFront.AnimateTo( Property( mLabelTime, Actor::Property::VISIBLE ), true );
    mMoveBack.AnimateTo( Property( mLabelIncoming, Actor::Property::VISIBLE ), true );
    mMoveBack.AnimateTo( Property( mLabelNumber, Actor::Property::VISIBLE ), true );
    mMoveBack.AnimateTo( Property( mLabelTime, Actor::Property::VISIBLE ), false );
  }

  bool OnButtonClicked(Button button)
  {
    if( button == mCallStartButton )
    {
      mBackground.SetStyleName( BACKGROUND_ACTIVE_STYLE_STR );
      mWindow.Add( mButtonClip );
      mMoveFront.Play();
    }
    else if( button == mCallEndButton )
    {
      mBackground.SetStyleName( BACKGROUND_INCOME_STYLE_STR );
      mTempTimer = Timer::New( mDuration * 1000.0f );
      mTempTimer.TickSignal().Connect( this, &CallController::smallTick );
      mTempTimer.Start();
      mMoveBack.Play();
    }
    return true;
  }
  bool smallTick()
  {
    mButtonClip.Unparent();
    return false;
  }

  void Reset()
  {
    mBackground.SetStyleName( BACKGROUND_INCOME_STYLE_STR );
  }

private:
  Application&  mApplication;
  Window mWindow;

  Control mBackground;

  // Show when call incommint
  PushButton mCallStartButton;
  ImageView mButtonIconDecall;
  ImageView mButtonIconBattery;
  TextLabel mLabelIncoming;
  TextLabel mLabelName;
  TextLabel mLabelNumber;
  TextLabel mLabelDecline;

  // Show when call active
  PushButton mCallEndButton;
  ImageView mButtonIcon;
  Control   mButtonBackground;
  Control   mButtonClip;
  TextLabel mLabelTime;

  Timer mTempTimer;

  Animation mMoveFront;
  Animation mMoveBack;

  Vector4 mColorStart;
  Vector4 mColorEnd;
  Vector4 mColorReduce;

  Vector4 mButtonColorStart;
  Vector4 mButtonColorEnd;

  float mDuration;
  float mBackgroundDurationIncoming;
  float mBackgroundDurationActive;
  float mButtonDuration;
  float mButtonDelay;
};

int DALI_EXPORT_API main(int argc, char **argv)
{
  Application application = Application::New( &argc, &argv );

  CallController test( application );

  application.MainLoop();
  return 0;
}
