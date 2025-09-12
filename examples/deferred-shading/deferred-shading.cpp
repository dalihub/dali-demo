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
#include <cstring>
#include <iostream>
#include <random>
#include "dali/dali.h"
#include "dali/public-api/actors/actor.h"
#include "dali/public-api/rendering/renderer.h"

#include "generated/deferred-shading-mainpass-frag.h"
#include "generated/deferred-shading-mainpass-vert.h"
#include "generated/deferred-shading-prepass-frag.h"
#include "generated/deferred-shading-prepass-vert.h"

using namespace Dali;

namespace
{
//=============================================================================
// Demonstrates deferred shading with multiple render targets (for color,
// position, and normal), a Phong lighting model and 32 point lights.
//
// Invoked with the --show-lights it will render a mesh at each light position.
//=============================================================================

#define QUOTE(x) DALI_COMPOSE_SHADER(x)

#define MAX_LIGHTS 32

#define DEFINE_MAX_LIGHTS "const int kMaxLights = " QUOTE(MAX_LIGHTS) ";"

//=============================================================================
// PRNG for floats.
struct FloatRand
{
  std::random_device                    mDevice;
  std::mt19937                          mMersenneTwister;
  std::uniform_real_distribution<float> mDistribution;

  FloatRand()
  : mMersenneTwister(mDevice()),
    mDistribution(0., 1.)
  {
  }

  float operator()()
  {
    return mDistribution(mMersenneTwister);
  }
};

//=============================================================================
float FastFloor(float x)
{
  return static_cast<int>(x) - static_cast<int>(x < 0);
}

//=============================================================================
Vector3 FromHueSaturationLightness(Vector3 hsl)
{
  Vector3 rgb;
  if(hsl.y * hsl.y > 0.f)
  {
    if(hsl.x >= 360.f)
    {
      hsl.x -= 360.f;
    }
    hsl.x /= 60.f;

    int   i  = FastFloor(hsl.x);
    float ff = hsl.x - i;
    float p  = hsl.z * (1.0 - hsl.y);
    float q  = hsl.z * (1.0 - (hsl.y * ff));
    float t  = hsl.z * (1.0 - (hsl.y * (1.f - ff)));

    switch(i)
    {
      case 0:
        rgb.r = hsl.z;
        rgb.g = t;
        rgb.b = p;
        break;

      case 1:
        rgb.r = q;
        rgb.g = hsl.z;
        rgb.b = p;
        break;

      case 2:
        rgb.r = p;
        rgb.g = hsl.z;
        rgb.b = t;
        break;

      case 3:
        rgb.r = p;
        rgb.g = q;
        rgb.b = hsl.z;
        break;

      case 4:
        rgb.r = t;
        rgb.g = p;
        rgb.b = hsl.z;
        break;

      case 5:
      default:
        rgb.r = hsl.z;
        rgb.g = p;
        rgb.b = q;
        break;
    }
  }
  else
  {
    rgb = Vector3::ONE * hsl.z;
  }

  return rgb;
}

//=============================================================================
Geometry CreateTexturedQuadGeometry(bool flipV)
{
  // Create geometry -- unit square with whole of the texture mapped to it.
  struct Vertex
  {
    Vector3 aPosition;
    Vector2 aTexCoord;
  };

  Vertex vertexData[] = {
    {Vector3(-.5f, .5f, .0f), Vector2(.0f, 1.0f)},
    {Vector3(.5f, .5f, .0f), Vector2(1.0f, 1.0f)},
    {Vector3(-.5f, -.5f, .0f), Vector2(.0f, .0f)},
    {Vector3(.5f, -.5f, .0f), Vector2(1.0f, .0f)},
  };

  if(flipV)
  {
    std::swap(vertexData[0].aTexCoord, vertexData[2].aTexCoord);
    std::swap(vertexData[1].aTexCoord, vertexData[3].aTexCoord);
  }

  VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map()
                                                  .Add("aPosition", Property::VECTOR3)
                                                  .Add("aTexCoord", Property::VECTOR2));
  vertexBuffer.SetData(vertexData, std::extent<decltype(vertexData)>::value);

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  geometry.SetType(Geometry::TRIANGLE_STRIP);
  return geometry;
}

