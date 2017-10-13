/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/dali.h>
#include <dali/devel-api/images/native-image-interface-extension.h>
#include <dali-toolkit/dali-toolkit.h>
#include <cstring>

// INTERNAL INCLUDES
#include "shared/utility.h"

using namespace Dali;
using namespace Toolkit;

namespace
{

const float BUTTON_HEIGHT = 100.0f;
const float BUTTON_COUNT  = 5.0f;

const std::string JPG_FILENAME = DEMO_IMAGE_DIR "gallery-medium-4.jpg";
const std::string CAPTURE_FILENAME = "/tmp/native-image-capture.png";

/**
 * @brief Creates a shader used to render a native image
 * @param[in] nativeImageInterface The native image interface
 * @return A shader to render the native image
 */
Shader CreateShader( NativeImageInterface& nativeImageInterface )
{
  static const char* DEFAULT_SAMPLER_TYPENAME = "sampler2D";

  static const char* VERTEX_SHADER_TEXTURE = DALI_COMPOSE_SHADER(
      attribute mediump vec2 aPosition;\n
      attribute mediump vec2 aTexCoord;\n
      uniform mediump mat4 uMvpMatrix;\n
      uniform mediump vec3 uSize;\n
      varying mediump vec2 vTexCoord;\n
      void main()\n
      {\n
        vec4 position = vec4(aPosition,0.0,1.0)*vec4(uSize,1.0);\n
        gl_Position = uMvpMatrix * position;\n
        vTexCoord = aTexCoord;\n
      }\n
  );

  static const char* FRAGMENT_SHADER_TEXTURE = DALI_COMPOSE_SHADER(
      uniform lowp vec4 uColor;\n
      uniform sampler2D sTexture;\n
      varying mediump vec2 vTexCoord;\n

      void main()\n
      {\n
        gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
      }\n
  );

  NativeImageInterface::Extension* extension( nativeImageInterface.GetExtension() );
  if( extension )
  {
    std::string fragmentShader;

    //Get custom fragment shader prefix
    const char* fragmentPreFix = extension->GetCustomFragmentPreFix();
    if( fragmentPreFix )
    {
      fragmentShader = fragmentPreFix;
      fragmentShader += FRAGMENT_SHADER_TEXTURE;
    }
    else
    {
      fragmentShader = FRAGMENT_SHADER_TEXTURE;
    }

    //Get custom sampler type name
    const char* customSamplerTypename = extension->GetCustomSamplerTypename();
    if( customSamplerTypename )
    {
      fragmentShader.replace( fragmentShader.find( DEFAULT_SAMPLER_TYPENAME ), strlen(DEFAULT_SAMPLER_TYPENAME), customSamplerTypename );
    }

    return Shader::New( VERTEX_SHADER_TEXTURE, fragmentShader );
  }
  else
  {
    return Shader::New( VERTEX_SHADER_TEXTURE, FRAGMENT_SHADER_TEXTURE );
  }
}

}

// This example shows how to create and use a NativeImageSource as the target of the render task.
//
class NativeImageSourceController : public ConnectionTracker
{
public:

  NativeImageSourceController( Application& application )
  : mApplication( application ),
    mRefreshAlways( true )
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

    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    stage.KeyEventSignal().Connect(this, &NativeImageSourceController::OnKeyEvent);

    CreateButtonArea();

