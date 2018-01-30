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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/tool-bar/tool-bar.h>
#include "shared/view.h"
#include "shared/utility.h"
#include <stdio.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;
using Dali::Toolkit::Control;
using Dali::Toolkit::ToolBar;

const char* BACKGROUND_IMAGE( "" );
const char* TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char* APPLICATION_TITLE( "Ray Marching" );
const char* SHADER_NAME("raymarch_sphere_shaded");

/**
 * @brief LoadShaderCode
 * @param filename
 * @param output
 * @return
 */
bool LoadShaderCode( const char* path, const char* filename, std::vector<char>& output )
{
  std::string fullpath( path );
  fullpath += filename;
  FILE* file = fopen( fullpath.c_str(), "rb" );
  if( ! file )
  {
    return false;
  }

  bool retValue = false;
  if( ! fseek( file, 0, SEEK_END ) )
  {
    long int size = ftell( file );

    if( ( size != -1L ) &&
        ( ! fseek( file, 0, SEEK_SET ) ) )
    {
      output.resize( size + 1 );
      std::fill( output.begin(), output.end(), 0 );
      ssize_t result = fread( output.data(), size, 1, file );

      retValue = ( result >= 0 );
    }
  }

  fclose( file );
  return retValue;
}

/**
 * @brief LoadShaders
 * @param shaderName
 * @return
 */
Shader LoadShaders( const std::string& shaderName )
{
  std::vector<char> bufV, bufF;
  std::string shaderVSH( shaderName );
  std::string shaderFSH( shaderName );
  shaderVSH += ".vsh";
  shaderFSH += ".fsh";

  Shader shader;
  if( LoadShaderCode( DEMO_SHADER_DIR, shaderVSH.c_str(), bufV ) &&
      LoadShaderCode( DEMO_SHADER_DIR, shaderFSH.c_str(), bufF ) )
  {
    shader = Shader::New( bufV.data(), bufF.data() );
  }
  return shader;
}

// This example shows how to create a Ray Marching using a shader
//
class RayMarchingExample : public ConnectionTracker
{
public:

  RayMarchingExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &RayMarchingExample::Create );
  }

  ~RayMarchingExample()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    stage.GetRootLayer().TouchSignal().Connect( this, &RayMarchingExample::OnTouch );

    stage.KeyEventSignal().Connect(this, &RayMarchingExample::OnKeyEvent);

    stage.SetBackgroundColor( Color::YELLOW );

    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            APPLICATION_TITLE );

    // Add an extra space on the right to center the title text.
    mToolBar.AddControl( Actor::New(), DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight );

    AddContentLayer();

  }
  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  /**
   * Main key event handler
   */
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

  /**
   * Creates quad renderer
   */
  Renderer CreateQuadRenderer()
  {
    // Create shader & geometry needed by Renderer
    Shader shader = LoadShaders( SHADER_NAME );

    Property::Map vertexFormat;
    vertexFormat["aPosition"] = Property::VECTOR2;
    PropertyBuffer vertexBuffer = PropertyBuffer::New( vertexFormat );

    const float P( 0.5f );
    const Vector2 vertices[] = {
      Vector2( -P, -P ),
      Vector2( +P, -P ),
      Vector2( -P, +P ),
      Vector2( +P, +P )
    };

    vertexBuffer.SetData( vertices, 4 );

    // Instantiate quad geometry
    Geometry geometry = Geometry::New();
    geometry.AddVertexBuffer( vertexBuffer );
    geometry.SetType( Geometry::TRIANGLE_STRIP );

    // Create renderer
    Renderer renderer = Renderer::New( geometry, shader );

    renderer.RegisterProperty("uRadius", 0.0f );
    renderer.RegisterProperty("uAdjuster", -4.0f );

    //  Animate the sphere radius uniform and a generic uAdjust uniform currently used to move the light around
    Animation animation = Animation::New(8.0f);
    animation.AnimateTo( Property(renderer,"uRadius"), 1.2f, AlphaFunction::BOUNCE);
    animation.AnimateTo( Property(renderer,"uAdjuster"), 4.0f, AlphaFunction::BOUNCE);
    animation.SetLooping( true );
    animation.Play();

    return renderer;
  }

  void AddContentLayer()
  {
    Stage stage = Stage::GetCurrent();

     //Create all the renderers
    Renderer renderer = CreateQuadRenderer();

    Actor actor = Actor::New();
    actor.AddRenderer( renderer );

    actor.SetAnchorPoint( Dali::AnchorPoint::CENTER );
    actor.SetParentOrigin( Dali::ParentOrigin::CENTER );
    actor.SetResizePolicy( Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::ALL_DIMENSIONS );

    mContentLayer.Add( actor );
  }

private:
  Application&  mApplication;
  Control mView;
  Layer mContentLayer;
  ToolBar mToolBar;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  RayMarchingExample test( application );
  application.MainLoop();
  return 0;
}
