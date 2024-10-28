/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/dali.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <shared/utility.h>
#include <stdlib.h>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

// This example shows the load-time image scaling and filtering features.
//
class MyTester : public ConnectionTracker
{
public:
  MyTester(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &MyTester::Create);
  }

  ~MyTester()
  {
    // Nothing to do here;
  }

  void ConnectEventSignal(Control control)
  {
    control.TouchedSignal().Connect(this, &MyTester::OnControlTouch);

    control.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
    control.KeyEventSignal().Connect(this, &MyTester::OnControlKeyEvent);
    control.KeyInputFocusGainedSignal().Connect(this, &MyTester::OnFocusSet);
    control.KeyInputFocusLostSignal().Connect(this, &MyTester::OnFocusUnSet);
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Color::BLACK);
    mWindow.KeyEventSignal().Connect(this, &MyTester::OnKey);
    mWindow.TouchedSignal().Connect(this, &MyTester::OnTouch);

    TextLabel rubric = TextLabel::New("You will need a working internet connection to see the images below");
    rubric.SetProperty(TextLabel::Property::MULTI_LINE, true);
    rubric.SetProperty(TextLabel::Property::TEXT_COLOR, Color::WHITE);
    rubric.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    rubric.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);
    rubric.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
    rubric.SetProperty(Actor::Property::ANCHOR_POINT, ParentOrigin::TOP_CENTER);
    mWindow.Add(rubric);

    mImageView1 = Toolkit::ImageView::New("https://raw.githubusercontent.com/dalihub/dali-test/refs/heads/master/visual-tests/remote-download/images/rockstar.jpg");

    mImageView1.SetProperty(Dali::Actor::Property::NAME, "mImageView1");
    mImageView1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mImageView1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    mImageView1.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
    mImageView1.SetProperty(Actor::Property::POSITION, Vector2(0, 50));
    mImageView1.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    mWindow.Add(mImageView1);

    mImageView2 = Toolkit::ImageView::New("https://raw.githubusercontent.com/dalihub/dali-test/refs/heads/master/visual-tests/remote-download/images/stars.jpg");
    mImageView2.SetProperty(Dali::Actor::Property::NAME, "mImageView2");
    mImageView2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mImageView2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    mImageView2.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
    mImageView2.SetProperty(Actor::Property::POSITION, Vector2(400, 100));
    mImageView2.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    mWindow.Add(mImageView2);

    mImageView3 = Toolkit::ImageView::New("https://raw.githubusercontent.com/dalihub/dali-test/refs/heads/master/visual-tests/remote-download/images/stormtrooper.jpg");
    mImageView3.SetProperty(Dali::Actor::Property::NAME, "mImageView3");
    mImageView3.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mImageView3.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    mImageView3.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
    mImageView3.SetProperty(Actor::Property::POSITION, Vector2(0, 400));
    mImageView3.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    mWindow.Add(mImageView3);

    mImageView5 = Toolkit::ImageView::New("http://static.midomi.com/h/images/w/weather_sunny.png");
    mImageView5.SetProperty(Dali::Actor::Property::NAME, "mImageView5");
    mImageView5.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mImageView5.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    mImageView5.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
    mImageView5.SetProperty(Actor::Property::POSITION, Vector2(800, 100));
    mImageView5.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    mWindow.Add(mImageView5);

    // Tie-in input event handlers:
    mWindow.KeyEventSignal().Connect(this, &MyTester::OnKeyEvent);
  }

  void OnAnimationEnd(Animation& source)
  {
    std::cout << "OnAnimationEnd" << std::endl;
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

  void OnKey(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      std::cout << "Window key : " << event.GetKeyCode() << std::endl;
    }
  }

  void OnTouch(const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::DOWN)
    {
      std::cout << "Window touch" << std::endl;
    }
  }

  bool OnControlKeyEvent(Toolkit::Control control, const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      std::cout << "Control down key : " << control.GetProperty<std::string>(Dali::Actor::Property::NAME) << ", keyCode : " << event.GetKeyCode() << std::endl;
    }
    else
    {
      std::cout << "Control up key : " << control.GetProperty<std::string>(Dali::Actor::Property::NAME) << ", keyCode : " << event.GetKeyCode() << std::endl;
    }
    return false;
  }

  bool OnControlTouch(Actor actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::DOWN)
    {
      std::cout << "Control touch " << actor.GetProperty<std::string>(Dali::Actor::Property::NAME) << ", parent " << actor.GetParent().GetProperty<std::string>(Dali::Actor::Property::NAME) << std::endl;
    }

    return false;
  }

  void OnFocusSet(Control control)
  {
    std::cout << "OnFocusSet " << control.GetProperty<std::string>(Dali::Actor::Property::NAME) << std::endl;
  }

  void OnFocusUnSet(Control control)
  {
    std::cout << "OnFocusUnSet " << control.GetProperty<std::string>(Dali::Actor::Property::NAME) << std::endl;
  }

private:
  Window       mWindow;
  Application& mApplication;

  Control   mControl1;
  Control   mControl2;
  ImageView mImageView1;
  ImageView mImageView2;
  ImageView mImageView3;
  ImageView mImageView4;
  ImageView mImageView5;

  TextLabel  mTextLabel1;
  TextLabel  mTextLabel2;
  TextField  mTextField;
  TextEditor mTextEditor;

  Animation mAnimation;
  Timer     mTimer;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv, "");
  MyTester    test(application);
  application.MainLoop();
  return 0;
}
