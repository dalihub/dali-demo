/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/drawable-actor.h>

#include "native-renderer.h"

// Include the GLES header
#include <GLES3/gl3.h>
#include <dali/graphics-api/graphics-buffer-create-info.h>

#include <dali/devel-api/common/stage-devel.h>

using TextLabel = Dali::Toolkit::TextLabel;
using namespace Dali;

// This example shows DrawableActor using native GL code to clear screen to red
// and renders TextLabel on top of it.
//
class DrawableActorExampleController : public ConnectionTracker
{
public:
  explicit DrawableActorExampleController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &DrawableActorExampleController::Create);
  }

  ~DrawableActorExampleController() override = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Create native renderer
    mRenderer = MakeUnique<NativeRenderer>(window.GetSize().GetWidth(), window.GetSize().GetHeight());

    // Create render callback
    mRenderCallback = RenderCallback::New<NativeRenderer>(mRenderer.Get(), &NativeRenderer::OnRender);

    // Create drawable actor
    mGLActor = DrawableActor::New(*mRenderCallback);

    mGLActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mGLActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);

    // Set size on the actor (half the window size to show that glClear() and scissor test work together)
    mGLActor.SetProperty(Actor::Property::SIZE, Size(window.GetSize()) * 0.75f);
    mGLActor[Actor::Property::POSITION] = Vector3(50.0f, 50.0f, 0.0f);

    // Add actor to the scene
    window.Add(mGLActor);

    // Create TextLabel
    mTextLabel = TextLabel::New("This text overlays DrawableActor");
    mTextLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mTextLabel.SetProperty(Dali::Actor::Property::NAME, "SomeTextLabel");
    window.Add(mTextLabel);

    // Respond to a touch anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &DrawableActorExampleController::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &DrawableActorExampleController::OnKeyEvent);
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

  TextLabel     mTextLabel;
  DrawableActor mGLActor;

  UniquePtr<RenderCallback> mRenderCallback;
  UniquePtr<NativeRenderer> mRenderer{nullptr};

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                    application = Application::New(&argc, &argv);
  DrawableActorExampleController test(application);
  application.MainLoop();
  return 0;
}
