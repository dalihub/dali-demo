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
#include <dali/devel-api/actors/camera-actor-devel.h>
#include <dali/devel-api/adaptor-framework/file-stream.h>

#include <map>

#include "gltf-scene.h"
#include "generated/reflection-vert.h"
#include "generated/reflection-frag.h"
#include "generated/reflection-simple-frag.h"
#include "generated/reflection-textured-frag.h"
#include "generated/reflection-plasma-frag.h"
#include "generated/reflection-tex-frag.h"

using namespace Dali;

namespace
{

struct Model
{
  Shader   shader;
  Geometry geometry;
};
using ModelPtr = std::unique_ptr<Model>;

using ActorContainer      = std::vector<Actor>;
using CameraContainer     = std::vector<CameraActor>;
using ModelContainer      = std::vector<ModelPtr>;
using TextureSetContainer = std::vector<TextureSet>;

const Vector3 DEFAULT_LIGHT_DIRECTION(0.5, 0.5, -1);

template<class T>
bool LoadFile(const std::string& filename, std::vector<T>& bytes)
{
  Dali::FileStream fileStream(filename, Dali::FileStream::READ | Dali::FileStream::BINARY);
  FILE*            fin = fileStream.GetFile();

  if(fin)
  {
    if(fseek(fin, 0, SEEK_END))
    {
      return false;
    }
    bytes.resize(uint32_t(ftell(fin)));
    std::fill(bytes.begin(), bytes.end(), 0);
    if(fseek(fin, 0, SEEK_SET))
    {
      return false;
    }
    size_t result = fread(bytes.data(), 1, bytes.size(), fin);
    return (result != 0);
  }

  return false;
}

Shader CreateShader(const std::string& vsh, const std::string& fsh)
{
  std::vector<char> vshShaderSource;
  std::vector<char> fshShaderSource;

  // VSH
  if(vsh[0] == '/')
  {
    std::string vshPath(DEMO_GAME_DIR);
    vshPath += '/';
    vshPath += vsh;
    LoadFile(vshPath, vshShaderSource);
  }
  else
  {
    vshShaderSource.insert(vshShaderSource.end(), vsh.begin(), vsh.end());
  }

  // FSH
  if(fsh[0] == '/')
  {
    std::string fshPath(DEMO_GAME_DIR);
    fshPath += '/';
    fshPath += fsh;
    LoadFile(fshPath, fshShaderSource);
  }
  else
  {
    fshShaderSource.insert(fshShaderSource.end(), fsh.begin(), fsh.end());
  }

  vshShaderSource.emplace_back(0);
  fshShaderSource.emplace_back(0);
  return Shader::New(std::string(vshShaderSource.data()), std::string(fshShaderSource.data()));
}

ModelPtr CreateModel(
  glTF&              gltf,
  const glTF_Mesh*   mesh,
  const std::string& vertexShaderSource,
  const std::string& fragmentShaderSource)
{
  /*
   * Obtain interleaved buffer for first mesh with position and normal attributes
   */
  auto positionBuffer = gltf.GetMeshAttributeBuffer(*mesh,
                                                    {glTFAttributeType::POSITION,
                                                     glTFAttributeType::NORMAL,
                                                     glTFAttributeType::TEXCOORD_0});

  auto attributeCount = gltf.GetMeshAttributeCount(mesh);
  /**
   * Create matching property buffer
   */
  auto vertexBuffer = VertexBuffer::New(Property::Map()
                                          .Add("aPosition", Property::VECTOR3)
                                          .Add("aNormal", Property::VECTOR3)
                                          .Add("aTexCoord", Property::VECTOR2));

  // set vertex data
  vertexBuffer.SetData(positionBuffer.data(), attributeCount);

  auto geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  auto indexBuffer = gltf.GetMeshIndexBuffer(mesh);
  geometry.SetIndexBuffer(indexBuffer.data(), indexBuffer.size());
  geometry.SetType(Geometry::Type::TRIANGLES);
  ModelPtr retval(new Model());
  retval->shader   = CreateShader(vertexShaderSource, fragmentShaderSource);
  retval->geometry = geometry;
  return retval;
}

void ReplaceShader(Actor& actor, const std::string& vsh, const std::string& fsh)
{
  auto renderer = actor.GetRendererAt(0);
  auto shader   = CreateShader(vsh, fsh);
  renderer.SetShader(shader);
}

void CreateTextureSetsFromGLTF(glTF* gltf, const std::string& basePath, TextureSetContainer& textureSets)
{
  const auto& materials = gltf->GetMaterials();
  const auto& textures  = gltf->GetTextures();

  std::map<std::string, Texture> textureCache{};

  for(const auto& material : materials)
  {
    TextureSet textureSet;
    if(material.pbrMetallicRoughness.enabled)
    {
      textureSet = TextureSet::New();
      std::string filename(basePath);
      filename += '/';
      filename += textures[material.pbrMetallicRoughness.baseTextureColor.index].uri;
      Dali::PixelData pixelData = Dali::Toolkit::SyncImageLoader::Load(filename);

      auto    cacheKey = textures[material.pbrMetallicRoughness.baseTextureColor.index].uri;
      auto    iter     = textureCache.find(cacheKey);
      Texture texture;
      if(iter == textureCache.end())
      {
        texture = Texture::New(TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight());
        texture.Upload(pixelData);
        texture.GenerateMipmaps();
        textureCache[cacheKey] = texture;
      }
      else
      {
        texture = iter->second;
      }
      textureSet.SetTexture(0, texture);
      Dali::Sampler sampler = Dali::Sampler::New();
      sampler.SetWrapMode(Dali::WrapMode::REPEAT, Dali::WrapMode::REPEAT, Dali::WrapMode::REPEAT);
      sampler.SetFilterMode(Dali::FilterMode::LINEAR_MIPMAP_LINEAR, Dali::FilterMode::LINEAR);
      textureSet.SetSampler(0, sampler);
    }
    textureSets.emplace_back(textureSet);
  }
}

/**
 * Creates models from glTF
 */
void CreateModelsFromGLTF(glTF* gltf, ModelContainer& models)
{
  const auto& meshes = gltf->GetMeshes();
  for(const auto& mesh : meshes)
  {
    // change shader to use texture if material indicates that
    if(mesh->material != 0xffffffff && gltf->GetMaterials()[mesh->material].pbrMetallicRoughness.enabled)
    {
      models.emplace_back(CreateModel(*gltf, mesh, SHADER_REFLECTION_VERT.data(), SHADER_REFLECTION_TEXTURED_FRAG.data()));
    }
    else
    {
      models.emplace_back(CreateModel(*gltf, mesh, SHADER_REFLECTION_VERT.data(), SHADER_REFLECTION_FRAG.data()));
    }
  }
}

Actor CreateSceneFromGLTF(
  Window               window,
  glTF*                gltf,
  ModelContainer&      models,
  ActorContainer&      actors,
  CameraContainer&     cameras,
  TextureSetContainer& textureSets)
{
  const auto& nodes = gltf->GetNodes();

  Vector3 cameraPosition;

  // for each node create nodes and children
  // resolve parents later
  actors.reserve(nodes.size());
  for(const auto& node : nodes)
  {
    auto actor = node.cameraId != 0xffffffff ? CameraActor::New(window.GetSize()) : Actor::New();

    actor.SetProperty(Actor::Property::SIZE, Vector3(1, 1, 1));
    actor.SetProperty(Dali::Actor::Property::NAME, node.name);
    actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    actor.SetProperty(Actor::Property::POSITION, Vector3(node.translation[0], node.translation[1], node.translation[2]));
    actor.SetProperty(Actor::Property::SCALE, Vector3(node.scale[0], node.scale[1], node.scale[2]));
    actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(node.rotationQuaternion[3], node.rotationQuaternion[0], node.rotationQuaternion[1], node.rotationQuaternion[2]));

    actors.emplace_back(actor);

    // Initially add each actor to the very first one
    if(actors.size() > 1)
    {
      actors[0].Add(actor);
    }

    // If mesh, create and add renderer
    if(node.meshId != 0xffffffff)
    {
      const auto& model    = models[node.meshId].get();
      auto        renderer = Renderer::New(model->geometry, model->shader);

      // if textured, add texture set
      auto materialId = gltf->GetMeshes()[node.meshId]->material;
      if(materialId != 0xffffffff)
      {
        if(gltf->GetMaterials()[materialId].pbrMetallicRoughness.enabled)
        {
          renderer.SetTextures(textureSets[materialId]);
        }
      }

      actor.AddRenderer(renderer);
    }

    // Reset and attach main camera
    if(node.cameraId != 0xffffffff)
    {
      cameraPosition   = Vector3(node.translation[0], node.translation[1], node.translation[2]);
      auto quatY       = Quaternion(Degree(180.0f), Vector3(0.0, 1.0, 0.0));
      auto cameraActor = CameraActor::DownCast(actor);
      cameraActor.SetProperty(Actor::Property::ORIENTATION, Quaternion(node.rotationQuaternion[3], node.rotationQuaternion[0], node.rotationQuaternion[1], node.rotationQuaternion[2]) * quatY);
      cameraActor.SetProjectionMode(Camera::PERSPECTIVE_PROJECTION);

      const auto camera = gltf->GetCameras()[node.cameraId];
      cameraActor.SetNearClippingPlane(camera->znear);
      cameraActor.SetFarClippingPlane(camera->zfar);
      cameraActor.SetFieldOfView(camera->yfov);

      cameraActor.SetProperty(CameraActor::Property::INVERT_Y_AXIS, true);
      cameraActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      cameraActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

      cameras.emplace_back(cameraActor);
    }
  }

