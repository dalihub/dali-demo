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
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/scene.h>

using namespace Dali;
using namespace Dali::Toolkit;

const int  MARGIN = 50;
static int count  = 0;

// This example shows how to create and display Hello World! using a simple TextActor
//
class WindowExampleController : public ConnectionTracker
{
public:
  WindowExampleController(Application& application)
  : mApplication(application),
    manual_rotation_flag(false),
    rotation_count(0),
    rot(0),
    bColor(0)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &WindowExampleController::Create);
    mApplication.PauseSignal().Connect(this, &WindowExampleController::Pause);
    mApplication.ResumeSignal().Connect(this, &WindowExampleController::Resume);
    mApplication.DeviceOrientationChangedSignal().Connect(this, &WindowExampleController::OnDeviceOrientationChanged);
    mApplication.LowMemorySignal().Connect(this, &WindowExampleController::OnLowMemory);
  }

  ~WindowExampleController() = default;

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window and set the background colour
    Window window = application.GetWindow();
    window.AddAuxiliaryHint("wm.policy.win.user.geometry", "1");
    window.SetBackgroundColor(Color::WHITE);
    window.SetTransparency(false);

    window.SetType(WindowType::NOTIFICATION);
    window.SetNotificationLevel(WindowNotificationLevel::MEDIUM);

    currentOrientation   = Dali::WindowOrientation::PORTRAIT;
    enableTimer          = false;
    bChangedOrientation  = false;
    manual_rotation_flag = false;

    // Add a text label to the window
    TextLabel textLabel = TextLabel::New("Window Example");
    textLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    textLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    textLabel.SetProperty(Actor::Property::NAME, "WindowExampleLabel");
    textLabel.SetBackgroundColor(Color::RED);
    window.Add(textLabel);

    // Create a clipping control and add a child to it
    mClipControl = Control::New();
    mClipControl.SetProperty(Actor::Property::SIZE, Vector2(250.0f, 100.0f));
    mClipControl.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.75f, 0.75f, 0.5f));
    mClipControl.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mClipControl.SetBackgroundColor(Color::BLUE);
    window.Add(mClipControl);

    auto child = Control::New();
    child.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 250.0f));
    child.SetProperty(Actor::Property::PARENT_ORIGIN, AnchorPoint::CENTER);
    child.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    child.SetBackgroundColor(Color::GREEN);
    mClipControl.Add(child);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &WindowExampleController::OnKeyEvent);

    // Respond to a click anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &WindowExampleController::OnTouch);
    window.ResizeSignal().Connect(this, &WindowExampleController::OnWindowResized);
    DevelWindow::MovedSignal(window).Connect(this, &WindowExampleController::OnWindowMoved);
    DevelWindow::OrientationChangedSignal(window).Connect(this, &WindowExampleController::OnWindowOrientationChanged);
    DevelWindow::MoveCompletedSignal(window).Connect(this, &WindowExampleController::OnWindowMovedByServer);
    DevelWindow::ResizeCompletedSignal(window).Connect(this, &WindowExampleController::OnWindowResizedByServer);

    window.AddAvailableOrientation(Dali::WindowOrientation::PORTRAIT);
    window.AddAvailableOrientation(Dali::WindowOrientation::LANDSCAPE);
    window.AddAvailableOrientation(Dali::WindowOrientation::PORTRAIT_INVERSE);
    window.AddAvailableOrientation(Dali::WindowOrientation::LANDSCAPE_INVERSE);

    window.Show();

    mTimer = Timer::New(50);
    mTimer.TickSignal().Connect(this, &WindowExampleController::OnTimerTick);
  }

  void Pause(Application& application)
  {
    DALI_LOG_RELEASE_INFO("Application is paused..***********************\n");
  }

  void Resume(Application& application)
  {
    DALI_LOG_RELEASE_INFO("Application is resumed..***********************\n");
  }

  void PresentedCallback(int frameId)
  {
    DALI_LOG_RELEASE_INFO("PresentedCallback: FrameId %d\n", frameId);
  }

  void OnDeviceOrientationChanged(DeviceStatus::Orientation::Status status)
  {
    DALI_LOG_RELEASE_INFO("orientation status: %d\n", status);
  }

  void OnLowMemory(DeviceStatus::Memory::Status status)
  {
    DALI_LOG_RELEASE_INFO("low memory status: %d\n", status);
  }

  bool OnTimerTick()
  {
    Dali::Window window = mApplication.GetWindow();
    bool         ret    = true;

    if(DevelWindow::IsWindowRotating(window) == true)
    {
      isRotating = true;
      DALI_LOG_RELEASE_INFO("Window is rotating!!!!\n");
      count++;
      if(count > 100)
      {
        DALI_LOG_RELEASE_INFO("Send AckWindowRotation in Timer\n");
        DevelWindow::SendRotationCompletedAcknowledgement(window);
        count = 0;
        ret   = false;
      }
    }
    else
    {
      DALI_LOG_RELEASE_INFO("Window is not rotating!!!!\n");
    }
    return ret;
  }

  void OnWindowMoved(Dali::Window winHandle, Dali::Window::WindowPosition position)
  {
    DALI_LOG_RELEASE_INFO("OnWindowMoved, x:%d, y:%d, width:%d\n", position.GetX(), position.GetY());
  }

  void OnWindowMovedByServer(Dali::Window winHandle, Dali::Window::WindowPosition position)
  {
    DALI_LOG_RELEASE_INFO("OnWindowMovedByServer, x:%d, y:%d, width:%d\n", position.GetX(), position.GetY());
  }

  void OnWindowResizedByServer(Dali::Window winHandle, Dali::Window::WindowSize size)
  {
    DALI_LOG_RELEASE_INFO("OnWindowResizedByServer, x:%d, y:%d, width:%d\n", size.GetWidth(), size.GetHeight());
  }

  void OnWindowOrientationChanged(Dali::Window window, Dali::WindowOrientation orientation)
  {
    DALI_LOG_RELEASE_INFO("OnWindowOrientationChanged, changed window orientation: %d\n", orientation);
  }

  void OnWindowResized(Dali::Window winHandle, Dali::Window::WindowSize size)
  {
    DALI_LOG_RELEASE_INFO("Resize finish %d  x heigt %d\n", size.GetWidth(), size.GetHeight());
    if(bChangedOrientation)
    {
      if(DevelWindow::GetCurrentOrientation(winHandle) == Dali::WindowOrientation::PORTRAIT)
      {
        DALI_LOG_RELEASE_INFO("DevelWindow::OnWindowResized, current orientation is Dali::WindowOrientation::PORTRAIT\n");
        PositionSize newWindowPosition(0, 0, 400, 300);
        DALI_LOG_RELEASE_INFO("DevelWindow::OnWindowResized, SetPositionSize(720 * 1280)\n");
        DevelWindow::SetPositionSize(winHandle, newWindowPosition);
      }
      else if(DevelWindow::GetCurrentOrientation(winHandle) == Dali::WindowOrientation::LANDSCAPE)
      {
        DALI_LOG_RELEASE_INFO("DevelWindow::OnWindowResized, current orientation is Dali::WindowOrientation::LANDSCAPE\n");
        PositionSize newWindowPosition(0, 0, 1280, 720);
        DALI_LOG_RELEASE_INFO("DevelWindow::OnWindowResized, SetPositionSize(1280 * 720)\n");
        DevelWindow::SetPositionSize(winHandle, newWindowPosition);
      }
      else if(DevelWindow::GetCurrentOrientation(winHandle) == Dali::WindowOrientation::PORTRAIT_INVERSE)
      {
        DALI_LOG_RELEASE_INFO("DevelWindow::OnWindowResized, current orientation is Dali::WindowOrientation::PORTRAIT_INVERSE\n");
        PositionSize newWindowPosition(100, 100, 720, 1280);
        DALI_LOG_RELEASE_INFO("DevelWindow::OnWindowResized, SetPositionSize(100, 100, 720 * 1280)\n");
        DevelWindow::SetPositionSize(winHandle, newWindowPosition);
      }
      else if(DevelWindow::GetCurrentOrientation(winHandle) == Dali::WindowOrientation::LANDSCAPE_INVERSE)
      {
        DALI_LOG_RELEASE_INFO("DevelWindow::OnWindowResized, current orientation is Dali::WindowOrientation::LANDSCAPE_INVERSE\n");
        PositionSize newWindowPosition(100, 100, 1280, 720);
        DALI_LOG_RELEASE_INFO("DevelWindow::OnWindowResized, SetPositionSize(100, 100, 1280 * 720)\n");
        DevelWindow::SetPositionSize(winHandle, newWindowPosition);
      }
    }
  }

  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::DOWN)
    {
      const int local_x  = static_cast<int>(touch.GetLocalPosition(0).x);
      const int local_y  = static_cast<int>(touch.GetLocalPosition(0).y);
      const int global_x = static_cast<int>(touch.GetScreenPosition(0).x);
      const int global_y = static_cast<int>(touch.GetScreenPosition(0).y);

      DALI_LOG_RELEASE_INFO("Main Window Touch Event : x:%d, y:%d\n", local_x, local_y);
      DALI_LOG_RELEASE_INFO("global position: x:%d, y:%d\n", global_x, global_y);

      Dali::Window       window     = mApplication.GetWindow();
      Window::WindowSize windowSize = window.GetSize();

      DALI_LOG_RELEASE_INFO("window size: w:%d, h:%d\n", windowSize.GetWidth(), windowSize.GetHeight());
      DevelWindow::EnableFloatingMode(window, true);

      if((local_x < MARGIN) && (local_y < MARGIN)) // left top corner
      {
        DALI_LOG_RELEASE_INFO("RequestResizeToServer: TOP_LEFT\n", local_x, local_y);
        DevelWindow::RequestResizeToServer(window, WindowResizeDirection::TOP_LEFT);
      }
      else if((local_x > (windowSize.GetWidth() - MARGIN)) && (local_y < MARGIN)) // rigth top corner
      {
        DALI_LOG_RELEASE_INFO("RequestResizeToServer: TOP_RIGHT\n", local_x, local_y);
        DevelWindow::RequestResizeToServer(window, WindowResizeDirection::TOP_RIGHT);
      }
      else if((local_x > MARGIN) && (local_x < (windowSize.GetWidth() - MARGIN)) && (local_y < MARGIN))
      {
        DALI_LOG_RELEASE_INFO("RequestResizeToServer: TOP\n", local_x, local_y);
        DevelWindow::RequestResizeToServer(window, WindowResizeDirection::TOP);
      }
      else if((local_x < MARGIN) && (local_y < (windowSize.GetHeight() - MARGIN)) && (local_y > MARGIN))
      {
        DALI_LOG_RELEASE_INFO("RequestResizeToServer: TOP\n", local_x, local_y);
        DevelWindow::RequestResizeToServer(window, WindowResizeDirection::LEFT);
      }
      else if((local_x > (windowSize.GetWidth() - MARGIN)) && (local_y < (windowSize.GetHeight() - MARGIN)) && (local_y > MARGIN))
      {
        DALI_LOG_RELEASE_INFO("RequestResizeToServer: LEFT\n", local_x, local_y);
        DevelWindow::RequestResizeToServer(window, WindowResizeDirection::RIGHT);
      }
      else if((local_x < MARGIN) && (local_y > (windowSize.GetHeight() - MARGIN))) // left bottom corner
      {
        DALI_LOG_RELEASE_INFO("RequestResizeToServer: RIGHT\n", local_x, local_y);
        DevelWindow::RequestResizeToServer(window, WindowResizeDirection::BOTTOM_LEFT);
      }
      else if((local_x > (windowSize.GetWidth() - MARGIN)) && (local_y > (windowSize.GetHeight() - MARGIN))) // right bottom corner
      {
        DALI_LOG_RELEASE_INFO("RequestResizeToServer: BOTTOM_RIGHT\n", local_x, local_y);
        DevelWindow::RequestResizeToServer(window, WindowResizeDirection::BOTTOM_RIGHT);
      }
      else
      {
        DALI_LOG_RELEASE_INFO("RequestMoveToServer\n", local_x, local_y);
        DevelWindow::RequestMoveToServer(window);
      }
    }
    return true;
  }

  //////////////////////////////////////////////////////////////////////////
  // Test Sub Window Resize

  void CreateSubWindow()
  {
    mSecondWindow = Window::New(PositionSize(0, 0, 600, 400), "", false);
    mSecondWindow.SetTransparency(true);
    mSecondWindow.SetBackgroundColor(Vector4(1.0, 0.3, 0.3, 0.5));

    mTextLabel2 = TextLabel::New("Second window");
    mTextLabel2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mTextLabel2.SetProperty(Actor::Property::NAME, "Second Window");
    mSecondWindow.Add(mTextLabel2);

    mSecondWindow.GetRootLayer().TouchedSignal().Connect(this, &WindowExampleController::OnSubWindowTouch);
    mSecondWindow.ResizeSignal().Connect(this, &WindowExampleController::OnSubWindowResized);
    DevelWindow::MovedSignal(mSecondWindow).Connect(this, &WindowExampleController::OnSubWindowMoved);
    DevelWindow::MoveCompletedSignal(mSecondWindow).Connect(this, &WindowExampleController::OnSubWindowMovedByServer);
    DevelWindow::ResizeCompletedSignal(mSecondWindow).Connect(this, &WindowExampleController::OnSubWindowResizedByServer);

    mSecondWindow.AddAvailableOrientation(Dali::WindowOrientation::PORTRAIT);
    mSecondWindow.AddAvailableOrientation(Dali::WindowOrientation::LANDSCAPE);
    mSecondWindow.AddAvailableOrientation(Dali::WindowOrientation::PORTRAIT_INVERSE);
    mSecondWindow.AddAvailableOrientation(Dali::WindowOrientation::LANDSCAPE_INVERSE);
    mSecondWindow.Show();

    Rect<int> setInputRegion(0, 0, 1920, 1080);
    //mSecondWindow.SetInputRegion(setInputRegion);
    DevelWindow::ExcludeInputRegion(mSecondWindow, setInputRegion);

    Dali::Window window = mApplication.GetWindow();
    DevelWindow::SetParent(mSecondWindow, window);
  }

  void OnSubWindowMoved(Dali::Window winHandle, Dali::Window::WindowPosition position)
  {
    DALI_LOG_RELEASE_INFO("OnWindowMoved, x:%d, y:%d, width:%d\n", position.GetX(), position.GetY());
  }

  void OnSubWindowResized(Dali::Window winHandle, Dali::Window::WindowSize size)
  {
    DALI_LOG_RELEASE_INFO("OnSubWindowResized %d  x heigt %d\n", size.GetWidth(), size.GetHeight());
  }

  void OnSubWindowMovedByServer(Dali::Window winHandle, Dali::Window::WindowPosition position)
  {
    DALI_LOG_RELEASE_INFO("OnWindowMovedByServer, x:%d, y:%d, width:%d\n", position.GetX(), position.GetY());
  }

  void OnSubWindowResizedByServer(Dali::Window winHandle, Dali::Window::WindowSize size)
  {
    DALI_LOG_RELEASE_INFO("OnWindowResizedByServer, x:%d, y:%d, width:%d\n", size.GetWidth(), size.GetHeight());
  }

  bool OnSubWindowTouch(Actor actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::DOWN)
    {
      const int local_x  = static_cast<int>(touch.GetLocalPosition(0).x);
      const int local_y  = static_cast<int>(touch.GetLocalPosition(0).y);
      const int global_x = static_cast<int>(touch.GetScreenPosition(0).x);
      const int global_y = static_cast<int>(touch.GetScreenPosition(0).y);

      DALI_LOG_RELEASE_INFO("SubWindow Touch Event : x:%d, y:%d\n", local_x, local_y);
      DALI_LOG_RELEASE_INFO("global position: x:%d, y:%d\n", global_x, global_y);

      Dali::Window       window     = mSecondWindow;
      Window::WindowSize windowSize = window.GetSize();

      DALI_LOG_RELEASE_INFO("window size: w:%d, h:%d\n", windowSize.GetWidth(), windowSize.GetHeight());
      DevelWindow::EnableFloatingMode(window, true);

      if((local_x < MARGIN) && (local_y < MARGIN)) // left top corner
      {
        DALI_LOG_RELEASE_INFO("RequestResizeToServer: TOP_LEFT\n", local_x, local_y);
        DevelWindow::RequestResizeToServer(window, WindowResizeDirection::TOP_LEFT);
      }
      else if((local_x > (windowSize.GetWidth() - MARGIN)) && (local_y < MARGIN)) // rigth top corner
      {
        DALI_LOG_RELEASE_INFO("RequestResizeToServer: TOP_RIGHT\n", local_x, local_y);
        DevelWindow::RequestResizeToServer(window, WindowResizeDirection::TOP_RIGHT);
      }
      else if((local_x > MARGIN) && (local_x < (windowSize.GetWidth() - MARGIN)) && (local_y < MARGIN))
      {
        DALI_LOG_RELEASE_INFO("RequestResizeToServer: TOP\n", local_x, local_y);
        DevelWindow::RequestResizeToServer(window, WindowResizeDirection::TOP);
      }
      else if((local_x < MARGIN) && (local_y < (windowSize.GetHeight() - MARGIN)) && (local_y > MARGIN))
      {
        DALI_LOG_RELEASE_INFO("RequestResizeToServer: TOP\n", local_x, local_y);
        DevelWindow::RequestResizeToServer(window, WindowResizeDirection::LEFT);
      }
      else if((local_x > (windowSize.GetWidth() - MARGIN)) && (local_y < (windowSize.GetHeight() - MARGIN)) && (local_y > MARGIN))
      {
        DALI_LOG_RELEASE_INFO("RequestResizeToServer: LEFT\n", local_x, local_y);
        DevelWindow::RequestResizeToServer(window, WindowResizeDirection::RIGHT);
      }
      else if((local_x < MARGIN) && (local_y > (windowSize.GetHeight() - MARGIN))) // left bottom corner
      {
        DALI_LOG_RELEASE_INFO("RequestResizeToServer: RIGHT\n", local_x, local_y);
        DevelWindow::RequestResizeToServer(window, WindowResizeDirection::BOTTOM_LEFT);
      }
      else if((local_x > (windowSize.GetWidth() - MARGIN)) && (local_y > (windowSize.GetHeight() - MARGIN))) // right bottom corner
      {
        DALI_LOG_RELEASE_INFO("RequestResizeToServer: BOTTOM_RIGHT\n", local_x, local_y);
        DevelWindow::RequestResizeToServer(window, WindowResizeDirection::BOTTOM_RIGHT);
      }
      else
      {
        DALI_LOG_RELEASE_INFO("RequestMoveToServer\n", local_x, local_y);
        DevelWindow::RequestMoveToServer(window);
      }
    }
    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    Dali::Window window = mApplication.GetWindow();
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
      else if(event.GetKeyName() == "1")
      {
        DALI_LOG_RELEASE_INFO("Move/Resize Test  ::: window move and resize (10, 10) (600 x 400)\n");
        PositionSize windowPosition(10, 10, 600, 400);
        DevelWindow::SetPositionSize(window, windowPosition);
      }
      else if(event.GetKeyName() == "2")
      {
        DALI_LOG_RELEASE_INFO("Present Callback Test\n");

        if(bColor == 0)
        {
          window.SetBackgroundColor(Color::WHITE);
          bColor = 1;
        }
        else
        {
          window.SetBackgroundColor(Color::BLACK);
          bColor = 0;
        }

        DevelWindow::AddFramePresentedCallback(window, std::unique_ptr<Dali::CallbackBase>(MakeCallback(this, &WindowExampleController::PresentedCallback)), count);
      }
      else if(event.GetKeyName() == "3")
      {
        DALI_LOG_RELEASE_INFO("Send AckWindowRotation test\n");
        if(manual_rotation_flag == true)
        {
          if(isRotating)
          {
            DALI_LOG_RELEASE_INFO("Send AckWindowRotation in Key Event Callback\n");
            DevelWindow::SendRotationCompletedAcknowledgement(window);
            manual_rotation_flag = false;
            isRotating           = false;
          }
          else
          {
            DALI_LOG_RELEASE_INFO("is not Rotating\n");
          }
        }
        else if(event.GetKeyName() == "4")
        {
          DALI_LOG_RELEASE_INFO("Set Manual Window Rotation Test\n");
          if(manual_rotation_flag)
          {
            mTimer.Start();
          }
          else
          {
            mTimer.Stop();
          }
          DALI_LOG_RELEASE_INFO("call SetNeedsRotationCompletedAcknowledgement with flag %d\n", manual_rotation_flag);
          DevelWindow::SetNeedsRotationCompletedAcknowledgement(window, manual_rotation_flag);
        }
        else
        {
          DALI_LOG_RELEASE_INFO("set flag %d\n", manual_rotation_flag);
          manual_rotation_flag = true;
        }
      }
      else if(event.GetKeyName() == "5")
      {
        DALI_LOG_RELEASE_INFO("Window Rotation and Resize test together\n");
        if(!bChangedOrientation)
        {
          DALI_LOG_RELEASE_INFO("Both Window Rotation and Resize Test\n");
          bChangedOrientation = true;
        }
        else
        {
          DALI_LOG_RELEASE_INFO("Only Window Rotation Test\n");
          bChangedOrientation = false;
        }
      }
      else if(event.GetKeyName() == "6")
      {
        DALI_LOG_RELEASE_INFO("Maximize Test\n");
        if(DevelWindow::IsMaximized(window))
        {
          DevelWindow::Maximize(window, false);
        }
        else
        {
          DevelWindow::Maximize(window, true);
        }
      }
      else if(event.GetKeyName() == "7")
      {
        DALI_LOG_RELEASE_INFO("Minimize Test\n");
        if(DevelWindow::IsMinimized(window))
        {
          DevelWindow::Minimize(window, false);
        }
        else
        {
          DevelWindow::Minimize(window, true);
        }
      }

      else if(event.GetKeyName() == "8")
      {
        DALI_LOG_RELEASE_INFO("Set/Get Full Screen window Test\n");
        if(DevelWindow::GetFullScreen(window))
        {
          DevelWindow::SetFullScreen(window, false);
        }
        else
        {
          DevelWindow::SetFullScreen(window, true);
        }
      }
      else if(event.GetKeyName() == "9")
      {
        DALI_LOG_RELEASE_INFO("Sub window Test\n");
        CreateSubWindow();
      }
      else if(event.GetKeyName() == "0")
      {
        // scissor test for Window Rotation
        // Toggle the clipping mode on mClippingControl if any other actor by pressing any key
        DALI_LOG_RELEASE_INFO("Scissor Test\n");
        ClippingMode::Type currentMode;
        mClipControl.GetProperty(Actor::Property::CLIPPING_MODE).Get(currentMode);
        mClipControl.SetProperty(Actor::Property::CLIPPING_MODE,
                                 ((currentMode == ClippingMode::DISABLED) ? ClippingMode::CLIP_TO_BOUNDING_BOX : ClippingMode::DISABLED));
      }
    }
  }

  void OnAuxMessageEvent(const std::string& key, const std::string& value, const Property::Array& options)
  {
    if(!key.empty())
    {
      std::cout << "Key: " << key << std::endl;
      if(!value.empty())
      {
        std::cout << "Value : " << value << std::endl;
      }

      if(!options.Empty())
      {
        uint32_t options_count = static_cast<uint32_t>(options.Count());
        for(uint32_t i(0); i != options_count; ++i)
        {
          std::cout << "Option : " << options.GetElementAt(i) << std::endl;
        }
      }
    }
  }

private:
  Application& mApplication;
  Control      mClipControl;
  bool         manual_rotation_flag;
  int          rotation_count;
  int          rot;
  Timer        mTimer;
  TextLabel    mTextLabel2;
  bool         enableTimer;
  bool         bChangedOrientation;
  bool         isRotating;
  bool         bColor;

  Dali::Window            mSecondWindow;
  Dali::WindowOrientation currentOrientation;

  CallbackBase* mPresentCallback{};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application             application = Application::New(&argc, &argv);
  WindowExampleController test(application);
  application.MainLoop();
  return 0;
}
