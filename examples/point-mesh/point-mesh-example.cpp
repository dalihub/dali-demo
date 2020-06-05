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
#include <dali-toolkit/dali-toolkit.h>

// INTERNAL INCLUDES
#include "shared/view.h"
#include "shared/utility.h"

using namespace Dali;

namespace
{
const char* MATERIAL_SAMPLE( DEMO_IMAGE_DIR "gallery-small-48.jpg" );
const char* MATERIAL_SAMPLE2( DEMO_IMAGE_DIR "gallery-medium-19.jpg" );

#define MAKE_SHADER(A)#A

const char* VERTEX_SHADER = MAKE_SHADER(
attribute mediump vec2    aPosition;
attribute highp   float   aHue;
varying   mediump vec2    vTexCoord;
uniform   mediump mat4    uMvpMatrix;
uniform   mediump vec3    uSize;
uniform   mediump float   uPointSize;
uniform   lowp    vec4    uFadeColor;
varying   mediump vec3    vVertexColor;
varying   mediump float   vHue;

vec3 hsv2rgb(vec3 c)
{
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= (uSize-uPointSize);
  vertexPosition = uMvpMatrix * vertexPosition;
  vVertexColor = hsv2rgb( vec3( aHue, 0.7, 1.0 ) );
  vHue = aHue;
  gl_PointSize = uPointSize;
  gl_Position = vertexPosition;
}
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
varying mediump vec3  vVertexColor;
varying mediump float vHue;
uniform lowp  vec4    uColor;
uniform sampler2D     sTexture1;
uniform sampler2D     sTexture2;
uniform lowp vec4     uFadeColor;

void main()
{
  mediump vec4 texCol1 = texture2D(sTexture1, gl_PointCoord);
  mediump vec4 texCol2 = texture2D(sTexture2, gl_PointCoord);
  gl_FragColor = vec4(vVertexColor, 1.0) * ((texCol1*vHue) + (texCol2*(1.0-vHue)));
}
);

Geometry CreateGeometry()
{
  // Create vertices
  struct Vertex { Vector2 position; float hue; };

  const unsigned int numSides = 20;
  Vertex polyhedraVertexData[numSides];
  float angle=0;
  float sectorAngle = 2.0f * Math::PI / (float) numSides;

  for(unsigned int i=0; i<numSides; ++i)
  {
    polyhedraVertexData[i].position.x = sinf(angle)*0.5f;
    polyhedraVertexData[i].position.y = cosf(angle)*0.5f;
    polyhedraVertexData[i].hue = angle / ( 2.0f * Math::PI);
    angle += sectorAngle;
  }

  Property::Map polyhedraVertexFormat;
  polyhedraVertexFormat["aPosition"] = Property::VECTOR2;
  polyhedraVertexFormat["aHue"] = Property::FLOAT;
  PropertyBuffer polyhedraVertices = PropertyBuffer::New( polyhedraVertexFormat );
  polyhedraVertices.SetData( polyhedraVertexData, numSides );

  // Create the geometry object
  Geometry polyhedraGeometry = Geometry::New();
  polyhedraGeometry.AddVertexBuffer( polyhedraVertices );
  polyhedraGeometry.SetType( Geometry::POINTS );

  return polyhedraGeometry;
}

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
  : mApplication( application )
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
    Stage stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &ExampleController::OnKeyEvent);

    mStageSize = stage.GetSize();

    // The Init signal is received once (only) during the Application lifetime

    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    Texture texture0 = DemoHelper::LoadTexture( MATERIAL_SAMPLE );
    Texture texture1 = DemoHelper::LoadTexture( MATERIAL_SAMPLE2 );

    Shader shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );

    TextureSet textureSet = TextureSet::New();
    textureSet.SetTexture( 0u, texture0 );
    textureSet.SetTexture( 1u, texture1 );

    Geometry geometry = CreateGeometry();

    mRenderer = Renderer::New( geometry, shader );
    mRenderer.SetTextures( textureSet );

    mMeshActor = Actor::New();
    mMeshActor.AddRenderer( mRenderer );
    mMeshActor.SetProperty( Actor::Property::SIZE, Vector2(400, 400) );

    mMeshActor.RegisterProperty( "uFadeColor", Color::GREEN );

    mRenderer.RegisterProperty( "uFadeColor", Color::MAGENTA );
    mRenderer.RegisterProperty( "uPointSize", 80.0f );
    mRenderer.SetProperty( Renderer::Property::DEPTH_INDEX, 0 );

    mMeshActor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mMeshActor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    stage.Add( mMeshActor );

    Animation  animation = Animation::New(15);
    KeyFrames keyFrames = KeyFrames::New();
    keyFrames.Add(0.0f, Vector4::ZERO);
    keyFrames.Add(1.0f, Vector4( 1.0f, 0.0f, 1.0f, 1.0f ));

    animation.AnimateBy( Property(mMeshActor, Actor::Property::ORIENTATION), Quaternion(Degree(360), Vector3::ZAXIS) );

    animation.SetLooping(true);
    animation.Play();

    stage.SetBackgroundColor(Vector4(0.0f, 0.2f, 0.2f, 1.0f));
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
  Vector3 mStageSize;                                     ///< The size of the stage

  Renderer mRenderer;
  Actor    mMeshActor;
  Renderer mRenderer2;
  Actor    mMeshActor2;
  Timer    mChangeImageTimer;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  ExampleController test( application );
  application.MainLoop();
  return 0;
}
