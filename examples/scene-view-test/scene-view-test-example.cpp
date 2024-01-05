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

// EXTERNAL INCLUDES
#include <dali-scene3d/dali-scene3d.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit/devel-api/focus-manager/keyboard-focus-manager-devel.h>
#include <dali/devel-api/actors/actor-devel.h>

// INTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/file-stream.h>

using namespace Dali;
using namespace Toolkit;
using namespace Scene3D;

namespace
{
const char* CUBEMAP_SKY_BOX_URL    = DEMO_IMAGE_DIR "veste_oberhaus_cubemap.png";
const char* CUBEMAP_IRRADIANCE_URL = DEMO_IMAGE_DIR "veste_oberhaus_irradiance.png";
const char* CAMERA_NAME("MyCamera");

const char*       MODEL_URL = DEMO_MODEL_DIR "SphereMetallic.gltf";
constexpr Vector3 MODEL_SIZE(300.0f, 300.0f, 300.0f);
const char*       SUB_MODEL_URL = DEMO_MODEL_DIR "Duck.gltf";
constexpr Vector3 SUB_MODEL_SIZE(300.0f, 300.0f, 300.0f);
constexpr Vector3 SUB_MODEL_SCALE(1.2f, 0.6f, 1.0f);

constexpr Vector4 ORIGINAL_BASE_COLOR_FACTOR(0.6039215686274509f, 0.6039215686274509f, 0.6039215686274509f, 1.0f);
constexpr Vector3 ORIGINAL_EMISSIVE_FACTOR(0.0f, 0.0f, 0.0f);
constexpr float   ORIGINAL_METALLIC_FACTOR(1.0f);
constexpr float   ORIGINAL_ROUGHNESS_FACTOR(0.0f);

constexpr Vector4 ORIGINAL_SUB_BASE_COLOR_FACTOR(1.0f, 1.0f, 1.0f, 1.0f);
constexpr Vector3 ORIGINAL_SUB_EMISSIVE_FACTOR(0.0f, 0.0f, 0.0f);
constexpr float   ORIGINAL_SUB_METALLIC_FACTOR(0.0f);
constexpr float   ORIGINAL_SUB_ROUGHNESS_FACTOR(1.0f);

constexpr Vector4 FOCUSED_BASE_COLOR_FACTOR(1.0f, 1.0f, 1.0f, 1.0f);
constexpr Vector3 FOCUSED_EMISSIVE_FACTOR(0.1f, 0.0f, 0.0f);
constexpr float   FOCUSED_METALLIC_FACTOR(0.2f);
constexpr float   FOCUSED_ROUGHNESS_FACTOR(0.5f);

constexpr uint8_t FRAMEBUFFER_MULTI_SAMPLING_LEVEL = 4u;

constexpr uint32_t MAX_BACKLOG_COUNT = 10;

constexpr float RESOLUTION_RATE_DOWNSCALED      = 0.25f;
constexpr float RESOLUTION_RATE_UPSCALED        = 1.5f;
constexpr float RESOLUTION_RATE_OVER_DOWNSCALED = 0.1f;
constexpr float RESOLUTION_RATE_OVER_UPSCALED   = 2.5f;

const Quaternion MODEL_ROTATION(Degree(360.f), Vector3(0.1f, 1.0f, -0.1f));

enum ResolutionType
{
  DEFAULT = 0,
  DOWNSCALED,
  UPSCALED,

  OVER_DOWNSCALED,
  OVER_UPSCALED,

  MAX,
};

void ApplyAllMaterialPropertyRecursively(Scene3D::ModelNode modelNode, const std::vector<KeyValuePair>& materialPropertyValues)
{
  if(!modelNode)
  {
    return;
  }

  for(uint32_t primitiveIndex = 0u; primitiveIndex < modelNode.GetModelPrimitiveCount(); ++primitiveIndex)
  {
    Scene3D::ModelPrimitive primitive = modelNode.GetModelPrimitive(primitiveIndex);
    if(primitive)
    {
      Scene3D::Material material = primitive.GetMaterial();
      if(material)
      {
        for(const auto& keyValuePair : materialPropertyValues)
        {
          if(keyValuePair.first.type == Property::Key::Type::INDEX)
          {
            material.SetProperty(keyValuePair.first.indexKey, keyValuePair.second);
          }
        }
      }
    }
  }

  for(uint32_t childIndex = 0u; childIndex < modelNode.GetChildCount(); ++childIndex)
  {
    Scene3D::ModelNode childNode = Scene3D::ModelNode::DownCast(modelNode.GetChildAt(childIndex));
    ApplyAllMaterialPropertyRecursively(childNode, materialPropertyValues);
  }
}

} // namespace

