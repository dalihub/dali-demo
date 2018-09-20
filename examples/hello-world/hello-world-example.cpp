/*
 * Copyright (control) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/actor-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;


class MyTester : public ConnectionTracker
{
public:
  MyTester(Application &application)
      : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &MyTester::Create);
  }

  ~MyTester()
  {
    // Nothing to do here;
  }

private:
  // The Init signal is received once (only) during the Application lifetime
  void Create(Application &application)
  {
    // Get a handle to the stage
    mStage = Stage::GetCurrent();
    mStage.SetBackgroundColor(Color::BLUE);

    mStage.KeyEventSignal().Connect(this, &MyTester::OnKeyEvent);

    mApplication.GetWindow().SetSize(Uint16Pair(1920 / 2, 1080));
    mApplication.GetWindow().SetPosition(Uint16Pair(0, 0));

    control = Dali::Toolkit::Control::New();
    control.SetPosition(50, 50);
    control.SetSize(100, 100);
    control.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    control.SetParentOrigin(ParentOrigin::TOP_LEFT);
    control.SetBackgroundColor(Color::YELLOW);

    mStage.Add(control);
  }

  void CreateScene()
  {

  }

  void OnKeyEvent(const KeyEvent &event)
  {
    if (event.state == KeyEvent::Down)
    {
      if (IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
      else if (event.keyPressedName == "1")
      {
        mApplication.GetWindow().SetSize(Uint16Pair(1920 / 2, 1080 / 2));
        control.SetBackgroundColor(Color::GREEN);
      }
      else if (event.keyPressedName == "2")
      {
        mApplication.GetWindow().SetSize(Uint16Pair(1920 / 2, 1080));
        control.SetBackgroundColor(Color::YELLOW);
      }else if (event.keyPressedName == "3")
      {
        mApplication.GetWindow().SetPosition(Uint16Pair(0, 0));
        control.SetBackgroundColor(Color::GREEN);
      }else if (event.keyPressedName == "4")
      {
        mApplication.GetWindow().SetPosition(Uint16Pair(1920 / 2, 0));
        control.SetBackgroundColor(Color::YELLOW);
      }
    }
  }

private:
  uint32_t focussedIndex = 0, focussedRow = 0;
  Stage mStage;
  Toolkit::Control control;
  Application &mApplication;
};

int DALI_EXPORT_API main(int argc, char **argv)
{
  Application application = Application::New(&argc, &argv);
  MyTester test(application);
  application.MainLoop();
  return 0;
}
