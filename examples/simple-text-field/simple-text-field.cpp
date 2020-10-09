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

/**
 * @file simple-text-field-example.cpp
 * @brief Very basic usage of TextField control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-field-devel.h>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

/**
 * @brief The main class of the demo.
 */
class SimpleTextFieldExample : public ConnectionTracker
{
public:
  SimpleTextFieldExample(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &SimpleTextFieldExample::Create);
  }

  ~SimpleTextFieldExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.KeyEventSignal().Connect(this, &SimpleTextFieldExample::OnKeyEvent);
    window.SetBackgroundColor(Vector4(0.04f, 0.345f, 0.392f, 1.0f));

    mTextField = TextField::New();
    mTextField.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mTextField.SetProperty(Actor::Property::SIZE, Vector2(300.f, 60.f));
    mTextField.SetBackgroundColor(Color::WHITE);
    mTextField.SetBackgroundColor(Vector4(1.f, 1.f, 1.f, 0.15f));

    mTextField.SetProperty(TextField::Property::TEXT_COLOR, Color::BLACK);
    mTextField.SetProperty(TextField::Property::PLACEHOLDER_TEXT, "Unnamed folder");
    mTextField.SetProperty(TextField::Property::PLACEHOLDER_TEXT_FOCUSED, "Enter folder name.");

    mButtonSelectionStart = PushButton::New();
    mButtonSelectionEnd = PushButton::New();

    mButtonSelectionStart.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mButtonSelectionStart.SetProperty(Actor::Property::SIZE, Vector2(140.f, 50.f));
    mButtonSelectionStart.SetProperty(Actor::Property::POSITION, Vector2(0.f, 80.f));
    mButtonSelectionStart.SetBackgroundColor(Color::BLUE);
    mButtonSelectionStart.SetProperty(Button::Property::LABEL, "select <--");
    mButtonSelectionStart.ClickedSignal().Connect(this, &SimpleTextFieldExample::OnButtonClicked);

    mButtonSelectionEnd.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mButtonSelectionEnd.SetProperty(Actor::Property::SIZE, Vector2(140.f, 50.f));
    mButtonSelectionEnd.SetProperty(Actor::Property::POSITION, Vector2(0.f, 140.f));
    mButtonSelectionEnd.SetBackgroundColor(Color::BLUE);
    mButtonSelectionEnd.SetProperty(Button::Property::LABEL, "select -->");
    mButtonSelectionEnd.ClickedSignal().Connect(this, &SimpleTextFieldExample::OnButtonClicked);

    window.Add(mTextField);
    window.Add(mButtonSelectionStart);
    window.Add(mButtonSelectionEnd);
  }


  bool OnButtonClicked(Button button)
  {
    if(button == mButtonSelectionStart)
    {
      int iStart = mTextField.GetProperty(DevelTextField::Property::SELECTED_TEXT_START).Get<int>() - 1;
      if (iStart < 0)
      {
        iStart = 0;
      }
      mTextField.SetProperty(DevelTextField::Property::SELECTED_TEXT_START, iStart);
    }
    else if(button == mButtonSelectionEnd)
    {
      mTextField.SetProperty(DevelTextField::Property::SELECTED_TEXT_END , mTextField.GetProperty(DevelTextField::Property::SELECTED_TEXT_END).Get<int>() + 1);
    }

    return true;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication;
  TextField mTextField;
  PushButton mButtonSelectionStart;
  PushButton mButtonSelectionEnd;
};

void RunTest(Application& application)
{
  SimpleTextFieldExample test(application);

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int DALI_EXPORT_API main(int argc, char** argv)
{
  // DALI_DEMO_THEME_PATH not passed to Application so TextField example uses default Toolkit style sheet.
  Application application = Application::New(&argc, &argv);

  RunTest(application);

  return 0;
}
