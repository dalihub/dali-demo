/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali/dali.h>
#include <dali/integration-api/debug.h>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

// This example shows how to create and display gradient-effect with animate start offset
//
namespace
{
// The value for scale-change between wearable-mobile
// Can be changed on App-Create time
Vector2 WINDOW_SIZE          = Vector2(360.0f, 360.0f);
Vector2 SCALED_WINDOW_SIZE   = Vector2(1.0f, 1.0f);
Vector3 SCALED_WINDOW_SIZE_3 = Vector3(1.0f, 1.0f, 0.0f);
float   SCALED_WIDTH         = 1.0f;
float   SCALED_HEIGHT        = 1.0f;
float   FONT_SCALE           = 0.25f;

// const parameters for color and animation
const float   CHANGE_DURATION            = 0.2f;
const float   CARD_MOVE_DURATION         = 1.0f;
const float   LOADING_ONE_CYCLE_DURATION = 2.0f;
const int     LOADING_CYCLE_CNT          = 4;
const int     LOADING_CYCLE_DT           = 10;
const Vector4 DEFAULT_COLOR              = Vector4(0.5f, 0.5f, 0.5f, 1.0f);

const float PSEUDO_SCROLL_TIME   = 100.0f;
const float PSEUDO_SCROLL_OFFSET = 0.05f;

// const static parameters for cards when resolution is 360x360
const int     CARD_NUM                   = 3;
const Vector2 CARD_SIZE                  = Vector2(210.0f, 143.0f);
const Vector2 CARD_OFFSET                = Vector2(0.0f, 12.5f);
const Vector2 CARD_DIFF                  = Vector2(240.0f, 0.0f);
const float   CARD_BOUNCE_ANIMATION_RATE = 0.3f;

const Vector2 CARD_SIZE_BIG   = Vector2(292.0f, 199.0f);
const Vector2 CARD_BIG_OFFSET = Vector2(0.0f, -5.5f);
const float   CARD_MOVE_DIST  = 40.0f;

// const private parameters for each cards
const Vector4 CARD_COLOR_START_LIST[] =
  {
    Vector4(0x24, 0x2C, 0x93, 255.f) / 255.f,
    Vector4(0x7A, 0x1C, 0x9E, 255.f) / 255.f,
    Vector4(0xA9, 0x0C, 0x96, 255.f) / 255.f,
};
const Vector4 CARD_COLOR_END_LIST[] =
  {
    Vector4(0x04, 0x13, 0x23, 255.f) / 255.f,
    Vector4(0x28, 0x01, 0x45, 255.f) / 255.f,
    Vector4(0x37, 0x0A, 0x2E, 255.f) / 255.f,
};
const Vector4 CARD_COLOR_BACKGROUND_LIST[] =
  {
    Vector4(0x28, 0x2B, 0x6E, 255.f) / 255.f,
    Vector4(0x4D, 0x15, 0x61, 255.f) / 255.f,
    Vector4(0x70, 0x21, 0x61, 255.f) / 255.f,
};
const char* const CARD_IMAGE_LIST[] =
  {
    (DEMO_IMAGE_DIR "Card_01.png"),
    (DEMO_IMAGE_DIR "Card_02.png"),
    (DEMO_IMAGE_DIR "Card_03.png"),
};

// const parameters for add button position and size when resolution is 360x360x
const Vector2     BUTTON_ADD_SIZE     = Vector2(292.0f, 52.0f);
const Vector3     BUTTON_ADD_POSITION = Vector3(0.0f, 0.0f, 0.0f);
const char* const BUTTON_ADD_IMAGE(DEMO_IMAGE_DIR "Card_Add_Button.png");

// const parameters for string position and size and font-size when resolution is 360x360
const Vector2 LABEL_TICKET_SIZE         = Vector2(148.0f, 31.0f);
const Vector3 LABEL_TICKET_POSITION     = Vector3(0.0f, 72.5f, 0.0f);
const Vector4 LABEL_TICKET_FONT_COLOR   = Vector4(0.98f, 0.98f, 0.98f, 1.0f);
const float   LABEL_TICKET_FONT_SIZE    = 25.0f;
const Vector2 LABEL_HOLD_SIZE           = Vector2(180.0f, 60.0f);
const Vector3 LABEL_HOLD_POSITION       = Vector3(1.0f, 103.0f, 0.0f);
const Vector4 LABEL_HOLD_FONT_COLOR     = Vector4(0.98f, 0.98f, 0.98f, 1.0f);
const float   LABEL_HOLD_FONT_SIZE      = 25.0f;
const Vector2 LABEL_TERMINAL_SIZE       = Vector2(180.0f, 60.0f);
const Vector3 LABEL_TERMINAL_POSITION   = Vector3(1.0f, 133.0f, 0.0f);
const Vector4 LABEL_TERMINAL_FONT_COLOR = Vector4(0.98f, 0.98f, 0.98f, 1.0f);
const float   LABEL_TERMINAL_FONT_SIZE  = 25.0f;

// string string
const char* const LABEL_TICKET_STR("Select Ticket");
const char* const LABEL_HOLD_STR("Hold near");
const char* const LABEL_TERMINAL_STR("terminal");

class CardManager
{
public:
  CardManager()
  : mSize(0.0f, 0.0f),
    mOffset(0.0f, 0.0f),
    mDiff(0.0f, 0.0f),
    mCurIndex(0),
    mCurState(0)
  {
  }
  ~CardManager()
  {
  }

