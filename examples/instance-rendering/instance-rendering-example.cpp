/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <shared/utility.h>

#include <dali/devel-api/rendering/renderer-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
constexpr uint32_t INSTANCE_COUNT_MAX               = 4096; ///< Maximum number of array size for vec2.
constexpr uint32_t INSTANCE_COUNT_PER_DRAW          = 2048;
constexpr uint32_t FIRST_INSTANCE_CHANGE_PER_SECOND = 100;

constexpr uint32_t VIEW_SIZE = 32;

Geometry CreateVertexQuadGeometry()
{
  // Create geometry -- unit square with whole of the texture mapped to it.
  struct Vertex
  {
    Vector3 aPosition;
  };

  Vertex vertexData[] = {
    {Vector3(-.5f, .5f, .0f)},
    {Vector3(.5f, .5f, .0f)},
    {Vector3(-.5f, -.5f, .0f)},
    {Vector3(.5f, -.5f, .0f)},
  };

  VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map()
                                                  .Add("aPosition", Property::VECTOR3));
  vertexBuffer.SetData(vertexData, std::extent<decltype(vertexData)>::value);

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  geometry.SetType(Geometry::TRIANGLE_STRIP);
  return geometry;
}

Geometry Create16BitIndexQuadGeometry()
{
  // Create geometry with 16bit index -- unit square with whole of the texture mapped to it.
  struct Vertex
  {
    Vector3 aPosition;
  };

  Vertex vertexData[] = {
    {Vector3(-.5f, .5f, .0f)},
    {Vector3(.5f, .5f, .0f)},
    {Vector3(-.5f, -.5f, .0f)},
    {Vector3(.5f, -.5f, .0f)},
  };

  VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map()
                                                  .Add("aPosition", Property::VECTOR3));
  vertexBuffer.SetData(vertexData, std::extent<decltype(vertexData)>::value);

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);

  const uint16_t indices[] = {0, 3, 1, 0, 2, 3};
  geometry.SetIndexBuffer(&indices[0], sizeof(indices) / sizeof(indices[0]));
  geometry.SetType(Geometry::TRIANGLES);
  return geometry;
}

Geometry Create32BitIndexQuadGeometry()
{
  // Create geometry with 32bit index -- unit square with whole of the texture mapped to it.
  struct Vertex
  {
    Vector3 aPosition;
  };

  Vertex vertexData[] = {
    {Vector3(-.5f, .5f, .0f)},
    {Vector3(.5f, .5f, .0f)},
    {Vector3(-.5f, -.5f, .0f)},
    {Vector3(.5f, -.5f, .0f)},
  };

  VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map()
                                                  .Add("aPosition", Property::VECTOR3));
  vertexBuffer.SetData(vertexData, std::extent<decltype(vertexData)>::value);

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);

  const uint32_t indices[] = {0, 3, 1, 0, 2, 3};
  geometry.SetIndexBuffer(&indices[0], sizeof(indices) / sizeof(indices[0]));
  geometry.SetType(Geometry::TRIANGLES);
  return geometry;
}

enum TestType
{
  TEST_MULTIPLE_RENDERER,

  TEST_INSTANCE_RENDERING_WITH_VERTEX_GEOMETRY,
  TEST_INSTANCE_RENDERING_WITH_16BIT_INDEX_GEOMETRY,
  TEST_INSTANCE_RENDERING_WITH_32BIT_INDEX_GEOMETRY,

  TEST_MAX,
};
} // namespace

#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/rendering/uniform-block.h>

#include "generated/instance-rendering-frag.h"
#include "generated/instance-rendering-vert.h"
#include "generated/not-instance-rendering-vert.h"

using namespace Dali;
using Dali::Toolkit::TextLabel;

/**
 * This application tests that shaders with uniform blocks work as expected.
 */
