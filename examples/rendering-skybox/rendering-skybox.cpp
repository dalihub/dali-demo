/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include "generated/rendering-skybox-cube-frag.h"
#include "generated/rendering-skybox-cube-vert.h"
#include "generated/rendering-skybox-equirectangular-frag.h"
#include "generated/rendering-skybox-frag.h"
#include "generated/rendering-skybox-vert.h"
#include "look-camera.h"

using namespace Dali;
using namespace Toolkit;

namespace
{
const float   CAMERA_DEFAULT_FOV(60.0f);
const float   CAMERA_DEFAULT_NEAR(0.1f);
const float   CAMERA_DEFAULT_FAR(1000.0f);
const Vector3 CAMERA_DEFAULT_POSITION(0.0f, 0.0f, 100.0f);

const char* TEXTURE_URL = DEMO_IMAGE_DIR "wood.png";

const unsigned int SKYBOX_FACE_COUNT  = 6;
const unsigned int SKYBOX_FACE_WIDTH  = 2048;
const unsigned int SKYBOX_FACE_HEIGHT = 2048;

/**
 * @brief The skybox types supported by the application.
 */
enum class SkyboxType
{
  CUBEMAP,        ///< Skybox in cubemap
  EQUIRECTANGULAR ///< Skybox in equirectangular projection
};

/*
 * Credit to Joey do Vries for the following cubemap images
 * Take from git://github.com/JoeyDeVries/LearnOpenGL.git
 * The images are licensed under the terms of the CC BY 4.0 license:
 * https://creativecommons.org/licenses/by/4.0/
 */
const char* SKYBOX_FACES[SKYBOX_FACE_COUNT] =
  {
    DEMO_IMAGE_DIR "lake_right.jpg",
    DEMO_IMAGE_DIR "lake_left.jpg",
    DEMO_IMAGE_DIR "lake_top.jpg",
    DEMO_IMAGE_DIR "lake_bottom.jpg",
    DEMO_IMAGE_DIR "lake_back.jpg",
    DEMO_IMAGE_DIR "lake_front.jpg"};

/*
 * Take from Wikimedia Commons, the free media repository
 * https://commons.wikimedia.org
 * The image is licensed under the terms of the CC BY-SA 4.0 license:
 * https://creativecommons.org/licenses/by-sa/4.0
 */
const char* EQUIRECTANGULAR_TEXTURE_URL = DEMO_IMAGE_DIR "veste_oberhaus_spherical_panoramic.jpg";
} // namespace

