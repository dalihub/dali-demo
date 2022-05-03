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
#include <dali-toolkit/devel-api/controls/scene3d-view/scene3d-view.h>
#include <dali/dali.h>
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include <dali/devel-api/adaptor-framework/file-stream.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/camera-actor.h>
#include <cstring>

using namespace Dali;
using namespace Dali::Toolkit;

/*
 * This example shows how to create and display a Scene3dView control.
 * The application can load 5 different glTF model to Scene3dView control.
 * Each model has diffirent material. BoomBox shows glossy or matt plastic material.
 * DamagedHelmet shows a kind of reflective glass and metallic object.
 * Microphone shows a roughness of metallic objects.
 * and Lantern shows a realistic difference between wood object and metallic object.
 * Rotate the camera by swiping.
 * A double tap changes the model.
 */

namespace
{
enum GLTF_MODEL_LIST
{
  GLTF_ANIMATED_BOX = 0,
  GLTF_LANTERN,
  GLTF_BOOMBOX,
  GLTF_DAMAGED_HELMET,
  GLTF_MICROPHONE,
  NUM_OF_GLTF_MODELS
};

const char* gltf_list[6] =
  {
    /**
     * For the BoxAnimated.gltf and its Assets
     * Donated by Cesium for glTF testing.
     * Take from https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BoxAnimated
     */
    "BoxAnimated.gltf",
    /**
     * For the Lantern.gltf and its Assets
     * Donated by Microsoft for glTF testing
     * Created by Ryan Martin
     * Take from https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Lantern
     */
    "Lantern.gltf",
    /**
     * For the BoomBox.gltf and its Assets
     * Donated by Microsoft for glTF testing
     * Created by Ryan Martin
     * Take from https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BoomBox
     */
    "BoomBox.gltf",
    /**
     * For the DamagedHelmet.gltf and its Assets
     * Battle Damaged Sci-fi Helmet - PBR by theblueturtle_, published under a
     * Creative Commons Attribution-NonCommercial license
     * https://sketchfab.com/models/b81008d513954189a063ff901f7abfe4
     */
    "DamagedHelmet.gltf",
    /**
     * For the microphone.gltf and its Assets
     * Microphone GXL 066 Bafhcteks by Gistold, published under a
     * Creative Commons Attribution-NonCommercial license
     * https://sketchfab.com/models/5172dbe9281a45f48cee8c15bdfa1831
     */
    "microphone.gltf"};

Vector3 camera_position_list[6] =
  {
    Vector3(-6.00, -8.00, 12.00),
    Vector3(-30.0, -40.0, 60.0),
    Vector3(-0.03, -0.04, 0.06),
    Vector3(-3.00, -4.00, 6.00),
    Vector3(-0.00, -3.00, 4.00)};

/**
 * For the diffuse and specular cube map texture.
 * These textures are based off version of Wave engine sample
 * Take from https://github.com/WaveEngine/Samples
 *
 * Copyright (c) 2016 Wave Coorporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

const std::string modeldir = DEMO_MODEL_DIR;
const std::string imagedir = DEMO_IMAGE_DIR;
const std::string uri_diffuse_texture(imagedir + "forest_diffuse_cubemap.png");
const std::string uri_specular_texture(imagedir + "forest_specular_cubemap.png");

const int32_t cubeMap_index_x[6] = {2, 0, 1, 1, 1, 3};
const int32_t cubeMap_index_y[6] = {1, 1, 0, 2, 1, 1};

const char* VERTEX_SHADER_URL   = DEMO_SHADER_DIR "cube_shader.vsh";
const char* FRAGMENT_SHADER_URL = DEMO_SHADER_DIR "cube_shader.fsh";

/**
 * @brief Load a shader source file
 * @param[in] The path of the source file
 * @param[out] The contents of file
 * @return True if the source was read successfully
 */
bool LoadShaderCode(const std::string& fullpath, std::vector<char>& output)
{
  Dali::FileStream fileStream(fullpath, FileStream::READ | FileStream::BINARY);
  FILE*            file = fileStream.GetFile();
  if(nullptr == file)
  {
    return false;
  }

  bool retValue = false;
  if(!fseek(file, 0, SEEK_END))
  {
    long int size = ftell(file);

    if((size != -1L) &&
       (!fseek(file, 0, SEEK_SET)))
    {
      output.resize(size + 1);
      std::fill(output.begin(), output.end(), 0);
      ssize_t result = fread(output.data(), size, 1, file);

      retValue = (result >= 0);
    }
  }

  return retValue;
}

/**
 * @brief Load vertex and fragment shader source
 * @param[in] shaderVertexFileName is the filepath of Vertex shader
 * @param[in] shaderFragFileName is the filepath of Fragment shader
 * @return the Dali::Shader object
 */
Shader LoadShaders(const std::string& shaderVertexFileName, const std::string& shaderFragFileName)
{
  Shader            shader;
  std::vector<char> bufV, bufF;

  if(LoadShaderCode(shaderVertexFileName.c_str(), bufV))
  {
    if(LoadShaderCode(shaderFragFileName.c_str(), bufF))
    {
      shader = Shader::New(bufV.data(), bufF.data());
    }
  }
  return shader;
}

} // namespace

