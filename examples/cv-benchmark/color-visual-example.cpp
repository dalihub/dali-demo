/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/devel-api/styling/style-manager-devel.h>
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali/integration-api/debug.h>

#include <dali/devel-api/adaptor-framework/performance-logger.h>

#include <chrono>
#include <list>
#include <thread>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
} // namespace

// This example shows the blur radius property of the color visual and animates it.
//
class ColorVisualExample : public ConnectionTracker
{
public:
  ColorVisualExample(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ColorVisualExample::Create);
  }

  ~ColorVisualExample()
  {
    // Nothing to do here;
  }

  Window             window;
  Timer              timer;
  std::list<Control> list;
  const int          n        = 20;
  const int          m        = 20;
  const int          duration = 100; // miliseconds.

  PerformanceLogger customLoopLogger;
  PerformanceLogger customNew1Logger;
  PerformanceLogger customColorLogger;

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    StyleManager instance = StyleManager::Get();
    window                = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    timer = Timer::New(duration);
    timer.TickSignal().Connect(this, &ColorVisualExample::OnTick);
    timer.Start();

    customLoopLogger  = PerformanceLogger::New("20Controls");
    customNew1Logger  = PerformanceLogger::New("1_New");
    customColorLogger = PerformanceLogger::New("2_SetBG");

    customLoopLogger.EnableLogging(true);
    customNew1Logger.EnableLogging(true);
    customColorLogger.EnableLogging(true);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &ColorVisualExample::OnKeyEvent);
  }

  bool OnTick()
  {
    float width  = (float)window.GetSize().GetWidth() / m;
    float height = (float)window.GetSize().GetHeight() / n;

    int i = n - 1;
    {
      customLoopLogger.AddMarker(PerformanceLogger::Marker::START_EVENT);
      customNew1Logger.AddMarker(PerformanceLogger::Marker::START_EVENT);
      Control rawView = Control::New(Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);
      customNew1Logger.AddMarker(PerformanceLogger::Marker::END_EVENT);

      customColorLogger.AddMarker(PerformanceLogger::Marker::START_EVENT);
      rawView.SetBackgroundColor(Color::BLUE);
      customColorLogger.AddMarker(PerformanceLogger::Marker::END_EVENT);
      rawView[Actor::Property::PARENT_ORIGIN] = ParentOrigin::TOP_LEFT;
      rawView[Actor::Property::ANCHOR_POINT]  = AnchorPoint::TOP_LEFT;
      rawView[Actor::Property::SIZE]          = Vector2((float)window.GetSize().GetWidth(), height);
      rawView[Actor::Property::POSITION]      = Vector2(0.0f, height * i);

      for(int j = 0; j < m; j++)
      {
        Control bgView;
        customNew1Logger.AddMarker(PerformanceLogger::Marker::START_EVENT);
        {
          bgView = Control::New(Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);
        }
        customNew1Logger.AddMarker(PerformanceLogger::Marker::END_EVENT);

        customColorLogger.AddMarker(PerformanceLogger::Marker::START_EVENT);
        bgView.SetBackgroundColor(Color::CRIMSON);
        customColorLogger.AddMarker(PerformanceLogger::Marker::END_EVENT);
        bgView[Actor::Property::PARENT_ORIGIN] = ParentOrigin::TOP_LEFT;
        bgView[Actor::Property::ANCHOR_POINT]  = AnchorPoint::TOP_LEFT;
        bgView[Actor::Property::SIZE]          = Vector2(width * 0.9f, height * 0.9f);
        bgView[Actor::Property::POSITION]      = Vector2(width * j + width * 0.05f, height * 0.05f);
        rawView.Add(bgView);
      }
      customLoopLogger.AddMarker(PerformanceLogger::Marker::END_EVENT);

      window.GetRootLayer().Add(rawView);
      list.push_back(rawView);

      Animation animation = Animation::New(duration * 0.001f * n);
      animation.AnimateTo(Property(rawView, Actor::Property::POSITION_Y), -height);
      animation.Play();

      while((int)list.size() > n)
      {
        list.front().Unparent();
        list.pop_front();
      }
    }

    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        timer.Stop();
        list.clear();
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application        application = Application::New(&argc, &argv);
  ColorVisualExample test(application);
  application.MainLoop();
  return 0;
}
