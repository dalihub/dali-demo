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

// const parameters for color and animation
const float CHANGE_DURATION = 0.2f;
const float CARD_MOVE_DURATION = 1.0f;
const float LOADING_ONE_CYCLE_DURATION = 2.0f;
const int   LOADING_CYCLE_CNT = 4;
const int   LOADING_CYCLE_DT = 10;
const Vector4 DEFAULT_COLOR = Vector4( 0.5f, 0.5f, 0.5f, 1.0f );

const float PSEUDO_SCROLL_TIME = 100.0f;
const float PSEUDO_SCROLL_OFFSET = 0.05f;

// const static parameters for cards when resolution is 360x360
const int CARD_NUM = 3;
const Vector2 CARD_SIZE = Vector2( 210.0f, 143.0f );
const Vector2 CARD_OFFSET = Vector2( 0.0f, 12.5f );
const Vector2 CARD_DIFF = Vector2( 240.0f, 0.0f );
const float CARD_BOUNCE_ANIMATION_RATE = 0.3f;

const Vector2 CARD_SIZE_BIG = Vector2( 292.0f, 199.0f );
const Vector2 CARD_BIG_OFFSET = Vector2( 0.0f, -5.5f );
const float CARD_MOVE_DIST = 40.0f;

// const private parameters for each cards
const Vector4 CARD_COLOR_START_LIST[] =
{
  Vector4( 0x24, 0x2C, 0x93, 255.f ) / 255.f,
  Vector4( 0x7A, 0x1C, 0x9E, 255.f ) / 255.f,
  Vector4( 0xA9, 0x0C, 0x96, 255.f ) / 255.f,
};
const Vector4 CARD_COLOR_END_LIST[] =
{
  Vector4( 0x04, 0x13, 0x23, 255.f ) / 255.f,
  Vector4( 0x28, 0x01, 0x45, 255.f ) / 255.f,
  Vector4( 0x37, 0x0A, 0x2E, 255.f ) / 255.f,
};
const Vector4 CARD_COLOR_BACKGROUND_LIST[] =
{
  Vector4( 0x28, 0x2B, 0x6E, 255.f ) / 255.f,
  Vector4( 0x4D, 0x15, 0x61, 255.f ) / 255.f,
  Vector4( 0x70, 0x21, 0x61, 255.f ) / 255.f,
};
const char * const CARD_IMAGE_LIST[] =
{
  ( DEMO_IMAGE_DIR "Card_01.png" ),
  ( DEMO_IMAGE_DIR "Card_02.png" ),
  ( DEMO_IMAGE_DIR "Card_03.png" ),
};

// const parameters for add button position and size when resolution is 360x360x
const Vector2 BUTTON_ADD_SIZE = Vector2( 292.0f, 52.0f );
const Vector3 BUTTON_ADD_POSITION = Vector3( 0.0f, 0.0f, 0.0f );
const char * const BUTTON_ADD_IMAGE( DEMO_IMAGE_DIR "Card_Add_Button.png" );

// const parameters for string position and size and font-size when resolution is 360x360
const Vector2 LABEL_TICKET_SIZE = Vector2( 148.0f, 31.0f );
const Vector3 LABEL_TICKET_POSITION = Vector3( 0.0f, 72.5f, 0.0f );
const Vector4 LABEL_TICKET_FONT_COLOR = Vector4( 0.98f, 0.98f, 0.98f, 1.0f );
const float   LABEL_TICKET_FONT_SIZE = 25.0f;
const Vector2 LABEL_HOLD_SIZE = Vector2( 180.0f, 60.0f );
const Vector3 LABEL_HOLD_POSITION = Vector3( 1.0f, 103.0f, 0.0f );
const Vector4 LABEL_HOLD_FONT_COLOR = Vector4( 0.98f, 0.98f, 0.98f, 1.0f );
const float   LABEL_HOLD_FONT_SIZE = 25.0f;
const Vector2 LABEL_TERMINAL_SIZE = Vector2( 180.0f, 60.0f );
const Vector3 LABEL_TERMINAL_POSITION = Vector3( 1.0f, 133.0f, 0.0f );
const Vector4 LABEL_TERMINAL_FONT_COLOR = Vector4( 0.98f, 0.98f, 0.98f, 1.0f );
const float   LABEL_TERMINAL_FONT_SIZE = 25.0f;

// string string
const char * const LABEL_TICKET_STR( "Select Ticket" );
const char * const LABEL_HOLD_STR( "Hold near" );
const char * const LABEL_TERMINAL_STR( "terminal" );