  void Init(Window& window)
  {
    mSize   = CARD_SIZE * SCALED_WIDTH;
    mOffset = CARD_OFFSET * SCALED_WIDTH;
    mDiff   = CARD_DIFF * SCALED_WIDTH;

    mCurIndex = 0;
    mCurState = 0;

    for(int k = 0; k < CARD_NUM; k++)
    {
      mPosition[k] = mOffset + mDiff * k;

      mColorStart[k]      = CARD_COLOR_START_LIST[k];
      mColorEnd[k]        = CARD_COLOR_END_LIST[k];
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

      mCard[k].SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      mCard[k].SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      mCard[k].SetProperty(Actor::Property::SIZE, Vector2(mSize.x, mSize.y));
      mCard[k].SetProperty(Actor::Property::POSITION, Vector2(mPosition[k].x, mPosition[k].y));

      window.Add(mCard[k]);
    }
  }

  bool MoveRight(float duration)
  {
    Animation anim = Animation::New(duration);
    for(int k = 0; k < CARD_NUM; k++)
    {
      if(mCurIndex == 0)
      {
        anim.AnimateBy(Property(mCard[k], Actor::Property::POSITION_X), mDiff.x * CARD_BOUNCE_ANIMATION_RATE, AlphaFunction::BOUNCE);
      }
      else
      {
        anim.AnimateBy(Property(mCard[k], Actor::Property::POSITION_X), mDiff.x, AlphaFunction::EASE_OUT);
      }
    }
    bool res = false;
    if(mCurIndex != 0)
    {
      mCurIndex = (mCurIndex - 1) % CARD_NUM;
      res       = true;
    }
    anim.Play();
    return res;
  }

  bool MoveLeft(float duration)
  {
    Animation anim = Animation::New(duration);
    for(int k = 0; k < CARD_NUM; k++)
    {
      if(mCurIndex == CARD_NUM - 1)
      {
        anim.AnimateBy(Property(mCard[k], Actor::Property::POSITION_X), -mDiff.x * CARD_BOUNCE_ANIMATION_RATE, AlphaFunction::BOUNCE);
      }
      else
      {
        anim.AnimateBy(Property(mCard[k], Actor::Property::POSITION_X), -mDiff.x, AlphaFunction::EASE_OUT);
      }
    }
    bool res = false;
    if(mCurIndex != CARD_NUM - 1)
    {
      mCurIndex = (mCurIndex + 1) % CARD_NUM;
      res       = true;
    }
    anim.Play();
    return res;
  }

  Vector4 GetColorStart(int index)
  {
    DALI_ASSERT_ALWAYS(index >= 0 && index < CARD_NUM);
    return mColorStart[index];
  }

  Vector4 GetColorEnd(int index)
  {
    DALI_ASSERT_ALWAYS(index >= 0 && index < CARD_NUM);
    return mColorEnd[index];
  }

  Vector4 GetColorBackground(int index)
  {
    DALI_ASSERT_ALWAYS(index >= 0 && index < CARD_NUM);
    return mColorBackground[index];
  }