/**
 * This example shows how to render glTF model with Scene3dView
 * How to test
 *  - Input UP or DOWN key to make the model rotate or stop.
 *  - Input LEFT or RIGHT key to change glTF model
 *  - Double Touch also changes glTF model.
 */
class Scene3dViewController : public ConnectionTracker
{
public:
  Scene3dViewController(Application& application)
  : mApplication(application),
    mModelOrientation(),
    mAnimationStop(false)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &Scene3dViewController::Create);
  }

  ~Scene3dViewController()
  {
    mAnimation.Stop();
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    mWindow = application.GetWindow();

    // Get a handle to the mWindow
    mWindow.SetBackgroundColor(Color::WHITE);

    RenderTask renderTask = mWindow.GetRenderTaskList().GetTask(0);
    renderTask.SetCullMode(false);

    mCurrentGlTF = GLTF_ANIMATED_BOX;
    CreateSceneFromGLTF(gltf_list[mCurrentGlTF]);
    SetCameraActor();
    CreateSkybox();
    SetAnimation();

    // Respond to a click anywhere on the mWindow
    mWindow.GetRootLayer().TouchedSignal().Connect(this, &Scene3dViewController::OnTouch);
    mWindow.KeyEventSignal().Connect(this, &Scene3dViewController::OnKeyEvent);
    mWindow.GetRootLayer().WheelEventSignal().Connect(this, &Scene3dViewController::OnWheel);

    mDoubleTap     = false;
    mDoubleTapTime = Timer::New(150);
    mDoubleTapTime.TickSignal().Connect(this, &Scene3dViewController::OnDoubleTapTime);
  }

  bool OnWheel(Actor actor, const WheelEvent& wheelEvent)
  {
    mWheelDelta -= wheelEvent.GetDelta() * 0.025f;
    mWheelDelta = std::max(0.5f, mWheelDelta);
    mWheelDelta = std::min(2.0f, mWheelDelta);

    if(mScene3dView)
    {
      mScene3dView.SetProperty(Actor::Property::SCALE, mWheelDelta);
    }

    return true;
  }

  bool OnDoubleTapTime()
  {
    mDoubleTap = false;
    return true;
  }

  void CreateSceneFromGLTF(std::string modelName)
  {
    if(mScene3dView)
    {
      mWindow.GetRootLayer().Remove(mScene3dView);
    }

    std::string gltfUrl = modeldir;
    gltfUrl += modelName;
    mScene3dView = Scene3dView::New(gltfUrl, uri_diffuse_texture, uri_specular_texture, Vector4::ONE);

    mScene3dView.SetLight(Scene3dView::LightType::POINT_LIGHT, Vector3(-5, -5, 5), Vector3(1, 1, 1));

    mWindow.Add(mScene3dView);
    mScene3dView.PlayAnimations();
  }

  void SetCameraActor()
  {
    mCameraPosition = camera_position_list[mCurrentGlTF];
    mCameraActor    = mWindow.GetRenderTaskList().GetTask(0).GetCameraActor();
    mCameraActor.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mCameraActor.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mCameraActor.SetProperty(Dali::Actor::Property::POSITION, mCameraPosition);
    mCameraActor.SetType(Dali::Camera::LOOK_AT_TARGET);
    mCameraActor.SetNearClippingPlane(0.01);
  }

  void CreateSkybox()
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

    const std::string currentVShaderFile(VERTEX_SHADER_URL);
    const std::string currentFShaderFile(FRAGMENT_SHADER_URL);

    mShaderSkybox = LoadShaders(currentVShaderFile, currentFShaderFile);

    Dali::VertexBuffer vertexBuffer = Dali::VertexBuffer::New(Property::Map()
                                                                .Add("aPosition", Property::VECTOR3));
    vertexBuffer.SetData(skyboxVertices, sizeof(skyboxVertices) / sizeof(Vertex));

    mSkyboxGeometry = Geometry::New();
    mSkyboxGeometry.AddVertexBuffer(vertexBuffer);
    mSkyboxGeometry.SetType(Geometry::TRIANGLES);

    // Diffuse Cube Map
    Devel::PixelBuffer diffusePixelBuffer = LoadImageFromFile(uri_diffuse_texture);
    int32_t            diffuseFaceSize    = diffusePixelBuffer.GetWidth() / 4;
    Texture            texture            = Texture::New(TextureType::TEXTURE_CUBE, diffusePixelBuffer.GetPixelFormat(), diffuseFaceSize, diffuseFaceSize);
    for(int32_t i = 0; i < 6; ++i)
    {
      UploadTextureFace(texture, diffusePixelBuffer, i);
    }
    texture.GenerateMipmaps();

    mSkyboxTextures = TextureSet::New();
    mSkyboxTextures.SetTexture(0, texture);

    mSkyboxRenderer = Renderer::New(mSkyboxGeometry, mShaderSkybox);
    mSkyboxRenderer.SetTextures(mSkyboxTextures);
    mSkyboxRenderer.SetProperty(Renderer::Property::DEPTH_INDEX, 2.0f);

    // Enables the depth test.
    mSkyboxRenderer.SetProperty(Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::ON);

    // The fragment shader will run only is those pixels that have the max depth value.
    mSkyboxRenderer.SetProperty(Renderer::Property::DEPTH_FUNCTION, DepthFunction::LESS_EQUAL);

    mSkyboxActor = Actor::New();
    mSkyboxActor.SetProperty(Dali::Actor::Property::NAME, "SkyBox");
    mSkyboxActor.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mSkyboxActor.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mSkyboxActor.SetProperty(Dali::Actor::Property::POSITION, mCameraPosition);
    mSkyboxActor.AddRenderer(mSkyboxRenderer);
    mWindow.Add(mSkyboxActor);
  }

  void SetAnimation()
  {
    /**
     * Rotate object Actor along Y axis.
     * Animation duration is 8 seconds.
     * Five keyframes are set for each 90 degree. i.e., 0, 90, 180, 270, and 360.
     * Each keyframes are interpolated by linear interpolator.
     */
    mAnimation                = Animation::New(8.0);
    KeyFrames keyframes       = KeyFrames::New();
    float     lengthAnimation = 0.25;
    for(int32_t i = 0; i < 5; ++i)
    {
      keyframes.Add(i * lengthAnimation, Quaternion(Degree(i * 90.0), Vector3::YAXIS));
    }
    mAnimation.AnimateBetween(Property(mScene3dView, Dali::Actor::Property::ORIENTATION), keyframes, Animation::Interpolation::LINEAR);
    mAnimation.SetLooping(true);
    mAnimation.Play();
  }

  uint8_t* CropBuffer(uint8_t* sourceBuffer, uint32_t bytesPerPixel, uint32_t width, uint32_t height, uint32_t xOffset, uint32_t yOffset, uint32_t xFaceSize, uint32_t yFaceSize)
  {
    if(xOffset + xFaceSize > width || yOffset + yFaceSize > height)
    {
      DALI_LOG_ERROR("Can not crop outside of texture area.\n");
      return nullptr;
    }
    uint32_t byteSize   = bytesPerPixel * xFaceSize * yFaceSize;
    uint8_t* destBuffer = reinterpret_cast<uint8_t*>(malloc(byteSize + 4u));

    int32_t srcStride  = width * bytesPerPixel;
    int32_t destStride = xFaceSize * bytesPerPixel;
    int32_t srcOffset  = xOffset * bytesPerPixel + yOffset * srcStride;
    int32_t destOffset = 0;
    for(uint16_t row = yOffset; row < yOffset + yFaceSize; ++row)
    {
      memcpy(destBuffer + destOffset, sourceBuffer + srcOffset, destStride);
      srcOffset += srcStride;
      destOffset += destStride;
    }

    return destBuffer;
  }

  void UploadTextureFace(Texture& texture, Devel::PixelBuffer pixelBuffer, int32_t faceIndex)
  {
    uint8_t* imageBuffer   = pixelBuffer.GetBuffer();
    uint32_t bytesPerPixel = Pixel::GetBytesPerPixel(pixelBuffer.GetPixelFormat());
    uint32_t imageWidth    = pixelBuffer.GetWidth();
    uint32_t imageHeight   = pixelBuffer.GetHeight();

    int32_t faceSize = imageWidth / 4;

    uint32_t xOffset = cubeMap_index_x[faceIndex] * faceSize;
    uint32_t yOffset = cubeMap_index_y[faceIndex] * faceSize;

    uint8_t* tempImageBuffer = CropBuffer(imageBuffer, bytesPerPixel, imageWidth, imageHeight, xOffset, yOffset, faceSize, faceSize);
    if(tempImageBuffer)
    {
      PixelData pixelData = PixelData::New(tempImageBuffer, faceSize * faceSize * bytesPerPixel, faceSize, faceSize, pixelBuffer.GetPixelFormat(), PixelData::FREE);
      texture.Upload(pixelData, CubeMapLayer::POSITIVE_X + faceIndex, 0, 0, 0, faceSize, faceSize);
    }
  }

  void ChangeModel(int32_t direction)
  {
    mCurrentGlTF += direction;
    if(mCurrentGlTF >= NUM_OF_GLTF_MODELS)
    {
      mCurrentGlTF = 0;
    }
    if(mCurrentGlTF < 0)
    {
      mCurrentGlTF = NUM_OF_GLTF_MODELS - 1;
    }
    CreateSceneFromGLTF(gltf_list[mCurrentGlTF]);
    mCameraPosition = camera_position_list[mCurrentGlTF];
    mCameraActor.SetProperty(Dali::Actor::Property::POSITION, mCameraPosition);
    if(mCurrentGlTF == GLTF_LANTERN)
    {
      mCameraActor.SetTargetPosition(Vector3(0.0, -15.0, 0.0));
    }
    else
    {
      mCameraActor.SetTargetPosition(Vector3::ZERO);
    }
    mSkyboxActor.SetProperty(Dali::Actor::Property::POSITION, mCameraPosition);
    SetAnimation();
    mAnimationStop = false;
    mWheelDelta    = 1.0f;
  }

  /**
   * This function will change the material Roughness, Metalness or the model orientation when touched
   */
  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    const PointState::Type state = touch.GetState(0);

    switch(state)
    {
      case PointState::DOWN:
      {
        if(mDoubleTap)
        {
          ChangeModel(1);
        }
        mDoubleTapTime.Stop();

        mPointZ = touch.GetScreenPosition(0);
        if(!mAnimationStop)
        {
          mAnimation.Pause();
        }
        break;
      }

      case PointState::MOTION:
      {
        const Size    size   = mWindow.GetSize();
        const float   scaleX = size.width;
        const float   scaleY = size.height;
        const Vector2 point  = touch.GetScreenPosition(0);
        mProcess             = false;
        // If the touch is not processed above, then change the model orientation
        if(!mProcess)
        {
          /**
           * This is the motion for the swipe to rotate camera that targeting Vector3::ZERO.
           * Each quaternion is used to rotate camera by Pitch and Yaw respectively.
           * Final Orientation of camera is combination of Pitch and Yaw quaternion.
           * For the natural rendering, Skybox also relocated at the new camera position.
           */
          mProcess           = true;
          const float angle1 = ((mPointZ.y - point.y) / scaleY);
          const float angle2 = ((mPointZ.x - point.x) / scaleX);

          Quaternion pitch(Radian(Degree(angle1 * -200.0f)), (mCameraPosition.z >= 0) ? Vector3::XAXIS : -Vector3::XAXIS);
          Quaternion yaw(Radian(Degree(angle2 * -200.0f)), -Vector3::YAXIS);

          Quaternion newModelOrientation = yaw * pitch;
          mCameraPosition                = newModelOrientation.Rotate(mCameraPosition);
          mCameraActor.SetProperty(Dali::Actor::Property::POSITION, mCameraPosition);
          mSkyboxActor.SetProperty(Dali::Actor::Property::POSITION, mCameraPosition);

          mPointZ = point;
        }
        break;
      }

      case PointState::UP:
      {
        mDoubleTapTime.Start();
        mDoubleTap = true;
        if(!mAnimationStop)
        {
          mAnimation.Play();
        }
        mProcess = false;
        break;
      }

      default:
      {
        break;
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
      if(event.GetKeyName() == "Down" ||
         event.GetKeyName() == "Up")
      {
        if(!mProcess)
        {
          if(mAnimationStop)
          {
            mAnimation.Play();
          }
          else
          {
            mAnimation.Pause();
          }
          mAnimationStop = !mAnimationStop;
        }
      }

      if(IsKey(event, Dali::DALI_KEY_CURSOR_RIGHT))
      {
        ChangeModel(1);
      }

      if(IsKey(event, Dali::DALI_KEY_CURSOR_LEFT))
      {
        ChangeModel(-1);
      }
    }
  }

private:
  Window       mWindow;
  Layer        mLayer3D;
  Application& mApplication;
  CameraActor  mCameraActor;
  Dali::Timer  mTimer;

  Vector3     mCameraPosition;
  Scene3dView mScene3dView;

  Vector2    mPointZ;
  Quaternion mModelOrientation;

  Shader     mShaderSkybox;
  Geometry   mSkyboxGeometry;
  TextureSet mSkyboxTextures;
  Renderer   mSkyboxRenderer;
  Actor      mSkyboxActor;

  Animation mAnimation;
  bool      mProcess{false};
  bool      mAnimationStop;

  Timer mDoubleTapTime;
  bool  mDoubleTap{false};

  float mWheelDelta{1.0f};

  int32_t mCurrentGlTF{0};
};

int32_t DALI_EXPORT_API main(int32_t argc, char** argv)
{
  Application           application = Application::New(&argc, &argv);
  Scene3dViewController test(application);
  application.MainLoop();
  return 0;
}
