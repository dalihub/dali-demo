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
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>

#include "shared/view.h"
#include "generated/image-view-url-frag.h"

using namespace Dali;

namespace
{
const char* BIG_TEST_IMAGE(DEMO_IMAGE_DIR "book-landscape-cover.jpg");

const char* const APPLICATION_TITLE("Image View URL");
const char* const TOOLBAR_IMAGE(DEMO_IMAGE_DIR "top-bar.png");
const char* const BUTTON_ICON(DEMO_IMAGE_DIR "icon-change.png");
const char* const BUTTON_ICON_SELECTED(DEMO_IMAGE_DIR "icon-change-selected.png");

const char* DELTA_UNIFORM_NAME = "uDelta";

const Vector2 TARGET_SIZE(800.f, 800.f);
} // namespace

class ImageViewUrlApp : public ConnectionTracker
{
public:
  ImageViewUrlApp(Application& application, std::string url)
  : mApplication(application),
    mUrl(url),
    mDeltaPropertyIndex(Property::INVALID_INDEX)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ImageViewUrlApp::Create);
  }

  ~ImageViewUrlApp() = default;

private:
  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.KeyEventSignal().Connect(this, &ImageViewUrlApp::OnKeyEvent);

    Toolkit::ToolBar toolBar;
    Toolkit::Control background;
    // Creates a default view with a default tool bar.
    mContent = DemoHelper::CreateView(application,
                                      background,
                                      toolBar,
                                      "",
                                      TOOLBAR_IMAGE,
                                      APPLICATION_TITLE);

    // Add a button to switch the scene. (right of toolbar)
    Toolkit::PushButton switchButton = Toolkit::PushButton::New();
    switchButton.SetProperty(Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, BUTTON_ICON);
    switchButton.SetProperty(Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, BUTTON_ICON_SELECTED);
    switchButton.ClickedSignal().Connect(this, &ImageViewUrlApp::OnButtonClicked);
    toolBar.AddControl(switchButton,
                       DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage,
                       Toolkit::Alignment::HORIZONTAL_RIGHT,
                       DemoHelper::DEFAULT_MODE_SWITCH_PADDING);

    std::string url = mUrl;
    if(url.empty())
    {
      url = BIG_TEST_IMAGE;
    }

    CreateRenderTask(url);
    CreateScene();
  }

  void CreateRenderTask(const std::string& url)
  {
    auto rootActor = mApplication.GetWindow().GetRootLayer();

    auto cameraActor = CameraActor::New(TARGET_SIZE);
    cameraActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    cameraActor.SetInvertYAxis(true);
    rootActor.Add(cameraActor);

    {
      // create actor to render input with applied shader
      mActorForInput = Toolkit::ImageView::New(url);
      mActorForInput.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      mActorForInput.SetProperty(Actor::Property::SIZE, TARGET_SIZE);
      Property::Map customShader;
      customShader[Toolkit::Visual::Shader::Property::FRAGMENT_SHADER] = SHADER_IMAGE_VIEW_URL_FRAG.data();
      Property::Map visualMap;
      visualMap.Insert(Toolkit::Visual::Property::SHADER, customShader);
      mActorForInput.SetProperty(Toolkit::ImageView::Property::IMAGE, visualMap);

      mDeltaPropertyIndex = mActorForInput.RegisterProperty(DELTA_UNIFORM_NAME, 0.f);

      rootActor.Add(mActorForInput);

      RenderTaskList taskList = mApplication.GetWindow().GetRenderTaskList();

      // perform a horizontal blur targeting the internal buffer
      auto renderTask = taskList.CreateTask();
      renderTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
      renderTask.SetSourceActor(mActorForInput);
      renderTask.SetExclusive(true);
      renderTask.SetInputEnabled(false);
      renderTask.SetClearColor(Vector4(1., 0., 0., 1.));
      renderTask.SetClearEnabled(true);
      renderTask.SetCameraActor(cameraActor);

      mOutputTexture   = Texture::New(TextureType::TEXTURE_2D,
                                    Pixel::RGB888,
                                    unsigned(TARGET_SIZE.width),
                                    unsigned(TARGET_SIZE.height));
      auto framebuffer = FrameBuffer::New(TARGET_SIZE.width, TARGET_SIZE.height, FrameBuffer::Attachment::NONE);
      framebuffer.AttachColorTexture(mOutputTexture);

      renderTask.SetFrameBuffer(framebuffer);
    }
    {
      // animate the shader uniform
      mAnimation = Animation::New(10.f);

      mActorForInput.SetProperty(mDeltaPropertyIndex, 0.f);
      mAnimation.AnimateTo(Property(mActorForInput, mDeltaPropertyIndex), Math::PI * 2.f);
      mAnimation.SetLooping(true);
      mAnimation.Play();
    }
  }

  void CreateScene()
  {
    auto url   = Dali::Toolkit::TextureManager::AddTexture(mOutputTexture);
    mImageView = Toolkit::ImageView::New(url);

    mImageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mImageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mContent.Add(mImageView);
  }

  bool OnButtonClicked(Toolkit::Button button)
  {
    if(mAnimation.GetState() == Animation::State::PLAYING)
    {
      mAnimation.Pause();
    }
    else
    {
      mAnimation.Play();
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
  Application&       mApplication;
  std::string        mUrl;
  Layer              mContent;
  Toolkit::ImageView mImageView;
  Animation          mAnimation;
  Actor              mActorForInput;
  Property::Index    mDeltaPropertyIndex;
  Texture            mOutputTexture;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  std::string url;
  if(argc > 1)
  {
    url = argv[1];
  }

  ImageViewUrlApp test(application, url);
  application.MainLoop();
  return 0;
}
