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

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/dali.h>
#include <cstring>

// INTERNAL INCLUDES
#include "shared/utility.h"
#include "generated/native-image-source-texture-vert.h"
#include "generated/native-image-source-texture-frag.h"

using namespace Dali;
using namespace Toolkit;

namespace
{
const float BUTTON_HEIGHT = 100.0f;
const float BUTTON_COUNT  = 5.0f;

const std::string JPG_FILENAME     = DEMO_IMAGE_DIR "gallery-medium-4.jpg";
const std::string CAPTURE_FILENAME = DEMO_DATA_PUBLIC_RW_DIR "native-image-capture.png";

/**
 * @brief Creates a shader used to render a native image
 * @param[in] nativeImage The native image
 * @return A shader to render the native image
 */
Shader CreateShader(NativeImageInterface& nativeImage)
{
  static const char* DEFAULT_SAMPLER_TYPENAME = "sampler2D";

  std::string fragmentShader;

  //Get custom fragment shader prefix
  const char* fragmentPrefix = nativeImage.GetCustomFragmentPrefix();
  if(fragmentPrefix)
  {
    fragmentShader = fragmentPrefix;
    fragmentShader += SHADER_NATIVE_IMAGE_SOURCE_TEXTURE_FRAG.data();
  }
  else
  {
    fragmentShader = SHADER_NATIVE_IMAGE_SOURCE_TEXTURE_FRAG.data();
  }

  //Get custom sampler type name
  const char* customSamplerTypename = nativeImage.GetCustomSamplerTypename();
  if(customSamplerTypename)
  {
    fragmentShader.replace(fragmentShader.find(DEFAULT_SAMPLER_TYPENAME), strlen(DEFAULT_SAMPLER_TYPENAME), customSamplerTypename);
  }

  return Shader::New(SHADER_NATIVE_IMAGE_SOURCE_TEXTURE_VERT, fragmentShader);
}

} // namespace