//=============================================================================
Geometry CreateOctahedron(bool invertNormals)
{
  Vector3 positions[] = {
    Vector3{-1.f, 0.f, 0.f},
    Vector3{1.f, 0.f, 0.f},
    Vector3{0.f, -1.f, 0.f},
    Vector3{0.f, 1.f, 0.f},
    Vector3{0.f, 0.f, -1.f},
    Vector3{0.f, 0.f, 1.f},
  };

  struct Vertex
  {
    Vector3 position{};
    Vector3 normal{};
  };
  Vertex vertexData[] = {
    {positions[0]},
    {positions[3]},
    {positions[5]},

    {positions[5]},
    {positions[3]},
    {positions[1]},

    {positions[1]},
    {positions[3]},
    {positions[4]},

    {positions[4]},
    {positions[3]},
    {positions[0]},

    {positions[0]},
    {positions[5]},
    {positions[2]},

    {positions[5]},
    {positions[1]},
    {positions[2]},

    {positions[1]},
    {positions[4]},
    {positions[2]},

    {positions[4]},
    {positions[0]},
    {positions[2]},
  };

  // Calculate normals
  for(uint32_t i = 0; i < std::extent<decltype(vertexData)>::value / 3; ++i)
  {
    uint32_t idx = i * 3;

    Vector3 normal = (vertexData[idx + 2].position - vertexData[idx].position).Cross(vertexData[idx + 1].position - vertexData[idx].position);
    normal.Normalize();
    normal *= invertNormals * 2.f - 1.f;

    vertexData[idx++].normal = normal;
    vertexData[idx++].normal = normal;
    vertexData[idx].normal   = normal;
  }

  // Configure property buffers and create geometry.
  VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map()
                                                  .Add("aPosition", Property::VECTOR3)
                                                  .Add("aNormal", Property::VECTOR3));
  vertexBuffer.SetData(vertexData, std::extent<decltype(vertexData)>::value);

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  geometry.SetType(Geometry::TRIANGLES);
  return geometry;
}

//=============================================================================
enum RendererOptions
{
  OPTION_NONE        = 0x0,
  OPTION_BLEND       = 0x01,
  OPTION_DEPTH_TEST  = 0x02,
  OPTION_DEPTH_WRITE = 0x04
};

Renderer CreateRenderer(TextureSet textures, Geometry geometry, Shader shader, uint32_t options = OPTION_NONE)
{
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetProperty(Renderer::Property::BLEND_MODE,
                       (options & OPTION_BLEND) ? BlendMode::ON : BlendMode::OFF);
  renderer.SetProperty(Renderer::Property::DEPTH_TEST_MODE,
                       (options & OPTION_DEPTH_TEST) ? DepthTestMode::ON : DepthTestMode::OFF);
  renderer.SetProperty(Renderer::Property::DEPTH_WRITE_MODE,
                       (options & OPTION_DEPTH_WRITE) ? DepthWriteMode::ON : DepthWriteMode::OFF);
  renderer.SetProperty(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK);

  if(!textures)
  {
    textures = TextureSet::New();
  }

  renderer.SetTextures(textures);
  return renderer;
}

//=============================================================================
void CenterActor(Actor actor)
{
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
}

//=============================================================================
void RegisterDepthProperties(float depth, float near, Handle& h)
{
  h.RegisterProperty("uDepth_InvDepth_Near", Vector3(depth, 1.f / depth, near));
}

} // namespace

//=============================================================================
/// Create a String whose size can be evaluated at compile time
struct ConstantString
{
  const char* const string;
  const uint16_t    size;

  template<uint16_t inputSize>
  constexpr ConstantString(const char (&input)[inputSize])
  : string(input),
    size(inputSize)
  {
  }
};

constexpr ConstantString POSITION_STRING("position");
constexpr ConstantString RADIUS_STRING("radius");
constexpr ConstantString COLOR_STRING("color");
constexpr uint16_t       LIGHT_SOURCE_BUFFER_SIZE(128u);

//=============================================================================
class DeferredShadingExample : public ConnectionTracker
{
public:
  struct Options
  {
    enum
    {
      NONE        = 0x0,
      SHOW_LIGHTS = 0x1,
    };
  };

  DeferredShadingExample(Application& app, uint32_t options = Options::NONE)
  : mApp(app),
    mOptions(options)
  {
    app.InitSignal().Connect(this, &DeferredShadingExample::Create);
    app.TerminateSignal().Connect(this, &DeferredShadingExample::Destroy);
  }

private:
  void Create(Application& app)
  {
    // Grab window, configure layer
    Window window    = app.GetWindow();
    auto   rootLayer = window.GetRootLayer();
    rootLayer.SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_3D);

