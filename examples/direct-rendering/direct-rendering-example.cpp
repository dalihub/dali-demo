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
#include "native-renderer.h"
#include <dali/integration-api/debug.h>

#include <dali/public-api/render-tasks/render-task-list.h>
using namespace Dali;
using namespace Dali::Toolkit;
namespace
{
/**
 * Helper function reading env variable as integer
 */
int GetEnvInt(const char* name, int def)
{
  auto v = getenv(name);
  return v ? atoi(v) : def;
}

/**
 * Environment variable: DR_THREAD_ENABLE
 *
 * if set to 1 the Direct Rendering will use offscreen buffer and render in parallel. It is a direct
 * equivalent of the EGL Image mode.
 */
const uint32_t DR_THREAD_ENABLED = GetEnvInt("DR_THREAD_ENABLED", 0);

/**
 * Environment variable: EGL_ENABLED
 *
 * When set to 1 the native image is used for direct rendering (rendering is parallel by default).
 */

/**
 * Environment variable: UNSAFE_MODE (default: 1)
 *
 * Enables/disables rendering within GL window context rather than creating isolated context
 */

} // namespace

/**
 * RenderView encapsulates single GLView callback and its parameters.
 */
struct RenderView
{
  explicit RenderView(const Dali::Window& window)
  {
    mWindow = window;
  }

  int Create(const Vector2& pos, Toolkit::GlView::BackendMode mode)
  {
    auto w = mWindow.GetSize().GetWidth();
    auto h = mWindow.GetSize().GetHeight();

    mWindow.SetBackgroundColor(Color::BLUE);
    NativeRenderer::CreateInfo info{};
    info.clearColor = {0, 0, 0, 0};
    info.name       = "DR";
    info.offscreen  = (mode == Toolkit::GlView::BackendMode::EGL_IMAGE_OFFSCREEN_RENDERING || DR_THREAD_ENABLED);
    info.viewportX  = 0;
    info.viewportY  = 0;
    info.width      = w;
    info.height     = h;
    info.threaded   = (mode != Toolkit::GlView::BackendMode::EGL_IMAGE_OFFSCREEN_RENDERING) && (DR_THREAD_ENABLED);

    // turn off threaded mode if rendering is direct
    if(mode == Dali::Toolkit::GlView::BackendMode::UNSAFE_DIRECT_RENDERING)
    {
      DALI_LOG_RELEASE_INFO("Threaded and offscreen rendering modes cannot be used with UNSAFE_RENDERING_DIRECT backend!\n");
      DALI_LOG_RELEASE_INFO("Setting threading and offscreen to false!\n");
      info.threaded = false;
      info.offscreen = false;
    }

    // Enable threaded rendering
    if(info.threaded && mode == Dali::Toolkit::GlView::BackendMode::DIRECT_RENDERING)
    {
      mode = Dali::Toolkit::GlView::BackendMode::DIRECT_RENDERING_THREADED;
    }

    mRenderer            = std::make_unique<NativeRenderer>(info);
    mGlInitCallback      = MakeCallback(mRenderer.get(), &NativeRenderer::GlViewInitCallback);
    mGlRenderCallback    = MakeCallback(mRenderer.get(), &NativeRenderer::GlViewRenderCallback);
    mGlTerminateCallback = MakeCallback(mRenderer.get(), &NativeRenderer::GlViewTerminateCallback);

    // Create GlView with callbacks
    auto glView = Toolkit::GlView::New(mode, Toolkit::GlView::ColorFormat::RGBA8888); //, Toolkit::GlView::BackendMode::DIRECT_RENDERING);
    glView.SetGraphicsConfig(true, false, 0, Toolkit::GlView::GraphicsApiVersion::GLES_VERSION_3_0);
    glView.RegisterGlCallbacks(mGlInitCallback, mGlRenderCallback, mGlTerminateCallback);

    glView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    glView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);

    // Set size on the actor (half the window size to show that glClear() and scissor test work together)
    glView.SetProperty(Actor::Property::SIZE, Size(w, h));
    glView.SetProperty(Actor::Property::POSITION, pos);
    glView.SetRenderingMode(Toolkit::GlView::RenderingMode::CONTINUOUS);

    std::string strMode = "Backend: Unknown"; // shouldn't happen
    if(mode == Dali::Toolkit::GlView::BackendMode::UNSAFE_DIRECT_RENDERING)
    {
      strMode = "Backend: UNSAFE_DIRECT_RENDERING";
    }
    else if(mode == Dali::Toolkit::GlView::BackendMode::DIRECT_RENDERING)
    {
      strMode = "Backend: DIRECT_RENDERING (isolated context)";
    }
    else if(mode == Dali::Toolkit::GlView::BackendMode::EGL_IMAGE_OFFSCREEN_RENDERING)
    {
      strMode = "Backend: EGL_IMAGE_OFFSCREEN_RENDERING";
    }
    strMode += ", threaded = " + std::to_string(info.threaded) + ", offscreen = " + std::to_string(info.offscreen);

    TextLabel textLabel = TextLabel::New(strMode);
    textLabel.SetProperty(TextLabel::Property::TEXT_COLOR, Color::WHITE);
    textLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    textLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
    textLabel.SetProperty(Dali::Actor::Property::NAME, "label");
    textLabel.SetProperty(Dali::Actor::Property::POSITION, Vector2(0, 0));
    glView.Add(textLabel);

    mWindow.Add(glView);

    mGlView = glView;

    return 0;
  }

  Dali::Window                    mWindow;
  Toolkit::GlView                 mGlView;
  std::unique_ptr<NativeRenderer> mRenderer{};

  CallbackBase* mGlInitCallback{};
  CallbackBase* mGlRenderCallback{};
  CallbackBase* mGlTerminateCallback{};
};

// This example oresents DirectRendering feature which allows injecting
// custom GL code into the DALi render commands stream

class DirectRenderingExampleController : public ConnectionTracker
{
public:
  explicit DirectRenderingExampleController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &DirectRenderingExampleController::Create);
  }

  ~DirectRenderingExampleController() override = default; // Nothing to do in destructor

  void Create(Application& application)
  {
    // Get a handle to the window
    Dali::Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    window.KeyEventSignal().Connect(this, &DirectRenderingExampleController::OnKeyEvent);
    window.GetRootLayer().TouchedSignal().Connect(this, &DirectRenderingExampleController::OnTouch);

    mDRView = std::make_unique<RenderView>(window);

    bool eglMode = GetEnvInt("EGL_ENABLED", 0);
    bool glDirectMode = GetEnvInt("UNSAFE_MODE", 1);

    Toolkit::GlView::BackendMode mode(Dali::Toolkit::GlView::BackendMode::UNSAFE_DIRECT_RENDERING);
    if(eglMode)
    {
      mode = Dali::Toolkit::GlView::BackendMode::EGL_IMAGE_OFFSCREEN_RENDERING;
    }
    else if(!glDirectMode)
    {
      mode = Dali::Toolkit::GlView::BackendMode::DIRECT_RENDERING;
    }

    mDRView->Create(Vector2::ZERO, mode);
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
  Application&                mApplication;
  std::unique_ptr<RenderView> mDRView;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                      application = Application::New(&argc, &argv);
  DirectRenderingExampleController test(application);
  application.MainLoop();
  return 0;
}