class InstanceRenderingController : public ConnectionTracker
{
public:
  InstanceRenderingController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &InstanceRenderingController::Create);
  }

  ~InstanceRenderingController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    mUniformBlocks = UniformBlock::New("SharedVertexBlock");

    mActor = Actor::New();
    mActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mActor.SetProperty(Actor::Property::POSITION, Vector3::ZERO);
    mActor.SetProperty(Actor::Property::SIZE, Vector2(VIEW_SIZE, VIEW_SIZE));

    mFirstInstance      = 0u;
    mFirstInstanceIndex = mActor.RegisterProperty("uFirstInstance", static_cast<int32_t>(mFirstInstance));

    Window::WindowSize windowSize = window.GetSize();
    offsetXRange                  = static_cast<uint16_t>((windowSize.GetWidth() - VIEW_SIZE) / 2);
    offsetYRange                  = static_cast<uint16_t>((windowSize.GetHeight() - VIEW_SIZE) / 2);

    CreateShader(TestType::TEST_MULTIPLE_RENDERER);
    CreateGeometry(TestType::TEST_MULTIPLE_RENDERER);
    CreateRenderer(TestType::TEST_MULTIPLE_RENDERER);

    window.GetRootLayer().Add(mActor);

    for(uint32_t i = 0; i < INSTANCE_COUNT_MAX; ++i)
    {
      ChangeArrayValue(i);
    }

    mLabel = TextLabel::New();
    mLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
    mLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
    mLabel.SetBackgroundColor(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
    window.GetRootLayer().Add(mLabel);

    UpdateLabel(TestType::TEST_MULTIPLE_RENDERER);

    // Respond to a touch anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &InstanceRenderingController::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &InstanceRenderingController::OnKeyEvent);

    Animation loopingAnimation = Animation::New(1.0f * static_cast<float>(INSTANCE_COUNT_MAX) / static_cast<float>(FIRST_INSTANCE_CHANGE_PER_SECOND));
    loopingAnimation.AnimateTo(Dali::Property(mActor, mFirstInstanceIndex), static_cast<int>(INSTANCE_COUNT_MAX - 1u));
    loopingAnimation.SetLooping(true);
    loopingAnimation.Play();
  }

  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    static int testNumber = 0;
    if(touch.GetState(0) == PointState::STARTED)
    {
      testNumber++;
      if(testNumber >= TestType::TEST_MAX)
      {
        mApplication.Quit();
        return true;
      }
      Window             window     = mApplication.GetWindow();
      Window::WindowSize windowSize = window.GetSize();
      offsetXRange                  = static_cast<uint16_t>((windowSize.GetWidth() - VIEW_SIZE) / 2);
      offsetYRange                  = static_cast<uint16_t>((windowSize.GetHeight() - VIEW_SIZE) / 2);

      CreateShader(testNumber);
      CreateGeometry(testNumber);
      CreateRenderer(testNumber);

      UpdateLabel(testNumber);

      for(uint32_t i = 0; i < INSTANCE_COUNT_MAX; ++i)
      {
        ChangeArrayValue(i);
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

  void ChangeArrayValue(int n)
  {
    Vector4 color(Random::Range(0.1f, 1.0f), Random::Range(0.1f, 1.0f), Random::Range(0.1f, 1.0f), 1.0f);
    Vector2 offset(Random::Range(-static_cast<float>(offsetXRange), static_cast<float>(offsetXRange)), Random::Range(-static_cast<float>(offsetYRange), static_cast<float>(offsetYRange)));

    char buffer[80];
    sprintf(buffer, "uColorArray[%d]", n);
    mUniformBlocks.RegisterProperty(buffer, color);
    sprintf(buffer, "uOffsetArray[%d]", n);
    mUniformBlocks.RegisterProperty(buffer, offset);
  }

  void CreateShader(int testNumber)
  {
    if(mShader)
    {
      mUniformBlocks.DisconnectFromShader(mShader);
    }

    std::ostringstream oss;
    oss << "\n#define ARRAY_COUNT " << INSTANCE_COUNT_MAX << "\n\n";

    // Create shaders
    switch(testNumber)
    {
      case TestType::TEST_MULTIPLE_RENDERER:
      default:
      {
        mShader = Shader::New(Dali::Shader::GetVertexShaderPrefix() + oss.str() + std::string(SHADER_NOT_INSTANCE_RENDERING_VERT),
                              Dali::Shader::GetFragmentShaderPrefix() + std::string(SHADER_INSTANCE_RENDERING_FRAG));
        break;
      }
      case TestType::TEST_INSTANCE_RENDERING_WITH_VERTEX_GEOMETRY:
      case TestType::TEST_INSTANCE_RENDERING_WITH_16BIT_INDEX_GEOMETRY:
      case TestType::TEST_INSTANCE_RENDERING_WITH_32BIT_INDEX_GEOMETRY:
      {
        mShader = Shader::New(Dali::Shader::GetVertexShaderPrefix() + oss.str() + std::string(SHADER_INSTANCE_RENDERING_VERT),
                              Dali::Shader::GetFragmentShaderPrefix() + std::string(SHADER_INSTANCE_RENDERING_FRAG));
        break;
      }
    }
    // Create shaders

    mUniformBlocks.ConnectToShader(mShader);
  }

  void CreateGeometry(int testNumber)
  {
    // Create shaders
    switch(testNumber)
    {
      case TestType::TEST_MULTIPLE_RENDERER:
      case TestType::TEST_INSTANCE_RENDERING_WITH_VERTEX_GEOMETRY:
      default:
      {
        mGeometry = CreateVertexQuadGeometry();
        break;
      }
      case TestType::TEST_INSTANCE_RENDERING_WITH_16BIT_INDEX_GEOMETRY:
      {
        mGeometry = Create16BitIndexQuadGeometry();
        break;
      }
      case TestType::TEST_INSTANCE_RENDERING_WITH_32BIT_INDEX_GEOMETRY:
      {
        mGeometry = Create32BitIndexQuadGeometry();
        break;
      }
    }
  }

  void CreateRenderer(int testNumber)
  {
    for(uint32_t i = mActor.GetRendererCount(); i > 0u; --i)
    {
      mActor.RemoveRenderer(i - 1);
    }

    if(testNumber == TestType::TEST_MULTIPLE_RENDERER)
    {
      for(uint32_t i = 0; i < INSTANCE_COUNT_PER_DRAW; ++i)
      {
        mRenderer = Renderer::New(mGeometry, mShader);

        mRenderer.RegisterUniqueProperty("uInstanceIndex", static_cast<int32_t>(i));

        // Increase update area extents, for partial rendering.
        mRenderer[DevelRenderer::Property::UPDATE_AREA_EXTENTS] = Dali::Extents(offsetXRange, offsetXRange, offsetYRange, offsetYRange);

        mActor.AddRenderer(mRenderer);
      }
    }
    else
    {
      mRenderer = Renderer::New(mGeometry, mShader);

      mRenderer[DevelRenderer::Property::INSTANCE_COUNT] = static_cast<int32_t>(INSTANCE_COUNT_PER_DRAW);

      // Increase update area extents, for partial rendering.
      mRenderer[DevelRenderer::Property::UPDATE_AREA_EXTENTS] = Dali::Extents(offsetXRange, offsetXRange, offsetYRange, offsetYRange);

      mActor.AddRenderer(mRenderer);
    }
  }

  void UpdateLabel(int testNumber)
  {
    switch(testNumber)
    {
      case TestType::TEST_MULTIPLE_RENDERER:
      {
        mLabel.SetProperty(TextLabel::Property::TEXT, "Multiple renderer");
        break;
      }
      case TestType::TEST_INSTANCE_RENDERING_WITH_VERTEX_GEOMETRY:
      {
        mLabel.SetProperty(TextLabel::Property::TEXT, "Instance rendering + vertex only geometry");
        break;
      }
      case TestType::TEST_INSTANCE_RENDERING_WITH_16BIT_INDEX_GEOMETRY:
      {
        mLabel.SetProperty(TextLabel::Property::TEXT, "Instance rendering + 16bit indices geometry");
        break;
      }
      case TestType::TEST_INSTANCE_RENDERING_WITH_32BIT_INDEX_GEOMETRY:
      {
        mLabel.SetProperty(TextLabel::Property::TEXT, "Instance rendering + 32bit indices geometry");
        break;
      }
      default:
      {
        mLabel.SetProperty(TextLabel::Property::TEXT, "ERROR");
        break;
      }
    }
  }

private:
  Application& mApplication;

  Shader   mShader;
  Geometry mGeometry;
  Renderer mRenderer;

  uint16_t offsetXRange;
  uint16_t offsetYRange;

  UniformBlock mUniformBlocks;

  Actor           mActor;
  Property::Index mFirstInstanceIndex{Property::INVALID_INDEX};

  TextLabel mLabel;

  uint32_t mFirstInstance{0};
  Timer    mTimer;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  setenv("DALI_FPS_TRACKING", "5", 0);

  Application                 application = Application::New(&argc, &argv);
  InstanceRenderingController test(application);
  application.MainLoop();
  return 0;
}
