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

#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
  const static uint32_t OriginTypes = 9;

  const Vector3 ParentOriginList[OriginTypes] = {
    ParentOrigin::TOP_LEFT,
    ParentOrigin::TOP_CENTER,
    ParentOrigin::TOP_RIGHT,
    ParentOrigin::CENTER_LEFT,
    ParentOrigin::CENTER,
    ParentOrigin::CENTER_RIGHT,
    ParentOrigin::BOTTOM_LEFT,
    ParentOrigin::BOTTOM_CENTER,
    ParentOrigin::BOTTOM_RIGHT};

  const Vector3 AnchorPointList[OriginTypes] = {
    AnchorPoint::TOP_LEFT,
    AnchorPoint::TOP_CENTER,
    AnchorPoint::TOP_RIGHT,
    AnchorPoint::CENTER_LEFT,
    AnchorPoint::CENTER,
    AnchorPoint::CENTER_RIGHT,
    AnchorPoint::BOTTOM_LEFT,
    AnchorPoint::BOTTOM_CENTER,
    AnchorPoint::BOTTOM_RIGHT};

public:
  HelloWorldController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &HelloWorldController::Create);
  }

  ~HelloWorldController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    control = Toolkit::Control::New();
    control.SetProperty(Actor::Property::SIZE, Vector2(400, 300));
    control.SetProperty(Actor::Property::SCALE, Vector3(0.5f, 0.5f, 1.0f));
    control.SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(Degree(45.0f)), Vector3::ZAXIS));
    control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    control.SetBackgroundColor(Color::BLUE);
    window.Add(control);

    SetMarker();

    TextLabel keyGuide = TextLabel::New();
    keyGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "Arrow Key : Move\nr : Rotate ClockWise\nR : Rotate CounterClockWise\nc : Change ParentOrigin\nC : Change AnchorPoint\ns : Scale Down\nS : Scale Up");
    keyGuide.SetProperty(Dali::Toolkit::TextLabel::Property::MULTI_LINE, true);
    keyGuide.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    keyGuide.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    window.Add(keyGuide);

    TextLabel screenExtentsColorGuide = TextLabel::New();
    screenExtentsColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "Actor's Screen Extents");
    screenExtentsColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Vector4(1.0f, 1.0f, 0.0f, 0.3f));
    screenExtentsColorGuide.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    screenExtentsColorGuide.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    keyGuide.Add(screenExtentsColorGuide);

    TextLabel screenPositionColorGuide = TextLabel::New();
    screenPositionColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "Actor's Screen Position");
    screenPositionColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Vector4(1.0f, 0.0f, 0.0f, 0.8f));
    screenPositionColorGuide.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    screenPositionColorGuide.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    screenExtentsColorGuide.Add(screenPositionColorGuide);

    TextLabel currentScreenExtentsColorGuide = TextLabel::New();
    currentScreenExtentsColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "Actor's Current Screen Extents");
    currentScreenExtentsColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Vector4(0.0f, 1.0f, 1.0f, 0.3f));
    currentScreenExtentsColorGuide.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    currentScreenExtentsColorGuide.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    screenPositionColorGuide.Add(currentScreenExtentsColorGuide);

    TextLabel currentScreenPositionColorGuide = TextLabel::New();
    currentScreenPositionColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "Actor's Current Screen Position");
    currentScreenPositionColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Vector4(0.0f, 1.0f, 0.0f, 0.8f));
    currentScreenPositionColorGuide.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    currentScreenPositionColorGuide.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    currentScreenExtentsColorGuide.Add(currentScreenPositionColorGuide);

    // Respond to a touch anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &HelloWorldController::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &HelloWorldController::OnKeyEvent);
  }

  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  void SetMarker()
  {
    auto screenPosition = Dali::DevelActor::CalculateScreenPosition(control);
    auto extents        = Dali::DevelActor::CalculateScreenExtents(control);

    if(!extentsMarker)
    {
      extentsMarker = Toolkit::Control::New();
      extentsMarker.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
      extentsMarker.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      extentsMarker.SetBackgroundColor(Vector4(1.0f, 1.0f, 0.0f, 0.3f));
      window.Add(extentsMarker);
    }

    if(!screenPositionMarker)
    {
      screenPositionMarker = Toolkit::Control::New();
      screenPositionMarker.SetProperty(Dali::Actor::Property::SIZE, Vector2(10, 10));
      screenPositionMarker.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      screenPositionMarker.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      screenPositionMarker.SetBackgroundColor(Vector4(1.0f, 0.0f, 0.0f, 0.8f));
      window.Add(screenPositionMarker);
    }

    if(!currentExtentsMarker)
    {
      currentExtentsMarker = Toolkit::Control::New();
      currentExtentsMarker.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
      currentExtentsMarker.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      currentExtentsMarker.SetBackgroundColor(Vector4(0.0f, 1.0f, 1.0f, 0.3f));
      window.Add(currentExtentsMarker);
    }

    if(!currentScreenPositionMarker)
    {
      currentScreenPositionMarker = Toolkit::Control::New();
      currentScreenPositionMarker.SetProperty(Dali::Actor::Property::SIZE, Vector2(10, 10));
      currentScreenPositionMarker.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      currentScreenPositionMarker.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      currentScreenPositionMarker.SetBackgroundColor(Vector4(0.0f, 1.0f, 0.0f, 0.8f));
      window.Add(currentScreenPositionMarker);
    }

    extentsMarker.SetProperty(Dali::Actor::Property::SIZE, Vector2(extents.width, extents.height));
    extentsMarker.SetProperty(Dali::Actor::Property::POSITION, Vector2(extents.x, extents.y));
    screenPositionMarker.SetProperty(Dali::Actor::Property::POSITION, screenPosition);
  }

  void UpdateMarker()
  {
    auto screenPosition = Dali::DevelActor::CalculateScreenPosition(control);
    auto extents        = Dali::DevelActor::CalculateScreenExtents(control);

    extentsMarker.SetProperty(Dali::Actor::Property::SIZE, Vector2(extents.width, extents.height));
    extentsMarker.SetProperty(Dali::Actor::Property::POSITION, Vector2(extents.x, extents.y));
    screenPositionMarker.SetProperty(Dali::Actor::Property::POSITION, screenPosition);

    auto currentScreenPosition = control.GetProperty<Vector2>(Dali::Actor::Property::SCREEN_POSITION);
    auto currentExtents        = Dali::DevelActor::CalculateCurrentScreenExtents(control);

    currentExtentsMarker.SetProperty(Dali::Actor::Property::SIZE, Vector2(currentExtents.width, currentExtents.height));
    currentExtentsMarker.SetProperty(Dali::Actor::Property::POSITION, Vector2(currentExtents.x, currentExtents.y));
    currentScreenPositionMarker.SetProperty(Dali::Actor::Property::POSITION, currentScreenPosition);
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    const float positionStep    = 2.0f;
    const float orientationStep = 2.0f;
    if(event.GetState() == KeyEvent::DOWN)
    {
      Vector2    controlPosition    = control.GetProperty<Vector2>(Dali::Actor::Property::POSITION);
      Quaternion controlOrientation = control.GetProperty<Quaternion>(Dali::Actor::Property::ORIENTATION);
      Vector3    controlScale       = control.GetProperty<Vector3>(Dali::Actor::Property::SCALE);
      if(IsKey(event, Dali::DALI_KEY_CURSOR_UP))
      {
        controlPosition.y -= positionStep;
      }
      if(IsKey(event, Dali::DALI_KEY_CURSOR_DOWN))
      {
        controlPosition.y += positionStep;
      }
      if(IsKey(event, Dali::DALI_KEY_CURSOR_RIGHT))
      {
        controlPosition.x += positionStep;
      }
      if(IsKey(event, Dali::DALI_KEY_CURSOR_LEFT))
      {
        controlPosition.x -= positionStep;
      }
      if(event.GetKeyString() == "r")
      {
        controlOrientation = Quaternion(Radian(Degree(-orientationStep)), Vector3::ZAXIS) * controlOrientation;
      }
      if(event.GetKeyString() == "R")
      {
        controlOrientation = Quaternion(Radian(Degree(orientationStep)), Vector3::ZAXIS) * controlOrientation;
      }
      if(event.GetKeyString() == "c")
      {
        parentOriginIdx++;
        parentOriginIdx = parentOriginIdx % OriginTypes;
      }
      if(event.GetKeyString() == "C")
      {
        anchorPointIdx++;
        anchorPointIdx = anchorPointIdx % OriginTypes;
      }
      if(event.GetKeyString() == "s")
      {
        controlScale -= Vector3(0.02f, 0.02f, 0.0f);
        if(controlScale.x < 0.0f || controlScale.y < 0.0f)
        {
          controlScale = Vector3(0.0f, 0.0f, 1.0f);
        }
      }
      if(event.GetKeyString() == "S")
      {
        controlScale += Vector3(0.02f, 0.02f, 0.0f);
        if(controlScale.x > 3.0f || controlScale.y > 3.0f)
        {
          controlScale = Vector3(3.0f, 3.0f, 1.0f);
        }
      }
      control.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOriginList[parentOriginIdx]);
      control.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPointList[anchorPointIdx]);
      control.SetProperty(Dali::Actor::Property::POSITION, controlPosition);
      control.SetProperty(Dali::Actor::Property::ORIENTATION, controlOrientation);
      control.SetProperty(Dali::Actor::Property::SCALE, controlScale);
      UpdateMarker();
    }
  }

private:
  uint32_t         parentOriginIdx{4};
  uint32_t         anchorPointIdx{0};
  Toolkit::Control extentsMarker;
  Toolkit::Control screenPositionMarker;
  Toolkit::Control currentExtentsMarker;
  Toolkit::Control currentScreenPositionMarker;
  Window           window;
  Toolkit::Control control;
  Application&     mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application          application = Application::New(&argc, &argv);
  HelloWorldController test(application);
  application.MainLoop();
  return 0;
}