class CardManager
{
public:
  CardManager()
    : mSize( 0.0f, 0.0f ),
      mOffset( 0.0f, 0.0f),
      mDiff( 0.0f, 0.0f),
      mCurIndex( 0 ),
      mCurState( 0 )
  {
  }
  ~CardManager() {}

  void Init(Stage& stage)
  {
    mSize = CARD_SIZE * SCALED_WIDTH;
    mOffset = CARD_OFFSET * SCALED_WIDTH;
    mDiff = CARD_DIFF * SCALED_WIDTH;

    mCurIndex = 0;
    mCurState = 0;

    for( int k = 0; k < CARD_NUM; k++ )
    {
      mPosition[k] = mOffset + mDiff * k;

      mColorStart[k] = CARD_COLOR_START_LIST[k];
      mColorEnd[k] = CARD_COLOR_END_LIST[k];
      mColorBackground[k] = CARD_COLOR_BACKGROUND_LIST[k];

      mImageUrl[k] = CARD_IMAGE_LIST[k];

      // Create an image view for this item
      mCard[k] = ImageView::New();
      {
        Property::Map propertyMap;
        propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
        propertyMap.Insert(ImageVisual::Property::URL, mImageUrl[k]);
        propertyMap.Insert(DevelVisual::Property::VISUAL_FITTING_MODE, DevelVisual::FILL);
        mCard[k].SetProperty(Toolkit::ImageView::Property::IMAGE, propertyMap);
      }

      mCard[k].SetParentOrigin( ParentOrigin::CENTER );
      mCard[k].SetAnchorPoint( AnchorPoint::CENTER );
      mCard[k].SetSize( mSize.x, mSize.y );
      mCard[k].SetPosition( mPosition[k].x, mPosition[k].y );

      stage.Add( mCard[k] );
    }
  }

  bool MoveRight(float duration)
  {
    Animation anim = Animation::New( duration );
    for( int k = 0; k < CARD_NUM; k++ )
    {
      if( mCurIndex == 0 )
      {
        anim.AnimateBy( Property( mCard[k], Actor::Property::POSITION_X ), mDiff.x * CARD_BOUNCE_ANIMATION_RATE, AlphaFunction::BOUNCE );
      }
      else
      {
        anim.AnimateBy( Property( mCard[k], Actor::Property::POSITION_X ), mDiff.x, AlphaFunction::EASE_OUT );
      }
    }
    bool res = false;
    if( mCurIndex != 0 )
    {
      mCurIndex = (mCurIndex - 1) % CARD_NUM;
      res = true;
    }
    anim.Play();
    return res;
  }

  bool MoveLeft(float duration)
  {
    Animation anim = Animation::New( duration );
    for( int k = 0; k < CARD_NUM; k++ )
    {
      if( mCurIndex == CARD_NUM - 1 )
      {
        anim.AnimateBy( Property( mCard[k], Actor::Property::POSITION_X ), -mDiff.x * CARD_BOUNCE_ANIMATION_RATE, AlphaFunction::BOUNCE );
      }
      else
      {
        anim.AnimateBy( Property( mCard[k], Actor::Property::POSITION_X ), -mDiff.x, AlphaFunction::EASE_OUT );
      }
    }
    bool res = false;
    if( mCurIndex != CARD_NUM - 1 )
    {
      mCurIndex = (mCurIndex + 1) % CARD_NUM;
      res = true;
    }
    anim.Play();
    return res;
  }

  Vector4 GetColorStart(int index)
  {
    DALI_ASSERT_ALWAYS( index >= 0 && index < CARD_NUM );
    return mColorStart[index];
  }

  Vector4 GetColorEnd(int index)
  {
    DALI_ASSERT_ALWAYS( index >= 0 && index < CARD_NUM );
    return mColorEnd[index];
  }

  Vector4 GetColorBackground(int index)
  {
    DALI_ASSERT_ALWAYS( index >= 0 && index < CARD_NUM );
    return mColorBackground[index];
  }

  ImageView& operator [](int index)
  {
    DALI_ASSERT_ALWAYS( index >= 0 && index < CARD_NUM );
    return mCard[index];
  }

  ImageView mCard[CARD_NUM];
  std::string mImageUrl[CARD_NUM];
  Vector4 mColorStart[CARD_NUM];
  Vector4 mColorEnd[CARD_NUM];
  Vector4 mColorBackground[CARD_NUM];
  Vector2 mPosition[CARD_NUM];
  Vector2 mSize;
  Vector2 mOffset;
  Vector2 mDiff;
  int mCurIndex;
  int mCurState;
};

} // unnamed namespace

