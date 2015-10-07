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
#include <stdio.h>
#include <sstream>
#include <cstring>

// INTERNAL INCLUDES
#include "shared/view.h"

using namespace Dali;

namespace
{

const char* MATERIAL_SAMPLES[] =
{
  DALI_IMAGE_DIR "people-medium-1.jpg",
  DALI_IMAGE_DIR "people-medium-4.jpg",
  DALI_IMAGE_DIR "people-medium-11.jpg",
  DALI_IMAGE_DIR "people-small-16.jpg",
  DALI_IMAGE_DIR "people-medium-15.jpg",
  DALI_IMAGE_DIR "people-medium-6.jpg",
};
const unsigned int NUMBER_OF_SAMPLES(sizeof(MATERIAL_SAMPLES)/sizeof(const char*));


#define MAKE_SHADER(A)#A

const char* VERTEX_SHADER = MAKE_SHADER(
uniform   highp   float   uHue;
attribute mediump vec2    aPosition;
attribute highp   vec2    aTexCoord;
varying   mediump vec2    vTexCoord;
uniform   mediump mat4    uMvpMatrix;
uniform   mediump vec3    uSize;
varying   mediump vec3    vGlobColor;

vec3 hsv2rgb(vec3 c)
{
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  vGlobColor = hsv2rgb( vec3( clamp(uHue, 0.0, 1.0), 1.0, 1.0 ) );

  vTexCoord = aTexCoord;
  gl_Position = vertexPosition;
}
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
varying mediump vec2  vTexCoord;
varying mediump vec3  vGlobColor;
uniform lowp    vec4  uColor;
uniform sampler2D     sTexture;

void main()
{
  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor * vec4(vGlobColor, 1.0) ;
}
);

Geometry CreateGeometry()
{
  // Create vertices
  const float halfQuadSize = .5f;
  struct TexturedQuadVertex { Vector2 position; Vector2 textureCoordinates; };
  TexturedQuadVertex texturedQuadVertexData[4] = {
    { Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f) },
    { Vector2( halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f) },
    { Vector2(-halfQuadSize,  halfQuadSize), Vector2(0.f, 1.f) },
    { Vector2( halfQuadSize,  halfQuadSize), Vector2(1.f, 1.f) } };

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aTexCoord"] = Property::VECTOR2;
  PropertyBuffer texturedQuadVertices = PropertyBuffer::New( texturedQuadVertexFormat, 4 );
  texturedQuadVertices.SetData(texturedQuadVertexData);

  // Create indices
  unsigned int indexData[6] = { 0, 3, 1, 0, 2, 3 };
  Property::Map indexFormat;
  indexFormat["indices"] = Property::INTEGER;
  PropertyBuffer indices = PropertyBuffer::New( indexFormat, 6 );
  indices.SetData(indexData);

  // Create the geometry object
  Geometry texturedQuadGeometry = Geometry::New();
  texturedQuadGeometry.AddVertexBuffer( texturedQuadVertices );
  texturedQuadGeometry.SetIndexBuffer( indices );

  return texturedQuadGeometry;
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
  : mApplication( application ),
    mZMode(0)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ExampleController::Create );
    memset(mDepthIndices, 0, sizeof(mDepthIndices));
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

    Material firstMat;

    for( unsigned i=0; i<NUMBER_OF_SAMPLES; ++i)
    {
      Image image = ResourceImage::New( MATERIAL_SAMPLES[i] );
      Material material = Material::New( mShader );
      material.AddTexture(image, "sTexture");
      if( i==0 ) { firstMat = material; }

      Renderer renderer = Renderer::New( mGeometry, material );
      Actor meshActor = Actor::New();
      mActors[i] = meshActor;
      meshActor.AddRenderer( renderer );
      meshActor.SetSize(175, 175);
      meshActor.RegisterProperty("index", (int)i);

      renderer.SetDepthIndex(0);
      // Test with actor alpha
      meshActor.SetParentOrigin( ParentOrigin::CENTER );
      meshActor.SetAnchorPoint( AnchorPoint::CENTER );
      meshActor.SetPosition( 40.0f*(i-(NUMBER_OF_SAMPLES*0.5f)), 40.0f*(i-(NUMBER_OF_SAMPLES*0.5f)), i*10 );

      meshActor.SetOpacity( i%2?0.7f:1.0f );

      meshActor.RegisterProperty("uHue", i/(float)NUMBER_OF_SAMPLES);

      meshActor.TouchedSignal().Connect(this, &ExampleController::OnTouched);
      std::ostringstream oss;
      oss << "Mesh Actor " << i;
      meshActor.SetName(oss.str());
      stage.Add( meshActor );
    }

    mActors[NUMBER_OF_SAMPLES-2].GetRendererAt(0).SetMaterial( firstMat );

    stage.GetRootLayer().TouchedSignal().Connect(this, &ExampleController::OnStageTouched);
  }

  void PrintDepths()
  {
    switch( mZMode )
    {
      case 0:
      {
        printf("Children Z ordered back to front\n");
        break;
      }
      case 1:
      {
        printf("All children set to same Z=0\n");
        break;
      }
      case 2:
      {
        printf("Children Z ordered front to back\n");
        break;
      }
    }

    for( unsigned i=0; i<NUMBER_OF_SAMPLES; ++i)
    {
      printf("DepthIndex[%d]=%d\n", i, mDepthIndices[i]);
    }
    printf("\n");
  }

  bool OnTouched( Actor actor, const TouchEvent& event )
  {
    if( event.GetPoint(0).state == TouchPoint::Finished )
    {
      int index = actor.GetProperty<int>(actor.GetPropertyIndex("index"));

      int newDepthIndex = (mDepthIndices[index] + 10) % 30;
      mDepthIndices[index] = newDepthIndex;

      Renderer renderer = actor.GetRendererAt(0);
      renderer.SetDepthIndex(newDepthIndex);

      PrintDepths();
    }
    return true;
  }

  bool OnStageTouched( Actor rootLayer, const TouchEvent& event )
  {
    if( event.GetPoint(0).state == TouchPoint::Finished )
    {
      switch( mZMode )
      {
        case 0:
        {
          mZMode = 1;
          for(unsigned int i=1; i < rootLayer.GetChildCount(); ++i)
          {
            Actor child = rootLayer.GetChildAt(i);
            child.SetZ( 0.0f );
          }
          PrintDepths();
          break;
        }
        case 1:
        {
          mZMode = 2;
          for(unsigned int i=1; i < rootLayer.GetChildCount(); ++i)
          {
            Actor child = rootLayer.GetChildAt(i);
            child.SetZ( 100-i*10 );
          }
          PrintDepths();
          break;
        }
        case 2:
        {
          mZMode = 0;
          for(unsigned int i=1; i < rootLayer.GetChildCount(); ++i)
          {
            Actor child = rootLayer.GetChildAt(i);
            child.SetZ( i*10 );
          }
          PrintDepths();
          break;
        }
      }
    }
    return true;
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

  int mDepthIndices[NUMBER_OF_SAMPLES];
  Actor mActors[NUMBER_OF_SAMPLES];
  int mZMode;
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