class SceneViewExample : public ConnectionTracker
{
public:
  SceneViewExample(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &SceneViewExample::Create);
  }

  ~SceneViewExample() = default;

private:
  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    mWindow                  = application.GetWindow();
    const Vector2 windowSize = mWindow.GetSize();

    // Create a SceneView and set the Skybox
    mSceneView = Handle::New<SceneView>({{Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER},
                                         {Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER},
                                         {Actor::Property::SIZE, windowSize}});
    mSceneView.SetSkybox(CUBEMAP_SKY_BOX_URL);
    mWindow.Add(mSceneView);

    // Load the model and set IBL
    mMainModel = Model::New(MODEL_URL);
    mMainModel.SetProperties({
      {Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER},
      {Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER},
      {Actor::Property::POSITION, Vector3::ZERO},
      {Actor::Property::SIZE, MODEL_SIZE},
      {Actor::Property::NAME, "MainModel"},
      {Actor::Property::KEYBOARD_FOCUSABLE, true},
      {DevelActor::Property::TOUCH_FOCUSABLE, true},
    });
    mMainModel.SetImageBasedLightSource(CUBEMAP_IRRADIANCE_URL, CUBEMAP_SKY_BOX_URL);
    mSceneView.Add(mMainModel);

    mSubModel = Model::New(SUB_MODEL_URL);
    mSubModel.SetProperties({
      {Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER},
      {Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER},
      {Actor::Property::POSITION, Vector3(MODEL_SIZE.x, MODEL_SIZE.y * 0.5f, 0.0f)},
      {Actor::Property::SIZE, SUB_MODEL_SIZE},
      {Actor::Property::SCALE, SUB_MODEL_SCALE},
      {Actor::Property::NAME, "SubModel"},
      {Actor::Property::KEYBOARD_FOCUSABLE, true},
      {DevelActor::Property::TOUCH_FOCUSABLE, true},
    });
    mSubModel.SetImageBasedLightSource(CUBEMAP_IRRADIANCE_URL, CUBEMAP_SKY_BOX_URL, 0.25f);
    mSceneView.Add(mSubModel);

    // Create a new camera and reparent as we want to rotate the camera around the origin
    CameraActor cameraActor = Handle::New<CameraActor>({{Actor::Property::NAME, CAMERA_NAME}});
    mSceneView.AddCamera(cameraActor);
    mSceneView.SelectCamera(CAMERA_NAME);
    cameraActor.SetType(Camera::LOOK_AT_TARGET);
    cameraActor.SetTargetPosition(Vector3::ZERO);
    cameraActor.Unparent();
    Actor rotatingActor = Handle::New<Actor>({{Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER},
                                              {Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER}});
    rotatingActor.Add(cameraActor);
    mSceneView.Add(rotatingActor);

    mCameraRotateAnimation = Animation::New(10.0f);
    mCameraRotateAnimation.SetLooping(true);
    mCameraRotateAnimation.AnimateBy(Property(rotatingActor, Actor::Property::ORIENTATION), MODEL_ROTATION);
    if(mRotateCamera)
    {
      mCameraRotateAnimation.Play();
    }

    // Respond to key events
    mWindow.KeyEventSignal().Connect(this, &SceneViewExample::OnKeyEvent);
    mWindow.ResizeSignal().Connect(this, &SceneViewExample::OnWindowResize);

    DevelKeyboardFocusManager::EnableDefaultAlgorithm(KeyboardFocusManager::Get(), true);

    mMainModel.KeyInputFocusGainedSignal().Connect(this, &SceneViewExample::OnFocusGained);
    mMainModel.KeyInputFocusLostSignal().Connect(this, &SceneViewExample::OnFocusLost);
    mSubModel.KeyInputFocusGainedSignal().Connect(this, &SceneViewExample::OnFocusGained);
    mSubModel.KeyInputFocusLostSignal().Connect(this, &SceneViewExample::OnFocusLost);
    KeyboardFocusManager::Get().FocusChangedSignal().Connect(this, &SceneViewExample::OnFocusChanged);

    mScreenExtentTimer = Timer::New(16);
    mScreenExtentTimer.TickSignal().Connect(this, &SceneViewExample::OnTick);
    if(mShowScreenExtent)
    {
      mScreenExtentTimer.Start();
    }

    ApplyDebugLog();
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
        if(KeyboardFocusManager::Get().GetCurrentFocusActor())
        {
          KeyboardFocusManager::Get().ClearFocus();
        }
        else
        {
          mApplication.Quit();
        }
      }

      if(event.GetKeyName() == "1")
      {
        mShowDebugLabel ^= 1;
      }
      else if(event.GetKeyName() == "2")
      {
        mUseFrameBuffer ^= 1;
        if(mSceneView)
        {
          mSceneView.UseFramebuffer(mUseFrameBuffer);
        }
      }
      else if(event.GetKeyName() == "3")
      {
        mUseFrameBufferMultiSample ^= 1;
        if(mSceneView)
        {
          mSceneView.SetFramebufferMultiSamplingLevel(mUseFrameBufferMultiSample ? FRAMEBUFFER_MULTI_SAMPLING_LEVEL : 0);
        }
      }
      else if(event.GetKeyName() == "4")
      {
        mResolutionType = static_cast<ResolutionType>(static_cast<int>(mResolutionType + 1) % ResolutionType::MAX);
        ApplyResolution();
      }
      else if(event.GetKeyName() == "5")
      {
        mRotateCamera ^= true;
        if(mCameraRotateAnimation)
        {
          if(!mRotateCamera && mCameraRotateAnimation.GetState() == Animation::PLAYING)
          {
            mCameraRotateAnimation.Stop();
          }
          else if(mRotateCamera && mCameraRotateAnimation.GetState() != Animation::PLAYING)
          {
            mCameraRotateAnimation.Play();
          }
        }
      }
      else if(event.GetKeyName() == "6")
      {
        mShowScreenExtent ^= true;
        if(mScreenExtentTimer)
        {
          if(!mShowScreenExtent && mScreenExtentTimer.IsRunning())
          {
            mScreenExtentTimer.Stop();
          }
          else if(mShowScreenExtent && !mScreenExtentTimer.IsRunning())
          {
            mScreenExtentTimer.Start();
          }
        }
        if(mMainModelAABB)
        {
          mMainModelAABB[Actor::Property::VISIBLE] = mShowScreenExtent;
        }
        if(mSubModelAABB)
        {
          mSubModelAABB[Actor::Property::VISIBLE] = mShowScreenExtent;
        }
      }
      ApplyDebugLog();
    }
  }

  void OnWindowResize(Window window, Window::WindowSize newSize)
  {
    if(mSceneView)
    {
      mSceneView[Actor::Property::SIZE_WIDTH]  = newSize.GetWidth();
      mSceneView[Actor::Property::SIZE_HEIGHT] = newSize.GetHeight();
      ApplyResolution();
    }
  }

  void OnFocusChanged(Actor from, Actor to)
  {
    if(!from && !to)
    {
      return;
    }
    std::ostringstream oss;
    oss << "Focus changed from : [";
    oss << (from ? from.GetProperty<std::string>(Actor::Property::NAME) : "(nil)");
    oss << "] -> [";
    oss << (to ? to.GetProperty<std::string>(Actor::Property::NAME) : "(nil)");
    oss << "]";
    ApplyBackLog(oss.str());
  }

  void OnFocusGained(Control control)
  {
    std::ostringstream oss;
    oss << "Focus gained : [";
    oss << (control ? control.GetProperty<std::string>(Actor::Property::NAME) : "(nil)");
    oss << "]";
    ApplyBackLog(oss.str());

    Model model = Model::DownCast(control);
    if(model)
    {
      ApplyAllMaterialPropertyRecursively(
        model.GetModelRoot(),
        {
          {Material::Property::BASE_COLOR_FACTOR, FOCUSED_BASE_COLOR_FACTOR},
          {Material::Property::EMISSIVE_FACTOR, FOCUSED_EMISSIVE_FACTOR},
          {Material::Property::METALLIC_FACTOR, FOCUSED_METALLIC_FACTOR},
          {Material::Property::ROUGHNESS_FACTOR, FOCUSED_ROUGHNESS_FACTOR},
        });
    }
  }

  void OnFocusLost(Control control)
  {
    std::ostringstream oss;
    oss << "Focus lost : [";
    oss << (control ? control.GetProperty<std::string>(Actor::Property::NAME) : "(nil)");
    oss << "]";
    ApplyBackLog(oss.str());

    Model model = Model::DownCast(control);
    if(model)
    {
      bool isSubModel = model.GetProperty<std::string>(Actor::Property::NAME) == "SubModel";
      ApplyAllMaterialPropertyRecursively(
        model.GetModelRoot(),
        {
          {Material::Property::BASE_COLOR_FACTOR, isSubModel ? ORIGINAL_SUB_BASE_COLOR_FACTOR : ORIGINAL_BASE_COLOR_FACTOR},
          {Material::Property::EMISSIVE_FACTOR, isSubModel ? ORIGINAL_SUB_EMISSIVE_FACTOR : ORIGINAL_EMISSIVE_FACTOR},
          {Material::Property::METALLIC_FACTOR, isSubModel ? ORIGINAL_SUB_METALLIC_FACTOR : ORIGINAL_METALLIC_FACTOR},
          {Material::Property::ROUGHNESS_FACTOR, isSubModel ? ORIGINAL_SUB_ROUGHNESS_FACTOR : ORIGINAL_ROUGHNESS_FACTOR},
        });
    }
  }

  bool OnTick()
  {
    if(mShowScreenExtent)
    {
      RecalculateScreenExtent();
    }
    return true;
  }