// This example shows how to create a skybox
//
// Recommended screen size on desktop: 1280x720
//
class TexturedCubeController : public ConnectionTracker
{
public:
  TexturedCubeController(Application& application)
  : mApplication(application),
    mCurrentSkyboxType(SkyboxType::CUBEMAP)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &TexturedCubeController::Create);
  }

  ~TexturedCubeController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Step 1. Setup camera
    SetupCamera();

    // Step 2. Create shaders
    CreateShaders();

    // Step 3. Create geometry
    CreateCubeGeometry();
    CreateSkyboxGeometry();

    // Step 4. Display first a cube at the world origin.
    //         The write on the depth buffer is enabled.
    DisplayCube();

    // Step 5. Display last the skybox surrounding the camera.
    //         The depth test is enabled, the shader sets 1.0, which is the maximum depth and
    //         the depth function is set to LESS or EQUAL so the fragment shader will run only
    //         in those pixels that any other object has written on them.
    DisplaySkybox(mCurrentSkyboxType);

    // Step 6. Play animation to rotate the cube
    PlayAnimation();

    // Respond to key events
    window.KeyEventSignal().Connect(this, &TexturedCubeController::OnKeyEvent);

    // Create a tap gesture detector to detect double tap
    mDoubleTapGesture = TapGestureDetector::New(2);
    mDoubleTapGesture.Attach(window.GetRootLayer());
    mDoubleTapGesture.DetectedSignal().Connect(this, &TexturedCubeController::OnDoubleTap);
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

  void OnDoubleTap(Actor /*actor*/, const TapGesture& /*gesture*/)
  {
    if(mCurrentSkyboxType == SkyboxType::CUBEMAP)
    {
      mCurrentSkyboxType = SkyboxType::EQUIRECTANGULAR;
    }
    else
    {
      mCurrentSkyboxType = SkyboxType::CUBEMAP;
    }

    DisplaySkybox(mCurrentSkyboxType);
  }

  /**
   * @brief Setup a perspective camera pointing in the negative Z direction
   */
  void SetupCamera()
  {
    Window window = mApplication.GetWindow();

    RenderTask renderTask = window.GetRenderTaskList().GetTask(0);
    renderTask.SetCullMode(false); // avoid frustum culling affecting the skybox

    mCamera.Initialise(window, CAMERA_DEFAULT_POSITION, CAMERA_DEFAULT_FOV, CAMERA_DEFAULT_NEAR, CAMERA_DEFAULT_FAR);
  }

  /**
   * Creates a shader using inlined variable VERTEX_SHADER and FRAGMENT_SHADER
   *
   * Shaders are very basic and all they do is transforming vertices and sampling
   * a texture.
   */
  void CreateShaders()
  {
    mShaderCube                  = Shader::New(SHADER_RENDERING_SKYBOX_CUBE_VERT, SHADER_RENDERING_SKYBOX_CUBE_FRAG);
    mShaderSkybox                = Shader::New(SHADER_RENDERING_SKYBOX_VERT, SHADER_RENDERING_SKYBOX_FRAG);
    mShaderSkyboxEquirectangular = Shader::New(SHADER_RENDERING_SKYBOX_VERT, SHADER_RENDERING_SKYBOX_EQUIRECTANGULAR_FRAG);
  }

  /**
   * @brief CreateCubeGeometry
   * This function creates a cube geometry including texture coordinates.
   * Also it demonstrates using the indexed draw feature by setting an index array.
   */
  void CreateCubeGeometry()
  {
    struct Vertex
    {
      Vector3 aPosition;
      Vector2 aTexCoord;
    };

    Vertex vertices[] = {
      {Vector3(1.0f, -1.0f, -1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, 1.0f, -1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, 1.0f, -1.0f), Vector2(0.0, 1.0)},
      {Vector3(-1.0f, 1.0f, 1.0f), Vector2(1.0, 1.0)},
      {Vector3(1.0f, -1.0f, 1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, 1.0f, 1.0f), Vector2(0.0, 1.0)},
      {Vector3(1.0f, 1.0f, 1.0f), Vector2(1.0, 1.0)},
      {Vector3(1.0f, -1.0f, -1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, 1.0f, -1.0f), Vector2(0.0, 1.0)},
      {Vector3(1.0f, -1.0f, 1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, -1.0f, -1.0f), Vector2(0.0, 1.0)},
      {Vector3(-1.0f, -1.0f, -1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, 1.0f, 1.0f), Vector2(0.0, 0.0)},
      {Vector3(-1.0f, 1.0f, -1.0f), Vector2(0.0, 1.0)},
      {Vector3(1.0f, 1.0f, -1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, 1.0f, 1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, 1.0f, 1.0f), Vector2(0.0, 1.0)},
      {Vector3(1.0f, -1.0f, -1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, -1.0f, -1.0f), Vector2(1.0, 0.0)},
      {Vector3(-1.0f, 1.0f, -1.0f), Vector2(0.0, 0.0)},
      {Vector3(-1.0f, 1.0f, 1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, -1.0f, 1.0f), Vector2(1.0, 0.0)},
      {Vector3(1.0f, -1.0f, 1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, 1.0f, 1.0f), Vector2(1.0, 1.0)},
      {Vector3(1.0f, -1.0f, 1.0f), Vector2(1.0, 0.0)},
      {Vector3(1.0f, -1.0f, -1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, -1.0f, 1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, -1.0f, 1.0f), Vector2(1.0, 0.0)},
      {Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0, 0.0)},
      {Vector3(-1.0f, -1.0f, -1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, -1.0f, 1.0f), Vector2(1.0, 0.0)},
      {Vector3(-1.0f, 1.0f, 1.0f), Vector2(0.0, 0.0)},
      {Vector3(1.0f, 1.0f, -1.0f), Vector2(1.0, 1.0)},
      {Vector3(-1.0f, 1.0f, -1.0f), Vector2(1.0, 0.0)},
      {Vector3(-1.0f, 1.0f, 1.0f), Vector2(0.0, 0.0)},
    };

    VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map()
                                                    .Add("aPosition", Property::VECTOR3)
                                                    .Add("aTexCoord", Property::VECTOR2));
    vertexBuffer.SetData(vertices, sizeof(vertices) / sizeof(Vertex));

    // create indices
    const unsigned short INDEX_CUBE[] = {
      2, 1, 0, 5, 4, 3, 8, 7, 6, 11, 10, 9, 14, 13, 12, 17, 16, 15, 20, 19, 18, 23, 22, 21, 26, 25, 24, 29, 28, 27, 32, 31, 30, 35, 34, 33};

    mGeometry = Geometry::New();
    mGeometry.AddVertexBuffer(vertexBuffer);
    mGeometry.SetIndexBuffer(INDEX_CUBE,
                             sizeof(INDEX_CUBE) / sizeof(INDEX_CUBE[0]));
    mGeometry.SetType(Geometry::TRIANGLES);
  }

  /**
   * @brief CreateCubeGeometry
   * This function creates a cube geometry including texture coordinates.
   * Also it demonstrates using the indexed draw feature by setting an index array.
   */
  void CreateSkyboxGeometry()
  {
    struct Vertex
    {
      Vector3 aPosition;
    };

    Vertex skyboxVertices[] = {
      // back
      {Vector3(-1.0f, 1.0f, -1.0f)},
      {Vector3(-1.0f, -1.0f, -1.0f)},
      {Vector3(1.0f, -1.0f, -1.0f)},
      {Vector3(1.0f, -1.0f, -1.0f)},
      {Vector3(1.0f, 1.0f, -1.0f)},
      {Vector3(-1.0f, 1.0f, -1.0f)},

      // left
      {Vector3(-1.0f, -1.0f, 1.0f)},
      {Vector3(-1.0f, -1.0f, -1.0f)},
      {Vector3(-1.0f, 1.0f, -1.0f)},
      {Vector3(-1.0f, 1.0f, -1.0f)},
      {Vector3(-1.0f, 1.0f, 1.0f)},
      {Vector3(-1.0f, -1.0f, 1.0f)},

      // right
      {Vector3(1.0f, -1.0f, -1.0f)},
      {Vector3(1.0f, -1.0f, 1.0f)},
      {Vector3(1.0f, 1.0f, 1.0f)},
      {Vector3(1.0f, 1.0f, 1.0f)},
      {Vector3(1.0f, 1.0f, -1.0f)},
      {Vector3(1.0f, -1.0f, -1.0f)},

      // front
      {Vector3(-1.0f, -1.0f, 1.0f)},
      {Vector3(-1.0f, 1.0f, 1.0f)},
      {Vector3(1.0f, 1.0f, 1.0f)},
      {Vector3(1.0f, 1.0f, 1.0f)},
      {Vector3(1.0f, -1.0f, 1.0f)},
      {Vector3(-1.0f, -1.0f, 1.0f)},

      // botton
      {Vector3(-1.0f, 1.0f, -1.0f)},
      {Vector3(1.0f, 1.0f, -1.0f)},
      {Vector3(1.0f, 1.0f, 1.0f)},
      {Vector3(1.0f, 1.0f, 1.0f)},
      {Vector3(-1.0f, 1.0f, 1.0f)},
      {Vector3(-1.0f, 1.0f, -1.0f)},

      // top
      {Vector3(-1.0f, -1.0f, -1.0f)},
      {Vector3(-1.0f, -1.0f, 1.0f)},
      {Vector3(1.0f, -1.0f, -1.0f)},
      {Vector3(1.0f, -1.0f, -1.0f)},
      {Vector3(-1.0f, -1.0f, 1.0f)},
      {Vector3(1.0f, -1.0f, 1.0f)}};

    VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map()
                                                    .Add("aPosition", Property::VECTOR3));
    vertexBuffer.SetData(skyboxVertices, sizeof(skyboxVertices) / sizeof(Vertex));

    mSkyboxGeometry = Geometry::New();
    mSkyboxGeometry.AddVertexBuffer(vertexBuffer);
    mSkyboxGeometry.SetType(Geometry::TRIANGLES);
  }

  /**
   * Display a cube at the world origin
   */
  void DisplayCube()
  {
    // Load image from file
    PixelData pixels = SyncImageLoader::Load(TEXTURE_URL);

    Texture texture = Texture::New(TextureType::TEXTURE_2D, pixels.GetPixelFormat(), pixels.GetWidth(), pixels.GetHeight());
    texture.Upload(pixels, 0, 0, 0, 0, pixels.GetWidth(), pixels.GetHeight());

    // create TextureSet
    mTextureSet = TextureSet::New();
    mTextureSet.SetTexture(0, texture);

    mRenderer = Renderer::New(mGeometry, mShaderCube);
    mRenderer.SetTextures(mTextureSet);
    mRenderer.SetProperty(Renderer::Property::DEPTH_INDEX, 1.0f);

    // A further optimization would be to enable debug testing instead
    mRenderer.SetProperty(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK);

    // Enables the write on the depth buffer.
    mRenderer.SetProperty(Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::ON);

    mActor = Actor::New();
    mActor.SetProperty(Dali::Actor::Property::NAME, "Cube");
    mActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mActor.AddRenderer(mRenderer);

    mActor.SetProperty(Actor::Property::SIZE, Vector3(10.f, 10.f, 10.f));

    Window window = mApplication.GetWindow();
    window.Add(mActor);
  }

  /**
   * Display a skybox surrounding the camera
   */
  void DisplaySkybox(SkyboxType type)
  {
    mSkyboxTextures.Reset();
    mSkyboxRenderer.Reset();
    mSkyboxActor.Reset();

    Texture texture;

    if(type == SkyboxType::CUBEMAP)
    {
      // Load skybox faces from file
      texture = Texture::New(TextureType::TEXTURE_CUBE, Pixel::RGBA8888, SKYBOX_FACE_WIDTH, SKYBOX_FACE_HEIGHT);
      for(unsigned int i = 0; i < SKYBOX_FACE_COUNT; i++)
      {
        PixelData pixels = SyncImageLoader::Load(SKYBOX_FACES[i]);
        texture.Upload(pixels, CubeMapLayer::POSITIVE_X + i, 0, 0, 0, SKYBOX_FACE_WIDTH, SKYBOX_FACE_HEIGHT);
      }

      mSkyboxRenderer = Renderer::New(mSkyboxGeometry, mShaderSkybox);
    }
    else
    {
      // Load equirectangular image from file
      PixelData pixels = SyncImageLoader::Load(EQUIRECTANGULAR_TEXTURE_URL);

      texture = Texture::New(TextureType::TEXTURE_2D, pixels.GetPixelFormat(), pixels.GetWidth(), pixels.GetHeight());
      texture.Upload(pixels, 0, 0, 0, 0, pixels.GetWidth(), pixels.GetHeight());

      mSkyboxRenderer = Renderer::New(mSkyboxGeometry, mShaderSkyboxEquirectangular);
    }

    // create TextureSet
    mSkyboxTextures = TextureSet::New();
    mSkyboxTextures.SetTexture(0, texture);

    mSkyboxRenderer.SetTextures(mSkyboxTextures);
    mSkyboxRenderer.SetProperty(Renderer::Property::DEPTH_INDEX, 2.0f);

    // Enables the depth test.
    mSkyboxRenderer.SetProperty(Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::ON);

    // The fragment shader will run only is those pixels that have the max depth value.
    mSkyboxRenderer.SetProperty(Renderer::Property::DEPTH_FUNCTION, DepthFunction::LESS_EQUAL);

    Window window = mApplication.GetWindow();

    mSkyboxActor = Actor::New();
    mSkyboxActor.SetProperty(Dali::Actor::Property::NAME, "SkyBox");
    mSkyboxActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mSkyboxActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mSkyboxActor.SetProperty(Actor::Property::POSITION, CAMERA_DEFAULT_POSITION);
    mSkyboxActor.AddRenderer(mSkyboxRenderer);
    window.Add(mSkyboxActor);
  }

  /**
   * Plays animation
   */
  void PlayAnimation()
  {
    mAnimation = Animation::New(5.0f);
    mAnimation.SetLooping(true);
    mAnimation.AnimateBy(Property(mActor, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(360)), Vector3::ZAXIS));
    mAnimation.AnimateBy(Property(mActor, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(360)), Vector3::YAXIS));
    mAnimation.AnimateBy(Property(mActor, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(360)), Vector3::XAXIS));
    mAnimation.Play();
  }

private:
  Application& mApplication;

  LookCamera mCamera;

  Shader mShaderCube;
  Shader mShaderSkybox;
  Shader mShaderSkyboxEquirectangular;

  Geometry   mGeometry;
  TextureSet mTextureSet;
  Renderer   mRenderer;
  Actor      mActor;
  Animation  mAnimation;

  Geometry   mSkyboxGeometry;
  TextureSet mSkyboxTextures;
  Renderer   mSkyboxRenderer;
  Actor      mSkyboxActor;

  TapGestureDetector mDoubleTapGesture;
  SkyboxType         mCurrentSkyboxType;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application            application = Application::New(&argc, &argv);
  TexturedCubeController test(application);
  application.MainLoop();
  return 0;
}
