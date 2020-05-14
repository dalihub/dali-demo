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
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/render-tasks/render-task-devel.h>

using namespace Dali;
using namespace Toolkit;

namespace
{

#define SHADER_HEADER "#version 300 es\nprecision mediump float;\n"

#define SHADER_SOURCE(x) SHADER_HEADER #x

const char* const VSH =
  SHADER_SOURCE(
  uniform mat4 uMvpMatrix;  // DALI
  uniform vec3 uSize;  // DALI
  uniform vec3 uPositionOffset;  // example

  in vec3 aPosition;

  void main()
  {
    vec4 position = vec4((aPosition * uSize) + uPositionOffset, 1.0);
    gl_Position = uMvpMatrix * position;
  }
);

const char* const FSH =
  SHADER_SOURCE(
  uniform vec3 uRendererColor;  // example

  out vec4 FragColor;

  void main()
  {
    FragColor = vec4(uRendererColor, 1.);
  }
);

//=============================================================================
Geometry CreateQuad()
{
  struct Vertex {
    Vector3 aPosition;
  };

  Vertex vertices[] = {
    { Vector3(-0.5f, -0.5f, 0.0f) },
    { Vector3(0.5f, -0.5f, 0.0f) },
    { Vector3(-0.5f, 0.5f, 0.0f) },
    { Vector3(0.5f, 0.5f, 0.0f) }
  };

  PropertyBuffer vertexBuffer = PropertyBuffer::New(
    Property::Map().Add("aPosition", Property::VECTOR3)
  );

  vertexBuffer.SetData(vertices, std::extent<decltype(vertices)>::value);

  Geometry quad = Geometry::New();
  quad.AddVertexBuffer(vertexBuffer);
  quad.SetType(Geometry::TRIANGLE_STRIP);
  return quad;
}

}

//=============================================================================
class RendererFilteringExample: public ConnectionTracker
{
public:
  RendererFilteringExample(Application &application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    application.InitSignal().Connect(this, &RendererFilteringExample::Init);
    application.TerminateSignal().Connect(this, &RendererFilteringExample::Terminate);
  }

private:
  void Init(Application &application)
  {
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor(Color::WHITE);

    Geometry quad = CreateQuad();
    Shader shader = Shader::New(VSH, FSH);

    Vector2 viewSize = stage.GetSize() * Vector2(.5f, .25f);

    float lc = .3f;
    float hc = .9f;

    Renderer renderer1 = Renderer::New(quad, shader);
    renderer1.RegisterProperty("uPositionOffset", Vector3(viewSize * -.125f));
    renderer1.RegisterProperty("uRendererColor", Vector3(hc, lc, lc));

    DevelRenderer::SetFilteringMask(renderer1, 0x1);

    Renderer renderer2 = Renderer::New(quad, shader);
    renderer2.RegisterProperty("uPositionOffset", Vector3(viewSize * 0.f));
    renderer2.RegisterProperty("uRendererColor", Vector3(hc * .75f, lc, lc));

    DevelRenderer::SetFilteringMask(renderer2, 0x2);

    Renderer renderer3 = Renderer::New(quad, shader);
    renderer3.RegisterProperty("uPositionOffset", Vector3(viewSize * .125f));
    renderer3.RegisterProperty("uRendererColor", Vector3(hc * .5f, lc, lc));

    DevelRenderer::SetFilteringMask(renderer3, 0x4);

    Actor actor = Actor::New();
    actor.SetParentOrigin(ParentOrigin::CENTER);
    actor.SetAnchorPoint(AnchorPoint::CENTER);
    actor.SetSize(Vector2::ONE * viewSize.x * .6f);
    actor.AddRenderer(renderer1);
    actor.AddRenderer(renderer2);
    actor.AddRenderer(renderer3);
    stage.Add(actor);

    mActors.push_back(actor);

    Actor labels = Actor::New();
    labels.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    labels.SetParentOrigin(ParentOrigin::TOP_LEFT);
    labels.SetSize(stage.GetSize());
    stage.Add(labels);

    mActors.push_back(labels);

    CameraActor cam = CameraActor::New(viewSize);
    cam.SetParentOrigin(ParentOrigin::CENTER);
    cam.SetAnchorPoint(AnchorPoint::CENTER);
    stage.Add(cam);

    mActors.push_back(cam);

    auto& vs = viewSize;
    mRenderTasks = {
      CreateRenderTask(stage, cam, actor, vs, Vector2(vs.x * 0.f, vs.y * 0.f), Vector3(lc, lc, Lerp(.25f, lc, hc)), 0x0),
      CreateRenderTask(stage, cam, actor, vs, Vector2(vs.x * 0.f, vs.y * 1.f), Vector3(lc, lc, Lerp(.5f,  lc, hc)), 0x1),
      CreateRenderTask(stage, cam, actor, vs, Vector2(vs.x * 0.f, vs.y * 2.f), Vector3(lc, lc, Lerp(.75f, lc, hc)), 0x2),
      CreateRenderTask(stage, cam, actor, vs, Vector2(vs.x * 0.f, vs.y * 3.f), Vector3(lc, lc, Lerp(1.f,  lc, hc)), 0x3),
    
      CreateRenderTask(stage, cam, actor, vs, Vector2(vs.x * 1.f, vs.y * 0.f), Vector3(lc, hc, Lerp(.25f, lc, hc)), 0x4),
      CreateRenderTask(stage, cam, actor, vs, Vector2(vs.x * 1.f, vs.y * 1.f), Vector3(lc, hc, Lerp(.5f,  lc, hc)), 0x5),
      CreateRenderTask(stage, cam, actor, vs, Vector2(vs.x * 1.f, vs.y * 2.f), Vector3(lc, hc, Lerp(.75f, lc, hc)), 0x6),
      CreateRenderTask(stage, cam, actor, vs, Vector2(vs.x * 1.f, vs.y * 3.f), Vector3(lc, hc, Lerp(1.f,  lc, hc)), 0x7),
    };

    mRenderTasks.push_back(stage.GetRenderTaskList().CreateTask());
    mRenderTasks.back().SetSourceActor(labels);

    labels.Add(CreateTextLabel("Task 1", Vector3::ONE, Vector2(vs.x * 0.f, vs.y * 0.f)));
    labels.Add(CreateTextLabel("Task 2", Vector3::ONE, Vector2(vs.x * 0.f, vs.y * 1.f)));
    labels.Add(CreateTextLabel("Task 3", Vector3::ONE, Vector2(vs.x * 0.f, vs.y * 2.f)));
    labels.Add(CreateTextLabel("Task 4", Vector3::ONE, Vector2(vs.x * 0.f, vs.y * 3.f)));

    labels.Add(CreateTextLabel("Task 5", Vector3::ONE, Vector2(vs.x * 1.f, vs.y * 0.f)));
    labels.Add(CreateTextLabel("Task 6", Vector3::ONE, Vector2(vs.x * 1.f, vs.y * 1.f)));
    labels.Add(CreateTextLabel("Task 7", Vector3::ONE, Vector2(vs.x * 1.f, vs.y * 2.f)));
    labels.Add(CreateTextLabel("Task 8", Vector3::ONE, Vector2(vs.x * 1.f, vs.y * 3.f)));

    stage.GetRootLayer().TouchSignal().Connect(this, &RendererFilteringExample::OnTouch);
    stage.KeyEventSignal().Connect(this, &RendererFilteringExample::OnKeyEvent);
  }

