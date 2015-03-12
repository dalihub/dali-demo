/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include "shared/view.h"

#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;

namespace
{
const char* MATERIAL_SAMPLE( DALI_IMAGE_DIR "gallery-small-48.jpg" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* APPLICATION_TITLE( "Mesh Example" );

#define MAKE_SHADER(A)#A

const char* VERTEX_SHADER = MAKE_SHADER(
attribute mediump vec3    aPosition;
attribute highp   vec2    aTexCoord;
varying   mediump vec2    vTexCoord;
uniform   mediump mat4    uMvpMatrix;
uniform   mediump vec3    uSize;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  vTexCoord = aTexCoord;
  gl_Position = vertexPosition;
}
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
varying mediump vec2  vTexCoord;
uniform lowp  vec4    uColor;
uniform sampler2D     sTexture;

void main()
{
  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;
}
);

} // anonymous namespace

// This example shows how to use a simple mesh
//
class ExampleController : public ConnectionTracker
{
public:

  /**
   * The example controller constructor.
   * @param[in] application The application instance
   */
  ExampleController( Application& application )
  : mApplication( application ),
    mView()
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ExampleController::Create );
  }

  /**
   * The example controller destructor
   */
  ~ExampleController()
  {
    // Nothing to do here;
  }

  /**
   * Invoked upon creation of application
   * @param[in] application The application instance
   */
  void Create( Application& application )
  {
    Stage::GetCurrent().KeyEventSignal().Connect(this, &ExampleController::OnKeyEvent);

    mStageSize = Stage::GetCurrent().GetSize();

    // The Init signal is received once (only) during the Application lifetime

    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Toolkit::ToolBar toolBar;
    mContent = DemoHelper::CreateView( application,
                                       mView,
                                       toolBar,
                                       "",
                                       TOOLBAR_IMAGE,
                                       APPLICATION_TITLE );

    mContent.TouchedSignal().Connect( this, &ExampleController::OnTouched );

    mImage = ResourceImage::New( MATERIAL_SAMPLE );
    mSampler = Sampler::New("sTexture");
    mSampler.SetImage(mImage);
    mShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );

    mMaterial = Material::New( mShader );
    mMaterial.AddSampler( mSampler );

    mGeometry = Geometry::New(); // Don't need to set a vertex buffer - it's a quad by default.
    mRenderer = Renderer::New( mGeometry, mMaterial );

    mMeshActor = Actor::New();
    mMeshActor.AddRenderer( mRenderer );
    mMeshActor.SetSize(400, 400);
    mContent.Add( mMeshActor );
  }

  /**
   * Invoked whenever the quit button is clicked
   * @param[in] button the quit button
   */
  bool OnQuitButtonClicked( Toolkit::Button button )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  /**
   * Invoked whenever the content (screen) is touched
   * @param[in] actor The actor that received the touch
   * @param[in] event The touch-event information
   */
  bool OnTouched( Actor actor, const TouchEvent& event )
  {
    if(event.GetPointCount() > 0)
    {
      const TouchPoint& point = event.GetPoint(0);
      switch(point.state)
      {
        case TouchPoint::Down:
        case TouchPoint::Motion:
        {
          break;
        }
        case TouchPoint::Up:
        case TouchPoint::Leave:
        case TouchPoint::Interrupted:
        {
          break;
        }
        default:
        {
          break;
        }
      } // end switch
    }
    return false;
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

  Application&  mApplication;                             ///< Application instance
  Toolkit::View mView;                                    ///< The view
  Layer mContent;                                         ///< The content layer
  Vector3 mStageSize;                                     ///< The size of the stage

  Image    mImage;
  Sampler  mSampler;
  Shader   mShader;
  Material mMaterial;
  Geometry mGeometry;
  Renderer mRenderer;
  Actor    mMeshActor;
};

void RunTest( Application& application )
{
  ExampleController test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
