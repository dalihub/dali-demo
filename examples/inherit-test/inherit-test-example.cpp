/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <sstream>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
constexpr Vector2 PARENT_VIEW_SIZE_LIST[] = {
  Vector2(100.0f, 150.0f),
  Vector2(200.0f, 100.0f),
  Vector2(100.0f, 200.0f),
  Vector2(100.0f, 100.0f),
};
constexpr int PARENT_VIEW_SIZE_LIST_COUNT = sizeof(PARENT_VIEW_SIZE_LIST) / sizeof(PARENT_VIEW_SIZE_LIST[0]);

constexpr Vector2 PARENT_VIEW_SCALE_LIST[] = {
  Vector2(2.0f, 0.5f),
  Vector2(1.0f, 1.0f),
  Vector2(0.5f, 2.0f),
};
constexpr int PARENT_VIEW_SCALE_LIST_COUNT = sizeof(PARENT_VIEW_SCALE_LIST) / sizeof(PARENT_VIEW_SCALE_LIST[0]);

constexpr Vector2 CURRENT_VIEW_SIZE_LIST[] = {
  Vector2(200.0f, 250.0f),
  Vector2(200.0f, 100.0f),
  Vector2(100.0f, 200.0f),
  Vector2(10.0f, 10.0f),
};
constexpr int CURRENT_VIEW_SIZE_LIST_COUNT = sizeof(CURRENT_VIEW_SIZE_LIST) / sizeof(CURRENT_VIEW_SIZE_LIST[0]);

constexpr Vector2 CURRENT_VIEW_POSITION_LIST[] = {
  Vector2(100.0f, 120.0f),
  Vector2(0.0f, 0.0f),
  Vector2(100.0f, 100.0f),
  Vector2(-100.0f, -200.0f),
};
constexpr int CURRENT_VIEW_POSITION_LIST_COUNT = sizeof(CURRENT_VIEW_POSITION_LIST) / sizeof(CURRENT_VIEW_POSITION_LIST[0]);

constexpr Vector2 CURRENT_VIEW_SCALE_LIST[] = {
  Vector2(0.5f, 2.0f),
  Vector2(1.0f, 1.0f),
  Vector2(2.0f, 0.5f),
};
constexpr int CURRENT_VIEW_SCALE_LIST_COUNT = sizeof(CURRENT_VIEW_SCALE_LIST) / sizeof(CURRENT_VIEW_SCALE_LIST[0]);

constexpr Vector3 PARENT_ORIGIN_LIST[] = {
  ParentOrigin::TOP_LEFT,
  ParentOrigin::TOP_CENTER,
  ParentOrigin::TOP_RIGHT,
  ParentOrigin::CENTER_LEFT,
  ParentOrigin::CENTER,
  ParentOrigin::CENTER_RIGHT,
  ParentOrigin::BOTTOM_LEFT,
  ParentOrigin::BOTTOM_CENTER,
  ParentOrigin::BOTTOM_RIGHT,
};
constexpr int PARENT_ORIGIN_LIST_COUNT = sizeof(PARENT_ORIGIN_LIST) / sizeof(PARENT_ORIGIN_LIST[0]);

constexpr Vector3 ANCHOR_POINT_LIST[] = {
  AnchorPoint::TOP_LEFT,
  AnchorPoint::TOP_CENTER,
  AnchorPoint::TOP_RIGHT,
  AnchorPoint::CENTER_LEFT,
  AnchorPoint::CENTER,
  AnchorPoint::CENTER_RIGHT,
  AnchorPoint::BOTTOM_LEFT,
  AnchorPoint::BOTTOM_CENTER,
  AnchorPoint::BOTTOM_RIGHT,
};
constexpr int ANCHOR_POINT_LIST_COUNT = sizeof(ANCHOR_POINT_LIST) / sizeof(ANCHOR_POINT_LIST[0]);
} // namespace