  void Terminate(Application& app)
  {
    auto stage = Stage::GetCurrent();
    auto rtl = stage.GetRenderTaskList();
    for (auto& rt : mRenderTasks)
    {
      rtl.RemoveTask(rt);
    }

    for (auto& a : mActors)
    {
      UnparentAndReset(a);
    }

    mRenderTasks.clear();
    mActors.clear();
  }

  RenderTask CreateRenderTask(Stage& stage, CameraActor camera, Actor sourceActor, Vector2 viewportSize, Vector2 viewportPosition, Vector3 color, uint32_t mask)
  {
    RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
    RenderTask task = taskList.CreateTask();
	task.SetCameraActor(camera);
    task.SetSourceActor(sourceActor);
    task.SetViewportSize(viewportSize);
    task.SetViewportPosition(viewportPosition);
    task.SetClearColor(Vector4(color.r, color.g, color.b, 1.f));
    task.SetClearEnabled(true);
    DevelRenderTask::SetRendererFilteringMask(task, mask);

    return task;
  }

  TextLabel CreateTextLabel(const char* text, Vector3 color, Vector2 position)
  {
    TextLabel label = TextLabel::New(text);
    label.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    label.SetParentOrigin(ParentOrigin::TOP_LEFT);
    label.SetProperty(TextLabel::Property::TEXT_COLOR, Vector4(color.r, color.g, color.b, 1.f));
    label.SetPosition(position.x, position.y);
    return label;
  }

  bool OnTouch(Actor actor, const TouchData &touch)
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  void OnKeyEvent(const KeyEvent &event)
  {
    if (event.state == KeyEvent::Down)
    {
      if (IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application &mApplication;

  std::vector<RenderTask> mRenderTasks;
  std::vector<Actor> mActors;
};

int DALI_EXPORT_API main(int argc, char **argv)
{
  Application application = Application::New(&argc, &argv, DEMO_THEME_PATH);
  RendererFilteringExample test(application);
  application.MainLoop();
  return 0;
}