    Vector2 windowSize = window.GetSize();

    float unit = windowSize.y / 24.f;

    // Get camera - we'll be re-using the same old camera in the two passes.
    RenderTaskList tasks  = window.GetRenderTaskList();
    CameraActor    camera = tasks.GetTask(0).GetCameraActor();

    auto zCameraPos = camera.GetProperty(Actor::Property::POSITION_Z).Get<float>();
    camera.SetFarClippingPlane(zCameraPos + windowSize.y * .5f);
    camera.SetNearClippingPlane(zCameraPos - windowSize.y * .5f);

    const float zNear = camera.GetNearClippingPlane();
    const float zFar  = camera.GetFarClippingPlane();
    const float depth = zFar - zNear;

    // Create root of scene that shall be rendered off-screen.
    auto sceneRoot = Actor::New();
    CenterActor(sceneRoot);

    mSceneRoot = sceneRoot;
    window.Add(sceneRoot);

    // Create an axis to spin our actors around.
    auto axis = Actor::New();
    CenterActor(axis);
    sceneRoot.Add(axis);
    mAxis = axis;

    // Create an octahedral mesh for our main actors and to visualise the light sources.
    Geometry mesh = CreateOctahedron(false);

    // Create main actors
    Shader     preShader        = Shader::New(SHADER_DEFERRED_SHADING_PREPASS_VERT, SHADER_DEFERRED_SHADING_PREPASS_FRAG);
    TextureSet noTexturesThanks = TextureSet::New();
    Renderer   meshRenderer     = CreateRenderer(noTexturesThanks, mesh, preShader, OPTION_DEPTH_TEST | OPTION_DEPTH_WRITE);
    meshRenderer.SetProperty(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK);
    RegisterDepthProperties(depth, zNear, meshRenderer);
    float c = 1.f;
    for(auto v : {
          Vector3{-c, -c, -c},
          Vector3{c, -c, -c},
          Vector3{-c, c, -c},
          Vector3{c, c, -c},
          Vector3{-c, -c, c},
          Vector3{c, -c, c},
          Vector3{-c, c, c},
          Vector3{c, c, c},

          Vector3{0.f, -c, -c},
          Vector3{0.f, c, -c},
          Vector3{0.f, -c, c},
          Vector3{0.f, c, c},

          Vector3{-c, 0.f, -c},
          Vector3{c, 0.f, -c},
          Vector3{-c, 0.f, c},
          Vector3{c, 0.f, c},

          Vector3{-c, -c, 0.f},
          Vector3{c, -c, 0.f},
          Vector3{-c, c, 0.f},
          Vector3{c, c, 0.f},
        })
    {
      Actor a = Actor::New();
      CenterActor(a);

      Vector3 position{v * unit * 5.f};
      a.SetProperty(Actor::Property::POSITION, position);

      float   scale = (c + ((v.x + v.y + v.z) + c * 3.f) * .5f) / (c * 4.f);
      Vector3 size{Vector3::ONE * scale * unit * 2.f};
      a.SetProperty(Actor::Property::SIZE, size);

      a.SetProperty(Actor::Property::COLOR, Color::WHITE * .25f + (Color::RED * (v.x + c) / (c * 2.f) + Color::GREEN * (v.y + c) / (c * 2.f) + Color::BLUE * (v.z + c) / (c * 2.f)) * .015625f);
      a.AddRenderer(meshRenderer);

      axis.Add(a);
    }

    // Create off-screen textures, fbo and render task.
    uint32_t width  = static_cast<uint32_t>(windowSize.x);
    uint32_t height = static_cast<uint32_t>(windowSize.y);

    Texture     rttNormal   = Texture::New(TextureType::TEXTURE_2D, Pixel::Format::RGB888, width, height);
    Texture     rttPosition = Texture::New(TextureType::TEXTURE_2D, Pixel::Format::RGBA8888, width, height);
    Texture     rttColor    = Texture::New(TextureType::TEXTURE_2D, Pixel::Format::RGB888, width, height);
    FrameBuffer fbo         = FrameBuffer::New(width, height, FrameBuffer::Attachment::DEPTH);
    fbo.AttachColorTexture(rttNormal);
    fbo.AttachColorTexture(rttPosition);
    fbo.AttachColorTexture(rttColor);

