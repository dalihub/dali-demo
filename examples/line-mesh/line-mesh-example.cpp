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

#define MAKE_SHADER(A)#A

const char* VERTEX_SHADER = MAKE_SHADER(
attribute mediump vec2    aPosition1;
attribute mediump vec2    aPosition2;
attribute lowp    vec3    aColor;
uniform   mediump mat4    uMvpMatrix;
uniform   mediump vec3    uSize;
uniform   mediump float   uMorphAmount;

varying   lowp    vec3    vColor;

void main()
{
  mediump vec2 morphPosition = mix(aPosition1, aPosition2, uMorphAmount);
  mediump vec4 vertexPosition = vec4(morphPosition, 0.0, 1.0);
  vColor = aColor;
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  gl_Position = vertexPosition;
}
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
uniform lowp  vec4    uColor;
uniform sampler2D     sTexture;

varying   lowp        vec3 vColor;

void main()
{
  gl_FragColor = uColor * vec4( vColor, 1.0 );
}
);

const unsigned short indexLines[] = { 0, 1, 1, 2, 2, 3, 3, 4, 4, 0 };
const unsigned short indexLoop[] =  { 0, 1, 2, 3, 4 };
const unsigned short indexStrip[] = { 0, 1, 2, 3, 4, 0 };
const unsigned short* indices[3] = { &indexLines[0], &indexLoop[0], &indexStrip[0] };
const unsigned int indicesSize[3] = { sizeof(indexLines)/sizeof(indexLines[0]), sizeof(indexLoop)/sizeof(indexLoop[0]), sizeof(indexStrip)/sizeof(indexStrip[0])};