// This example shows how to create and use a NativeImageSource as the target of the render task.
//
class NativeImageSourceController : public ConnectionTracker
{
public:
  NativeImageSourceController(Application& application)
  : mApplication(application),
    mRefreshAlways(true)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &NativeImageSourceController::Create);
  }

  ~NativeImageSourceController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    window.KeyEventSignal().Connect(this, &NativeImageSourceController::OnKeyEvent);

    CreateButtonArea();

    CreateContentAreas();
  }

  void CreateButtonArea()
  {
    Window  window     = mApplication.GetWindow();
    Vector2 windowSize = window.GetSize();

    mButtonArea = Layer::New();
    mButtonArea.SetProperty(Actor::Property::SIZE, Vector2(windowSize.x, BUTTON_HEIGHT));
    mButtonArea.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
    mButtonArea.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    window.Add(mButtonArea);

    mButtonShow = PushButton::New();
    mButtonShow.SetProperty(Button::Property::TOGGLABLE, true);
    mButtonShow.SetProperty(Toolkit::Button::Property::LABEL, "SHOW");
    mButtonShow.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mButtonShow.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mButtonShow.SetProperty(Actor::Property::SIZE, Vector2(windowSize.x / BUTTON_COUNT, BUTTON_HEIGHT));
    mButtonShow.ClickedSignal().Connect(this, &NativeImageSourceController::OnButtonSelected);
    mButtonArea.Add(mButtonShow);

    mButtonRefreshAlways = PushButton::New();
    mButtonRefreshAlways.SetProperty(Button::Property::TOGGLABLE, true);
    mButtonRefreshAlways.SetProperty(Toolkit::Button::Property::LABEL, "ALWAYS");
    mButtonRefreshAlways.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mButtonRefreshAlways.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mButtonRefreshAlways.SetProperty(Actor::Property::SIZE, Vector2(windowSize.x / BUTTON_COUNT, BUTTON_HEIGHT));
    mButtonRefreshAlways.SetProperty(Actor::Property::POSITION, Vector2((windowSize.x / BUTTON_COUNT) * 1.0f, 0.0f));
    mButtonRefreshAlways.StateChangedSignal().Connect(this, &NativeImageSourceController::OnButtonSelected);
    mButtonArea.Add(mButtonRefreshAlways);

    mButtonRefreshOnce = PushButton::New();
    mButtonRefreshOnce.SetProperty(Toolkit::Button::Property::LABEL, "ONCE");
    mButtonRefreshOnce.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mButtonRefreshOnce.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mButtonRefreshOnce.SetProperty(Actor::Property::SIZE, Vector2(windowSize.x / BUTTON_COUNT, BUTTON_HEIGHT));
    mButtonRefreshOnce.SetProperty(Actor::Property::POSITION, Vector2((windowSize.x / BUTTON_COUNT) * 2.0f, 0.0f));
    mButtonRefreshOnce.ClickedSignal().Connect(this, &NativeImageSourceController::OnButtonSelected);
    mButtonArea.Add(mButtonRefreshOnce);

    mButtonCapture = PushButton::New();
    mButtonCapture.SetProperty(Toolkit::Button::Property::LABEL, "CAPTURE");
    mButtonCapture.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mButtonCapture.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mButtonCapture.SetProperty(Actor::Property::SIZE, Vector2(windowSize.x / BUTTON_COUNT, BUTTON_HEIGHT));
    mButtonCapture.SetProperty(Actor::Property::POSITION, Vector2((windowSize.x / BUTTON_COUNT) * 3.0f, 0.0f));
    mButtonCapture.ClickedSignal().Connect(this, &NativeImageSourceController::OnButtonSelected);
    mButtonArea.Add(mButtonCapture);

    mButtonReset = PushButton::New();
    mButtonReset.SetProperty(Toolkit::Button::Property::LABEL, "RESET");
    mButtonReset.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mButtonReset.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mButtonReset.SetProperty(Actor::Property::SIZE, Vector2(windowSize.x / BUTTON_COUNT, BUTTON_HEIGHT));
    mButtonReset.SetProperty(Actor::Property::POSITION, Vector2((windowSize.x / BUTTON_COUNT) * 4.0f, 0.0f));
    mButtonReset.ClickedSignal().Connect(this, &NativeImageSourceController::OnButtonSelected);
    mButtonArea.Add(mButtonReset);
  }

  void CreateContentAreas()
  {
    Window  window     = mApplication.GetWindow();
    Vector2 windowSize = window.GetSize();

    float contentHeight((windowSize.y - BUTTON_HEIGHT) / 2.0f);

    mTopContentArea = Actor::New();
    mTopContentArea.SetProperty(Actor::Property::SIZE, Vector2(windowSize.x, contentHeight));
    mTopContentArea.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
    mTopContentArea.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    mTopContentArea.SetProperty(Actor::Property::POSITION_Y, BUTTON_HEIGHT);
    window.Add(mTopContentArea);

    mBottomContentArea = Actor::New();
    mBottomContentArea.SetProperty(Actor::Property::SIZE, Vector2(windowSize.x, contentHeight));
    mBottomContentArea.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
    mBottomContentArea.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
    window.Add(mBottomContentArea);

    mSourceActor = ImageView::New(JPG_FILENAME);
    mSourceActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mSourceActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mTopContentArea.Add(mSourceActor);

    Animation animation = Animation::New(2.f);
    Degree    relativeRotationDegrees(90.0f);
    Radian    relativeRotationRadians(relativeRotationDegrees);
    animation.AnimateTo(Property(mSourceActor, Actor::Property::ORIENTATION), Quaternion(relativeRotationRadians, Vector3::ZAXIS), AlphaFunction::LINEAR, TimePeriod(0.f, 0.5f));
    animation.AnimateBy(Property(mSourceActor, Actor::Property::ORIENTATION), Quaternion(relativeRotationRadians, Vector3::ZAXIS), AlphaFunction::LINEAR, TimePeriod(0.5f, 0.5f));
    animation.AnimateBy(Property(mSourceActor, Actor::Property::ORIENTATION), Quaternion(relativeRotationRadians, Vector3::ZAXIS), AlphaFunction::LINEAR, TimePeriod(1.f, 0.5f));
    animation.AnimateBy(Property(mSourceActor, Actor::Property::ORIENTATION), Quaternion(relativeRotationRadians, Vector3::ZAXIS), AlphaFunction::LINEAR, TimePeriod(1.5f, 0.5f));
    animation.SetLooping(true);
    animation.Play();

    TextLabel textLabel1 = TextLabel::New("Image");
    textLabel1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
    textLabel1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
    mTopContentArea.Add(textLabel1);

    // Wait until button press before creating mOffscreenRenderTask

    TextLabel textLabel2 = TextLabel::New("Native Image");
    textLabel2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
    textLabel2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
    mBottomContentArea.Add(textLabel2);
  }

  void SetupNativeImage()
  {
    if(!mOffscreenRenderTask)
    {
      Window  window     = mApplication.GetWindow();
      Vector2 windowSize = window.GetSize();

      float   contentHeight((windowSize.y - BUTTON_HEIGHT) / 2.0f);
      Vector2 imageSize(windowSize.x, contentHeight);

      mNativeImageSourcePtr = NativeImageSource::New(imageSize.width, imageSize.height, NativeImageSource::COLOR_DEPTH_DEFAULT);
      mNativeTexture        = Texture::New(*mNativeImageSourcePtr);

      mFrameBuffer = FrameBuffer::New(mNativeTexture.GetWidth(), mNativeTexture.GetHeight(), FrameBuffer::Attachment::NONE);
      mFrameBuffer.AttachColorTexture(mNativeTexture);

      mCameraActor = CameraActor::New(imageSize);
      mCameraActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      mCameraActor.SetProperty(Actor::Property::PARENT_ORIGIN, AnchorPoint::CENTER);
      mTopContentArea.Add(mCameraActor);

      RenderTaskList taskList = window.GetRenderTaskList();
      mOffscreenRenderTask    = taskList.CreateTask();
      mOffscreenRenderTask.SetSourceActor(mSourceActor);
      mOffscreenRenderTask.SetClearColor(Color::WHITE);
      mOffscreenRenderTask.SetClearEnabled(true);
      mOffscreenRenderTask.SetCameraActor(mCameraActor);
      mOffscreenRenderTask.GetCameraActor().SetInvertYAxis(true);
      mOffscreenRenderTask.SetFrameBuffer(mFrameBuffer);
    }

    if(mRefreshAlways)
    {
      mOffscreenRenderTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
    }
    else
    {
      mOffscreenRenderTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
    }
  }

  void SetupDisplayActor(bool show)
  {
    if(show)
    {
      if(!mDisplayActor)
      {
        // Make sure we have something to display
        SetupNativeImage();

        mDisplayActor = Actor::New();
        mDisplayActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
        mDisplayActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);

        Geometry geometry = DemoHelper::CreateTexturedQuad();

        Shader shader = CreateShader(*mNativeImageSourcePtr);

        Renderer renderer = Renderer::New(geometry, shader);

        TextureSet textureSet = TextureSet::New();
        textureSet.SetTexture(0u, mNativeTexture);
        renderer.SetTextures(textureSet);

        mDisplayActor.AddRenderer(renderer);
        mDisplayActor.SetProperty(Actor::Property::SIZE, Vector2(mNativeTexture.GetWidth(), mNativeTexture.GetHeight()));

        mBottomContentArea.Add(mDisplayActor);
      }
    }
    else
    {
      UnparentAndReset(mDisplayActor);
    }
  }

  void Capture()
  {
    mRefreshAlways = false;
    SetupNativeImage();

    mOffscreenRenderTask.FinishedSignal().Connect(this, &NativeImageSourceController::DoCapture);
  }

  void DoCapture(RenderTask& task)
  {
    task.FinishedSignal().Disconnect(this, &NativeImageSourceController::DoCapture);

    mNativeImageSourcePtr->EncodeToFile(CAPTURE_FILENAME);
  }

  void Reset()
  {
    SetupDisplayActor(false);

    Window         window   = mApplication.GetWindow();
    RenderTaskList taskList = window.GetRenderTaskList();
    taskList.RemoveTask(mOffscreenRenderTask);
    mOffscreenRenderTask.Reset();
    mCameraActor.Reset();

    mFrameBuffer.Reset();
    mNativeTexture.Reset();
    mNativeImageSourcePtr.Reset();
  }

  bool OnButtonSelected(Toolkit::Button button)
  {
    Toolkit::PushButton pushButton = Toolkit::PushButton::DownCast(button);

    if(pushButton == mButtonShow)
    {
      bool isSelected = mButtonShow.GetProperty(Toolkit::Button::Property::SELECTED).Get<bool>();

      SetupDisplayActor(isSelected);
    }
    else if(pushButton == mButtonRefreshAlways)
    {
      bool isSelected = mButtonRefreshAlways.GetProperty(Toolkit::Button::Property::SELECTED).Get<bool>();

      mRefreshAlways = isSelected;
      SetupNativeImage();
    }
    else if(pushButton == mButtonRefreshOnce)
    {
      bool isSelected = mButtonRefreshAlways.GetProperty(Toolkit::Button::Property::SELECTED).Get<bool>();

      if(isSelected)
      {
        mButtonRefreshAlways.SetProperty(Button::Property::SELECTED, false);
      }

      mRefreshAlways = false;
      SetupNativeImage();
    }
    else if(pushButton == mButtonCapture)
    {
      Capture();
    }
    else if(pushButton == mButtonReset)
    {
      Reset();
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

private:
  Application& mApplication;

  Layer mButtonArea;
  Actor mTopContentArea;
  Actor mBottomContentArea;

  PushButton mButtonShow;
  PushButton mButtonRefreshAlways;
  PushButton mButtonRefreshOnce;
  PushButton mButtonCapture;
  PushButton mButtonReset;

  Actor mSourceActor;

  NativeImageSourcePtr mNativeImageSourcePtr;
  Texture              mNativeTexture;
  FrameBuffer          mFrameBuffer;

  RenderTask  mOffscreenRenderTask;
  CameraActor mCameraActor;

  Actor mDisplayActor;

  bool mRefreshAlways;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                 application = Application::New(&argc, &argv);
  NativeImageSourceController test(application);
  application.MainLoop();
  return 0;
}