// This example shows how to create and display Hello World! using a simple TextActor
//
class InheritTestController : public ConnectionTracker
{
public:
  InheritTestController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &InheritTestController::Create);
  }

  ~InheritTestController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Color::WHITE);

    Vector2 windowSize(1280, 800);
    if(mWindow.GetSize().GetWidth() < 1280)
    {
      mWindow.SetSize(Uint16Pair(windowSize.x, windowSize.y));
    }
    else
    {
      windowSize.x = mWindow.GetSize().GetWidth();
      windowSize.y = mWindow.GetSize().GetHeight();
    }

    mTitle = TextLabel::New();
    mTitle.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mTitle.SetProperty(TextLabel::Property::MULTI_LINE, true);
    mWindow.Add(mTitle);

    SetupView();

    // Respond to a touch anywhere on the window
    mWindow.GetRootLayer().TouchedSignal().Connect(this, &InheritTestController::OnTouch);

    // Respond to key events
    mWindow.KeyEventSignal().Connect(this, &InheritTestController::OnKeyEvent);
  }

  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      bool needUpdate = false;
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
      if(event.GetKeyName() == "1") // Inherit flag
      {
        mInheritFlag = (mInheritFlag + 1) % 8;
        needUpdate   = true;
      }
      else if(event.GetKeyName() == "2") // ParentView Size
      {
        mParentSizeIndex = (mParentSizeIndex + 1) % PARENT_VIEW_SIZE_LIST_COUNT;
        needUpdate       = true;
      }
      else if(event.GetKeyName() == "3") // ParentView Scale
      {
        mParentScaleIndex = (mParentScaleIndex + 1) % PARENT_VIEW_SCALE_LIST_COUNT;
        needUpdate        = true;
      }
      else if(event.GetKeyName() == "4") // ParentOrigin
      {
        mParentOriginIndex = (mParentOriginIndex + 1) % PARENT_ORIGIN_LIST_COUNT;
        needUpdate         = true;
      }
      else if(event.GetKeyName() == "5") // AnchorPoint
      {
        mAnchorPointIndex = (mAnchorPointIndex + 1) % ANCHOR_POINT_LIST_COUNT;
        needUpdate        = true;
      }
      else if(event.GetKeyName() == "6") // Size
      {
        mCurrentSizeIndex = (mCurrentSizeIndex + 1) % CURRENT_VIEW_SIZE_LIST_COUNT;
        needUpdate        = true;
      }
      else if(event.GetKeyName() == "7") // Position
      {
        mCurrentPositionIndex = (mCurrentPositionIndex + 1) % CURRENT_VIEW_POSITION_LIST_COUNT;
        needUpdate            = true;
      }
      else if(event.GetKeyName() == "8") // Scale
      {
        mCurrentScaleIndex = (mCurrentScaleIndex + 1) % CURRENT_VIEW_SCALE_LIST_COUNT;
        needUpdate         = true;
      }

      if(needUpdate)
      {
        SetupView();
      }
    }
  }

  void SetupView()
  {
    if(mParent)
    {
      mParent.Unparent();
    }
    if(mCurrent)
    {
      mCurrent.Unparent();
    }

    Vector2 parentViewSize  = PARENT_VIEW_SIZE_LIST[mParentSizeIndex];
    Vector2 parentViewScale = PARENT_VIEW_SCALE_LIST[mParentScaleIndex];

    mParent = Control::New();
    mParent.SetBackgroundColor(Color::BLUE);
    mParent.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mParent.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mParent.SetProperty(Actor::Property::SIZE, Vector3(parentViewSize));
    mParent.SetProperty(Actor::Property::POSITION, Vector3::ZERO);
    mParent.SetProperty(Actor::Property::SCALE, Vector3(parentViewScale.x, parentViewScale.y, 1.0f));

    Vector3 parentOrigin        = PARENT_ORIGIN_LIST[mParentOriginIndex];
    Vector3 anchorPoint         = ANCHOR_POINT_LIST[mAnchorPointIndex];
    Vector2 currentViewSize     = CURRENT_VIEW_SIZE_LIST[mCurrentSizeIndex];
    Vector2 currentViewPosition = CURRENT_VIEW_POSITION_LIST[mCurrentPositionIndex];
    Vector2 currentViewScale    = CURRENT_VIEW_SCALE_LIST[mCurrentScaleIndex];

    bool inheritPosition    = mInheritFlag & 1;
    bool inheritScale       = mInheritFlag & 2;
    bool inheritOrientation = mInheritFlag & 4;

    mCurrent = Control::New();
    mCurrent.SetBackgroundColor(Color::RED);
    mCurrent.SetProperty(Actor::Property::PARENT_ORIGIN, parentOrigin);
    mCurrent.SetProperty(Actor::Property::ANCHOR_POINT, anchorPoint);
    mCurrent.SetProperty(Actor::Property::SIZE, Vector3(currentViewSize));
    mCurrent.SetProperty(Actor::Property::POSITION, Vector3(currentViewPosition));
    mCurrent.SetProperty(Actor::Property::SCALE, Vector3(currentViewScale.x, currentViewScale.y, 1.0f));

    mCurrent.SetProperty(Actor::Property::INHERIT_POSITION, inheritPosition);
    mCurrent.SetProperty(Actor::Property::INHERIT_SCALE, inheritScale);
    mCurrent.SetProperty(Actor::Property::INHERIT_ORIENTATION, inheritOrientation);

    mParent.Add(mCurrent);
    mWindow.Add(mParent);

    mAnimation = Animation::New(5.0f);
    mAnimation.AnimateBy(Property(mParent, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(360.0f)), Vector3::ZAXIS));
    mAnimation.AnimateBy(Property(mCurrent, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(360.0f)), Vector3::ZAXIS));
    mAnimation.SetLooping(true);
    mAnimation.PlayAfter(1.0f); // Play animation after world value printed

    mTimer = Timer::New(32);
    mTimer.TickSignal().Connect(this, &InheritTestController::OnTickLableUpdate);
    mTimer.Start();
  }

  bool OnTickLableUpdate()
  {
    UpdateLabelInfo();
    return false;
  }

  void UpdateLabelInfo()
  {
    std::ostringstream oss;

    Vector2 parentViewSize  = PARENT_VIEW_SIZE_LIST[mParentSizeIndex];
    Vector2 parentViewScale = PARENT_VIEW_SCALE_LIST[mParentScaleIndex];

    Vector3 parentOrigin        = PARENT_ORIGIN_LIST[mParentOriginIndex];
    Vector3 anchorPoint         = ANCHOR_POINT_LIST[mAnchorPointIndex];
    Vector2 currentViewSize     = CURRENT_VIEW_SIZE_LIST[mCurrentSizeIndex];
    Vector2 currentViewPosition = CURRENT_VIEW_POSITION_LIST[mCurrentPositionIndex];
    Vector2 currentViewScale    = CURRENT_VIEW_SCALE_LIST[mCurrentScaleIndex];

    Vector2    screenPositon    = mCurrent.GetProperty<Vector2>(Actor::Property::SCREEN_POSITION);
    Vector3    worldPosition    = mCurrent.GetProperty<Vector3>(Actor::Property::WORLD_POSITION);
    Vector3    worldScale       = mCurrent.GetProperty<Vector3>(Actor::Property::WORLD_SCALE);
    Quaternion worldOrientation = mCurrent.GetProperty<Quaternion>(Actor::Property::WORLD_ORIENTATION);

    bool inheritPosition    = mInheritFlag & 1;
    bool inheritScale       = mInheritFlag & 2;
    bool inheritOrientation = mInheritFlag & 4;

    oss << "Inherit P S O : " << inheritPosition << " " << inheritScale << " " << inheritOrientation << "\n";
    oss << "screenPosition : " << screenPositon << "\n";
    oss << "\n";
    oss << "parentViewSize : " << parentViewSize << "\n";
    oss << "parentViewScale : " << parentViewScale << "\n";
    oss << "\n";
    oss << "parentOrigin : " << parentOrigin << "\n";
    oss << "anchorPoint : " << anchorPoint << "\n";
    oss << "currentViewSize : " << currentViewSize << "\n";
    oss << "currentViewPosition : " << currentViewPosition << "\n";
    oss << "currentViewScale : " << currentViewScale << "\n";
    oss << "\n";
    oss << "worldPosition : " << worldPosition << "\n";
    oss << "worldScale : " << worldScale << "\n";
    oss << "worldOrientation : " << worldOrientation << "\n";
    oss << "\n";
    oss << "Key 1 : Change flag\n";
    oss << "Key 2 : Change ParentView's Size\n";
    oss << "Key 3 : Change ParentView's Scale\n";
    oss << "Key 4 : Change CurrentView's ParentOrigin\n";
    oss << "Key 5 : Change CurrentView's AnchorPoint\n";
    oss << "Key 6 : Change CurrentView's Size\n";
    oss << "Key 7 : Change CurrentView's Position\n";
    oss << "Key 8 : Change CurrentView's Scale\n";

    mTitle.SetProperty(TextLabel::Property::TEXT, oss.str());
  }

private:
  Application& mApplication;
  Window       mWindow;
  TextLabel    mTitle;

  Control mParent;
  Control mCurrent;

  Timer     mTimer;
  Animation mAnimation;

  int mInheritFlag = 0;

  int mParentSizeIndex      = 0;
  int mParentScaleIndex     = 0;
  int mCurrentSizeIndex     = 0;
  int mCurrentPositionIndex = 0;
  int mCurrentScaleIndex    = 0;
  int mParentOriginIndex    = 0;
  int mAnchorPointIndex     = 0;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application           application = Application::New(&argc, &argv);
  InheritTestController test(application);
  application.MainLoop();
  return 0;
}
