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

#include <dali-toolkit/dali-toolkit.h>
#include <dali/dali.h>

#include "generated/rendering-line-vert.h"
#include "generated/rendering-line-frag.h"

using namespace Dali;
using namespace Toolkit;

// This example shows how to draw a line in actor's color
//
class DrawLineController : public ConnectionTracker
{
public:
  DrawLineController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &DrawLineController::Create);
  }

  ~DrawLineController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Step 1. Create shader
    CreateLineShader();

    // Step 2. Prepare geometry
    CreateLineGeometry();

    // Step 3. Create a renderer
    CreateRenderer();

    // Step 4. Create an Actor
    CreateActor();

    // Respond to a click anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &DrawLineController::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &DrawLineController::OnKeyEvent);
  }

  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  /**
   * @brief Called when any key event is received
   *
   * Will use this to quit the application if Back or the Escape key is received
   * @param[in] event The key event information
   */
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

  /**
   * This function creates a line geometry made of two vertices in order
   * to draw a diagonal line.
   */
  void CreateLineGeometry()
  {
    Vector2 vertices[] = {
      Vector2(-1.0f, -1.0f),
      Vector2(1.0f, 1.0f)};

    VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map()
                                                    .Add("aPosition", Property::VECTOR2));
    vertexBuffer.SetData(vertices, sizeof(vertices) / sizeof(Vector2));

    mGeometry = Geometry::New();
    mGeometry.AddVertexBuffer(vertexBuffer);
    mGeometry.SetType(Geometry::LINES);
  }

  /**
   * Creates a shader using inlined variable SHADER_RENDERING_LINE_VERT and SHADER_RENDERING_LINE_FRAG
   *
   * Shaders are very basic and all they do is transforming vertices and applying actor's colour.
   */
  void CreateLineShader()
  {
    mShader = Shader::New(SHADER_RENDERING_LINE_VERT, SHADER_RENDERING_LINE_FRAG);
  }

  /**
   * Function creates renderer.
   */
  void CreateRenderer()
  {
    mRenderer = Renderer::New(mGeometry, mShader);
  }

  /**
   * Creates new actor and attaches renderer.
   */
  void CreateActor()
  {
    Window window = mApplication.GetWindow();
    Size   size   = Vector2(window.GetSize()) * 0.25f;
    mActor        = Actor::New();
    mActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mActor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
    mActor.SetProperty(Actor::Property::COLOR, Color::BLACK);
    mActor.SetProperty(Actor::Property::SIZE, Vector3(size.x, size.x, size.x));
    mActor.AddRenderer(mRenderer);
    window.Add(mActor);
  }

private:
  Application& mApplication;

  Renderer mRenderer;
  Shader   mShader;
  Geometry mGeometry;
  Actor    mActor;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application        application = Application::New(&argc, &argv);
  DrawLineController test(application);
  application.MainLoop();
  return 0;
}