  ImageView& operator[](int index)
  {
    DALI_ASSERT_ALWAYS(index >= 0 && index < CARD_NUM);
    return mCard[index];
  }

  ImageView   mCard[CARD_NUM];
  std::string mImageUrl[CARD_NUM];
  Vector4     mColorStart[CARD_NUM];
  Vector4     mColorEnd[CARD_NUM];
  Vector4     mColorBackground[CARD_NUM];
  Vector2     mPosition[CARD_NUM];
  Vector2     mSize;
  Vector2     mOffset;
  Vector2     mDiff;
  int         mCurIndex;
  int         mCurState;
};

} // unnamed namespace

// This example shows how to render animated gradients
//
class CardController : public ConnectionTracker
{
public:
  CardController(Application& application)
  : mApplication(application),
    mNormalColor(0.0f, 0.0f, 0.0f, 0.0f),
    mNormalStartColor(0.0f, 0.0f, 0.0f, 0.0f),
    mNormalEndColor(0.0f, 0.0f, 0.0f, 0.0f),
    mFirstTouchPos(0.0f, 0.0f),
    mLastTouchPos(0.0f, 0.0f),
    mCardDuration(0.0f),
    mDuration(0.0f),
    mLoadingTime(0.0f),
    mLoadingCount(0),
    mLoadingCountScale(0),
    mTickCount(0),
    mCancelSignal(false),
    mIsTouchedActor(false)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &CardController::Create);
  }

  ~CardController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    mWindow = application.GetWindow();
    mWindow.KeyEventSignal().Connect(this, &CardController::OnKeyEvent);

    // Get current device's width and height.
    const Window::WindowSize windowSize = mWindow.GetSize();
    WINDOW_SIZE                         = Vector2(windowSize.GetWidth(), windowSize.GetHeight());
    SCALED_WINDOW_SIZE                  = WINDOW_SIZE / 360.0f;
    SCALED_WINDOW_SIZE_3                = Vector3(SCALED_WINDOW_SIZE.x, SCALED_WINDOW_SIZE.y, 0.0f);
    SCALED_WIDTH                        = SCALED_WINDOW_SIZE.x < SCALED_WINDOW_SIZE.y ? SCALED_WINDOW_SIZE.x : SCALED_WINDOW_SIZE.y;
    SCALED_HEIGHT                       = SCALED_WIDTH;

    // Note that this is heuristic value
    FONT_SCALE = 0.25f * WINDOW_SIZE.y / WINDOW_SIZE.x;

    mBackground = Control::New();
    mBackground.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mBackground.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mBackground.SetProperty(Actor::Property::SIZE, WINDOW_SIZE * 2.0f);

    mWindow.Add(mBackground);

    BuildParameter();
    InitMap();
    SetupCards();
    SetupActors();
    SetupAnimation();

    mWindow.GetRootLayer().TouchedSignal().Connect(this, &CardController::OnTouchLayer);
    Reset();
  }

  bool OnTouchCards(Actor actor, const TouchEvent& data)
  {
    if(data.GetPointCount() > 0)
    {
      if(data.GetState(0) == PointState::DOWN)
      {
        if(mCards.mCurState == 0)
        {
          mIsTouchedActor = false;
          if(mCards[mCards.mCurIndex] == actor)
          {
            mIsTouchedActor = true;
          }
          mCards.mCurState = 3;
          mTempTimer       = Timer::New(PSEUDO_SCROLL_TIME);
          mTempTimer.TickSignal().Connect(this, &CardController::OnDetectMotionLayer);
          mTempTimer.Start();

          mFirstTouchPos = data.GetScreenPosition(0);
          mLastTouchPos  = mFirstTouchPos;
        }
        else if(mCards.mCurState == 1)
        {
          mCancelSignal = true;
          return false;
        }
      }
      else
      {
        mLastTouchPos = data.GetScreenPosition(0);
      }
    }
    return true;
  }

  bool OnTouchLayer(Actor actor, const TouchEvent& data)
  {
    if(data.GetPointCount() > 0)
    {
      if(data.GetState(0) == PointState::DOWN)
      {
        if(mCards.mCurState == 0)
        {
          mIsTouchedActor  = false;
          mCards.mCurState = 3;
          mTempTimer       = Timer::New(PSEUDO_SCROLL_TIME);
          mTempTimer.TickSignal().Connect(this, &CardController::OnDetectMotionLayer);
          mTempTimer.Start();

          mFirstTouchPos = data.GetScreenPosition(0);
          mLastTouchPos  = mFirstTouchPos;
        }
      }
      else
      {
        mLastTouchPos = data.GetScreenPosition(0);
      }
    }
    return true;
  }

  // Heuristic Scroll View
  bool OnDetectMotionLayer()
  {
    if(mCards.mCurState == 3)
    {
      Vector2     diff        = (mLastTouchPos - mFirstTouchPos);
      float       offset      = PSEUDO_SCROLL_OFFSET;
      const float windowWidth = mWindow.GetSize().GetWidth();
      // Scroll to right
      if(diff.x > windowWidth * offset)
      {
        mCards.mCurState = 2;
        MoveRight();
        mCardChanger = Timer::New(mCardDuration * 1000.0f);
        mCardChanger.TickSignal().Connect(this, &CardController::OnTickLayer);
        mCardChanger.Start();
      }
      // Scroll to left
      else if(diff.x < -windowWidth * offset)
      {
        mCards.mCurState = 2;
        MoveLeft();
        mCardChanger = Timer::New(mCardDuration * 1000.0f);
        mCardChanger.TickSignal().Connect(this, &CardController::OnTickLayer);
        mCardChanger.Start();
      }
      // Not a scroll input
      else
      {
        // Run NFC Tag effect if we touch a card
        if(mIsTouchedActor)
        {
          mCards.mCurState = 1;
          RunAnimation(mCards.mCurIndex);
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
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  // Setup background visual property during nothing action
  void InitMapNormal()
  {
    mBackgroundNormalMap.Clear();
    mBackgroundNormalMap.Insert(Visual::Property::TYPE, Visual::COLOR);
    mBackgroundNormalMap.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, mNormalColor);
  }

  // Setup background visual property during NFC tagging
  void InitMapActive()
  {
    mBackgroundMapActive.Clear();
    mBackgroundMapActive.Insert(Visual::Property::TYPE, DevelVisual::GRADIENT);

    mBackgroundMapActive.Insert(Toolkit::GradientVisual::Property::START_POSITION, Vector2(-0.5, -0.5));
    mBackgroundMapActive.Insert(Toolkit::GradientVisual::Property::END_POSITION, Vector2(0.5, 0.5));
    mBackgroundMapActive.Insert(Toolkit::GradientVisual::Property::STOP_OFFSET, Property::Array().Add(0.0f).Add(1.0f));
    mBackgroundMapActive.Insert(Toolkit::GradientVisual::Property::STOP_COLOR, Property::Array().Add(mNormalColor).Add(mNormalColor));
    mBackgroundMapActive.Insert(Toolkit::GradientVisual::Property::UNITS, GradientVisual::Units::OBJECT_BOUNDING_BOX);
    mBackgroundMapActive.Insert(Toolkit::GradientVisual::Property::SPREAD_METHOD, GradientVisual::SpreadMethod::REFLECT);
  }

  // Setup background visual property during card change
  void InitMapMove()
  {
    mBackgroundMapMove.Clear();
    mBackgroundMapMove.Insert(Visual::Property::TYPE, DevelVisual::GRADIENT);

    mBackgroundMapMove.Insert(Toolkit::GradientVisual::Property::START_POSITION, Vector2(-0.5, 0.0));
    mBackgroundMapMove.Insert(Toolkit::GradientVisual::Property::END_POSITION, Vector2(0.5, 0.0));
    mBackgroundMapMove.Insert(Toolkit::GradientVisual::Property::STOP_OFFSET, Property::Array().Add(0.0f).Add(1.0f));
    mBackgroundMapMove.Insert(Toolkit::GradientVisual::Property::STOP_COLOR, Property::Array().Add(mNormalStartColor).Add(mNormalEndColor));
    mBackgroundMapMove.Insert(Toolkit::GradientVisual::Property::UNITS, GradientVisual::Units::OBJECT_BOUNDING_BOX);
    mBackgroundMapMove.Insert(Toolkit::GradientVisual::Property::SPREAD_METHOD, GradientVisual::SpreadMethod::PAD);
  }

  void InitMap()
  {
    InitMapNormal();
    InitMapActive();
    InitMapMove();
  }

  // Setup const parameter values
  void BuildParameter()
  {
    mNormalColor       = DEFAULT_COLOR;
    mCardDuration      = CARD_MOVE_DURATION;
    mDuration          = CHANGE_DURATION;
    mLoadingTime       = LOADING_ONE_CYCLE_DURATION;
    mLoadingCount      = LOADING_CYCLE_CNT;
    mLoadingCountScale = LOADING_CYCLE_DT;

    mNormalStartColor = mNormalColor;
    mNormalEndColor   = mNormalColor;
  }

  void SetupCards()
  {
    mCards.Init(mWindow);
    for(int k = 0; k < CARD_NUM; k++)
    {
      mCards[k].TouchedSignal().Connect(this, &CardController::OnTouchCards);
    }
    mNormalStartColor = mCards.GetColorBackground(mCards.mCurIndex);
    mNormalEndColor   = mCards.GetColorBackground(mCards.mCurIndex);
  }

  // Create and Add to window
  void SetupActors()
  {
    mAddButton = ImageView::New();
    mAddButton.SetImage(BUTTON_ADD_IMAGE);
    mAddButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
    mAddButton.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
    mAddButton.SetProperty(Actor::Property::SIZE, BUTTON_ADD_SIZE * SCALED_WIDTH);
    mAddButton.SetProperty(Actor::Property::POSITION, BUTTON_ADD_POSITION * SCALED_WIDTH);

    mLabel1 = TextLabel::New(LABEL_TICKET_STR);
    mLabel1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
    mLabel1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    mLabel1.SetProperty(Actor::Property::SIZE, LABEL_TICKET_SIZE * SCALED_WIDTH);
    mLabel1.SetProperty(Actor::Property::POSITION, LABEL_TICKET_POSITION * SCALED_WIDTH);
    mLabel1.SetProperty(Actor::Property::VISIBLE, true);
    mLabel1.SetProperty(TextLabel::Property::TEXT_COLOR, LABEL_TICKET_FONT_COLOR);
    mLabel1.SetProperty(TextLabel::Property::POINT_SIZE, LABEL_TICKET_FONT_SIZE * FONT_SCALE);
    mLabel1.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
    mLabel1.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");

    mLabel2 = TextLabel::New(LABEL_HOLD_STR);
    mLabel2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mLabel2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mLabel2.SetProperty(Actor::Property::SIZE, LABEL_HOLD_SIZE * SCALED_WIDTH);
    mLabel2.SetProperty(Actor::Property::POSITION, LABEL_HOLD_POSITION * SCALED_WIDTH);
    mLabel2.SetProperty(Actor::Property::VISIBLE, false);
    mLabel2.SetProperty(TextLabel::Property::TEXT_COLOR, LABEL_HOLD_FONT_COLOR);
    mLabel2.SetProperty(TextLabel::Property::POINT_SIZE, LABEL_HOLD_FONT_SIZE * FONT_SCALE);
    mLabel2.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
    mLabel2.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");

    mLabel3 = TextLabel::New(LABEL_TERMINAL_STR);
    mLabel3.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mLabel3.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mLabel3.SetProperty(Actor::Property::SIZE, LABEL_TERMINAL_SIZE * SCALED_WIDTH);
    mLabel3.SetProperty(Actor::Property::POSITION, LABEL_TERMINAL_POSITION * SCALED_WIDTH);
    mLabel3.SetProperty(Actor::Property::VISIBLE, false);
    mLabel3.SetProperty(TextLabel::Property::TEXT_COLOR, LABEL_TERMINAL_FONT_COLOR);
    mLabel3.SetProperty(TextLabel::Property::POINT_SIZE, LABEL_TERMINAL_FONT_SIZE * FONT_SCALE);
    mLabel3.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
    mLabel3.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");

    mWindow.Add(mAddButton);
    mWindow.Add(mLabel1);
    mWindow.Add(mLabel2);
    mWindow.Add(mLabel3);
  }

  void SetupAnimation()
  {
    mMoveFront = Animation::New(mDuration);
    mMoveBack  = Animation::New(mDuration);
  }

  // Run animations when 'index' card active
  void RunAnimation(int index)
  {
    mBackgroundMapActive[Toolkit::GradientVisual::Property::STOP_COLOR] = Property::Array().Add(mCards.GetColorStart(index)).Add(mCards.GetColorEnd(index));
    mBackground.SetProperty(Control::Property::BACKGROUND, mBackgroundMapActive);

    if(mActiveAnimation)
    {
      mActiveAnimation.Clear();
      mActiveAnimation.Reset();
    }

    mActiveAnimation = Animation::New(mLoadingTime);
    mActiveAnimation.SetEndAction(Animation::EndAction::DISCARD);
    mActiveAnimation.SetLooping(true);

    if(index == 1)
    {
      mActiveAnimation.AnimateTo(DevelControl::GetVisualProperty(mBackground, Control::Property::BACKGROUND, GradientVisual::Property::START_OFFSET), 2.0f);
      // Rotate background gradient
      mActiveAnimation.AnimateBy(Property(mBackground, Actor::Property::ORIENTATION), Dali::Quaternion(Dali::Radian(Dali::Degree(360.0f)), Vector3::ZAXIS));
    }
    else if(index == 2)
    {
      mActiveAnimation.SetDuration(mLoadingTime * 2.0f);
      mActiveAnimation.AnimateTo(DevelControl::GetVisualProperty(mBackground, Control::Property::BACKGROUND, GradientVisual::Property::START_OFFSET), 4.0f);
      // Rotate background gradient more slow
      mActiveAnimation.AnimateBy(Property(mBackground, Actor::Property::ORIENTATION), Dali::Quaternion(Dali::Radian(Dali::Degree(360.0f)), Vector3::ZAXIS));
    }
    else
    {
      mActiveAnimation.AnimateTo(DevelControl::GetVisualProperty(mBackground, Control::Property::BACKGROUND, GradientVisual::Property::START_OFFSET), 2.0f);
    }

    mActiveAnimation.Play();

    mTickCount         = 0;
    mBackgroundChanger = Timer::New(mLoadingTime * 1000.0f / mLoadingCountScale);
    mBackgroundChanger.TickSignal().Connect(this, &CardController::OnTickBackground);
    mBackgroundChanger.Start();

    mMoveFront.AnimateTo(Property(mCards[index], Actor::Property::SIZE_WIDTH), CARD_SIZE_BIG.x * SCALED_WIDTH);
    mMoveFront.AnimateTo(Property(mCards[index], Actor::Property::SIZE_HEIGHT), CARD_SIZE_BIG.y * SCALED_HEIGHT);
    mMoveFront.AnimateTo(Property(mCards[index], Actor::Property::POSITION_Y), CARD_BIG_OFFSET.y * SCALED_HEIGHT);
    mMoveBack.AnimateTo(Property(mCards[index], Actor::Property::SIZE_WIDTH), CARD_SIZE.x * SCALED_WIDTH);
    mMoveBack.AnimateTo(Property(mCards[index], Actor::Property::SIZE_HEIGHT), CARD_SIZE.y * SCALED_HEIGHT);
    mMoveBack.AnimateTo(Property(mCards[index], Actor::Property::POSITION_Y), CARD_OFFSET.y * SCALED_HEIGHT);
    for(int i = 0; i < index; i++)
    {
      mMoveFront.AnimateBy(Property(mCards[i], Actor::Property::POSITION_X), -CARD_MOVE_DIST * SCALED_WIDTH);
      mMoveBack.AnimateBy(Property(mCards[i], Actor::Property::POSITION_X), CARD_MOVE_DIST * SCALED_WIDTH);
    }
    for(int i = index + 1; i < CARD_NUM; i++)
    {
      mMoveFront.AnimateBy(Property(mCards[i], Actor::Property::POSITION_X), CARD_MOVE_DIST * SCALED_WIDTH);
      mMoveBack.AnimateBy(Property(mCards[i], Actor::Property::POSITION_X), -CARD_MOVE_DIST * SCALED_WIDTH);
    }
    mMoveFront.AnimateTo(Property(mAddButton, Actor::Property::POSITION_Y), BUTTON_ADD_SIZE.y * SCALED_HEIGHT);
    mMoveBack.AnimateTo(Property(mAddButton, Actor::Property::POSITION_Y), 0.f * SCALED_HEIGHT);

    mMoveFront.AnimateTo(Property(mLabel1, Actor::Property::VISIBLE), false);
    mMoveFront.AnimateTo(Property(mLabel2, Actor::Property::VISIBLE), true);
    mMoveFront.AnimateTo(Property(mLabel3, Actor::Property::VISIBLE), true);
    mMoveBack.AnimateTo(Property(mLabel1, Actor::Property::VISIBLE), true);
    mMoveBack.AnimateTo(Property(mLabel2, Actor::Property::VISIBLE), false);
    mMoveBack.AnimateTo(Property(mLabel3, Actor::Property::VISIBLE), false);

    mMoveFront.Play();
  }
  bool OnTickBackground()
  {
    mTickCount++;
    if(mCancelSignal || mTickCount >= mLoadingCount * mLoadingCountScale)
    {
      if(mCards.mCurState == 1)
      {
        mCards.mCurState = 2;
        mMoveBack.Play();
        mBackgroundChanger.SetInterval(mDuration * 1000.0f);
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
    if(mCards.MoveRight(mCardDuration))
    {
      // Set smooth background color change here
      mNormalEndColor                                                   = mCards.GetColorBackground(mCards.mCurIndex);
      mBackgroundMapMove[Toolkit::GradientVisual::Property::STOP_COLOR] = Property::Array().Add(mNormalEndColor).Add(mNormalStartColor);
      mBackground.SetProperty(Control::Property::BACKGROUND, mBackgroundMapMove);

      Dali::KeyFrames keyFrames = KeyFrames::New();
      keyFrames.Add(0.0f, 1.0f);
      keyFrames.Add(1.0f, -1.0f);

      Animation anim = Animation::New(mCardDuration);
      anim.AnimateBetween(DevelControl::GetVisualProperty(mBackground, Control::Property::BACKGROUND, GradientVisual::Property::START_OFFSET), keyFrames, AlphaFunction::EASE_OUT);
      anim.Play();
    }
  }
  void MoveLeft()
  {
    auto anim = mCards.MoveLeft(mCardDuration);
    if(anim)
    {
      //Set smooth background color change here
      mNormalEndColor                                                   = mCards.GetColorBackground(mCards.mCurIndex);
      mBackgroundMapMove[Toolkit::GradientVisual::Property::STOP_COLOR] = Property::Array().Add(mNormalStartColor).Add(mNormalEndColor);
      mBackground.SetProperty(Control::Property::BACKGROUND, mBackgroundMapMove);

      Dali::KeyFrames keyFrames = KeyFrames::New();
      keyFrames.Add(0.0f, -1.0f);
      keyFrames.Add(1.0f, 1.0f);

      Animation anim = Animation::New(mCardDuration);
      anim.AnimateBetween(DevelControl::GetVisualProperty(mBackground, Control::Property::BACKGROUND, GradientVisual::Property::START_OFFSET), keyFrames, AlphaFunction::EASE_OUT);
      anim.Play();
    }
  }

  void Reset()
  {
    mCards.mCurState = 0;
    mCancelSignal    = false;
    mMoveFront.Clear();
    mMoveBack.Clear();
    if(mActiveAnimation)
    {
      mActiveAnimation.Clear();
      mActiveAnimation.Reset();
    }

    mNormalStartColor = mNormalEndColor;

    mBackgroundNormalMap[ColorVisual::Property::MIX_COLOR] = mNormalStartColor;

    mBackground.SetProperty(Control::Property::BACKGROUND, mBackgroundNormalMap);
  }

private:
  Application& mApplication;
  Window       mWindow;

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
  Animation mActiveAnimation;

  // Property for background gradient visual
  Property::Map mBackgroundNormalMap;
  Property::Map mBackgroundMapActive;
  Property::Map mBackgroundMapMove;

  Vector4 mNormalColor;
  Vector4 mNormalStartColor;
  Vector4 mNormalEndColor;

  Vector2 mFirstTouchPos;
  Vector2 mLastTouchPos;

  float mCardDuration;
  float mDuration;
  float mLoadingTime;
  int   mLoadingCount;
  int   mLoadingCountScale;
  int   mTickCount;

  bool mCancelSignal;
  bool mIsTouchedActor;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  CardController test(application);

  application.MainLoop();

  return 0;
}