    RenderTask sceneRender = tasks.CreateTask();
    sceneRender.SetViewportSize(windowSize);
    sceneRender.SetFrameBuffer(fbo);
    sceneRender.SetCameraActor(camera);
    sceneRender.SetSourceActor(sceneRoot);
    sceneRender.SetInputEnabled(false);
    sceneRender.SetCullMode(false);
    sceneRender.SetClearEnabled(true);
    sceneRender.SetClearColor(Color::BLACK);
    sceneRender.SetExclusive(true);

    mSceneRender = sceneRender;

    // Create final image for deferred shading
    auto finalImage = Actor::New();
    CenterActor(finalImage);
    finalImage.SetProperty(Actor::Property::SIZE, windowSize);

    TextureSet finalImageTextures = TextureSet::New();
    finalImageTextures.SetTexture(0, rttNormal);
    finalImageTextures.SetTexture(1, rttPosition);
    finalImageTextures.SetTexture(2, rttColor);

    Sampler sampler = Sampler::New();
    sampler.SetFilterMode(FilterMode::NEAREST, FilterMode::NEAREST);
    finalImageTextures.SetSampler(0, sampler);
    finalImageTextures.SetSampler(1, sampler);
    finalImageTextures.SetSampler(2, sampler);

    Shader   shdMain            = Shader::New(SHADER_DEFERRED_SHADING_MAINPASS_VERT, SHADER_DEFERRED_SHADING_MAINPASS_FRAG);
    Geometry finalImageGeom     = CreateTexturedQuadGeometry(true);
    Renderer finalImageRenderer = CreateRenderer(finalImageTextures, finalImageGeom, shdMain);
    RegisterDepthProperties(depth, zNear, finalImageRenderer);

    auto       propInvProjection  = finalImageRenderer.RegisterProperty("uInvProjection", Matrix::IDENTITY);
    Constraint cnstrInvProjection = Constraint::New<Matrix>(finalImageRenderer, propInvProjection, [zCameraPos, zNear, depth](Matrix& output, const PropertyInputContainer& input)
    {
      output = input[0]->GetMatrix();
      DALI_ASSERT_ALWAYS(output.Invert() && "Failed to invert projection matrix.");
    });
    cnstrInvProjection.AddSource(Source(camera, CameraActor::Property::PROJECTION_MATRIX));
    cnstrInvProjection.AddSource(Source(camera, CameraActor::Property::VIEW_MATRIX));
    cnstrInvProjection.Apply();

    finalImage.AddRenderer(finalImageRenderer);

    mFinalImage = finalImage;
    window.Add(finalImage);

    // Create a node for our lights
    auto lights = Actor::New();
    CenterActor(lights);
    sceneRoot.Add(lights);

    // Create Lights
    const bool showLights = mOptions & Options::SHOW_LIGHTS;
    Renderer   lightRenderer;
    if(showLights)
    {
      Geometry lightMesh = CreateOctahedron(true);
      lightRenderer      = CreateRenderer(noTexturesThanks, lightMesh, preShader, OPTION_DEPTH_TEST | OPTION_DEPTH_WRITE);
      lightRenderer.SetProperty(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::FRONT);
    }

    Vector3 lightPos{unit * 12.f, 0.f, 0.f};
    float   theta    = M_PI * 2.f / MAX_LIGHTS;
    float   cosTheta = std::cos(theta);
    float   sinTheta = std::sin(theta);
    for(int i = 0; i < MAX_LIGHTS; ++i)
    {
      Vector3 color = FromHueSaturationLightness(Vector3((360.f * i) / MAX_LIGHTS, .5f, 1.f));

      Actor light = CreateLight(lightPos * (1 + (i % 8)) / 8.f, unit * 16.f, color, camera, finalImageRenderer);

      float z  = (((i & 1) << 1) - 1) * unit * 8.f;
      lightPos = Vector3(cosTheta * lightPos.x - sinTheta * lightPos.y, sinTheta * lightPos.x + cosTheta * lightPos.y, z);

      if(showLights)
      {
        light.SetProperty(Actor::Property::SIZE, Vector3::ONE * unit / 8.f);
        light.AddRenderer(lightRenderer);
      }

      lights.Add(light);
    }

    // Take them for a spin.
    Animation animLights = Animation::New(40.f);
    animLights.SetLooping(true);
    animLights.AnimateBy(Property(lights, Actor::Property::ORIENTATION), Quaternion(Radian(M_PI * 2.f), Vector3::YAXIS));
    animLights.Play();

