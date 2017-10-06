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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

namespace
{

/*
 * Vertex shader
 */
const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
attribute mediump vec3 aPosition;\n // DALi shader builtin
attribute mediump vec2 aTexCoord;\n // DALi shader builtin
uniform   mediump mat4 uMvpMatrix;\n // DALi shader builtin
uniform   mediump vec3 uSize;\n // DALi shader builtin
\n
varying mediump vec2 vTexCoord;\n
void main()\n
{\n
  mediump vec4 vertexPosition = vec4(aPosition, 1.0);\n
  vertexPosition.xyz *= uSize;\n
  vTexCoord = aTexCoord;\n
  gl_Position = uMvpMatrix * vertexPosition;\n
}\n
);

/*
 * Fragment shader
 */
const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
uniform sampler2D uTexture;\n
\n
varying mediump vec2 vTexCoord;\n
void main()\n
{\n
  mediump vec4 texColor = texture2D( uTexture, vTexCoord );\n
  gl_FragColor = texColor;\n
}\n
);

const char* TEXTURE_URL = DEMO_IMAGE_DIR "wood.png";

}

// This example shows how to create textured cube
//
class TexturedCubeController : public ConnectionTracker
{
public:

  TexturedCubeController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TexturedCubeController::Create );
  }

  ~TexturedCubeController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    // Step 1. Create shader
    CreateCubeShader();

    // Step 2. Load a texture
    CreateTexture();

    // Step 3. Prepare geometry
    CreateCubeGeometry();

    // Step 4. Create a renderer
    CreateRenderer();

    // Step 5. Create an Actor
    CreateActor();

    // Step 6. Play animation to rotate the cube
    PlayAnimation();

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &TexturedCubeController::OnTouch );

    // Respond to key events
    stage.KeyEventSignal().Connect( this, &TexturedCubeController::OnKeyEvent );
  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  /**
   * @brief Called when any key event is received
   *
   * Will use this to quit the application if Back or the Escape key is received
   * @param[in] event The key event information
   */
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if ( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

  /**
   * @brief CreateCubeGeometry
   * This function creates a cube geometry including texture coordinates.
   * Also it demonstrates using the indexed draw feature by setting an index array.
   */
  void CreateCubeGeometry()
  {
    struct Vertex
    {
      Vector3 aPosition;
      Vector2 aTexCoord;
    };

    Vertex vertices[] = {
      { Vector3(  1.0f,-1.0f,-1.0f ), Vector2( 1.0, 1.0 ) },
      { Vector3( -1.0f, 1.0f,-1.0f ), Vector2( 0.0, 0.0 ) },
      { Vector3(  1.0f, 1.0f,-1.0f ), Vector2( 0.0, 1.0 ) },
      { Vector3( -1.0f, 1.0f, 1.0f ), Vector2( 1.0, 1.0 ) },
      { Vector3(  1.0f,-1.0f, 1.0f ), Vector2( 0.0, 0.0 ) },
      { Vector3(  1.0f, 1.0f, 1.0f ), Vector2( 0.0, 1.0 ) },
      { Vector3(  1.0f, 1.0f, 1.0f ), Vector2( 1.0, 1.0 ) },
      { Vector3(  1.0f,-1.0f,-1.0f ), Vector2( 0.0, 0.0 ) },
      { Vector3(  1.0f, 1.0f,-1.0f ), Vector2( 0.0, 1.0 ) },
      { Vector3(  1.0f,-1.0f, 1.0f ), Vector2( 1.0, 1.0 ) },
      { Vector3( -1.0f,-1.0f,-1.0f ), Vector2( 0.0, 0.0 ) },
      { Vector3(  1.0f,-1.0f,-1.0f ), Vector2( 0.0, 1.0 ) },
      { Vector3( -1.0f,-1.0f,-1.0f ), Vector2( 1.0, 1.0 ) },
      { Vector3( -1.0f, 1.0f, 1.0f ), Vector2( 0.0, 0.0 ) },
      { Vector3( -1.0f, 1.0f,-1.0f ), Vector2( 0.0, 1.0 ) },
      { Vector3(  1.0f, 1.0f,-1.0f ), Vector2( 1.0, 1.0 ) },
      { Vector3( -1.0f, 1.0f, 1.0f ), Vector2( 0.0, 0.0 ) },
      { Vector3(  1.0f, 1.0f, 1.0f ), Vector2( 0.0, 1.0 ) },
      { Vector3(  1.0f,-1.0f,-1.0f ), Vector2( 1.0, 1.0 ) },
      { Vector3( -1.0f,-1.0f,-1.0f ), Vector2( 1.0, 0.0 ) },
      { Vector3( -1.0f, 1.0f,-1.0f ), Vector2( 0.0, 0.0 ) },
      { Vector3( -1.0f, 1.0f, 1.0f ), Vector2( 1.0, 1.0 ) },
      { Vector3( -1.0f,-1.0f, 1.0f ), Vector2( 1.0, 0.0 ) },
      { Vector3(  1.0f,-1.0f, 1.0f ), Vector2( 0.0, 0.0 ) },
      { Vector3(  1.0f, 1.0f, 1.0f ), Vector2( 1.0, 1.0 ) },
      { Vector3(  1.0f,-1.0f, 1.0f ), Vector2( 1.0, 0.0 ) },
      { Vector3(  1.0f,-1.0f,-1.0f ), Vector2( 0.0, 0.0 ) },
      { Vector3(  1.0f,-1.0f, 1.0f ), Vector2( 1.0, 1.0 ) },
      { Vector3( -1.0f,-1.0f, 1.0f ), Vector2( 1.0, 0.0 ) },
      { Vector3( -1.0f,-1.0f,-1.0f ), Vector2( 0.0, 0.0 ) },
      { Vector3( -1.0f,-1.0f,-1.0f ), Vector2( 1.0, 1.0 ) },
      { Vector3( -1.0f,-1.0f, 1.0f ), Vector2( 1.0, 0.0 ) },
      { Vector3( -1.0f, 1.0f, 1.0f ), Vector2( 0.0, 0.0 ) },
      { Vector3(  1.0f, 1.0f,-1.0f ), Vector2( 1.0, 1.0 ) },
      { Vector3( -1.0f, 1.0f,-1.0f ), Vector2( 1.0, 0.0 ) },
      { Vector3( -1.0f, 1.0f, 1.0f ), Vector2( 0.0, 0.0 ) },
    };

    PropertyBuffer vertexBuffer = PropertyBuffer::New( Property::Map()
                                                       .Add( "aPosition", Property::VECTOR3 )
                                                       .Add( "aTexCoord", Property::VECTOR2 ) );
    vertexBuffer.SetData( vertices, sizeof(vertices) / sizeof(Vertex) );

    // create indices
    const unsigned short INDEX_CUBE[] = {
      2, 1, 0,
      5, 4, 3,
      8, 7, 6,
      11, 10, 9,
      14, 13, 12,
      17, 16, 15,
      20, 19, 18,
      23, 22, 21,
      26, 25, 24,
      29, 28, 27,
      32, 31, 30,
      35, 34, 33
    };
    mGeometry = Geometry::New();
    mGeometry.AddVertexBuffer( vertexBuffer );
    mGeometry.SetIndexBuffer( INDEX_CUBE,
                              sizeof(INDEX_CUBE)/sizeof(INDEX_CUBE[0]) );
    mGeometry.SetType( Geometry::TRIANGLES );
  }

  /**
   * Creates a shader using inlined variable VERTEX_SHADER and FRAGMENT_SHADER
   *
   * Shaders are very basic and all they do is transforming vertices and sampling
   * a texture.
   */
  void CreateCubeShader()
  {
    mShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
  }

  /**
   * This function loads a pixel data from a file. In order to load it we use SyncImageLoader utility.
   * If loading succeeds returned PixelData object can be used to create a texture.
   * Texture must be uploaded. In the end the texture must be set on the TextureSet object.
   */
  void CreateTexture()
  {
    // Load image from file
    PixelData pixels = SyncImageLoader::Load( TEXTURE_URL );

    Texture texture = Texture::New( TextureType::TEXTURE_2D, pixels.GetPixelFormat(), pixels.GetWidth(), pixels.GetHeight() );
    texture.Upload( pixels, 0, 0, 0, 0, pixels.GetWidth(), pixels.GetHeight() );

    // create TextureSet
    mTextureSet = TextureSet::New();
    mTextureSet.SetTexture( 0, texture );
  }

  /**
   * Function creates renderer. It turns on depth test and depth write.
   */
  void CreateRenderer()
  {
    mRenderer = Renderer::New( mGeometry, mShader );
    mRenderer.SetTextures( mTextureSet );

    // Face culling is enabled to hide the backwards facing sides of the cube
    // This is sufficient to render a single object; for more complex scenes depth-testing might be required
    mRenderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK );
  }

  /**
   * Creates new actor and attaches renderer.
   */
  void CreateActor()
  {
    Stage stage = Stage::GetCurrent();

    float quarterStageWidth = stage.GetSize().x * 0.25f;
    mActor = Actor::New();
    mActor.SetAnchorPoint( AnchorPoint::CENTER );
    mActor.SetParentOrigin( ParentOrigin::CENTER );
    mActor.SetPosition( Vector3( 0.0f, 0.0f, 0.0f ) );
    mActor.SetSize( Vector3( quarterStageWidth, quarterStageWidth, quarterStageWidth ) );
    mActor.AddRenderer( mRenderer );
    stage.Add( mActor );
  }

  /**
   * Plays animation
   */
  void PlayAnimation()
  {
    mAnimation = Animation::New( 5.0f );
    mAnimation.SetLooping( true );
    mAnimation.AnimateBy( Property( mActor, Actor::Property::ORIENTATION ), Quaternion( Radian( Degree( 360 )), Vector3::ZAXIS ) );
    mAnimation.AnimateBy( Property( mActor, Actor::Property::ORIENTATION ), Quaternion( Radian( Degree( 360 )), Vector3::YAXIS ) );
    mAnimation.AnimateBy( Property( mActor, Actor::Property::ORIENTATION ), Quaternion( Radian( Degree( 360 )), Vector3::XAXIS ) );
    mAnimation.Play();
  }

private:
  Application&  mApplication;

  Renderer mRenderer;
  Shader mShader;
  Geometry mGeometry;
  TextureSet mTextureSet;
  Actor mActor;
  Animation mAnimation;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  TexturedCubeController test( application );
  application.MainLoop();
  return 0;
}