    CreateContentAreas();
  }

  void CreateButtonArea()
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    mButtonArea = Layer::New();
    mButtonArea.SetSize( stageSize.x, BUTTON_HEIGHT );
    mButtonArea.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mButtonArea.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    stage.Add( mButtonArea );

    mButtonShow = PushButton::New();
    mButtonShow.SetProperty( Button::Property::TOGGLABLE, true );
    mButtonShow.SetProperty( Toolkit::Button::Property::LABEL, "SHOW" );
    mButtonShow.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mButtonShow.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mButtonShow.SetSize( stageSize.x / BUTTON_COUNT, BUTTON_HEIGHT );
    mButtonShow.ClickedSignal().Connect( this, &NativeImageSourceController::OnButtonSelected );
    mButtonArea.Add( mButtonShow );

    mButtonRefreshAlways = PushButton::New();
    mButtonRefreshAlways.SetProperty( Button::Property::TOGGLABLE, true );
    mButtonRefreshAlways.SetProperty( Toolkit::Button::Property::LABEL, "ALWAYS" );
    mButtonRefreshAlways.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mButtonRefreshAlways.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mButtonRefreshAlways.SetSize( stageSize.x / BUTTON_COUNT, BUTTON_HEIGHT );
    mButtonRefreshAlways.SetPosition( (stageSize.x / BUTTON_COUNT)*1.0f, 0.0f );
    mButtonRefreshAlways.StateChangedSignal().Connect( this, &NativeImageSourceController::OnButtonSelected );
    mButtonArea.Add( mButtonRefreshAlways );

    mButtonRefreshOnce = PushButton::New();
    mButtonRefreshOnce.SetProperty( Toolkit::Button::Property::LABEL, "ONCE" );
    mButtonRefreshOnce.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mButtonRefreshOnce.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mButtonRefreshOnce.SetSize( stageSize.x / BUTTON_COUNT, BUTTON_HEIGHT );
    mButtonRefreshOnce.SetPosition( (stageSize.x / BUTTON_COUNT)*2.0f, 0.0f );
    mButtonRefreshOnce.ClickedSignal().Connect( this, &NativeImageSourceController::OnButtonSelected );
    mButtonArea.Add( mButtonRefreshOnce );

    mButtonCapture = PushButton::New();
    mButtonCapture.SetProperty( Toolkit::Button::Property::LABEL, "CAPTURE" );
    mButtonCapture.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mButtonCapture.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mButtonCapture.SetSize( stageSize.x / BUTTON_COUNT, BUTTON_HEIGHT );
    mButtonCapture.SetPosition( (stageSize.x / BUTTON_COUNT)*3.0f, 0.0f );
    mButtonCapture.ClickedSignal().Connect( this, &NativeImageSourceController::OnButtonSelected );
    mButtonArea.Add( mButtonCapture );

    mButtonReset = PushButton::New();
    mButtonReset.SetProperty( Toolkit::Button::Property::LABEL, "RESET" );
    mButtonReset.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mButtonReset.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mButtonReset.SetSize( stageSize.x / BUTTON_COUNT, BUTTON_HEIGHT );
    mButtonReset.SetPosition( (stageSize.x / BUTTON_COUNT)*4.0f, 0.0f );
    mButtonReset.ClickedSignal().Connect( this, &NativeImageSourceController::OnButtonSelected );
    mButtonArea.Add( mButtonReset );
  }

  void CreateContentAreas()
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    float contentHeight( (stageSize.y - BUTTON_HEIGHT)/2.0f );

    mTopContentArea = Actor::New();
    mTopContentArea.SetSize( stageSize.x, contentHeight );
    mTopContentArea.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mTopContentArea.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mTopContentArea.SetY( BUTTON_HEIGHT );
    stage.Add( mTopContentArea );

    mBottomContentArea = Actor::New();
    mBottomContentArea.SetSize( stageSize.x, contentHeight );
    mBottomContentArea.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mBottomContentArea.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    stage.Add( mBottomContentArea );

    mSourceActor = ImageView::New(JPG_FILENAME);
    mSourceActor.SetParentOrigin( ParentOrigin::CENTER);
    mSourceActor.SetAnchorPoint( AnchorPoint::CENTER );
    mTopContentArea.Add( mSourceActor );

    Animation animation = Animation::New(2.f);
    Degree relativeRotationDegrees(90.0f);
    Radian relativeRotationRadians(relativeRotationDegrees);
    animation.AnimateTo( Property( mSourceActor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::ZAXIS ), AlphaFunction::LINEAR, TimePeriod(0.f, 0.5f));
    animation.AnimateBy( Property( mSourceActor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::ZAXIS ), AlphaFunction::LINEAR, TimePeriod(0.5f, 0.5f));
    animation.AnimateBy( Property( mSourceActor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::ZAXIS ), AlphaFunction::LINEAR, TimePeriod(1.f, 0.5f));
    animation.AnimateBy( Property( mSourceActor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::ZAXIS ), AlphaFunction::LINEAR, TimePeriod(1.5f, 0.5f));
    animation.SetLooping(true);
    animation.Play();

    TextLabel textLabel1 = TextLabel::New( "Image" );
    textLabel1.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    textLabel1.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mTopContentArea.Add( textLabel1 );

    // Wait until button press before creating mOffscreenRenderTask

    TextLabel textLabel2 = TextLabel::New( "Native Image" );
    textLabel2.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    textLabel2.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mBottomContentArea.Add( textLabel2 );
  }

  void SetupNativeImage()
  {
    if( ! mOffscreenRenderTask )
    {
      Stage stage = Stage::GetCurrent();
      Vector2 stageSize = stage.GetSize();

      float contentHeight( (stageSize.y - BUTTON_HEIGHT)/2.0f );
      Vector2 imageSize( stageSize.x, contentHeight );

      mNativeImageSourcePtr = NativeImageSource::New( imageSize.width, imageSize.height, NativeImageSource::COLOR_DEPTH_DEFAULT );
      mNativeTexture = Texture::New( *mNativeImageSourcePtr );

      mFrameBuffer = FrameBuffer::New( mNativeTexture.GetWidth(), mNativeTexture.GetHeight(), FrameBuffer::Attachment::NONE );
      mFrameBuffer.AttachColorTexture( mNativeTexture );

      mCameraActor = CameraActor::New( imageSize );
      mCameraActor.SetParentOrigin( ParentOrigin::CENTER );
      mCameraActor.SetParentOrigin( AnchorPoint::CENTER );
      mTopContentArea.Add( mCameraActor );

      RenderTaskList taskList = stage.GetRenderTaskList();
      mOffscreenRenderTask = taskList.CreateTask();
      mOffscreenRenderTask.SetSourceActor( mSourceActor );
      mOffscreenRenderTask.SetClearColor( Color::WHITE );
      mOffscreenRenderTask.SetClearEnabled( true );
      mOffscreenRenderTask.SetCameraActor( mCameraActor );
      mOffscreenRenderTask.GetCameraActor().SetInvertYAxis( true );
      mOffscreenRenderTask.SetFrameBuffer( mFrameBuffer );
    }

    if( mRefreshAlways )
    {
      mOffscreenRenderTask.SetRefreshRate( RenderTask::REFRESH_ALWAYS );
    }
    else
    {
      mOffscreenRenderTask.SetRefreshRate( RenderTask::REFRESH_ONCE );
    }
  }

  void SetupDisplayActor( bool show )
  {
    if( show )
    {
      if( ! mDisplayActor )
      {
        // Make sure we have something to display
        SetupNativeImage();

        mDisplayActor = Actor::New();
        mDisplayActor.SetParentOrigin( ParentOrigin::CENTER );
        mDisplayActor.SetAnchorPoint( AnchorPoint::CENTER );

        Geometry geometry = DemoHelper::CreateTexturedQuad();

        Shader shader = CreateShader( *mNativeImageSourcePtr );

        Renderer renderer = Renderer::New( geometry, shader );

        TextureSet textureSet = TextureSet::New();
        textureSet.SetTexture( 0u, mNativeTexture );
        renderer.SetTextures( textureSet );

        mDisplayActor.AddRenderer( renderer );
        mDisplayActor.SetSize( mNativeTexture.GetWidth(), mNativeTexture.GetHeight() );

        mBottomContentArea.Add( mDisplayActor );
      }
    }
    else
    {
      UnparentAndReset( mDisplayActor );
    }
  }

  void Capture()
  {
    mRefreshAlways = false;
    SetupNativeImage();

    mOffscreenRenderTask.FinishedSignal().Connect( this, &NativeImageSourceController::DoCapture );
  }

  void DoCapture(RenderTask& task)
  {
    task.FinishedSignal().Disconnect( this, &NativeImageSourceController::DoCapture );

    mNativeImageSourcePtr->EncodeToFile( CAPTURE_FILENAME );
  }

  void Reset()
  {
    SetupDisplayActor( false );

    Stage stage = Stage::GetCurrent();
    RenderTaskList taskList = stage.GetRenderTaskList();
    taskList.RemoveTask( mOffscreenRenderTask );
    mOffscreenRenderTask.Reset();
    mCameraActor.Reset();

    mFrameBuffer.Reset();
    mNativeTexture.Reset();
    mNativeImageSourcePtr.Reset();
  }

  bool OnButtonSelected( Toolkit::Button button )
  {
    Toolkit::PushButton pushButton = Toolkit::PushButton::DownCast( button );

    if( pushButton == mButtonShow )
    {
      bool isSelected = mButtonShow.GetProperty( Toolkit::Button::Property::SELECTED ).Get<bool>();

      SetupDisplayActor( isSelected );
    }
    else if( pushButton == mButtonRefreshAlways )
    {
      bool isSelected = mButtonRefreshAlways.GetProperty( Toolkit::Button::Property::SELECTED ).Get<bool>();

      mRefreshAlways = isSelected;
      SetupNativeImage();
    }
    else if( pushButton == mButtonRefreshOnce )
    {
      bool isSelected = mButtonRefreshAlways.GetProperty( Toolkit::Button::Property::SELECTED ).Get<bool>();

      if( isSelected )
      {
        mButtonRefreshAlways.SetProperty( Button::Property::SELECTED, false );
      }

      mRefreshAlways = false;
      SetupNativeImage();
    }
    else if( pushButton == mButtonCapture )
    {
      Capture();
    }
    else if( pushButton == mButtonReset )
    {
      Reset();
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

  RenderTask mOffscreenRenderTask;
  CameraActor mCameraActor;

  Actor mDisplayActor;

  bool mRefreshAlways;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  NativeImageSourceController test( application );
  application.MainLoop();
  return 0;
}