// This example shows how to render animated gradients
//
class CardController : public ConnectionTracker
{
public:
  CardController(Application& application)
    : mApplication( application ),
      mNormalColor( 0.0f, 0.0f, 0.0f, 0.0f ),
      mNormalStartColor( 0.0f, 0.0f, 0.0f, 0.0f ),
      mNormalEndColor( 0.0f, 0.0f, 0.0f, 0.0f),
      mFirstTouchPos( 0.0f, 0.0f ),
      mLastTouchPos( 0.0f, 0.0f ),
      mCardDuration( 0.0f ),
      mDuration( 0.0f ),
      mLoadingTime( 0.0f ),
      mLoadingCount( 0 ),
      mLoadingCountScale( 0 ),
      mTickCount( 0 ),
      mCancelSignal( false ),
      mIsTouchedActor( false )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &CardController::Create );
  }

  ~CardController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the stage
    mStage = Stage::GetCurrent();
    mStage.KeyEventSignal().Connect( this, &CardController::OnKeyEvent );

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
    InitMap();
    SetupCards();
    SetupActors();
    SetupAnimation();

    mStage.GetRootLayer().TouchSignal().Connect( this, &CardController::OnTouchLayer );
    Reset();
  }

  bool OnTouchCards(Actor actor, const TouchData &data)
  {
    if( data.GetPointCount() > 0 )
    {
      if( data.GetState( 0 ) == PointState::DOWN )
      {
        if( mCards.mCurState == 0 )
        {
          mIsTouchedActor = false;
          if( mCards[mCards.mCurIndex] == actor )
          {
            mIsTouchedActor = true;
          }
          mCards.mCurState = 3;
          mTempTimer = Timer::New( PSEUDO_SCROLL_TIME );
          mTempTimer.TickSignal().Connect( this, &CardController::OnDetectMotionLayer );
          mTempTimer.Start();

          mFirstTouchPos = data.GetScreenPosition( 0 );
          mLastTouchPos = mFirstTouchPos;
        }
        else if( mCards.mCurState == 1 )
        {
          mCancelSignal = true;
          return false;
        }
      }
      else
      {
        mLastTouchPos = data.GetScreenPosition( 0 );
      }
    }
    return true;
  }

  bool OnTouchLayer(Actor actor, const TouchData &data)
  {
    if( data.GetPointCount() > 0 )
    {
      if( data.GetState( 0 ) == PointState::DOWN )
      {
        if( mCards.mCurState == 0 )
        {
          mIsTouchedActor = false;
          mCards.mCurState = 3;
          mTempTimer = Timer::New( PSEUDO_SCROLL_TIME );
          mTempTimer.TickSignal().Connect( this, &CardController::OnDetectMotionLayer );
          mTempTimer.Start();

          mFirstTouchPos = data.GetScreenPosition( 0 );
          mLastTouchPos = mFirstTouchPos;
        }
      }
      else
      {
        mLastTouchPos = data.GetScreenPosition( 0 );
      }
    }
    return true;
  }

  // Heuristic Scroll View
  bool OnDetectMotionLayer()
  {
    if( mCards.mCurState == 3 )
    {
      Vector2 diff = (mLastTouchPos - mFirstTouchPos);
      float offset = PSEUDO_SCROLL_OFFSET;
      // Scroll to right
      if( diff.x > mStage.GetSize().x * offset )
      {
        mCards.mCurState = 2;
        MoveRight();
        mCardChanger = Timer::New( mCardDuration * 1000.0f );
        mCardChanger.TickSignal().Connect( this, &CardController::OnTickLayer );
        mCardChanger.Start();
      }
      // Scroll to left
      else if( diff.x < -mStage.GetSize().x * offset )
      {
        mCards.mCurState = 2;
        MoveLeft();
        mCardChanger = Timer::New( mCardDuration * 1000.0f );
        mCardChanger.TickSignal().Connect( this, &CardController::OnTickLayer );
        mCardChanger.Start();
      }
      // Not a scroll input
      else
      {
        // Run NFC Tag effect if we touch a card
        if (mIsTouchedActor)
        {
          mCards.mCurState = 1;
          RunAnimation( mCards.mCurIndex );
        }
        else
        {
          Reset();
        }
      }
    }
    return false;
  }

  bool OnTickLayer()
  {
    Reset();
    return false;
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

  // Utility function to make animation parameter map. return Property::Map
  Property::Value BuildMap(const Property::Value &start, const Property::Value &target, int dir, float duration, float delay, int repeat, float repeat_delay, int motion, int easing)
  {
    Property::Map map;

    map.Clear();
    map.Insert( DevelAnimatedGradientVisual::AnimationParameter::Property::START, start );
    map.Insert( DevelAnimatedGradientVisual::AnimationParameter::Property::TARGET, target );
    if( dir == 0 )
    {
      map.Insert( DevelAnimatedGradientVisual::AnimationParameter::Property::DIRECTION, DevelAnimatedGradientVisual::AnimationParameter::DirectionType::FORWARD );
    }
    else
    {
      map.Insert( DevelAnimatedGradientVisual::AnimationParameter::Property::DIRECTION, DevelAnimatedGradientVisual::AnimationParameter::DirectionType::BACKWARD );
    }
    map.Insert( DevelAnimatedGradientVisual::AnimationParameter::Property::DURATION, duration );
    map.Insert( DevelAnimatedGradientVisual::AnimationParameter::Property::DELAY, delay );
    map.Insert( DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT, repeat );
    map.Insert( DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT_DELAY, repeat_delay );
    if( motion == 0 )
    {
      map.Insert( DevelAnimatedGradientVisual::AnimationParameter::Property::MOTION_TYPE, DevelAnimatedGradientVisual::AnimationParameter::MotionType::LOOP );
    }
    else
    {
      map.Insert( DevelAnimatedGradientVisual::AnimationParameter::Property::MOTION_TYPE, DevelAnimatedGradientVisual::AnimationParameter::MotionType::MIRROR );
    }
    if( easing == 0 )
    {
      map.Insert( DevelAnimatedGradientVisual::AnimationParameter::Property::EASING_TYPE, DevelAnimatedGradientVisual::AnimationParameter::EasingType::LINEAR );
    }
    else if( easing == 1 )
    {
      map.Insert( DevelAnimatedGradientVisual::AnimationParameter::Property::EASING_TYPE, DevelAnimatedGradientVisual::AnimationParameter::EasingType::IN );
    }
    else if( easing == 2 )
    {
      map.Insert( DevelAnimatedGradientVisual::AnimationParameter::Property::EASING_TYPE, DevelAnimatedGradientVisual::AnimationParameter::EasingType::OUT );
    }
    else
    {
      map.Insert( DevelAnimatedGradientVisual::AnimationParameter::Property::EASING_TYPE, DevelAnimatedGradientVisual::AnimationParameter::EasingType::IN_OUT );
    }

    return Property::Value( map );
  }

  // Setup background visual property during nothing action
  void InitMapNormal()
  {
    mBackgroundNormalMap.Clear();
    mBackgroundNormalMap.Insert( Visual::Property::TYPE, DevelVisual::ANIMATED_GRADIENT );

    mBackgroundNormalMap.Insert( Toolkit::DevelAnimatedGradientVisual::Property::START_POSITION, Vector2( -0.5, -0.5 ) );
    mBackgroundNormalMap.Insert( Toolkit::DevelAnimatedGradientVisual::Property::END_POSITION, Vector2( 0.5, 0.5 ) );
    mBackgroundNormalMap.Insert( Toolkit::DevelAnimatedGradientVisual::Property::START_COLOR, mNormalColor );
    mBackgroundNormalMap.Insert( Toolkit::DevelAnimatedGradientVisual::Property::END_COLOR, mNormalColor );
    mBackgroundNormalMap.Insert( Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_CENTER, Vector2( 0.0f, 0.0f ) );
    mBackgroundNormalMap.Insert( Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT, 0.0f );
    mBackgroundNormalMap.Insert( Toolkit::DevelAnimatedGradientVisual::Property::OFFSET, 0.0f );
  }

  // Setup background visual property during NFC tagging start
  void InitMapStart()
  {
    mBackgroundMapStart.Clear();
    mBackgroundMapStart.Insert( Visual::Property::TYPE, DevelVisual::ANIMATED_GRADIENT );

    mBackgroundMapStart.Insert( Toolkit::DevelAnimatedGradientVisual::Property::START_POSITION, Vector2( -0.5, -0.5 ) );
    mBackgroundMapStart.Insert( Toolkit::DevelAnimatedGradientVisual::Property::END_POSITION, Vector2( 0.5, 0.5 ) );
    mBackgroundMapStart.Insert( Toolkit::DevelAnimatedGradientVisual::Property::START_COLOR, mNormalColor );
    mBackgroundMapStart.Insert( Toolkit::DevelAnimatedGradientVisual::Property::END_COLOR, mNormalColor );
    mBackgroundMapStart.Insert( Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_CENTER, Vector2( 0.0f, 0.0f ) );
    mBackgroundMapStart.Insert( Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT, 0.0f );
    mBackgroundMapStart.Insert( Toolkit::DevelAnimatedGradientVisual::Property::OFFSET, BuildMap( 0.0f, 2.0f, 0, mLoadingTime, 0.0f, -1, 0.0f, 0, 0 ) );

    mColorAnimationStartStart = *( BuildMap( mNormalColor, Vector4( 0, 0, 0, 0 ), 0, mDuration, 0.0f, 1, 0.0f, 0, 0 ).GetMap() );
    mColorAnimationStartEnd = *( BuildMap( mNormalColor, Vector4( 0, 0, 0, 0 ), 0, mDuration, 0.0f, 1, 0.0f, 0, 0 ).GetMap() );
  }

  // Setup background visual property during NFC tagging end
  void InitMapEnd()
  {
    mBackgroundMapEnd.Clear();
    mBackgroundMapEnd.Insert( Visual::Property::TYPE, DevelVisual::ANIMATED_GRADIENT );

    mBackgroundMapEnd.Insert( Toolkit::DevelAnimatedGradientVisual::Property::START_POSITION, Vector2( -0.5, -0.5 ) );
    mBackgroundMapEnd.Insert( Toolkit::DevelAnimatedGradientVisual::Property::END_POSITION, Vector2( 0.5, 0.5 ) );
    mBackgroundMapEnd.Insert( Toolkit::DevelAnimatedGradientVisual::Property::START_COLOR, mNormalColor );
    mBackgroundMapEnd.Insert( Toolkit::DevelAnimatedGradientVisual::Property::END_COLOR, mNormalColor );
    mBackgroundMapEnd.Insert( Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_CENTER, Vector2( 0.0f, 0.0f ) );
    mBackgroundMapEnd.Insert( Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT, 0.0f );
    mBackgroundMapEnd.Insert( Toolkit::DevelAnimatedGradientVisual::Property::OFFSET, BuildMap( 0.0f, 2.0f, 0, mLoadingTime, 0.0f, -1, 0.0f, 0, 0 ) );

    mColorAnimationEndStart = *( BuildMap( mNormalColor, Vector4( 0, 0, 0, 0 ), 1, mDuration, 0.0f, 1, 0.0f, 0, 0 ).GetMap() );
    mColorAnimationEndEnd = *( BuildMap( mNormalColor, Vector4( 0, 0, 0, 0 ), 1, mDuration, 0.0f, 1, 0.0f, 0, 0 ).GetMap() );
  }

  // Setup background visual property during card change
  void InitMapMove()
  {
    mBackgroundMapMove.Clear();
    mBackgroundMapMove.Insert( Visual::Property::TYPE, DevelVisual::ANIMATED_GRADIENT);
    mBackgroundMapMove.Insert( Toolkit::DevelAnimatedGradientVisual::Property::SPREAD_TYPE, Toolkit::DevelAnimatedGradientVisual::SpreadType::CLAMP);

    mBackgroundMapMove.Insert( Toolkit::DevelAnimatedGradientVisual::Property::START_POSITION, Vector2( -0.5, 0.0 ) );
    mBackgroundMapMove.Insert( Toolkit::DevelAnimatedGradientVisual::Property::END_POSITION, Vector2( 0.5, 0.0 ) );
    mBackgroundMapMove.Insert( Toolkit::DevelAnimatedGradientVisual::Property::START_COLOR, mNormalStartColor );
    mBackgroundMapMove.Insert( Toolkit::DevelAnimatedGradientVisual::Property::END_COLOR, mNormalEndColor );
    mBackgroundMapMove.Insert( Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_CENTER, Vector2( 0.0f, 0.0f ) );
    mBackgroundMapMove.Insert( Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT, 0.0f );
    mBackgroundMapMove.Insert( Toolkit::DevelAnimatedGradientVisual::Property::OFFSET, BuildMap( -1.0f, 1.0f, 0, mCardDuration, 0.0f, 1, 0.0f, 0, 2 ) );
  }

  void InitMap()
  {
    InitMapNormal();
    InitMapStart();
    InitMapEnd();
    InitMapMove();
  }

  // Setup const parameter values
  void BuildParameter()
  {
    mNormalColor = DEFAULT_COLOR;
    mCardDuration = CARD_MOVE_DURATION;
    mDuration = CHANGE_DURATION;
    mLoadingTime = LOADING_ONE_CYCLE_DURATION;
    mLoadingCount = LOADING_CYCLE_CNT;
    mLoadingCountScale = LOADING_CYCLE_DT;

    mNormalStartColor = mNormalColor;
    mNormalEndColor = mNormalColor;
  }

  void BuildAnimation()
  {
    InitMap();
  }

  void SetupCards()
  {
    mCards.Init( mStage );
    for( int k = 0; k < CARD_NUM; k++ )
    {
      mCards[k].TouchSignal().Connect( this, &CardController::OnTouchCards );
    }
    mNormalStartColor = mCards.GetColorBackground( mCards.mCurIndex );
    mNormalEndColor = mCards.GetColorBackground( mCards.mCurIndex );
  }

  // Create and Add to stage
  void SetupActors()
  {
    mAddButton = ImageView::New();
    mAddButton.SetImage( BUTTON_ADD_IMAGE );
    mAddButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mAddButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mAddButton.SetSize( BUTTON_ADD_SIZE * SCALED_WIDTH );
    mAddButton.SetPosition( BUTTON_ADD_POSITION * SCALED_WIDTH );

    mLabel1 = TextLabel::New( LABEL_TICKET_STR );
    mLabel1.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mLabel1.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mLabel1.SetSize( LABEL_TICKET_SIZE * SCALED_WIDTH );
    mLabel1.SetPosition( LABEL_TICKET_POSITION * SCALED_WIDTH );
    mLabel1.SetVisible( true );
    mLabel1.SetProperty( TextLabel::Property::TEXT_COLOR, LABEL_TICKET_FONT_COLOR );
    mLabel1.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_TICKET_FONT_SIZE * FONT_SCALE );
    mLabel1.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mLabel1.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

    mLabel2 = TextLabel::New( LABEL_HOLD_STR );
    mLabel2.SetParentOrigin( ParentOrigin::CENTER );
    mLabel2.SetAnchorPoint( AnchorPoint::CENTER );
    mLabel2.SetSize( LABEL_HOLD_SIZE * SCALED_WIDTH );
    mLabel2.SetPosition( LABEL_HOLD_POSITION * SCALED_WIDTH );
    mLabel2.SetVisible( false );
    mLabel2.SetProperty( TextLabel::Property::TEXT_COLOR, LABEL_HOLD_FONT_COLOR );
    mLabel2.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_HOLD_FONT_SIZE * FONT_SCALE );
    mLabel2.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mLabel2.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

    mLabel3 = TextLabel::New( LABEL_TERMINAL_STR );
    mLabel3.SetParentOrigin( ParentOrigin::CENTER );
    mLabel3.SetAnchorPoint( AnchorPoint::CENTER );
    mLabel3.SetSize( LABEL_TERMINAL_SIZE * SCALED_WIDTH );
    mLabel3.SetPosition( LABEL_TERMINAL_POSITION * SCALED_WIDTH );
    mLabel3.SetVisible( false );
    mLabel3.SetProperty( TextLabel::Property::TEXT_COLOR, LABEL_TERMINAL_FONT_COLOR );
    mLabel3.SetProperty( TextLabel::Property::POINT_SIZE, LABEL_TERMINAL_FONT_SIZE * FONT_SCALE );
    mLabel3.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mLabel3.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

    mStage.Add( mAddButton );
    mStage.Add( mLabel1 );
    mStage.Add( mLabel2 );
    mStage.Add( mLabel3 );
  }

  void SetupAnimation()
  {
    mMoveFront = Animation::New( mDuration );
    mMoveBack = Animation::New( mDuration );
  }

  // Run animations when 'index' card active
  void RunAnimation(int index)
  {
    //set animated background color here
    mColorAnimationStartStart[DevelAnimatedGradientVisual::AnimationParameter::Property::START] = mNormalStartColor;
    mColorAnimationStartEnd[DevelAnimatedGradientVisual::AnimationParameter::Property::START] = mNormalStartColor;
    mColorAnimationEndStart[DevelAnimatedGradientVisual::AnimationParameter::Property::START] = mNormalStartColor;
    mColorAnimationEndEnd[DevelAnimatedGradientVisual::AnimationParameter::Property::START] = mNormalStartColor;

    mColorAnimationStartStart[DevelAnimatedGradientVisual::AnimationParameter::Property::TARGET] = mCards.GetColorStart( index );
    mColorAnimationStartEnd[DevelAnimatedGradientVisual::AnimationParameter::Property::TARGET] = mCards.GetColorEnd( index );
    mColorAnimationEndStart[DevelAnimatedGradientVisual::AnimationParameter::Property::TARGET] = mCards.GetColorStart( index );
    mColorAnimationEndEnd[DevelAnimatedGradientVisual::AnimationParameter::Property::TARGET] = mCards.GetColorEnd( index );

    mBackgroundMapStart[Toolkit::DevelAnimatedGradientVisual::Property::START_COLOR] = mColorAnimationStartStart;
    mBackgroundMapStart[Toolkit::DevelAnimatedGradientVisual::Property::END_COLOR] = mColorAnimationStartEnd;
    mBackgroundMapEnd[Toolkit::DevelAnimatedGradientVisual::Property::START_COLOR] = mColorAnimationEndStart;
    mBackgroundMapEnd[Toolkit::DevelAnimatedGradientVisual::Property::END_COLOR] = mColorAnimationEndEnd;

    if( index == 1 )
    {
      // Rotate background gradient
      mBackgroundMapStart[Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT] = BuildMap( 0.0f, Math::PI * 2.0f, 0, mLoadingTime, 0.0f, -1, 0.0f, 0, 0 );
    }
    else if( index == 2 )
    {
      // Rotate background gradient more slow
      mBackgroundMapStart[Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT] = BuildMap( 0.0f, Math::PI * 2.0f, 0, mLoadingTime * 2.0, 0.0f, -1, 0.0f, 0, 0 );
    }
    else
    {
      mBackgroundMapStart[Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT] = 0.0f;
    }

    mBackground.SetProperty( Control::Property::BACKGROUND, mBackgroundMapStart );

    mTickCount = 0;
    mBackgroundChanger = Timer::New( mLoadingTime * 1000.0f / mLoadingCountScale );
    mBackgroundChanger.TickSignal().Connect( this, &CardController::OnTickBackground );
    mBackgroundChanger.Start();

    mMoveFront.AnimateTo( Property( mCards[index], Actor::Property::SIZE_WIDTH ), CARD_SIZE_BIG.x * SCALED_WIDTH );
    mMoveFront.AnimateTo( Property( mCards[index], Actor::Property::SIZE_HEIGHT ), CARD_SIZE_BIG.y * SCALED_HEIGHT );
    mMoveFront.AnimateTo( Property( mCards[index], Actor::Property::POSITION_Y ), CARD_BIG_OFFSET.y * SCALED_HEIGHT );
    mMoveBack.AnimateTo( Property( mCards[index], Actor::Property::SIZE_WIDTH ), CARD_SIZE.x * SCALED_WIDTH );
    mMoveBack.AnimateTo( Property( mCards[index], Actor::Property::SIZE_HEIGHT ), CARD_SIZE.y * SCALED_HEIGHT );
    mMoveBack.AnimateTo( Property( mCards[index], Actor::Property::POSITION_Y ), CARD_OFFSET.y * SCALED_HEIGHT );
    for( int i = 0; i < index; i++ )
    {
      mMoveFront.AnimateBy( Property( mCards[i], Actor::Property::POSITION_X ), -CARD_MOVE_DIST * SCALED_WIDTH );
      mMoveBack.AnimateBy( Property( mCards[i], Actor::Property::POSITION_X ), CARD_MOVE_DIST * SCALED_WIDTH );
    }
    for( int i = index + 1; i < CARD_NUM; i++ )
    {
      mMoveFront.AnimateBy( Property( mCards[i], Actor::Property::POSITION_X ), CARD_MOVE_DIST * SCALED_WIDTH );
      mMoveBack.AnimateBy( Property( mCards[i], Actor::Property::POSITION_X ), -CARD_MOVE_DIST * SCALED_WIDTH );
    }
    mMoveFront.AnimateTo( Property( mAddButton, Actor::Property::POSITION_Y ), BUTTON_ADD_SIZE.y * SCALED_HEIGHT );
    mMoveBack.AnimateTo( Property( mAddButton, Actor::Property::POSITION_Y ), 0.f * SCALED_HEIGHT );

    mMoveFront.AnimateTo( Property( mLabel1, Actor::Property::VISIBLE), false );
    mMoveFront.AnimateTo( Property( mLabel2, Actor::Property::VISIBLE), true );
    mMoveFront.AnimateTo( Property( mLabel3, Actor::Property::VISIBLE), true );
    mMoveBack.AnimateTo( Property( mLabel1, Actor::Property::VISIBLE), true );
    mMoveBack.AnimateTo( Property( mLabel2, Actor::Property::VISIBLE), false );
    mMoveBack.AnimateTo( Property( mLabel3, Actor::Property::VISIBLE), false );

    mMoveFront.Play();
  }
  bool OnTickBackground()
  {
    mTickCount++;
    if( mCancelSignal || mTickCount >= mLoadingCount * mLoadingCountScale )
    {
      if( mCards.mCurState == 1 )
      {
        mCards.mCurState = 2;
        mBackground.SetProperty( Control::Property::BACKGROUND, mBackgroundMapEnd );
        mMoveBack.Play();
        mBackgroundChanger.SetInterval( mDuration * 1000.0f );
        return true;
      }
      else
      {
        Reset();
        return false;
      }
    }
    return true;
  }
  void MoveRight()
  {
    if( mCards.MoveRight( mCardDuration ) )
    {
      // Set smooth background color change here
      mNormalEndColor = mCards.GetColorBackground( mCards.mCurIndex );
      mBackgroundMapMove[Toolkit::DevelAnimatedGradientVisual::Property::START_COLOR] = mNormalEndColor;
      mBackgroundMapMove[Toolkit::DevelAnimatedGradientVisual::Property::END_COLOR] = mNormalStartColor;

      (*mBackgroundMapMove[Toolkit::DevelAnimatedGradientVisual::Property::OFFSET].GetMap())[DevelAnimatedGradientVisual::AnimationParameter::Property::DIRECTION] = Property::Value( DevelAnimatedGradientVisual::AnimationParameter::DirectionType::BACKWARD );

      mBackground.SetProperty( Control::Property::BACKGROUND, mBackgroundMapMove );
    }
  }
  void MoveLeft()
  {
    if( mCards.MoveLeft( mCardDuration ) )
    {
      //Set smooth background color change here
      mNormalEndColor = mCards.GetColorBackground( mCards.mCurIndex );
      mBackgroundMapMove[Toolkit::DevelAnimatedGradientVisual::Property::START_COLOR] = mNormalStartColor;
      mBackgroundMapMove[Toolkit::DevelAnimatedGradientVisual::Property::END_COLOR] = mNormalEndColor;

      (*mBackgroundMapMove[Toolkit::DevelAnimatedGradientVisual::Property::OFFSET].GetMap())[DevelAnimatedGradientVisual::AnimationParameter::Property::DIRECTION] = Property::Value( DevelAnimatedGradientVisual::AnimationParameter::DirectionType::FORWARD );

      mBackground.SetProperty( Control::Property::BACKGROUND, mBackgroundMapMove );
    }
  }

  void Reset()
  {
    mCards.mCurState = 0;
    mCancelSignal = false;
    mMoveFront.Clear();
    mMoveBack.Clear();

    mNormalStartColor = mNormalEndColor;
    mBackgroundNormalMap[Toolkit::DevelAnimatedGradientVisual::Property::START_COLOR] = mNormalStartColor;
    mBackgroundNormalMap[Toolkit::DevelAnimatedGradientVisual::Property::END_COLOR] = mNormalEndColor;
    mBackground.SetProperty(Control::Property::BACKGROUND, mBackgroundNormalMap);
  }

