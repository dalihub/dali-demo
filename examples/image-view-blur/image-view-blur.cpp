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
 */

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/builder/base64-encoding.h>
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include "gaussian-blur-frag-shader.h"
#include "utils.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const Vector2 TARGET_SIZE(900.f, 600.f);
const char* URL="https://upload.wikimedia.org/wikipedia/commons/2/2c/NZ_Landscape_from_the_van.jpg";
}

class ImageViewBlurExample : public ConnectionTracker
{
public:
  ImageViewBlurExample( Application& app )
  : mApp( app )
  {
    app.InitSignal().Connect(this, &ImageViewBlurExample::Create);
  }

  void Create(Application& app)
  {
    auto stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &ImageViewBlurExample::OnKeyEvent);
    stage.SetBackgroundColor( Vector4( 0.2f, 0.1f, 0.2f, 1.0f ) );

    mSceneActor = CreateScene( TARGET_SIZE );

    // Don't add to stage until it's ready.... Must use immediate only.
    // If image view's aren't added with loading policy = Immediate, then the render task cannot use REFRESH_ONCE,
    // and must run until de-activated

    // Build render tasks in ready callback.
    Stage::GetCurrent().Add( mSceneActor ); // Will make all text labels ready
    mSceneActor.SetVisible(false);
  }


  Actor CreateScene( Vector2 targetSize )
  {
    auto sceneActor = Actor::New();
    sceneActor.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
    sceneActor.SetParentOrigin( ParentOrigin::CENTER );
    auto sceneImage = ImageView::New();
    sceneImage.SetProperty(ImageView::Property::IMAGE, Property::Map()
                           .Add( ImageVisual::Property::URL, URL )
                           .Add( ImageVisual::Property::LOAD_POLICY, ImageVisual::LoadPolicy::IMMEDIATE ) );

    sceneImage.ResourceReadySignal().Connect( this, &ImageViewBlurExample::OnImageReady );

    sceneImage.SetSize( targetSize );
    sceneImage.SetParentOrigin( ParentOrigin::CENTER );
    sceneImage.SetAnchorPoint( AnchorPoint::CENTER );
    sceneImage.SetName("SceneImage");
    sceneActor.Add(sceneImage);

    auto sceneText = TextLabel::New( "Landscape photo");
    sceneText.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    sceneText.SetProperty( TextLabel::Property::POINT_SIZE, 40 );
    sceneText.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLACK );
    sceneText.SetProperty( TextLabel::Property::OUTLINE, Property::Map().Add("color",Color::WHITE).Add("width", 2) );
    sceneText.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    sceneText.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    sceneText.SetName("SceneText");
    sceneImage.Add(sceneText);
    return sceneActor;
  }

  void OnImageReady( Control control )
  {
    mSceneActor.SetVisible(true);
    CreateBlurView( mSceneActor, TARGET_SIZE ); // Renders the scene in an exclusive render task
  }

  /**
   * Create a scene render + 2 stage filter, reusing Framebuffers
   */
  void CreateBlurView( Actor sceneActor, Vector2 targetSize )
  {
    auto stage = Stage::GetCurrent();

    // Step 1 captures the scene into a framebuffer without any effects
    auto step1Output = Texture::New( TextureType::TEXTURE_2D,
                                     Pixel::RGBA8888,
                                     unsigned(targetSize.width),
                                     unsigned(targetSize.height) );

    // Should be final size
    auto fb1 = FrameBuffer::New(targetSize.width, targetSize.height, Pixel::RGBA8888);
    fb1.AttachColorTexture( step1Output );

    Utils::CreateRenderTask( sceneActor, targetSize, fb1 ); // Renders scene exclusively - it shouldn't be drawn to main fb
    auto step1Url = TextureManager::AddTexture( step1Output );

    mPreview1 = Utils::CreatePreview( step1Url, targetSize, ParentOrigin::TOP_LEFT, ParentOrigin::BOTTOM_LEFT,"Step 1", true );
    stage.Add( mPreview1 );

    // Step2 executes the first pass of the effect shader
    // Can be reduced size:
    Vector2 downscaledSize = targetSize * 0.5f;

    auto step2Url = Utils::CreateEffectPassTexture( step1Url, GAUSSIAN_BLUR_FRAG_SHADER, downscaledSize, Utils::Direction::HORIZONTAL );

    mPreview2 = Utils::CreatePreview( step2Url, targetSize, ParentOrigin::BOTTOM_LEFT, ParentOrigin::TOP_LEFT,"Step 2", true );
    stage.Add( mPreview2 );

    // Step 3 renders the second pass of the effect shader
    auto step3Url = Utils::CreateEffectPassTexture( step2Url, GAUSSIAN_BLUR_FRAG_SHADER, downscaledSize, Utils::Direction::VERTICAL );

    // Don't bother with preview 3, we're going to draw it at larger scale back to original fbo
    std::vector<uint32_t> empty;
    Utils::CreateEffectPassTexture( step3Url, empty, targetSize, Utils::Direction::HORIZONTAL, fb1 );

    // Render step 4 output (same as step 1 output) to center:
    mFinalImage = ImageView::New( step1Url );
    mFinalImage.SetSize( targetSize );
    mFinalImage.SetParentOrigin( ParentOrigin::CENTER );
    mFinalImage.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( mFinalImage );
  }

  void Deactivate()
  {
    Stage stage = Stage::GetCurrent();
    auto taskList = stage.GetRenderTaskList();
    for( unsigned int i=taskList.GetTaskCount()-1; i>0; --i )
    {
      auto task = taskList.GetTask(i);
      Actor cameraActor = task.GetCameraActor();
      Actor sourceActor = task.GetSourceActor();
      stage.Remove( cameraActor );
      stage.Remove( sourceActor );
      task.SetCameraActor( CameraActor() );
      task.SetSourceActor( Actor() );
      taskList.RemoveTask(taskList.GetTask(i));
    }
    UnparentAndReset(mPreview1);
    UnparentAndReset(mPreview2);
    mFinalImage.SetSize( TARGET_SIZE * 1.8f );
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApp.Quit();
      }
      if( event.keyCode == 10 )
      {
        Deactivate();
      }
    }
  }

private:
  Application& mApp;
  Actor mSceneActor;
  ImageView mFinalImage;
  Actor mPreview1;
  Actor mPreview2;
};

int main( int argc, char** argv )
{
  Application app = Application::New( &argc, &argv );
  ImageViewBlurExample exampleApp( app );
  app.MainLoop();
  return 0;
}
