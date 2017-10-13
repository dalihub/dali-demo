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

using namespace Dali;

namespace
{

#define MAKE_SHADER(A)#A

const char* VERTEX_SHADER = MAKE_SHADER(
attribute mediump vec2    aInitPos;
attribute mediump vec2    aFinalPos;
attribute mediump vec3    aColor;
uniform   mediump mat4    uMvpMatrix;
uniform   mediump vec3    uSize;
uniform   mediump float   uDelta;
uniform   lowp    vec4    uColor;
varying   lowp    vec4    vColor;

void main()
{
  mediump vec4 vertexPosition = vec4(mix(aInitPos, aFinalPos, uDelta), 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  gl_Position = vertexPosition;
  vColor = vec4(aColor, 0.) * uColor;
}
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
varying   lowp    vec4    vColor;

void main()
{
  gl_FragColor = vColor;
}
);

Geometry CreateGeometry()
{
  // Create vertices
  struct VertexPosition { Vector2 position; };
  struct VertexColor { Vector3 color; };

  VertexPosition quad[] = {
    // yellow
    { Vector2(-.5, -.5) },
    { Vector2( .0,  .0) },
    { Vector2(-.5,  .5) },

    // green
    { Vector2(-.5, -.5) },
    { Vector2( .5, -.5) },
    { Vector2( .0,  .0) },

    // blue
    { Vector2(.5,  -.5)  },
    { Vector2(.5,   .0)  },
    { Vector2(.25, -.25) },

    // red
    { Vector2(.25, -.25) },
    { Vector2(.5,   .0)  },
    { Vector2(.25,  .25) },
    { Vector2(.25,  .25) },
    { Vector2(.0,   .0)  },
    { Vector2(.25, -.25) },

    // cyan
    { Vector2( .0,  .0)  },
    { Vector2( .25, .25) },
    { Vector2(-.25, .25) },

    // magenta
    { Vector2(-.25, .25) },
    { Vector2( .25, .25) },
    { Vector2( .0,  .5)  },
    { Vector2( .0,  .5)  },
    { Vector2(-.5,  .5)  },
    { Vector2(-.25, .25) },

    // orange
    { Vector2( .5, .0) },
    { Vector2( .5, .5) },
    { Vector2( .0, .5) },
  };

  float bigSide = 0.707106781;
  float side = bigSide * .5f;
 // float smallSide = side * .5f;

  Vector2 pA = Vector2( side, .25 );
  Vector2 pB = pA + Vector2( 0., bigSide );
  Vector2 pC = pB + Vector2( -bigSide, 0. );
  Vector2 pD = pA + Vector2(-.5, -.5 );
  Vector2 pE = pD + Vector2( .0, 1. );
  Vector2 pF = pD + Vector2(-side, side );
  Vector2 pF2 = pD + Vector2( 0., bigSide );
  Vector2 pG = pD + Vector2(-.25, .25 );
  Vector2 pH = pD + Vector2( -.5, .0 );
  Vector2 pI = pD + Vector2(-.25, -.25 );
  Vector2 pJ = pD + Vector2( 0., -.5);
  Vector2 pK = pD + Vector2(-.5, -.5);
  Vector2 pL = pB + Vector2(0, -side);
  Vector2 pM = pL + Vector2(side, -side);
  Vector2 pN = pB + Vector2(side, -side);

  VertexPosition cat[] = {
    // yellow
    { pA },
    { pB },
    { pC },

    // green
    { pD },
    { pA },
    { pE },

    // blue
    { pJ },
    { pD },
    { pI },

    // red
    { pI },
    { pD },
    { pG },
    { pG },
    { pH },
    { pI },

    // cyan
    { pI },
    { pH },
    { pK },

    // magenta
    { pL },
    { pM },
    { pN },
    { pN },
    { pB },
    { pL },

    // orange
    { pD },
    { pF2 },
    { pF },
  };

  VertexColor colors[] = {
    // yellow
    { Vector3( 1., 1., 0. ) },
    { Vector3( 1., 1., 0. ) },
    { Vector3( 1., 1., 0. ) },

    // green
    { Vector3( 0., 1., 0. ) },
    { Vector3( 0., 1., 0. ) },
    { Vector3( 0., 1., 0. ) },

    // blue
    { Vector3( 0., 0., 1. ) },
    { Vector3( 0., 0., 1. ) },
    { Vector3( 0., 0., 1. ) },

    // red
    { Vector3( 1., 0., 0. ) },
    { Vector3( 1., 0., 0. ) },
    { Vector3( 1., 0., 0. ) },
    { Vector3( 1., 0., 0. ) },
    { Vector3( 1., 0., 0. ) },
    { Vector3( 1., 0., 0. ) },

    // cyan
    { Vector3( 0., 1., 1. ) },
    { Vector3( 0., 1., 1. ) },
    { Vector3( 0., 1., 1. ) },

    // magenta
    { Vector3( 1., 0., 1. ) },
    { Vector3( 1., 0., 1. ) },
    { Vector3( 1., 0., 1. ) },
    { Vector3( 1., 0., 1. ) },
    { Vector3( 1., 0., 1. ) },
    { Vector3( 1., 0., 1. ) },

    // orange
    { Vector3( 1., 0.5, 0. ) },
    { Vector3( 1., 0.5, 0. ) },
    { Vector3( 1., 0.5, 0. ) },

  };

  unsigned int numberOfVertices = sizeof(quad)/sizeof(VertexPosition);

  Property::Map initialPositionVertexFormat;
  initialPositionVertexFormat["aInitPos"] = Property::VECTOR2;
  PropertyBuffer initialPositionVertices = PropertyBuffer::New( initialPositionVertexFormat );
  initialPositionVertices.SetData( quad, numberOfVertices );

  Property::Map finalPositionVertexFormat;
  finalPositionVertexFormat["aFinalPos"] = Property::VECTOR2;
  PropertyBuffer finalPositionVertices = PropertyBuffer::New( finalPositionVertexFormat );
  finalPositionVertices.SetData( cat, numberOfVertices );

  Property::Map colorVertexFormat;
  colorVertexFormat["aColor"] = Property::VECTOR3;
  PropertyBuffer colorVertices = PropertyBuffer::New( colorVertexFormat );
  colorVertices.SetData( colors, numberOfVertices );

  // Create the geometry object
  Geometry texturedQuadGeometry = Geometry::New();
  texturedQuadGeometry.AddVertexBuffer( initialPositionVertices );
  texturedQuadGeometry.AddVertexBuffer( finalPositionVertices );
  texturedQuadGeometry.AddVertexBuffer( colorVertices );

  return texturedQuadGeometry;
}

inline float StationarySin( float progress ) ///< Single revolution
{
  float val = cosf(progress * 2.0f * Math::PI) + .5f;
  val = val > 1.f ? 1.f : val;
  val = val < 0.f ? 0.f : val;
  return val;
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

    mShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    mGeometry = CreateGeometry();
    mRenderer = Renderer::New( mGeometry, mShader );

    mMeshActor = Actor::New();
    mMeshActor.AddRenderer( mRenderer );
    mMeshActor.SetSize(400, 400);

    Property::Index morphDeltaIndex = mMeshActor.RegisterProperty( "uDelta", 0.f );

    mRenderer.SetProperty( Renderer::Property::DEPTH_INDEX, 0 );

    mMeshActor.SetParentOrigin( ParentOrigin::CENTER );
    mMeshActor.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( mMeshActor );

    Animation  animation = Animation::New(10);
    animation.AnimateTo( Property( mMeshActor, morphDeltaIndex ), 1.f, StationarySin );
    animation.SetLooping( true );
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

  Shader   mShader;
  Geometry mGeometry;
  Renderer mRenderer;
  Actor    mMeshActor;
  Timer    mMorphTimer;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  ExampleController test( application );
  application.MainLoop();
  return 0;
}
