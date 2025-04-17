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

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/dali.h>

#include <dali/devel-api/common/stage.h>

#include <dali-toolkit/devel-api/controls/control-devel.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
uint32_t gRows(50);
uint32_t gColumns(20);
uint32_t gRunningDurationMilliSeconds(60 * 1000); // 1 minutes

enum ControlPropertyUsesFlag
{
  NONE = 0,

  CORNER_RADIUS     = 1 << 0,
  CORNER_SQUARENESS = 1 << 1,

  OFFSCREEN_RENDERING = 1 << 2,
};

int gControlPropertyUses = ControlPropertyUsesFlag::NONE;
} // namespace

// Test application to check performance for rendering simple colors
// -r NumberOfRows    (Modifies the number of rows per page)
// -c NumberOfColumns (Modifies the number of columns per page)
// -d RunningDurationMilliSeconds (Modifies the duration of the test in milliseconds. 0 means infinite)
//
// -p Add options for control properties. C : Corner Radius, S : Corner Squareness, O : OffscreenRendering with refresh always.
//
// For example:
// $ ./benchmark-color.example -r200 -c800 -d60000 -pCSO

//
class BenchmarkColor : public ConnectionTracker
{
public:
  BenchmarkColor(Application& application)
  : mApplication(application),
    mRows(gRows),
    mColumns(gColumns),
    mRunningDurationMilliSeconds(gRunningDurationMilliSeconds)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &BenchmarkColor::Create);
  }

  ~BenchmarkColor() = default;

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::BLACK);
    Vector2 windowSize = window.GetSize();

    mSize = Vector3(windowSize.x / static_cast<float>(mColumns), windowSize.y / static_cast<float>(mRows), 0.0f);

    // Respond to a click anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &BenchmarkColor::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &BenchmarkColor::OnKeyEvent);

    mRootActor = Actor::New();
    mRootActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mRootActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mRootActor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    mRootActor.SetProperty(Actor::Property::OPACITY, 0.8f); ///< Do not make it as 1.0f, to avoid full-opaque optimization.

    for(auto i = 0u; i < mRows; ++i)
    {
      for(auto j = 0u; j < mColumns; ++j)
      {
        Control control = Control::New();
        control.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
        control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
        control.SetProperty(Actor::Property::POSITION, Vector3(j * mSize.x, i * mSize.y, 0.0f));
        control.SetProperty(Actor::Property::SIZE, mSize);

        if(gControlPropertyUses & ControlPropertyUsesFlag::CORNER_RADIUS)
        {
          control.SetProperty(DevelControl::Property::CORNER_RADIUS, Vector4(0.5f, 0.5f, 0.5f, 0.5f));
          control.SetProperty(DevelControl::Property::CORNER_RADIUS_POLICY, Toolkit::Visual::Transform::Policy::Type::RELATIVE);
        }
        if(gControlPropertyUses & ControlPropertyUsesFlag::CORNER_SQUARENESS)
        {
          control.SetProperty(DevelControl::Property::CORNER_SQUARENESS, Vector4(0.6f, 0.6f, 0.6f, 0.6f));
        }
        if(gControlPropertyUses & ControlPropertyUsesFlag::OFFSCREEN_RENDERING)
        {
          control.SetProperty(DevelControl::Property::OFFSCREEN_RENDERING, DevelControl::OffScreenRenderingType::REFRESH_ALWAYS);
        }
        control.SetProperty(Control::Property::BACKGROUND, Vector4(Random::Range(0.0f, 1.0f), Random::Range(0.0f, 1.0f), Random::Range(0.0f, 1.0f), 1.0f));

        mRootActor.Add(control);
      }
    }

    window.GetRootLayer().Add(mRootActor);

    mAnimation = Animation::New(10.0f);
    mAnimation.SetLooping(true);
    mAnimation.AnimateTo(Property(mRootActor, Actor::Property::OPACITY), 0.2f); ///< Do not make it as 0.0f, to avoid full-transparent optimization.
    mAnimation.Play();

    if(mRunningDurationMilliSeconds > 0u)
    {
      Stage::GetCurrent().KeepRendering(mRunningDurationMilliSeconds / 1000.0f + 30.0f);
      mTimer = Timer::New(mRunningDurationMilliSeconds);
      mTimer.TickSignal().Connect(this, &BenchmarkColor::OnTimer);
      mTimer.Start();
    }
  }

  bool OnTimer()
  {
    mApplication.Quit();
    return false; // Stop the timer
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
  Application& mApplication;

  uint32_t mRows;
  uint32_t mColumns;
  uint32_t mRunningDurationMilliSeconds;

  Vector3 mSize;

  Actor     mRootActor;
  Animation mAnimation;
  Timer     mTimer;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  for(int i(1); i < argc; ++i)
  {
    std::string arg(argv[i]);
    if(arg.compare(0, 2, "-r") == 0)
    {
      gRows = atoi(arg.substr(2, arg.size()).c_str());
    }
    else if(arg.compare(0, 2, "-c") == 0)
    {
      gColumns = atoi(arg.substr(2, arg.size()).c_str());
    }
    else if(arg.compare(0, 2, "-d") == 0)
    {
      gRunningDurationMilliSeconds = atoi(arg.substr(2, arg.size()).c_str());
    }
    else if(arg.compare(0, 2, "-p") == 0)
    {
      std::string subarg = arg.substr(2, arg.size());
      for(auto c : subarg)
      {
        if(c == 'C' || c == 'c')
        {
          gControlPropertyUses |= ControlPropertyUsesFlag::CORNER_RADIUS;
        }
        if(c == 'S' || c == 's')
        {
          gControlPropertyUses |= ControlPropertyUsesFlag::CORNER_SQUARENESS;
        }
        if(c == 'O' || c == 'o')
        {
          gControlPropertyUses |= ControlPropertyUsesFlag::OFFSCREEN_RENDERING;
        }
      }
    }
  }

  BenchmarkColor test(application);
  application.MainLoop();

  return 0;
}
