/*
* Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
Vector2 STAGE_SIZE = Vector2( 360.0f, 360.0f );
Vector2 SCALED_STAGE_SIZE = Vector2( 1.0f, 1.0f );
Vector3 SCALED_STAGE_SIZE_3 = Vector3( 1.0f, 1.0f, 0.0f );
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
    // Get a handle to the stage
    mStage = Stage::GetCurrent();
    mStage.KeyEventSignal().Connect( this, &CallController::OnKeyEvent );

    // Apply custom style for background and button.
    StyleManager::Get().ApplyTheme( BACKGROUND_STYLE_JSON );

    // Get current device's width and height.
    STAGE_SIZE = mStage.GetSize();
    SCALED_STAGE_SIZE = STAGE_SIZE / 360.0f;
    SCALED_STAGE_SIZE_3 = Vector3( SCALED_STAGE_SIZE.x, SCALED_STAGE_SIZE.y, 0.0f );
    SCALED_WIDTH = SCALED_STAGE_SIZE.x < SCALED_STAGE_SIZE.y ? SCALED_STAGE_SIZE.x : SCALED_STAGE_SIZE.y;
    SCALED_HEIGHT = SCALED_WIDTH;

    // Note that this is heuristic value
    FONT_SCALE = 0.25f * STAGE_SIZE.y / STAGE_SIZE.x;

    mBackground = Control::New();
    mBackground.SetParentOrigin( ParentOrigin::CENTER );
    mBackground.SetAnchorPoint( AnchorPoint::CENTER );
    mBackground.SetSize( STAGE_SIZE );

    mStage.Add( mBackground );

    BuildParameter();
    SetupActors();
    SetupAnimation();

    Reset();
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if( event.state == KeyEvent::Down )
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

  // Create and Add to stage when visible at call incomming
  void SetupComingActors()
  {
    mButtonIconDecall = ImageView::New();
    mButtonIconDecall.SetImage( ICON_DECALL_IMAGE );
    mButtonIconDecall.SetParentOrigin( ParentOrigin::CENTER );
    mButtonIconDecall.SetAnchorPoint( AnchorPoint::CENTER );
    mButtonIconDecall.SetSize( ICON_DECALL_SIZE * SCALED_WIDTH );
    mButtonIconDecall.SetPosition( ICON_DECALL_POSITION * SCALED_WIDTH );

    mButtonIconBattery = ImageView::New();
    mButtonIconBattery.SetImage( ICON_BATTERY_IMAGE );
    mButtonIconBattery.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mButtonIconBattery.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mButtonIconBattery.SetSize( ICON_BATTERY_SIZE * SCALED_WIDTH );
    mButtonIconBattery.SetPosition( ICON_BATTERY_POSITION * SCALED_WIDTH );

    mCallStartButton = PushButton::New();
    mCallStartButton.SetParentOrigin( ParentOrigin::CENTER );
    mCallStartButton.SetAnchorPoint( AnchorPoint::CENTER );
    mCallStartButton.SetSize( BUTTON_CALL_START_SIZE * SCALED_WIDTH );
    mCallStartButton.SetPosition( BUTTON_CALL_START_POSITION * SCALED_WIDTH );
    mCallStartButton.ClickedSignal().Connect( this, &CallController::OnButtonClicked );
    mCallStartButton.SetProperty( Button::Property::SELECTED_STATE_IMAGE, ICON_CALL_IMAGE );
    mCallStartButton.SetProperty( Button::Property::UNSELECTED_STATE_IMAGE, ICON_CALL_IMAGE );

    mStage.Add( mCallStartButton );
    mStage.Add( mButtonIconDecall );
    mStage.Add( mButtonIconBattery );

    mLabelIncoming = TextLabel::New( LABEL_INCOMING_STR );
    mLabelIncoming.SetParentOrigin( ParentOrigin::CENTER );
    mLabelIncoming.SetAnchorPoint( AnchorPoint::CENTER );
    mLabelIncoming.SetSize( LABEL_INCOMING_SIZE * SCALED_STAGE_SIZE );
    mLabelIncoming.SetPosition( LABEL_INCOMING_POSITION * SCALED_STAGE_SIZE_3 );
    mLabelIncoming.SetVisible( true );
    mLabelIncoming.SetProperty( TextLabel::Property::TEXT_COLOR, LABEL_INCOMING_FONT_COLOR );
    mLabelIncoming.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_INCOMING_FONT_SIZE * FONT_SCALE );
    mLabelIncoming.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mLabelIncoming.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

    mLabelName = TextLabel::New( LABEL_NAME_STR );
    mLabelName.SetParentOrigin( ParentOrigin::CENTER );
    mLabelName.SetAnchorPoint( AnchorPoint::CENTER );
    mLabelName.SetSize( LABEL_NAME_SIZE * SCALED_STAGE_SIZE );
    mLabelName.SetPosition( LABEL_NAME_POSITION * SCALED_STAGE_SIZE_3 );
    mLabelName.SetVisible( true );
    mLabelName.SetProperty( TextLabel::Property::TEXT_COLOR, LABEL_NAME_FONT_COLOR );
    mLabelName.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_NAME_FONT_SIZE * FONT_SCALE );
    mLabelName.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mLabelName.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

    mLabelNumber = TextLabel::New( LABEL_NUMBER_STR );
    mLabelNumber.SetParentOrigin( ParentOrigin::CENTER );
    mLabelNumber.SetAnchorPoint( AnchorPoint::CENTER );
    mLabelNumber.SetSize( LABEL_NUMBER_SIZE * SCALED_STAGE_SIZE );
    mLabelNumber.SetPosition( LABEL_NUMBER_POSITION * SCALED_STAGE_SIZE_3 );
    mLabelNumber.SetVisible( true );
    mLabelNumber.SetProperty( TextLabel::Property::TEXT_COLOR, LABEL_NUMBER_FONT_COLOR );
    mLabelNumber.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_NUMBER_FONT_SIZE * FONT_SCALE );
    mLabelNumber.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mLabelNumber.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

    mLabelDecline = TextLabel::New( LABEL_DECLINE_STR );
    mLabelDecline.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mLabelDecline.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mLabelDecline.SetSize( LABEL_DECLINE_SIZE * SCALED_STAGE_SIZE );
    mLabelDecline.SetPosition( LABEL_DECLINE_POSITION * SCALED_WIDTH );
    mLabelDecline.SetVisible( true );
    mLabelDecline.SetProperty( TextLabel::Property::TEXT_COLOR, LABEL_DECLINE_FONT_COLOR );
    mLabelDecline.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_DECLINE_FONT_SIZE * FONT_SCALE );
    mLabelDecline.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mLabelDecline.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

    mStage.Add( mLabelIncoming );
    mStage.Add( mLabelName );
    mStage.Add( mLabelNumber );
    mStage.Add( mLabelDecline );
  }

  // Create and Add to stage when visible at call active
  void SetupActiveActors()
  {
    mButtonClip = Control::New();
    mButtonClip.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mButtonClip.SetAnchorPoint( ParentOrigin::BOTTOM_CENTER );
    mButtonClip.SetSize( BUTTON_DECALL_SIZE * SCALED_WIDTH );
    mButtonClip.SetPosition( BUTTON_DECALL_CLIP_POSITION * SCALED_WIDTH );

    mButtonIcon = ImageView::New();
    mButtonIcon.SetImage( BUTTON_DECALL_ICON_IMAGE );
    mButtonIcon.SetParentOrigin( ParentOrigin::CENTER );
    mButtonIcon.SetAnchorPoint( AnchorPoint::CENTER );
    mButtonIcon.SetSize( BUTTON_DECALL_ICON_SIZE * SCALED_WIDTH );
    mButtonIcon.SetPosition( BUTTON_DECALL_ICON_POSITION * SCALED_WIDTH );

    mCallEndButton = PushButton::New();
    mCallEndButton.SetParentOrigin( ParentOrigin::CENTER );
    mCallEndButton.SetAnchorPoint( AnchorPoint::CENTER );
    mCallEndButton.SetSize( BUTTON_DECALL_SIZE * SCALED_WIDTH );
    mCallEndButton.SetPosition( BUTTON_DECALL_POSITION * SCALED_WIDTH );
    mCallEndButton.ClickedSignal().Connect( this, &CallController::OnButtonClicked );
    mCallEndButton.SetProperty( Button::Property::SELECTED_STATE_IMAGE, "" );
    mCallEndButton.SetProperty( Button::Property::UNSELECTED_STATE_IMAGE, "" );
    mCallEndButton.SetStyleName( DECLINE_BUTTON_STYLE_STR );

    mButtonClip.Add( mCallEndButton );
    mButtonClip.Add( mButtonIcon );

    mLabelTime = TextLabel::New( LABEL_TIME_STR );
    mLabelTime.SetParentOrigin( ParentOrigin::CENTER );
    mLabelTime.SetAnchorPoint( AnchorPoint::CENTER );
    mLabelTime.SetSize( LABEL_TIME_SIZE * SCALED_STAGE_SIZE );
    mLabelTime.SetPosition( LABEL_TIME_POSITION * SCALED_STAGE_SIZE_3 );
    mLabelTime.SetVisible( false );
    mLabelTime.SetProperty( TextLabel::Property::TEXT_COLOR, LABEL_TIME_FONT_COLOR );
    mLabelTime.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_TIME_FONT_SIZE * FONT_SCALE );
    mLabelTime.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mLabelTime.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

    mStage.Add( mLabelTime );
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
      mStage.Add( mButtonClip );
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
  Stage mStage;

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

int main(int argc, char **argv)
{
  Application application = Application::New( &argc, &argv );

  CallController test( application );

  application.MainLoop();
  return 0;
}
