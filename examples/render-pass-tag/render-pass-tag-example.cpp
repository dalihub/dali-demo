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
#include <shared/utility.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

namespace
{

Geometry CreateQuadGeometry()
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

const std::string_view SHADER_COLOR_TEST_SHADER_VERT{
  R"(
//@version 100
INPUT mediump vec2 aPosition;
UNIFORM_BLOCK Vanilla
{
UNIFORM highp mat4 uMvpMatrix;
UNIFORM highp vec3 uSize;
};

void main()
{
  gl_Position = uMvpMatrix * vec4(uSize.xy * aPosition, 0.0, 1.0);
}
)"};

const std::string_view SHADER_COLOR_TEST_SHADER_FRAG{
  R"(
//@version 100
void main()
{
  gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
}
)"};

const std::string_view SHADER_IMAGE_TEST_SHADER_VERT{
  R"(
//@version 100
INPUT mediump vec2 aPosition;
OUTPUT mediump vec2 vTexCoord;

UNIFORM_BLOCK Vanilla
{
UNIFORM highp mat4 uMvpMatrix;
UNIFORM highp vec3 uSize;
};

void main()
{
  vTexCoord = vec2(0.5) + aPosition.xy;
  gl_Position = uMvpMatrix * vec4(uSize.xy * aPosition, 0.0, 1.0);
}
)"};

const std::string_view SHADER_IMAGE_TEST_SHADER_FRAG{
  R"(
//@version 100
INPUT mediump vec2 vTexCoord;

UNIFORM sampler2D sColorTexture;
UNIFORM sampler2D sTexture;

void main()
{
  mediump vec2 texCoord = vTexCoord;
  lowp vec4 textureColor = TEXTURE( sTexture, texCoord ) * TEXTURE( sColorTexture, texCoord );
  gl_FragColor = textureColor;
}
)"};

static constexpr std::string_view SAMPLE_IMAGE_PASS(DEMO_IMAGE_DIR "gallery-medium-19.jpg");
} // namespace

// This example shows how to create and display Hello World! using a simple TextActor
//
class RenderPassTagController : public ConnectionTracker
{
public:
  RenderPassTagController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &RenderPassTagController::Create);
  }

  ~RenderPassTagController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Color::WHITE);

    Dali::Toolkit::Control control = Dali::Toolkit::Control::New();
    control.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    control.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    control.SetProperty(Dali::Actor::Property::SIZE, Vector2(300.0f, 300.0f));
    mWindow.Add(control);

    Property::Map shader1;
    shader1.Add("renderPassTag", 1);
    shader1.Add("vertex", Dali::Shader::GetVertexShaderPrefix() + SHADER_COLOR_TEST_SHADER_VERT.data());
    shader1.Add("fragment", Dali::Shader::GetFragmentShaderPrefix() + SHADER_COLOR_TEST_SHADER_FRAG.data());

    Property::Map shader2;
    shader2.Add("renderPassTag", 0);
    shader2.Add("vertex", Dali::Shader::GetVertexShaderPrefix() + SHADER_IMAGE_TEST_SHADER_VERT.data());
    shader2.Add("fragment", Dali::Shader::GetFragmentShaderPrefix() + SHADER_IMAGE_TEST_SHADER_FRAG.data());

    Property::Array array;
    array.PushBack(shader1);
    array.PushBack(shader2);

    Dali::Shader shader   = Dali::Shader::New(array);
    Geometry     geometry = CreateQuadGeometry();
    Renderer renderer = Renderer::New(geometry, shader);
    control.AddRenderer(renderer);

    RenderTask renderTask = mWindow.GetRenderTaskList().CreateTask();
    renderTask.SetSourceActor(control);
    renderTask.SetExclusive(false);
    renderTask.SetInputEnabled(false);
    renderTask.SetCullMode(false);
    renderTask.SetClearEnabled(true);
    renderTask.SetClearColor(Color::TRANSPARENT);
    renderTask.SetRenderPassTag(1u);

    CameraActor cameraActor      = Dali::CameraActor::New(Vector2(300.0f, 300.0f));
    float   cameraDefaultZPosition = cameraActor.GetProperty<float>(Dali::Actor::Property::POSITION_Z);
    cameraActor.SetProperty(Dali::Actor::Property::POSITION, Vector3(0.0f, 0.0f, cameraDefaultZPosition));
    cameraActor.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    cameraActor.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    renderTask.SetCameraActor(cameraActor);
    mWindow.Add(cameraActor);

    Texture     colorTexture = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 300u, 300u);
    FrameBuffer frameBuffer  = FrameBuffer::New(300u, 300u, FrameBuffer::Attachment::NONE);
    frameBuffer.AttachColorTexture(colorTexture);

    renderTask.SetFrameBuffer(frameBuffer);

    TextureSet textures = TextureSet::New();
    textures.SetTexture(0u, colorTexture);

    Texture texture = DemoHelper::LoadTexture(SAMPLE_IMAGE_PASS.data());
    textures.SetTexture(1u, texture);

    renderer.SetTextures(textures);

    // Respond to a touch anywhere on the window
    mWindow.GetRootLayer().TouchedSignal().Connect(this, &RenderPassTagController::OnTouch);

    // Respond to key events
    mWindow.KeyEventSignal().Connect(this, &RenderPassTagController::OnKeyEvent);
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
  Application& mApplication;
  Window       mWindow;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application          application = Application::New(&argc, &argv);
  RenderPassTagController test(application);
  application.MainLoop();
  return 0;
}
