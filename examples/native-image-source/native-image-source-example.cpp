/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
  const std::string JPG_FILENAME = DEMO_IMAGE_DIR "gallery-medium-4.jpg";
}

// This example shows how to create and use a NativeImageSource as the target of the render task.
//
class NativeImageSourceController : public ConnectionTracker
{
public:

  NativeImageSourceController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &NativeImageSourceController::Create );
  }

  ~NativeImageSourceController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );
    stage.KeyEventSignal().Connect(this, &NativeImageSourceController::OnKeyEvent);

    mButtonRefreshAlways = PushButton::New();
    mButtonRefreshAlways.SetTogglableButton( true );
    mButtonRefreshAlways.SetSelected( true );
    mButtonRefreshAlways.SetLabelText( "Refresh ALWAYS" );
    mButtonRefreshAlways.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mButtonRefreshAlways.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mButtonRefreshAlways.StateChangedSignal().Connect( this, &NativeImageSourceController::OnButtonSelected );
    stage.Add( mButtonRefreshAlways );

    mButtonRefreshOnce = PushButton::New();
    mButtonRefreshOnce.SetLabelText( "Refresh ONCE" );
    mButtonRefreshOnce.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    mButtonRefreshOnce.SetAnchorPoint( AnchorPoint::TOP_RIGHT );
    mButtonRefreshOnce.ClickedSignal().Connect( this, &NativeImageSourceController::OnButtonSelected );
    stage.Add( mButtonRefreshOnce);

    CreateScene();
  }

  bool CreateScene()
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    float buttonHeight = 100.f;
    mButtonRefreshAlways.SetSize( stageSize.x / 2.f, buttonHeight );
    mButtonRefreshOnce.SetSize( stageSize.x / 2.f, buttonHeight );

    Vector2 imageSize( stageSize.x, (stageSize.y-buttonHeight)/2.f );

    // Create the native image source
    NativeImageSourcePtr nativeImageSourcePtr = NativeImageSource::New( imageSize.width, imageSize.height, NativeImageSource::COLOR_DEPTH_DEFAULT );

    // Create a image view as source actor to be renderer to the native image source
    Actor sourceActor = ImageView::New(JPG_FILENAME);
    sourceActor.SetParentOrigin( ParentOrigin::CENTER);
    sourceActor.SetAnchorPoint( AnchorPoint::CENTER );
    sourceActor.SetY( - (imageSize.height-buttonHeight)/2.f );
    stage.Add( sourceActor );

    Animation animation = Animation::New(2.f);
    Degree relativeRotationDegrees(90.0f);
    Radian relativeRotationRadians(relativeRotationDegrees);
    animation.AnimateTo( Property( sourceActor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::ZAXIS ), AlphaFunction::LINEAR, TimePeriod(0.f, 0.5f));
    animation.AnimateBy( Property( sourceActor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::ZAXIS ), AlphaFunction::LINEAR, TimePeriod(0.5f, 0.5f));
    animation.AnimateBy( Property( sourceActor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::ZAXIS ), AlphaFunction::LINEAR, TimePeriod(1.f, 0.5f));
    animation.AnimateBy( Property( sourceActor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::ZAXIS ), AlphaFunction::LINEAR, TimePeriod(1.5f, 0.5f));
    animation.SetLooping(true);
    animation.Play();

    // create a offscreen renderer task to render content into the native image source
    FrameBufferImage targetBuffer = FrameBufferImage::New( *nativeImageSourcePtr );

    CameraActor cameraActor = CameraActor::New(imageSize);
    cameraActor.SetParentOrigin(ParentOrigin::TOP_CENTER);
    cameraActor.SetParentOrigin( AnchorPoint::TOP_CENTER );
    cameraActor.SetY( buttonHeight + imageSize.height/2.f );
    stage.Add(cameraActor);

    RenderTaskList taskList = stage.GetRenderTaskList();
    mOffscreenRenderTask = taskList.CreateTask();
    mOffscreenRenderTask.SetSourceActor( sourceActor );
    mOffscreenRenderTask.SetClearColor( Color::WHITE );
    mOffscreenRenderTask.SetClearEnabled(true);
    mOffscreenRenderTask.SetCameraActor(cameraActor);
    mOffscreenRenderTask.GetCameraActor().SetInvertYAxis(true);
    mOffscreenRenderTask.SetTargetFrameBuffer( targetBuffer );
    mOffscreenRenderTask.SetRefreshRate( RenderTask::REFRESH_ALWAYS );

    // Display the native image on the screen
    NativeImage nativeImage = NativeImage::New( *nativeImageSourcePtr );
    ImageView nativeImageView = ImageView::New( nativeImage );
    nativeImageView.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    nativeImageView.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    stage.Add( nativeImageView );

    TextLabel textLabel1 = TextLabel::New( "Resource Image" );
    textLabel1.SetParentOrigin( ParentOrigin::TOP_CENTER );
    textLabel1.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    nativeImageView.Add( textLabel1 );

    TextLabel textLabel2 = TextLabel::New( "Native Image" );
    textLabel2.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    textLabel2.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    nativeImageView.Add( textLabel2 );

    return false;
  }

  bool OnButtonSelected( Toolkit::Button button )
  {
    Toolkit::PushButton pushButton = Toolkit::PushButton::DownCast( button );
    if( pushButton == mButtonRefreshAlways )
    {
      if( mButtonRefreshAlways.IsSelected() )
      {
        mOffscreenRenderTask.SetRefreshRate( RenderTask::REFRESH_ALWAYS );
      }
      else
      {
        mOffscreenRenderTask.SetRefreshRate( RenderTask::REFRESH_ONCE );
      }
    }
    else if( pushButton == mButtonRefreshOnce )
    {
      if( mButtonRefreshAlways.IsSelected() )
      {
        mButtonRefreshAlways.SetSelected( false );
      }
      mOffscreenRenderTask.SetRefreshRate( RenderTask::REFRESH_ONCE );
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
  RenderTask mOffscreenRenderTask;
  PushButton mButtonRefreshAlways;
  PushButton mButtonRefreshOnce;

};

void RunTest( Application& application )
{
  NativeImageSourceController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
