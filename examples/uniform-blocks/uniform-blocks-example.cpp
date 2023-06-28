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

#include "generated/uniform-block-vert.h"
#include "generated/uniform-block-frag.h"
#include "generated/uniform-block-alt-frag.h"

using namespace Dali;
using Dali::Toolkit::TextLabel;



/**
 * This application tests that shaders with uniform blocks work as expected.
 */
class UniformBlocksController : public ConnectionTracker
{
public:
  UniformBlocksController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &UniformBlocksController::Create);
  }

  ~UniformBlocksController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    CreateShader(0);
    CreateGeometry();
    CreateRenderer();
    mFirstActor = window.GetRootLayer().GetChildCount();

    for(int i=0; i<200; ++i)
    {
      AddActor(i);
    }

    // Respond to a touch anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &UniformBlocksController::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &UniformBlocksController::OnKeyEvent);
    mTimer = Timer::New(100);
    mTimer.TickSignal().Connect(this, &UniformBlocksController::OnTick);
    mTimer.Start();
  }

  bool OnTick()
  {
    static int index=200;
    Window window = mApplication.GetWindow();
    Layer layer = window.GetRootLayer();
    Actor child = layer.GetChildAt(mFirstActor);
    UnparentAndReset(child);
    AddActor(index);
    index = (index+1)%1024;
    return true;
  }

  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    static int testNumber=0;
    if(touch.GetState(0) == PointState::STARTED)
    {
      testNumber++;
      if(testNumber >=2)
        mApplication.Quit();

      CreateShader(testNumber);
      mRenderer = Renderer::New(mGeometry, mShader);

      Actor parent = mApplication.GetWindow().GetRootLayer();
      for(uint32_t i=0; i<parent.GetChildCount(); ++i)
      {
        parent.GetChildAt(i).RemoveRenderer(0);
        parent.GetChildAt(i).AddRenderer(mRenderer);
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
        mApplication.Quit();
      }
    }
  }

  void AddActor(int n)
  {
    Actor actor = Actor::New();
    actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    actor.SetProperty(Actor::Property::POSITION, Vector3(Random::Range(-200.0f, 200.0f), Random::Range(-300.0f, 300.0f), 0.0f));;
    actor.SetProperty( Actor::Property::SIZE, Vector2(32, 32) );
    Vector4 color(Random::Range(0.1f,1.0f),Random::Range(0.1f,1.0f),Random::Range(0.1f,1.0f),1.0f);
    actor.SetProperty( Actor::Property::COLOR, color);

    actor.AddRenderer(mRenderer);

    actor.RegisterProperty("uColorIndex", n);
    char buffer[80];
    sprintf(buffer, "uColorArray[%d]", n);
    actor.RegisterProperty(buffer, color);

    Window window = mApplication.GetWindow();
    window.Add(actor);
  }

  void CreateShader(int testNumber)
  {


    // Create shaders
    switch(testNumber)
    {
      case 0:
      {
        mShader = Shader::New(Dali::Shader::GetVertexShaderPrefix() + std::string(SHADER_UNIFORM_BLOCK_VERT),
                              Dali::Shader::GetFragmentShaderPrefix() + std::string(SHADER_UNIFORM_BLOCK_FRAG));
        break;
      }
      case 1:
      {
        mShader = Shader::New(Dali::Shader::GetVertexShaderPrefix() + std::string(SHADER_UNIFORM_BLOCK_VERT),
                              Dali::Shader::GetFragmentShaderPrefix() + std::string(SHADER_UNIFORM_BLOCK_ALT_FRAG));
        break;
      }
    }
  }

  void CreateGeometry()
  {
    struct Vertex2D
    {
      Vertex2D(const Vector2& _co) :
        co(_co){}
      Dali::Vector2 co{};
    };
    const static Vector2 C(0.5f, 0.5f);
    struct Quad2D
    {
      Vertex2D a0{Vector2(0.0f, 0.0f)-C};
      Vertex2D a1{Vector2(1.0f, 0.0f)-C};
      Vertex2D a2{Vector2(1.0f, 1.0f)-C};
      Vertex2D a3{Vector2(0.0f, 0.0f)-C};
      Vertex2D a4{Vector2(1.0f, 1.0f)-C};
      Vertex2D a5{Vector2(0.0f, 1.0f)-C};
    } QUAD;

    // Create geometry
    Geometry geometry = Geometry::New();

    Property::Map attrMap{};
    attrMap.Add("aPosition", Property::Type::VECTOR2);
    VertexBuffer vb = VertexBuffer::New(attrMap);
    vb.SetData( &QUAD, 6 );

    geometry.SetType( Geometry::Type::TRIANGLES);
    geometry.AddVertexBuffer(vb);
    mGeometry = geometry;
  }

  void CreateRenderer()
  {
    mRenderer = Renderer::New( mGeometry, mShader);
  }

private:
  Application& mApplication;

  Shader mShader;
  Geometry mGeometry;
  Renderer mRenderer;
  uint32_t mFirstActor{0};
  Timer mTimer;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application          application = Application::New(&argc, &argv);
  UniformBlocksController test(application);
  application.MainLoop();
  return 0;
}
