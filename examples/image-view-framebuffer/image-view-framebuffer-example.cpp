/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include "custom-shader.h"
#include "shared/view.h"

using namespace Dali;

namespace
{
const char* BIG_TEST_IMAGE( DEMO_IMAGE_DIR "book-landscape-cover.jpg" );
//const char* BIG_TEST_IMAGE( DEMO_IMAGE_DIR "Dino-Painted_lambert2SG_nmap.png" );

const char * const APPLICATION_TITLE( "Image View URL" );
const char * const TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char * const BUTTON_ICON( DEMO_IMAGE_DIR "icon-change.png" );
const char * const BUTTON_ICON_SELECTED( DEMO_IMAGE_DIR "icon-change-selected.png" );

const char* DELTA_UNIFORM_NAME = "uDelta";

const Vector2 TARGET_SIZE(800.f, 800.f);
}

class ImageViewUrlApp : public ConnectionTracker
{
public:
  ImageViewUrlApp( Application& application )
  : mApplication( application ),
    mDeltaPropertyIndex( Property::INVALID_INDEX )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageViewUrlApp::Create );
  }

  ~ImageViewUrlApp() = default;

private:
  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &ImageViewUrlApp::OnKeyEvent);

    CreateRenderTask(  );
    CreateScene( );
  }

  void CreateRenderTask()
  {
    auto rootActor = Stage::GetCurrent().GetRootLayer();

    auto cameraActor = CameraActor::New(TARGET_SIZE);
    cameraActor.SetParentOrigin(ParentOrigin::CENTER);
    rootActor.Add(cameraActor);


    // create actor to render input with applied shader
    mActorForInput = Toolkit::ImageView::New(BIG_TEST_IMAGE);
    mActorForInput.SetParentOrigin(ParentOrigin::CENTER);
    mActorForInput.SetSize(TARGET_SIZE);
    mActorForInput.SetName("ActorForInput");

    mDeltaPropertyIndex = mActorForInput.RegisterProperty(DELTA_UNIFORM_NAME, 0.f);

    rootActor.Add(mActorForInput);

    auto anim = Animation::New(5.0f);
    anim.AnimateTo( Property( mActorForInput, Actor::Property::ORIENTATION ), AngleAxis( Degree(360.0f), Vector3::ZAXIS ) );
    anim.SetLooping(true);
    anim.Play();

    RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

    // perform a horizontal blur targeting the internal buffer
    auto renderTask = taskList.CreateTask();
    renderTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
    renderTask.SetSourceActor(mActorForInput);
    renderTask.SetExclusive(true);
    renderTask.SetInputEnabled(false);
    renderTask.SetClearColor(Vector4(1.,0.,1.,1.));
    renderTask.SetClearEnabled(true);
    renderTask.SetCameraActor(cameraActor);

    mOutputTexture = Texture::New( TextureType::TEXTURE_2D,
                                   Pixel::RGBA8888,
                                   unsigned(TARGET_SIZE.width),
                                   unsigned(TARGET_SIZE.height) );

    auto framebuffer = FrameBuffer::New( TARGET_SIZE.width, TARGET_SIZE.height,
                                         FrameBuffer::Attachment::Mask::NONE );
    framebuffer.AttachColorTexture( mOutputTexture );
    renderTask.SetFrameBuffer(framebuffer);
  }

  void CreateScene( )
  {
    auto url = Dali::Toolkit::TextureManager::AddTexture(mOutputTexture);
    mImageView = Toolkit::ImageView::New(url);

    mImageView.SetParentOrigin(ParentOrigin::CENTER);
    mImageView.SetAnchorPoint(AnchorPoint::CENTER);
    mImageView.SetSize( 300, 600 );
    mImageView.SetName( "OutputActor" );

    Stage::GetCurrent().Add(mImageView);
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
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;
  Toolkit::ImageView mImageView;
  Animation mAnimation;
  Actor mActorForInput;
  Property::Index mDeltaPropertyIndex;
  Texture mOutputTexture;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  std::vector<const char*> newArgv = { "image-view-url.example", "-w", "1280", "-h", "1080" };
  argc = newArgv.size();
  argv = const_cast<char**>(&newArgv[0]);

  Application application = Application::New( &argc, &argv );
  ImageViewUrlApp test( application );
  application.MainLoop();
  return 0;
}
