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
#include <dali/devel-api/adaptor-framework/gl-window.h>
#include "../direct-rendering/native-renderer.h"

using namespace Dali;
using namespace Dali::Toolkit;

class GlWindowExample : public ConnectionTracker
{
public:
  explicit GlWindowExample(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &GlWindowExample::Create);
  }

  ~GlWindowExample() override = default; // Nothing to do in destructor

  void Create(Application& application)
  {
    // Get a handle to the window
    Dali::Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);
    window.KeyEventSignal().Connect(this, &GlWindowExample::OnKeyEvent);
    window.GetRootLayer().TouchedSignal().Connect(this, &GlWindowExample::OnTouch);

    // Create a textLabel to add to our main window
    auto textLabel = Handle::New<TextLabel>(
      {
        {TextLabel::Property::TEXT, "Touch here to Exit"},
        {TextLabel::Property::TEXT_COLOR, Color::BLACK},
        {TextLabel::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::CENTER},
        {TextLabel::Property::VERTICAL_ALIGNMENT, VerticalAlignment::CENTER},
        {Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER},
        {Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER},
        {Dali::Actor::Property::NAME, "label"},
      });
    window.Add(textLabel);

    // Create GL Window and give it the same size as the main window
    CreateGlWindow(window.GetSize());

    // Resize main window to double the textLabel size so the GL Window takes focus
    auto textSize = textLabel.GetNaturalSize() * 2.0f;
    window.SetSize({uint32_t(textSize.width), uint32_t(textSize.y)});
  }

  int CreateGlWindow(const Vector2& windowSize)
  {
    auto w = windowSize.width;
    auto h = windowSize.height;

    NativeRenderer::CreateInfo info{};
    info.clearColor = {0, 0, 0, 0};
    info.name       = "GLWindow";
    info.offscreen  = false;
    info.viewportX  = 0;
    info.viewportY  = 0;
    info.width      = w;
    info.height     = h;
    info.threaded   = false;

    mRenderer            = std::make_unique<NativeRenderer>(info);
    mGlInitCallback      = MakeCallback(mRenderer.get(), &NativeRenderer::GlViewInitCallback);
    mGlRenderCallback    = MakeCallback(mRenderer.get(), &NativeRenderer::GlViewRenderCallback);
    mGlTerminateCallback = MakeCallback(mRenderer.get(), &NativeRenderer::GlViewTerminateCallback);

    // Create GlVWindow with callbacks
    mGlWindow = GlWindow::New();
    mGlWindow.RegisterGlCallbacks(mGlInitCallback, mGlRenderCallback, mGlTerminateCallback);
    mGlWindow.SetGraphicsConfig(true, false, 0, GlWindow::GlesVersion::VERSION_3_0);
    mGlWindow.SetPositionSize(PositionSize(0, 0, w, h));
    mGlWindow.SetRenderingMode(GlWindow::RenderingMode::CONTINUOUS);
    mGlWindow.KeyEventSignal().Connect(this, &GlWindowExample::OnKeyEvent);
    return 0;
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
  Application&                    mApplication;
  Dali::GlWindow                  mGlWindow;
  std::unique_ptr<NativeRenderer> mRenderer{};

  CallbackBase* mGlInitCallback{nullptr};
  CallbackBase* mGlRenderCallback{nullptr};
  CallbackBase* mGlTerminateCallback{nullptr};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application     application = Application::New(&argc, &argv);
  GlWindowExample example(application);
  application.MainLoop();
  return 0;
}