private:
  void ApplyResolution()
  {
    if(mWindow && mSceneView)
    {
      Window::WindowSize windowSize = mWindow.GetSize();
      switch(mResolutionType)
      {
        case ResolutionType::DEFAULT:
        default:
        {
          mResolutionRate = 0.0f;
          break;
        }
        case ResolutionType::DOWNSCALED:
        {
          mResolutionRate = RESOLUTION_RATE_DOWNSCALED;
          break;
        }
        case ResolutionType::UPSCALED:
        {
          mResolutionRate = RESOLUTION_RATE_UPSCALED;
          break;
        }
        case ResolutionType::OVER_DOWNSCALED:
        {
          mResolutionRate = RESOLUTION_RATE_OVER_DOWNSCALED;
          break;
        }
        case ResolutionType::OVER_UPSCALED:
        {
          mResolutionRate = RESOLUTION_RATE_OVER_UPSCALED;
          break;
        }
      }

      if(Dali::EqualsZero(mResolutionRate))
      {
        mSceneView.ResetResolution();
      }
      else
      {
        mSceneView.SetResolution(static_cast<uint32_t>(windowSize.GetWidth() * mResolutionRate), static_cast<uint32_t>(windowSize.GetHeight() * mResolutionRate));
      }
    }
  }

  void ApplyDebugLog()
  {
    if(!mDebugLabel)
    {
      mDebugLabel                                  = TextLabel::New();
      mDebugLabel[Actor::Property::PARENT_ORIGIN]  = ParentOrigin::TOP_LEFT;
      mDebugLabel[Actor::Property::ANCHOR_POINT]   = AnchorPoint::TOP_LEFT;
      mDebugLabel[TextLabel::Property::MULTI_LINE] = true;
      mDebugLabel[Control::Property::BACKGROUND]   = Vector4(1.0f, 1.0f, 1.0f, 0.2f);
    }

    if(mShowDebugLabel)
    {
      if(mWindow && mDebugLabel && !mDebugLabel.GetParent())
      {
        mWindow.GetOverlayLayer().Add(mDebugLabel);
      }
      std::ostringstream oss;
      oss << "Key 1 : Toggle to show debug log\n";
      oss << "Key 2 : Toggle to use Framebuffer. Currently " << (mUseFrameBuffer ? "ON" : "OFF") << "\n";
      oss << "Key 3 : Toggle to use FramebufferMultiSampling. Currently " << (mUseFrameBufferMultiSample ? FRAMEBUFFER_MULTI_SAMPLING_LEVEL : 0) << "\n";
      oss << "Key 4 : Toggle to use SetResolution. Currently ";
      oss << (mResolutionType == ResolutionType::DEFAULT ? "DEFAULT" : mResolutionType == ResolutionType::DOWNSCALED      ? "DOWNSCALED"
                                                                     : mResolutionType == ResolutionType::UPSCALED        ? "UPSCALED"
                                                                     : mResolutionType == ResolutionType::OVER_DOWNSCALED ? "OVER_DOWNSCALED"
                                                                     : mResolutionType == ResolutionType::OVER_UPSCALED   ? "OVER_UPSCALED"
                                                                                                                          : "ERROR")
          << " (Scale rate : " << mResolutionRate << ")\n";
      oss << "Key 5 : Toggle to Rotate camera. Currently " << (mRotateCamera ? "ON" : "OFF") << "\n";
      oss << "Key 6 : Toggle to show Model ScreenExtent. Currently " << (mShowScreenExtent ? "ON" : "OFF") << "\n";

      for(const auto& log : mBackLog)
      {
        oss << log;
        if(log.length() == 0 || log.back() != '\n')
        {
          oss << "\n";
        }
      }

      mDebugLabel[TextLabel::Property::TEXT] = oss.str();
    }
    else
    {
      if(mDebugLabel)
      {
        mDebugLabel.Unparent();
      }
    }
  }

  void ApplyBackLog(std::string message)
  {
    if(mBackLog.size() == MAX_BACKLOG_COUNT)
    {
      mBackLog.clear();
    }
    mBackLog.push_back(message);

    ApplyDebugLog();
  }

  void ApplyModelScreenExtent(Control aabb, Model model)
  {
    Rect<float> screenExtent = Dali::DevelActor::CalculateCurrentScreenExtents(model);

    aabb[Actor::Property::POSITION_X]  = screenExtent.x;
    aabb[Actor::Property::POSITION_Y]  = screenExtent.y;
    aabb[Actor::Property::SIZE_WIDTH]  = screenExtent.width;
    aabb[Actor::Property::SIZE_HEIGHT] = screenExtent.height;
  }