Geometry CreateGeometry()
{
  // Create vertices
  struct Vertex
  {
    Vector2 position1;
    Vector2 position2;
    Vector3 color;
  };

  // Create new geometry object
  Vertex pentagonVertexData[5] =
    {
      { Vector2(  0.0f,   1.00f),  Vector2(  0.0f,  -1.00f),  Vector3( 1.0f, 1.0f, 1.0f ) }, // 0
      { Vector2( -0.95f,  0.31f),  Vector2(  0.59f,  0.81f),  Vector3( 1.0f, 0.0f, 0.0f ) }, // 1
      { Vector2( -0.59f, -0.81f),  Vector2( -0.95f, -0.31f),  Vector3( 0.0f, 1.0f, 0.0f ) }, // 2
      { Vector2(  0.59f, -0.81f),  Vector2(  0.95f, -0.31f),  Vector3( 0.0f, 0.0f, 1.0f ) }, // 3
      { Vector2(  0.95f,  0.31f),  Vector2( -0.59f,  0.81f),  Vector3( 1.0f, 1.0f, 0.0f ) }, // 4
    };

  Property::Map pentagonVertexFormat;
  pentagonVertexFormat["aPosition1"] = Property::VECTOR2;
  pentagonVertexFormat["aPosition2"] = Property::VECTOR2;
  pentagonVertexFormat["aColor"] = Property::VECTOR3;
  PropertyBuffer pentagonVertices = PropertyBuffer::New( pentagonVertexFormat );
  pentagonVertices.SetData(pentagonVertexData, 5);


  // Create the geometry object
  Geometry pentagonGeometry = Geometry::New();
  pentagonGeometry.AddVertexBuffer( pentagonVertices );
  pentagonGeometry.SetIndexBuffer( indices[0], indicesSize[0] );
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

    CreateRadioButtons();

    stage.KeyEventSignal().Connect(this, &ExampleController::OnKeyEvent);

    mStageSize = stage.GetSize();

    // The Init signal is received once (only) during the Application lifetime
    ReInitialise( Geometry::LINES );

    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    stage.SetBackgroundColor(Vector4(0.0f, 0.2f, 0.2f, 1.0f));
  }

  /**
   * Invoked whenever application changes the type of geometry drawn
   * @param[in] type of geometry
   */
  void ReInitialise( Geometry::GeometryType geometryType )
  {
    Stage stage = Stage::GetCurrent();

    // destroy mesh actor and its resources if already exists
    if( mMeshActor )
    {
      stage.Remove( mMeshActor );
      mMeshActor.Reset();
    }

    mShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    mGeometry = CreateGeometry();
    mRenderer = Renderer::New( mGeometry, mShader );

    mMeshActor = Actor::New();
    mMeshActor.AddRenderer( mRenderer );
    mMeshActor.SetSize(200, 200);

    Property::Index morphAmountIndex = mMeshActor.RegisterProperty( "uMorphAmount", 0.0f );

    mRenderer.SetProperty( Renderer::Property::DEPTH_INDEX, 0 );

    mMeshActor.SetParentOrigin( ParentOrigin::CENTER );
    mMeshActor.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( mMeshActor );

    Animation  animation = Animation::New(5);
    KeyFrames keyFrames = KeyFrames::New();
    keyFrames.Add(0.0f, 0.0f);
    keyFrames.Add(1.0f, 1.0f);

    animation.AnimateBetween( Property( mMeshActor, morphAmountIndex ), keyFrames, AlphaFunction(AlphaFunction::SIN) );
    animation.SetLooping(true);
    animation.Play();
  }

  /**
   * Invoked on create
   */
  void CreateRadioButtons()
  {
    Stage stage = Stage::GetCurrent();

    Toolkit::TableView modeSelectTableView = Toolkit::TableView::New( 3, 1 );
    modeSelectTableView.SetParentOrigin( ParentOrigin::TOP_LEFT );
    modeSelectTableView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    modeSelectTableView.SetFitHeight( 0 );
    modeSelectTableView.SetFitHeight( 1 );
    modeSelectTableView.SetFitHeight( 2 );
    modeSelectTableView.SetCellPadding( Vector2( 6.0f, 0.0f ) );
    modeSelectTableView.SetScale( Vector3( 0.5f, 0.5f, 0.5f ));

    const char* labels[] =
    {
      "LINES",
      "LINE_LOOP",
      "LINE_STRIP"
    };

    for( int i = 0; i < 3; ++i )
    {
      Property::Map labelMap;
      labelMap[ "text" ]      = labels[i];
      labelMap[ "textColor" ] = Vector4( 0.8f, 0.8f, 0.8f, 1.0f );

      Dali::Toolkit::RadioButton radio = Dali::Toolkit::RadioButton::New();

      radio.SetProperty( Dali::Toolkit::RadioButton::Property::LABEL, labelMap );
      radio.SetParentOrigin( ParentOrigin::TOP_LEFT );
      radio.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      radio.SetSelected( i == 0 );
      radio.PressedSignal().Connect( this, &ExampleController::OnButtonPressed );
      mButtons[i] = radio;
      modeSelectTableView.AddChild( radio, Toolkit::TableView::CellPosition( i,  0 ) );
    }
    stage.Add(modeSelectTableView);
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

  bool OnButtonPressed( Toolkit::Button button )
  {
    const Geometry::GeometryType geomTypes[] =
    {
      Geometry::LINES,
      Geometry::LINE_LOOP,
      Geometry::LINE_STRIP
    };

    size_t index;
    if( button == mButtons[0] )
    {
      index = 0;
    }
    else if( button == mButtons[1] )
    {
      index = 1;
    }
    else
    {
      index = 2;
    }

    mGeometry.SetIndexBuffer( indices[index], indicesSize[index] );
    mGeometry.SetGeometryType( geomTypes[ index ] );

    return true;
  }



private:

  Application&  mApplication;                             ///< Application instance
  Vector3 mStageSize;                                     ///< The size of the stage

  Shader   mShader;
  Geometry mGeometry;
  Renderer mRenderer;
  Actor    mMeshActor;
  Toolkit::RadioButton mButtons[3];
};

void RunTest( Application& application )
{
  ExampleController test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
