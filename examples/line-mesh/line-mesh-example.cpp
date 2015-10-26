/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/rendering/renderer.h>
#include <dali-toolkit/dali-toolkit.h>

// INTERNAL INCLUDES
#include "shared/view.h"

using namespace Dali;

namespace
{
const char* MATERIAL_SAMPLE( DALI_IMAGE_DIR "gallery-small-48.jpg" );
const char* MATERIAL_SAMPLE2( DALI_IMAGE_DIR "gallery-medium-19.jpg" );

#define MAKE_SHADER(A)#A

const char* VERTEX_SHADER = MAKE_SHADER(
attribute mediump vec2    aPosition1;
attribute mediump vec2    aPosition2;
uniform   mediump mat4    uMvpMatrix;
uniform   mediump vec3    uSize;
uniform   mediump float   uMorphAmount;

void main()
{
  mediump vec2 morphPosition = mix(aPosition1, aPosition2, uMorphAmount);
  mediump vec4 vertexPosition = vec4(morphPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  gl_Position = vertexPosition;
}
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
uniform lowp  vec4    uColor;
uniform sampler2D     sTexture;

void main()
{
  gl_FragColor = uColor;
}
);

Geometry CreateGeometry()
{
  // Create vertices
  struct Vertex { Vector2 position; };
  Vertex pentagonVertexData[5] =
    {
      { Vector2(  0.0f,   1.00f) }, // 0
      { Vector2( -0.95f,  0.31f) }, // 1
      { Vector2( -0.59f, -0.81f) }, // 2
      { Vector2(  0.59f, -0.81f) }, // 3
      { Vector2(  0.95f,  0.31f) }, // 4
    };

  Vertex pentacleVertexData[5] =
    {
      { Vector2(  0.0f,  -1.00f) }, //
      { Vector2(  0.59f,  0.81f) }, //
      { Vector2( -0.95f, -0.31f) }, //
      { Vector2(  0.95f, -0.31f) }, //
      { Vector2( -0.59f,  0.81f) }, //
    };

  Property::Map pentagonVertexFormat;
  pentagonVertexFormat["aPosition1"] = Property::VECTOR2;
  PropertyBuffer pentagonVertices = PropertyBuffer::New( pentagonVertexFormat, 5 );
  pentagonVertices.SetData(pentagonVertexData);

  Property::Map pentacleVertexFormat;
  pentacleVertexFormat["aPosition2"] = Property::VECTOR2;
  PropertyBuffer pentacleVertices = PropertyBuffer::New( pentacleVertexFormat, 5 );
  pentacleVertices.SetData(pentacleVertexData);

  // Create indices
  unsigned int indexData[10] = { 0, 1, 1, 2, 2, 3, 3, 4, 4, 0 };
  Property::Map indexFormat;
  indexFormat["indices"] = Property::INTEGER;
  PropertyBuffer indices = PropertyBuffer::New( indexFormat, sizeof(indexData)/sizeof(indexData[0]) );
  indices.SetData(indexData);

  // Create the geometry object
  Geometry pentagonGeometry = Geometry::New();
  pentagonGeometry.AddVertexBuffer( pentagonVertices );
  pentagonGeometry.AddVertexBuffer( pentacleVertices );
  pentagonGeometry.SetIndexBuffer( indices );

  pentagonGeometry.SetGeometryType( Geometry::LINES );

  return pentagonGeometry;
}

} // anonymous namespace

// This example shows how to morph between 2 meshes with the same number of
// vertices.
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

    mShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );

    mMaterial = Material::New( mShader );
    mImage = ResourceImage::New( MATERIAL_SAMPLE );
    mMaterial.AddTexture(mImage, "sTexture");

    mGeometry = CreateGeometry();

    mRenderer = Renderer::New( mGeometry, mMaterial );

    mMeshActor = Actor::New();
    mMeshActor.AddRenderer( mRenderer );
    mMeshActor.SetSize(200, 200);

    Property::Index morphAmountIndex = mMeshActor.RegisterProperty( "uMorphAmount", 0.0f );

    mRenderer.SetDepthIndex(0);

    mMeshActor.SetParentOrigin( ParentOrigin::CENTER );
    mMeshActor.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( mMeshActor );

    mChangeImageTimer = Timer::New( 5000 );
    mChangeImageTimer.TickSignal().Connect( this, &ExampleController::OnTimer );
    mChangeImageTimer.Start();

    Animation  animation = Animation::New(5);
    KeyFrames keyFrames = KeyFrames::New();
    keyFrames.Add(0.0f, 0.0f);
    keyFrames.Add(1.0f, 1.0f);

    animation.AnimateBetween( Property( mMeshActor, morphAmountIndex ), keyFrames, AlphaFunction(AlphaFunction::SIN) );
    animation.SetLooping(true);
    animation.Play();

    stage.SetBackgroundColor(Vector4(0.0f, 0.2f, 0.2f, 1.0f));;
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

  bool OnTimer()
  {
    Image image = ResourceImage::New( MATERIAL_SAMPLE2 );

    mMaterial.SetTextureImage(0,image);
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
  Vector3 mStageSize;                                     ///< The size of the stage

  Image    mImage;
  Shader   mShader;
  Material mMaterial;
  Geometry mGeometry;
  Renderer mRenderer;
  Actor    mMeshActor;
  Renderer mRenderer2;
  Actor    mMeshActor2;
  Timer    mChangeImageTimer;
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