private:
  void RecalculateScreenExtent()
  {
    if(mWindow && !mMainModelAABB)
    {
      mMainModelAABB = Control::New();
      mMainModelAABB.SetProperties({
        {Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT},
        {Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT},
        {Control::Property::BACKGROUND, Vector4(0.0f, 0.0f, 1.0f, 0.2f)},
      });
      mWindow.GetOverlayLayer().Add(mMainModelAABB);
    }
    if(mWindow && !mSubModelAABB)
    {
      mSubModelAABB = Control::New();
      mSubModelAABB.SetProperties({
        {Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT},
        {Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT},
        {Control::Property::BACKGROUND, Vector4(1.0f, 0.0f, 0.0f, 0.2f)},
      });
      mWindow.GetOverlayLayer().Add(mSubModelAABB);
    }

    ApplyModelScreenExtent(mMainModelAABB, mMainModel);
    ApplyModelScreenExtent(mSubModelAABB, mSubModel);
  }

private:
  Application& mApplication;
  Window       mWindow;
  SceneView    mSceneView;

  Model mMainModel;
  Model mSubModel;

  Animation mCameraRotateAnimation;

  // State of demo
  TextLabel mDebugLabel;
  bool      mShowDebugLabel{true};

  std::vector<std::string> mBackLog;

  Timer   mScreenExtentTimer;
  Control mMainModelAABB;
  Control mSubModelAABB;

  bool mUseFrameBuffer{false};
  bool mUseFrameBufferMultiSample{false};
  bool mRotateCamera{true};
  bool mShowScreenExtent{false};

  ResolutionType mResolutionType{ResolutionType::DEFAULT};
  float          mResolutionRate{0.0f};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application      application = Application::New(&argc, &argv);
  SceneViewExample test(application);
  application.MainLoop();
  return 0;
}