private:
  Application&  mApplication;
  Stage mStage;

  CardManager mCards;

  Control mBackground;

  ImageView mAddButton;
  TextLabel mLabel1;
  TextLabel mLabel2;
  TextLabel mLabel3;

  Timer mBackgroundChanger;
  Timer mCardChanger;
  Timer mTempTimer;

  Animation mMoveFront;
  Animation mMoveBack;

  // Property for background animated gradient visual
  Property::Map mBackgroundNormalMap;
  Property::Map mBackgroundMapStart;
  Property::Map mBackgroundMapEnd;
  Property::Map mBackgroundMapMove;

  // Property for animation of color in animated gradient visual
  Property::Map mColorAnimationStartStart;
  Property::Map mColorAnimationStartEnd;
  Property::Map mColorAnimationEndStart;
  Property::Map mColorAnimationEndEnd;

  Vector4 mNormalColor;
  Vector4 mNormalStartColor;
  Vector4 mNormalEndColor;

  Vector2 mFirstTouchPos;
  Vector2 mLastTouchPos;

  float mCardDuration;
  float mDuration;
  float mLoadingTime;
  int mLoadingCount;
  int mLoadingCountScale;
  int mTickCount;

  bool mCancelSignal;
  bool mIsTouchedActor;
};

int main(int argc, char **argv)
{
  Application application = Application::New( &argc, &argv );

  CardController test( application );

  application.MainLoop();

  return 0;
}