  // Resolve hierarchy dependencies
  auto i = 0u;
  for(const auto& node : nodes)
  {
    if(!node.children.empty())
    {
      for(const auto& childId : node.children)
      {
        actors[i].Add(actors[childId + 1]);
      }
    }
    ++i;
  }

  for(auto& actor : actors)
  {
    actor.RegisterProperty("lightDir", DEFAULT_LIGHT_DIRECTION);
    actor.RegisterProperty("eyePos", cameraPosition);
  }

  return actors[0];
}

} // unnamed namespace

// This example shows how to create and display mirrored reflection using CameraActor
//
class ReflectionExample : public ConnectionTracker
{
public:
  ReflectionExample(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ReflectionExample::Create);
  }

  ~ReflectionExample() = default;

private:
  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window   window       = application.GetWindow();
    uint32_t windowWidth  = uint32_t(window.GetSize().GetWidth());
    uint32_t windowHeight = uint32_t(window.GetSize().GetHeight());

    window.GetRenderTaskList().GetTask(0).SetClearEnabled(false);
    mLayer3D = Layer::New();
    mLayer3D.SetProperty(Actor::Property::SIZE, Vector2(windowWidth, windowHeight));
    window.Add(mLayer3D);

    mLayer3D.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mLayer3D.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mLayer3D.SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_3D);
    mLayer3D.SetProperty(Layer::Property::DEPTH_TEST, true);

    auto gltf = glTF(DEMO_GAME_DIR "/reflection");

    // Define direction of light

    /**
     * Instantiate texture sets
     */
    CreateTextureSetsFromGLTF(&gltf, DEMO_GAME_DIR, mTextureSets);

    /**
     * Create models
     */
    CreateModelsFromGLTF(&gltf, mModels);

    /**
     * Create scene nodes & add to 3D Layer
     */
    mLayer3D.Add(CreateSceneFromGLTF(window, &gltf, mModels, mActors, mCameras, mTextureSets));

    auto planeActor      = mLayer3D.FindChildByName("Plane");
    auto solarActor      = mLayer3D.FindChildByName("solar_root");
    auto backgroundActor = mLayer3D.FindChildByName("background");
    ReplaceShader(backgroundActor, SHADER_REFLECTION_VERT.data(), SHADER_REFLECTION_SIMPLE_FRAG.data());
    mCenterActor      = mLayer3D.FindChildByName("center");
    mCenterHorizActor = mLayer3D.FindChildByName("center2");

    // Prepare Sun
    auto sun = mLayer3D.FindChildByName("sun");
    ReplaceShader(sun, SHADER_REFLECTION_VERT.data(), SHADER_REFLECTION_PLASMA_FRAG.data());
    mSunTimeUniformIndex    = sun.RegisterProperty("uTime", 0.0f);
    mSunKFactorUniformIndex = sun.RegisterProperty("uKFactor", 0.0f);

    mTickTimer = Timer::New(16);
    mTickTimer.TickSignal().Connect(this, &ReflectionExample::TickTimerSignal);

    // Milkyway
    auto milkyway = mLayer3D.FindChildByName("milkyway");
    ReplaceShader(milkyway, SHADER_REFLECTION_VERT.data(), SHADER_REFLECTION_FRAG.data());

    auto renderTaskSourceActor = mLayer3D.FindChildByName("RenderTaskSource");

    /**
     * Access camera (it's a child of "Camera" node)
     */
    auto camera    = mLayer3D.FindChildByName("Camera_Orientation");
    auto cameraRef = mLayer3D.FindChildByName("CameraReflection_Orientation");

    auto cameraActor = CameraActor::DownCast(camera);
    mCamera3D        = cameraActor;

    auto cameraRefActor = CameraActor::DownCast(cameraRef);
    cameraRefActor.SetProperty(DevelCameraActor::Property::REFLECTION_PLANE, Vector4(0.0f, -1.0f, 0.0f, 0.0f));
    mReflectionCamera3D = cameraRefActor;

    auto task3D = window.GetRenderTaskList().CreateTask();
    task3D.SetSourceActor(mLayer3D);
    task3D.SetViewport(Rect<int>(0, 0, windowWidth, windowHeight));
    task3D.SetCameraActor(cameraActor);
    task3D.SetClearColor(Color::BLACK);
    task3D.SetClearEnabled(true);
    task3D.SetExclusive(false);
    task3D.SetCameraActor(cameraActor);

    /**
     * Change shader to textured
     */
    Shader texShader = CreateShader(SHADER_REFLECTION_VERT.data(), SHADER_REFLECTION_TEX_FRAG.data());
    planeActor.RegisterProperty("uScreenSize", Vector2(windowWidth, windowHeight));
    auto renderer   = planeActor.GetRendererAt(0);
    auto textureSet = renderer.GetTextures();
    renderer.SetShader(texShader);

    Texture fbTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::Format::RGBA8888, windowWidth, windowHeight);
    textureSet.SetTexture(1u, fbTexture);

    auto fb = FrameBuffer::New(windowWidth, windowHeight, FrameBuffer::Attachment::DEPTH);

    fb.AttachColorTexture(fbTexture);

    auto renderTask = window.GetRenderTaskList().CreateTask();
    renderTask.SetFrameBuffer(fb);
    renderTask.SetSourceActor(renderTaskSourceActor);
    renderTask.SetViewport(Rect<int>(0, 0, windowWidth, windowHeight));
    renderTask.SetCameraActor(cameraRefActor);
    renderTask.SetClearColor(Color::BLACK);
    renderTask.SetClearEnabled(true);
    renderTask.SetExclusive(false);

    mAnimation = Animation::New(30.0f);
    mAnimation.AnimateBy(Property(solarActor, Actor::Property::ORIENTATION),
                         Quaternion(Degree(359), Vector3(0.0, 1.0, 0.0)));
    mAnimation.AnimateBy(Property(milkyway, Actor::Property::ORIENTATION),
                         Quaternion(Degree(-359), Vector3(0.0, 1.0, 0.0)));
    mAnimation.SetLooping(true);
    mAnimation.Play();

    Actor   panScreen  = Actor::New();
    Vector2 windowSize = window.GetSize();
    panScreen.SetProperty(Actor::Property::SIZE, windowSize);
    panScreen.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    panScreen.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    auto camera2d = window.GetRenderTaskList().GetTask(0).GetCameraActor();
    panScreen.SetProperty(Actor::Property::POSITION, Vector3(0, 0, camera2d.GetNearClippingPlane()));
    camera2d.Add(panScreen);
    camera2d.RotateBy(Degree(180.0f), Vector3(0.0, 1.0, 0.0));
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach(panScreen);
    mPanGestureDetector.DetectedSignal().Connect(this, &ReflectionExample::OnPan);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &ReflectionExample::OnKeyEvent);

    mTickTimer.Start();
  }

  void OnPan(Actor actor, const PanGesture& panGesture)
  {
    Vector2 displacement = panGesture.GetScreenDisplacement();
    Vector2 rotation{
      displacement.y * -0.1f,
      displacement.x * 0.1f
    };

    Quaternion q(Degree(0.f), Degree(rotation.y), Degree(rotation.x));
    Quaternion q0 = mCenterActor.GetProperty(Actor::Property::ORIENTATION).Get<Quaternion>();
    mCenterActor.SetProperty(Actor::Property::ORIENTATION, q * q0);

    Matrix m = Matrix::IDENTITY;
    m.SetTransformComponents(Vector3::ONE, q0, Vector3::ZERO);
    auto yAxis = m.GetYAxis() * -1.0f;

    yAxis.Normalize();
    mReflectionCamera3D.SetProperty(DevelCameraActor::Property::REFLECTION_PLANE, Vector4(yAxis.x, yAxis.y, yAxis.z, 0.0f));
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

  bool TickTimerSignal()
  {
    auto         root          = mLayer3D;
    static float rotationAngle = 0.0f;

    const auto ROTATION_ANGLE_STEP = 0.05f;
    const auto FRAME_DELTA_TIME    = 0.016f;
    const auto PLASMA_K_FACTOR     = 12.0f; // 'granularity' of plasma effect

    rotationAngle += ROTATION_ANGLE_STEP;
    mMockTime += FRAME_DELTA_TIME;
    mKFactor = PLASMA_K_FACTOR;

    auto sun = root.FindChildByName("sun");
    sun.SetProperty(mSunTimeUniformIndex, mMockTime);
    sun.SetProperty(mSunKFactorUniformIndex, mKFactor);
    sun.SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(Degree(rotationAngle)), Vector3(0.0, 1.0, 0.0)));
    return true;
  }

private:
  Application& mApplication;

  Layer mLayer3D{};

  ActorContainer      mActors{};
  CameraContainer     mCameras{};
  ModelContainer      mModels{};
  TextureSetContainer mTextureSets{};

  Animation          mAnimation{};
  float              mMockTime{0.0f};
  float              mKFactor{0.0f};
  Property::Index    mSunTimeUniformIndex{};
  Property::Index    mSunKFactorUniformIndex{};
  PanGestureDetector mPanGestureDetector{};

  Timer mTickTimer{};

  CameraActor mCamera3D{};
  CameraActor mReflectionCamera3D{};
  Actor       mCenterActor{};
  Actor       mCenterHorizActor{};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application       application = Application::New(&argc, &argv);
  ReflectionExample test(application);
  application.MainLoop();
  return 0;
}
