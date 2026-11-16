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
#include <dali/devel-api/adaptor-framework/application.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
using Dali::Integration::GetStdString;
using Dali::Integration::ToDaliString;
using Dali::Integration::ToDaliStringView;
using Dali::Integration::ToPropertyValue;
using Dali::Integration::ToStdString;

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

  const Vector3 PivotList[OriginTypes] = {
    Pivot::TOP_LEFT,
    Pivot::TOP_CENTER,
    Pivot::TOP_RIGHT,
    Pivot::CENTER_LEFT,
    Pivot::CENTER,
    Pivot::CENTER_RIGHT,
    Pivot::BOTTOM_LEFT,
    Pivot::BOTTOM_CENTER,
    Pivot::BOTTOM_RIGHT};

public:
  HelloWorldController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &HelloWorldController::Create);
  }

  ~HelloWorldController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application application)
  {
    // Get a handle to the window
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Color::WHITE);

    control = Toolkit::Control::New();
    control.SetProperty(Actor::Property::SIZE, Vector2(400, 300));
    control.SetProperty(Actor::Property::SCALE, Vector3(0.5f, 0.5f, 1.0f));
    control.SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(Degree(45.0f)), Vector3::ZAXIS));
    control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    control.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    control.SetBackgroundColor(Color::BLUE);
    mWindow.Add(control);

    SetMarker();

    TextLabel keyGuide = TextLabel::New();
    keyGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "Arrow Key : Move\nr : Rotate ClockWise\nR : Rotate CounterClockWise\nc : Change ParentOrigin\nC : Change Pivot\ns : Scale Down\nS : Scale Up");
    keyGuide.SetProperty(Dali::Toolkit::TextLabel::Property::MULTI_LINE, true);
    keyGuide.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    keyGuide.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    mWindow.Add(keyGuide);

    TextLabel screenBoundsColorGuide = TextLabel::New();
    screenBoundsColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "Actor's Screen Extents");
    screenBoundsColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Vector4(1.0f, 1.0f, 0.0f, 0.3f));
    screenBoundsColorGuide.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    screenBoundsColorGuide.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    keyGuide.Add(screenBoundsColorGuide);

    TextLabel screenPositionColorGuide = TextLabel::New();
    screenPositionColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "Actor's Screen Position");
    screenPositionColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Vector4(1.0f, 0.0f, 0.0f, 0.8f));
    screenPositionColorGuide.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    screenPositionColorGuide.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    screenBoundsColorGuide.Add(screenPositionColorGuide);

    TextLabel currentScreenBoundsColorGuide = TextLabel::New();
    currentScreenBoundsColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "Actor's Current Screen Extents");
    currentScreenBoundsColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Vector4(0.0f, 1.0f, 1.0f, 0.3f));
    currentScreenBoundsColorGuide.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    currentScreenBoundsColorGuide.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    screenPositionColorGuide.Add(currentScreenBoundsColorGuide);

    TextLabel currentScreenPositionColorGuide = TextLabel::New();
    currentScreenPositionColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "Actor's Current Screen Position");
    currentScreenPositionColorGuide.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Vector4(0.0f, 1.0f, 0.0f, 0.8f));
    currentScreenPositionColorGuide.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    currentScreenPositionColorGuide.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    currentScreenBoundsColorGuide.Add(currentScreenPositionColorGuide);

    // Respond to a touch anywhere on the window
    mWindow.TouchEventSignal().Connect(this, &HelloWorldController::OnTouch);

    // Respond to key events
    mWindow.KeyEventSignal().Connect(this, &HelloWorldController::OnKeyEvent);
  }

  void OnTouch(Window window, TouchEvent touch)
  {
    // quit the application
    mApplication.Quit();
    return;
  }

  void SetMarker()
  {
    auto screenPosition = control.CalculateScreenPosition();
    auto bounds         = control.CalculateScreenBounds();

    if(!boundsMarker)
    {
      boundsMarker = Toolkit::Control::New();
      boundsMarker.SetProperty(Dali::Actor::Property::PIVOT, Pivot::TOP_LEFT);
      boundsMarker.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      boundsMarker.SetBackgroundColor(Vector4(1.0f, 1.0f, 0.0f, 0.3f));
      mWindow.Add(boundsMarker);
    }

    if(!screenPositionMarker)
    {
      screenPositionMarker = Toolkit::Control::New();
      screenPositionMarker.SetProperty(Dali::Actor::Property::SIZE, Vector2(10, 10));
      screenPositionMarker.SetProperty(Dali::Actor::Property::PIVOT, Pivot::CENTER);
      screenPositionMarker.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      screenPositionMarker.SetBackgroundColor(Vector4(1.0f, 0.0f, 0.0f, 0.8f));
      mWindow.Add(screenPositionMarker);
    }

    if(!currentBoundsMarker)
    {
      currentBoundsMarker = Toolkit::Control::New();
      currentBoundsMarker.SetProperty(Dali::Actor::Property::PIVOT, Pivot::TOP_LEFT);
      currentBoundsMarker.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      currentBoundsMarker.SetBackgroundColor(Vector4(0.0f, 1.0f, 1.0f, 0.3f));
      mWindow.Add(currentBoundsMarker);
    }

    if(!currentScreenPositionMarker)
    {
      currentScreenPositionMarker = Toolkit::Control::New();
      currentScreenPositionMarker.SetProperty(Dali::Actor::Property::SIZE, Vector2(10, 10));
      currentScreenPositionMarker.SetProperty(Dali::Actor::Property::PIVOT, Pivot::CENTER);
      currentScreenPositionMarker.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      currentScreenPositionMarker.SetBackgroundColor(Vector4(0.0f, 1.0f, 0.0f, 0.8f));
      mWindow.Add(currentScreenPositionMarker);
    }

    boundsMarker.SetProperty(Dali::Actor::Property::SIZE, Vector2(bounds.width, bounds.height));
    boundsMarker.SetProperty(Dali::Actor::Property::POSITION, Vector2(bounds.x, bounds.y));
    screenPositionMarker.SetProperty(Dali::Actor::Property::POSITION, screenPosition);
  }

  void UpdateMarker()
  {
    auto screenPosition = control.CalculateScreenPosition();
    auto bounds         = control.CalculateScreenBounds();

    boundsMarker.SetProperty(Dali::Actor::Property::SIZE, Vector2(bounds.width, bounds.height));
    boundsMarker.SetProperty(Dali::Actor::Property::POSITION, Vector2(bounds.x, bounds.y));
    screenPositionMarker.SetProperty(Dali::Actor::Property::POSITION, screenPosition);

    auto currentScreenPosition = control.GetProperty<Vector2>(Dali::Actor::Property::SCREEN_POSITION);
    auto currentBounds         = control.CalculateCurrentScreenBounds();

    currentBoundsMarker.SetProperty(Dali::Actor::Property::SIZE, Vector2(currentBounds.width, currentBounds.height));
    currentBoundsMarker.SetProperty(Dali::Actor::Property::POSITION, Vector2(currentBounds.x, currentBounds.y));
    currentScreenPositionMarker.SetProperty(Dali::Actor::Property::POSITION, currentScreenPosition);
  }

  void OnKeyEvent(Window window, KeyEvent event)
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
        pivotIdx++;
        pivotIdx = pivotIdx % OriginTypes;
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
      control.SetProperty(Dali::Actor::Property::PIVOT, PivotList[pivotIdx]);
      control.SetProperty(Dali::Actor::Property::POSITION, controlPosition);
      control.SetProperty(Dali::Actor::Property::ORIENTATION, controlOrientation);
      control.SetProperty(Dali::Actor::Property::SCALE, controlScale);
      UpdateMarker();
    }
  }

private:
  uint32_t         parentOriginIdx{4};
  uint32_t         pivotIdx{0};
  Toolkit::Control boundsMarker;
  Toolkit::Control screenPositionMarker;
  Toolkit::Control currentBoundsMarker;
  Toolkit::Control currentScreenPositionMarker;
  Window           mWindow;
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
