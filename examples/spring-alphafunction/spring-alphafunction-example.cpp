/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/debug.h>

using namespace Dali;

// This example shows how to create and display Hello World! using a simple TextActor
//
class SpringAlphaFunctionController : public ConnectionTracker
{
public:
  SpringAlphaFunctionController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &SpringAlphaFunctionController::Create);
  }

  ~SpringAlphaFunctionController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    Dali::Toolkit::Control control = Dali::Toolkit::Control::New();
    control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    control.SetProperty(Dali::Actor::Property::SIZE, Vector2(250, 1000));
    control.SetProperty(Dali::Actor::Property::POSITION_X, 250);
    control.SetBackgroundColor(Color::PINK);
    window.Add(control);

    mGentle = Dali::Toolkit::TextLabel::New("Gentle");
    mGentle.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mGentle.SetProperty(Dali::Actor::Property::NAME, "Gentle");
    mGentle.SetProperty(Dali::Actor::Property::POSITION_Y, 0.0f);
    window.Add(mGentle);

    mQuick = Dali::Toolkit::TextLabel::New("Quick");
    mQuick.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mQuick.SetProperty(Dali::Actor::Property::NAME, "Quick");
    mQuick.SetProperty(Dali::Actor::Property::POSITION_Y, 100.0f);
    window.Add(mQuick);

    mBouncy = Dali::Toolkit::TextLabel::New("Bouncy");
    mBouncy.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mBouncy.SetProperty(Dali::Actor::Property::NAME, "Bouncy");
    mBouncy.SetProperty(Dali::Actor::Property::POSITION_Y, 200.0f);
    window.Add(mBouncy);

    mSlow = Dali::Toolkit::TextLabel::New("Slow");
    mSlow.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mSlow.SetProperty(Dali::Actor::Property::NAME, "Slow");
    mSlow.SetProperty(Dali::Actor::Property::POSITION_Y, 300.0f);
    window.Add(mSlow);

    mS100D10M1 = Dali::Toolkit::TextLabel::New("S100_D10_M1");
    mS100D10M1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mS100D10M1.SetProperty(Dali::Actor::Property::NAME, "S100_D10_M1");
    mS100D10M1.SetProperty(Dali::Actor::Property::POSITION_Y, 400.0f);
    window.Add(mS100D10M1);

    mS4420D20_8M1 = Dali::Toolkit::TextLabel::New("S4420_D20.8_M1");
    mS4420D20_8M1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mS4420D20_8M1.SetProperty(Dali::Actor::Property::NAME, "S4420_D20.8_M1");
    mS4420D20_8M1.SetProperty(Dali::Actor::Property::POSITION_Y, 500.0f);
    window.Add(mS4420D20_8M1);

    mS1000D10M10 = Dali::Toolkit::TextLabel::New("S1000_D10_M10");
    mS1000D10M10.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mS1000D10M10.SetProperty(Dali::Actor::Property::NAME, "S1000_D10_M10");
    mS1000D10M10.SetProperty(Dali::Actor::Property::POSITION_Y, 600.0f);
    window.Add(mS1000D10M10);

    Animation animation = Animation::New(1.0f);  // Set the longest duration.
    animation.AnimateTo(Dali::Property(mGentle, Dali::Actor::Property::POSITION_X), 500.0f, Dali::AlphaFunction(Dali::AlphaFunction::SpringType::GENTLE));
    animation.SetLooping(true);
    animation.Play();

    animation = Animation::New(1.0f);  // Set the longest duration.
    animation.AnimateTo(Dali::Property(mQuick, Dali::Actor::Property::POSITION_X), 500.0f, Dali::AlphaFunction(Dali::AlphaFunction::SpringType::QUICK));
    animation.SetLooping(true);
    animation.Play();

    animation = Animation::New(1.0f);  // Set the longest duration.
    animation.AnimateTo(Dali::Property(mBouncy, Dali::Actor::Property::POSITION_X), 500.0f, Dali::AlphaFunction(Dali::AlphaFunction::SpringType::BOUNCY));
    animation.SetLooping(true);
    animation.Play();

    animation = Animation::New(1.0f);  // Set the longest duration.
    animation.AnimateTo(Dali::Property(mSlow, Dali::Actor::Property::POSITION_X), 500.0f, Dali::AlphaFunction(Dali::AlphaFunction::SpringType::SLOW));
    animation.SetLooping(true);
    animation.Play();

    animation = Animation::New(SpringData{100.0f, 10.0f, 1.0f}.GetDuration());  // Set the longest duration.
    animation.AnimateTo(Dali::Property(mS100D10M1, Dali::Actor::Property::POSITION_X), 500.0f, Dali::AlphaFunction({100.0f, 10.0f, 1.0f}));
    animation.SetLooping(true);
    animation.Play();

    animation = Animation::New(SpringData({4420.0f, 20.8f, 1.0f}).GetDuration());  // Set the longest duration.
    animation.AnimateTo(Dali::Property(mS4420D20_8M1, Dali::Actor::Property::POSITION_X), 500.0f, Dali::AlphaFunction({4420.0f, 20.8f, 1.0f}));
    animation.SetLooping(true);
    animation.Play();

    animation = Animation::New(SpringData({1000.0f, 10.0f, 10.0f}).GetDuration());  // Set the longest duration.
    animation.AnimateTo(Dali::Property(mS1000D10M10, Dali::Actor::Property::POSITION_X), 500.0f, Dali::AlphaFunction({1000.0f, 10.0f, 10.0f}));
    animation.SetLooping(true);
    animation.Play();

    // Respond to a touch anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &SpringAlphaFunctionController::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &SpringAlphaFunctionController::OnKeyEvent);
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
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Dali::Toolkit::TextLabel mGentle, mQuick, mBouncy, mSlow, mS100D10M1, mS4420D20_8M1, mS1000D10M10;
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application          application = Application::New(&argc, &argv);
  SpringAlphaFunctionController test(application);
  application.MainLoop();
  return 0;
}