    // Event handling
    window.KeyEventSignal().Connect(this, &DeferredShadingExample::OnKeyEvent);

    mPanDetector = PanGestureDetector::New();
    mPanDetector.DetectedSignal().Connect(this, &DeferredShadingExample::OnPan);
    mPanDetector.Attach(window.GetRootLayer());
  }

  void Destroy(Application& app)
  {
    app.GetWindow().GetRenderTaskList().RemoveTask(mSceneRender);
    mSceneRender.Reset();

    UnparentAndReset(mSceneRoot);
    UnparentAndReset(mFinalImage);
  }

  Actor CreateLight(Vector3 position, float radius, Vector3 color, CameraActor camera, Renderer renderer)
  {
    Actor light = Actor::New();
    CenterActor(light);
    light.SetProperty(Actor::Property::COLOR, Color::WHITE);
    light.SetProperty(Actor::Property::POSITION, position);

    auto iPropRadius     = light.RegisterProperty("radius", radius);
    auto iPropLightColor = light.RegisterProperty("lightcolor", color);

    // Create light source uniforms on lighting shader.
    char  buffer[LIGHT_SOURCE_BUFFER_SIZE];
    char* writep = buffer + snprintf(buffer, LIGHT_SOURCE_BUFFER_SIZE, "uLights[%d].", mNumLights);
    ++mNumLights;

    strncpy(writep, POSITION_STRING.string, POSITION_STRING.size);
    auto oPropLightPos = renderer.RegisterProperty(buffer, position);

    strncpy(writep, RADIUS_STRING.string, RADIUS_STRING.size);
    auto oPropLightRadius = renderer.RegisterProperty(buffer, radius);

    strncpy(writep, COLOR_STRING.string, COLOR_STRING.size);
    auto oPropLightColor = renderer.RegisterProperty(buffer, color);

    // Constrain the light position, radius and color to lighting shader uniforms.
    // Convert light position to view space;
    Constraint cLightPos = Constraint::New<Vector3>(renderer, oPropLightPos, [](Vector3& output, const PropertyInputContainer& input)
    {
      Vector4 worldPos(input[0]->GetVector3());
      worldPos.w = 1.f;

      worldPos = input[1]->GetMatrix() * worldPos;
      output   = Vector3(worldPos);
    });
    cLightPos.AddSource(Source(light, Actor::Property::WORLD_POSITION));
    cLightPos.AddSource(Source(camera, CameraActor::Property::VIEW_MATRIX));
    cLightPos.Apply();

    Constraint cLightRadius = Constraint::New<float>(renderer, oPropLightRadius, EqualToConstraint());
    cLightRadius.AddSource(Source(light, iPropRadius));
    cLightRadius.Apply();

    Constraint cLightColor = Constraint::New<Vector3>(renderer, oPropLightColor, EqualToConstraint());
    cLightColor.AddSource(Source(light, iPropLightColor));
    cLightColor.Apply();

    return light;
  }

  void OnPan(Actor, PanGesture const& gesture)
  {
    Quaternion     q            = mAxis.GetProperty(Actor::Property::ORIENTATION).Get<Quaternion>();
    const Vector2& displacement = gesture.GetScreenDisplacement();
    Quaternion     qx(Radian(Degree(displacement.y) * -.5f), Vector3::XAXIS);
    Quaternion     qy(Radian(Degree(displacement.x) * .5f), Vector3::YAXIS);
    mAxis.SetProperty(Actor::Property::ORIENTATION, qy * qx * q);
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApp.Quit();
      }
    }
  }

  Application& mApp;
  uint32_t     mOptions;

  Actor mSceneRoot;
  Actor mAxis;

  RenderTask mSceneRender;
  Actor      mFinalImage;

  int mNumLights = 0;

  PanGestureDetector mPanDetector;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  const bool showLights = [argc, argv]()
  {
    auto endArgs = argv + argc;
    return std::find_if(argv, endArgs, [](const char* arg)
    {
      return strcmp(arg, "--show-lights") == 0;
    }) != endArgs;
  }();

  Application            app = Application::New(&argc, &argv);
  DeferredShadingExample example(app, (showLights ? DeferredShadingExample::Options::SHOW_LIGHTS : 0));
  app.MainLoop();
  return 0;
}
