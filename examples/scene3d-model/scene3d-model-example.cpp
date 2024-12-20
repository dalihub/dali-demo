/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include <dali-scene3d/dali-scene3d.h>
#include <dali-toolkit/dali-toolkit.h>
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
 * This example shows how to create and display a Model control.
 * The application can load 3D models in different formats (i.e. glTF, USD, and DLI) to Model control.
 * Each model has different material. BoomBox shows glossy or matt plastic material.
 * DamagedHelmet shows a kind of reflective glass and metallic object.
 * Microphone shows a roughness of metallic objects.
 * and Lantern shows a realistic difference between wood object and metallic object.
 * Rotate the camera by swiping.
 * A double tap changes the model.
 */

namespace
{
const Vector3 DEFAULT_CAMERA_POSITION(53.0f, -1300.0f, 3200.0f);

struct ModelInfo
{
  const char*   name;      ///< The name of the model.
  const Vector2 size;      ///< The size of the model
  const float   yPosition; ///< The position of the model in the Y axis.
};

const ModelInfo model_list[] =
  {
    /**
     * For the BoxAnimated.glb
     * Donated by Cesium for glTF testing.
     * Take from https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BoxAnimated
     */
    {"BoxAnimated.glb", Vector2(300.0f, 300.0f), 100.0f},
    /**
     * For the quantized Duck.gltf and its Assets
     * Created by Sony Computer Entertainment Inc.
     * Licensed under the SCEA Shared Source License, Version 1.0
     * Take from https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Duck/glTF-Quantized
     */
    {"Duck.gltf", Vector2(600.0f, 600.0f), 300.0f},
    /**
     * For the Lantern.gltf and its Assets
     * Donated by Microsoft for glTF testing
     * Created by Ryan Martin
     * Take from https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Lantern
     */
    {"Lantern.gltf", Vector2(600.0f, 600.0f), 300.0f},
    /**
     * For the BoomBox.gltf and its Assets
     * Donated by Microsoft for glTF testing
     * Created by Ryan Martin
     * Take from https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BoomBox
     */
    {"BoomBox.gltf", Vector2(600.0f, 600.0f), 0.0f},
    /**
     * For the DamagedHelmet.glb
     * Battle Damaged Sci-fi Helmet - PBR by theblueturtle_, published under a
     * Creative Commons Attribution-NonCommercial license
     * https://sketchfab.com/models/b81008d513954189a063ff901f7abfe4
     */
    {"DamagedHelmet.glb", Vector2(600.0f, 600.0f), 0.0f},
    /**
     * For the microphone.gltf and its Assets
     * Microphone GXL 066 Bafhcteks by Gistold, published under a
     * Creative Commons Attribution-NonCommercial license
     * https://sketchfab.com/models/5172dbe9281a45f48cee8c15bdfa1831
     */
    {"microphone.gltf", Vector2(600.0f, 600.0f), 0.0f},
    /**
     * Converted from the AntiqueCamera glTF file and its Assets
     * Old camera model by Maximilian Kamps
     * Dontated by UX3D for glTF testing
     * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/AntiqueCamera
     */
    {"AntiqueCamera.usdz", Vector2(60.0f, 60.0f), 350.0f},
    /**
     * Converted from the BarramundiFish glTF file and its Assets
     * Donated by Microsoft for glTF testing
     * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/BarramundiFish
     */
    {"BarramundiFish.usdz", Vector2(2500.0f, 2500.0f), 300.0f},
    /**
     * Converted from the CesiumMilkTruck glTF file and its Assets
     * Published under a Creative Commons Attribution 4.0 International License
     * Donated by Cesium for glTF testing
     * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/CesiumMilkTruck
     */
    {"CesiumMilkTruck.usdz", Vector2(300.0f, 300.0f), 300.0f},
    /**
     * Converted from the Corset glTF file and its Assets
     * Donated by Microsoft for glTF testing
     * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/Corset
     */
    {"Corset.usdz", Vector2(12000.0f, 12000.0f), 300.0f},
    /**
     * Converted from the WaterBottle glTF file and its Assets
     * Donated by Microsoft for glTF testing
     * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/WaterBottle
     */
    {"WaterBottle.usdz", Vector2(3000.0f, 3000.0f), 0.0f},
    /**
     * For the beer_model.dli and its Assets
     * This model includes a bottle of beer and cube box.
     */
    {"beer_model.dli", Vector2(600.0f, 600.0f), 0.0f},
    /**
     * For the exercise_model.dli and its Assets
     * This model includes a sportsman
     */
    {"exercise_model.dli", Vector2(600.0f, 600.0f), 0.0f},
};

const int32_t NUM_OF_MODELS = sizeof(model_list) / sizeof(model_list[0]);

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
const std::string uri_cube_diffuse_texture(imagedir + "forest_diffuse_cubemap.png");
const std::string uri_diffuse_texture(imagedir + "Studio/Irradiance.ktx");
const std::string uri_specular_texture(imagedir + "Studio/Radiance.ktx");

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
 * This example shows how to render 3D model with Model
 * How to test
 *  - Input UP or DOWN key to make the model rotate or stop.
 *  - Input LEFT or RIGHT key to change 3D model
 *  - Double Touch also changes 3D model.
 */
class Scene3DModelExample : public ConnectionTracker
{
public:
  Scene3DModelExample(Application& application)
  : mApplication(application),
    mModelOrientation(),
    mAnimationStop(false)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &Scene3DModelExample::Create);
  }

  ~Scene3DModelExample()
  {
    if(mAnimation)
    {
      mAnimation.Stop();
    }
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    mWindow = application.GetWindow();
    mWindow.GetRootLayer().SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_3D);

    // Get a handle to the mWindow
    mWindow.SetBackgroundColor(Color::WHITE);

    // Create a text label for error message when a model is failed to load
    mTextLabel = TextLabel::New();
    mTextLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mTextLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mTextLabel.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
    mTextLabel.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::CENTER);
    mTextLabel.SetProperty(TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE);
    mWindow.Add(mTextLabel);

    mTextLabel.SetProperty(Actor::Property::VISIBLE, false);

    RenderTask renderTask = mWindow.GetRenderTaskList().GetTask(0);
    renderTask.SetCullMode(false);

    mCurrentModel = 0u;
    CreateSceneFromModel(mCurrentModel);
    SetCameraActor();
    CreateSkybox();
    SetAnimation();

    // Respond to a click anywhere on the mWindow
    mWindow.GetRootLayer().TouchedSignal().Connect(this, &Scene3DModelExample::OnTouch);
    mWindow.KeyEventSignal().Connect(this, &Scene3DModelExample::OnKeyEvent);
    mWindow.GetRootLayer().WheelEventSignal().Connect(this, &Scene3DModelExample::OnWheel);

    mDoubleTap     = false;
    mDoubleTapTime = Timer::New(150);
    mDoubleTapTime.TickSignal().Connect(this, &Scene3DModelExample::OnDoubleTapTime);
  }

  bool OnWheel(Actor actor, const WheelEvent& wheelEvent)
  {
    mWheelDelta -= wheelEvent.GetDelta() * 0.025f;
    mWheelDelta = std::max(0.5f, mWheelDelta);
    mWheelDelta = std::min(2.0f, mWheelDelta);

    if(mModel)
    {
      mModel.SetProperty(Actor::Property::SCALE, mWheelDelta);
    }

    return true;
  }

  bool OnDoubleTapTime()
  {
    mDoubleTap = false;
    return true;
  }

  void CreateSceneFromModel(uint32_t index)
  {
    mReadyToLoad = false;
    if(mModel)
    {
      mWindow.GetRootLayer().Remove(mModel);
    }

    std::string modelUrl = modeldir;
    modelUrl += model_list[index].name;

    mModel = Dali::Scene3D::Model::New(modelUrl);
    mModel.SetProperty(Dali::Actor::Property::SIZE, model_list[index].size);
    mModel.SetProperty(Dali::Actor::Property::POSITION_Y, model_list[index].yPosition);
    mModel.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mModel.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mModel.SetImageBasedLightSource(uri_diffuse_texture, uri_specular_texture, 0.6f);

    mModel.ResourceReadySignal().Connect(this, &Scene3DModelExample::ResourceReady);
    mModel.LoadCompletedSignal().Connect(this, &Scene3DModelExample::LoadCompleted);

    mWindow.Add(mModel);
  }

  void ResourceReady(Control control)
  {
    mReadyToLoad = true;
    if(mModel.GetAnimationCount() > 0)
    {
      Animation animation = (std::string("exercise_model.dli") == model_list[mCurrentModel].name) ? mModel.GetAnimation("idleToSquatClip_0") : mModel.GetAnimation(0u);
      animation.Play();
      animation.SetLoopCount(0);
    }
  }

  void LoadCompleted(Dali::Scene3D::Model model, bool succeeded)
  {
    if(!succeeded)
    {
      // Show an error message
      std::ostringstream messageStream;
      messageStream << "Failed to load " << model_list[mCurrentModel].name;
      mTextLabel.SetProperty(TextLabel::Property::TEXT, messageStream.str());
      mTextLabel.SetProperty(TextLabel::Property::ENABLE_AUTO_SCROLL, true);
      mTextLabel.SetProperty(Actor::Property::VISIBLE, true);
    }

    // Current model finishes loading (regardless of successfully or not)
    // So allow to load the next model
    mReadyToLoad = true;
  }

  void SetCameraActor()
  {
    mCameraActor = mWindow.GetRenderTaskList().GetTask(0).GetCameraActor();
    mCameraActor.SetProperty(Dali::Actor::Property::POSITION, DEFAULT_CAMERA_POSITION);
    mCameraPosition = mCameraActor.GetProperty<Vector3>(Dali::Actor::Property::POSITION);
    mCameraActor.SetType(Dali::Camera::LOOK_AT_TARGET);
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

    Dali::Scene3D::Loader::EnvironmentMapData environmentMapData;
    Dali::Scene3D::Loader::LoadEnvironmentMap(uri_cube_diffuse_texture, environmentMapData);
    Texture texture = environmentMapData.GetTexture();

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
    mAnimation.AnimateBetween(Property(mModel, Dali::Actor::Property::ORIENTATION), keyframes, Animation::Interpolation::LINEAR);
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
    if(!mReadyToLoad)
    {
      return;
    }

    // Hide any error message
    mTextLabel.SetProperty(Actor::Property::VISIBLE, false);
    mTextLabel.SetProperty(TextLabel::Property::ENABLE_AUTO_SCROLL, false);

    mCurrentModel += direction;
    if(mCurrentModel >= NUM_OF_MODELS)
    {
      mCurrentModel = 0;
    }
    if(mCurrentModel < 0)
    {
      mCurrentModel = NUM_OF_MODELS - 1;
    }
    CreateSceneFromModel(mCurrentModel);
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

  Vector3              mCameraPosition;
  Dali::Scene3D::Model mModel;

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

  int32_t mCurrentModel{0};

  bool mReadyToLoad{true};

  TextLabel mTextLabel;
};

int32_t DALI_EXPORT_API main(int32_t argc, char** argv)
{
  Application         application = Application::New(&argc, &argv);
  Scene3DModelExample test(application);
  application.MainLoop();
  return 0;
}
