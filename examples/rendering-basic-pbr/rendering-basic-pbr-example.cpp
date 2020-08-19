/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <stdio.h>
#include <sstream>

// INTERNAL INCLUDES
#include "ktx-loader.h"
#include "model-skybox.h"
#include "model-pbr.h"
#include <dali/integration-api/debug.h>
#include <dali/devel-api/adaptor-framework/file-stream.h>

using namespace Dali;
using namespace Toolkit;

/*
 *
 * "papermill_pmrem.ktx" and "papermill_E_diffuse-64.ktx are image files generated from
 * image file downloaded from "http://www.hdrlabs.com/sibl/archive.html" with title
 * "Papermill Ruins E" by Blochi and is licensed under Creative Commons License
 * http://creativecommons.org/licenses/by-nc-sa/3.0/us/
 *
*/

namespace
{

const char* NORMAL_ROUGH_TEXTURE_URL = DEMO_IMAGE_DIR "Test_100_normal_roughness.png";
const char* ALBEDO_METAL_TEXTURE_URL = DEMO_IMAGE_DIR "Test_wblue_100_albedo_metal.png";
const char* CUBEMAP_SPECULAR_TEXTURE_URL = DEMO_IMAGE_DIR "papermill_pmrem.ktx";
const char* CUBEMAP_DIFFUSE_TEXTURE_URL = DEMO_IMAGE_DIR "papermill_E_diffuse-64.ktx";

const char* SPHERE_URL = DEMO_MODEL_DIR "sphere.obj";
const char* TEAPOT_URL = DEMO_MODEL_DIR "teapot.obj";

const char* VERTEX_SHADER_URL = DEMO_SHADER_DIR "pbr_shader.vsh";
const char* FRAGMENT_SHADER_URL = DEMO_SHADER_DIR "pbr_shader.fsh";

const Vector3 SKYBOX_SCALE( 1.0f, 1.0f, 1.0f );
const Vector3 SPHERE_SCALE( 1.5f, 1.5f, 1.5f );
const Vector3 TEAPOT_SCALE( 2.0f, 2.0f, 2.0f );

const float CAMERA_DEFAULT_FOV(    60.0f );
const float CAMERA_DEFAULT_NEAR(    0.1f );
const float CAMERA_DEFAULT_FAR(  1000.0f );
const Vector3 CAMERA_DEFAULT_POSITION( 0.0f, 0.0f, 3.5f );

}

/*
 *
 * This example shows a Physically Based Rendering illumination model.
 *
 * - Double-tap to toggle between 3D models (teapot & cube)
 * - Pan up/down on left side of screen to change roughness
 * - Pan up/down on right side of screen to change metalness
 * - Pan anywhere else to rotate scene
 *
*/

class BasicPbrController : public ConnectionTracker
{
public:

  BasicPbrController( Application& application )
  : mApplication( application ),
    mLabel(),
    m3dRoot(),
    mUiRoot(),
    mDoubleTapTime(),
    mModelOrientation(),
    mRoughness( 1.f ),
    mMetalness( 0.f ),
    mDoubleTap(false),
    mTeapotView(true)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &BasicPbrController::Create );
  }

  ~BasicPbrController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor( Color::BLACK );
    mAnimation = Animation::New( 1.0f );
    mLabel = TextLabel::New( "R:1 M:0" );
    mLabel.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER );
    mLabel.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER );
    mLabel.SetProperty( Actor::Property::SIZE, Vector2( window.GetSize().GetWidth() * 0.5f, window.GetSize().GetHeight() * 0.083f ) );
    mLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mLabel.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mLabel.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE );
    mLabel.SetProperty( Actor::Property::COLOR_ALPHA, 0.0f );
    // Step 1. Create shader
    CreateModelShader();

    // Step 2. Create texture
    CreateTexture();

    // Step 3. Initialise Main Actor
    InitActors();

    // Respond to a click anywhere on the window
    window.GetRootLayer().TouchSignal().Connect( this, &BasicPbrController::OnTouch );

    // Respond to key events
    window.KeyEventSignal().Connect( this, &BasicPbrController::OnKeyEvent );

    mDoubleTapTime = Timer::New(150);
    mDoubleTapTime.TickSignal().Connect( this, &BasicPbrController::OnDoubleTapTime );
  }


  bool OnDoubleTapTime()
  {
    mDoubleTap = false;
    return true;
  }

  /**
   * This function will change the material Roughness, Metalness or the model orientation when touched
   */
  bool OnTouch( Actor actor, const TouchData& touch )
  {
    const PointState::Type state = touch.GetState( 0 );

    switch( state )
    {
      case PointState::DOWN:
      {
        if(mDoubleTap)
        {
          mTeapotView = !mTeapotView;
          mModel[0].GetActor().SetProperty(Dali::Actor::Property::VISIBLE, !mTeapotView);
          mModel[1].GetActor().SetProperty(Dali::Actor::Property::VISIBLE, mTeapotView);
        }
        mDoubleTapTime.Stop();
        mStartTouch = touch.GetScreenPosition(0);
        mPointZ = mStartTouch;
        mAnimation.Stop();
        mLabel.SetProperty( Actor::Property::COLOR_ALPHA, 1.0f );
        break;
      }
      case PointState::MOTION:
      {
        const Window window = mApplication.GetWindow();
        const Size size = window.GetSize();
        const float scaleX = size.width;
        const float scaleY = size.height;
        const Vector2 point = touch.GetScreenPosition(0);
        bool process = false;
        if( ( mStartTouch.x < ( scaleX * 0.3f ) ) && ( point.x < ( scaleX * 0.3f ) ) )
        {
          mRoughness += ( mStartTouch.y - point.y ) / ( scaleY * 0.9f );

          //Clamp Roughness to 0.0 to 1.0
          mRoughness = std::max( 0.f, std::min( 1.f, mRoughness ) );

          mShader.SetProperty( mShader.GetPropertyIndex( "uRoughness" ), mRoughness );
          std::ostringstream oss;
          oss.precision(2);
          oss << " R:" << mRoughness<< "," << " M:" << mMetalness;
          mLabel.SetProperty( TextLabel::Property::TEXT, oss.str() );
          mStartTouch = point;
          process = true;
        }
        if( ( mStartTouch.x > ( scaleX * 0.7f ) ) && ( point.x > ( scaleX * 0.7f ) ) )
        {
          mMetalness += ( mStartTouch.y - point.y ) / ( scaleY * 0.9f );

          //Clamp Metalness to 0.0 to 1.0
          mMetalness = std::max( 0.f, std::min( 1.f, mMetalness ) );

          mShader.SetProperty( mShader.GetPropertyIndex( "uMetallic" ), mMetalness );
          std::ostringstream oss;
          oss.precision(2);
          oss << " R:" << mRoughness<< "," << " M:" << mMetalness;
          mLabel.SetProperty( TextLabel::Property::TEXT, oss.str() );
          mStartTouch = point;
          process = true;
        }
        //If the touch is not processed above, then change the model orientation
        if( !process )
        {
          process = true;
          const float angle1 = ( ( mPointZ.y - point.y ) / scaleY );
          const float angle2 = ( ( mPointZ.x - point.x ) / scaleX );
          Actor actor1 = mModel[0].GetActor();
          Actor actor2 = mModel[1].GetActor();

          Quaternion modelOrientation = mModelOrientation;
          modelOrientation.Conjugate();
          const Quaternion pitchRot(Radian(Degree(angle1 * -200.0f)), modelOrientation.Rotate( Vector3::XAXIS ) );
          const Quaternion yawRot(Radian(Degree(angle2 * -200.0f)), modelOrientation.Rotate( Vector3::YAXIS ) );

          mModelOrientation = mModelOrientation * yawRot * pitchRot ;
          mSkybox.GetActor().SetProperty( Actor::Property::ORIENTATION, mModelOrientation );
          actor1.SetProperty( Actor::Property::ORIENTATION, mModelOrientation );
          actor2.SetProperty( Actor::Property::ORIENTATION, mModelOrientation );

          mPointZ = point;
        }
        break;
      }
      case PointState::UP:
      {
        mDoubleTapTime.Start();
        mDoubleTap = true;
        mAnimation.AnimateTo( Property( mLabel, Actor::Property::COLOR_ALPHA ), 0.0f, TimePeriod( 0.5f, 1.0f ) );
        mAnimation.Play();
        break;
      }

      default:
      {
        break;
      }
    }
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
      if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

  /**
   * Creates new main actor
   */
  void InitActors()
  {
    Window window = mApplication.GetWindow();
    Vector2 windowSize = window.GetSize();

    mSkybox.Init( SKYBOX_SCALE );
    mModel[0].Init( mShader, SPHERE_URL, Vector3::ZERO, SPHERE_SCALE );
    mModel[1].Init( mShader, TEAPOT_URL, Vector3::ZERO, TEAPOT_SCALE );

    // Hide the model according with mTeapotView variable
    mModel[0].GetActor().SetProperty(Dali::Actor::Property::VISIBLE, !mTeapotView);
    mModel[1].GetActor().SetProperty(Dali::Actor::Property::VISIBLE,  mTeapotView);

    // Creating root and camera actor for rendertask for 3D Scene rendering
    mUiRoot = Actor::New();
    m3dRoot = Actor::New();
    CameraActor cameraUi = CameraActor::New(window.GetSize());
    cameraUi.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::CENTER);
    cameraUi.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER);

    RenderTask rendertask = window.GetRenderTaskList().CreateTask();
    rendertask.SetCameraActor( cameraUi );
    rendertask.SetSourceActor( mUiRoot );

    mUiRoot.SetProperty(Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
    mUiRoot.SetProperty(Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
    mUiRoot.SetProperty(Actor::Property::SIZE, Vector2(window.GetSize()) );

    m3dRoot.SetProperty(Actor::Property::ANCHOR_POINT,AnchorPoint::CENTER);
    m3dRoot.SetProperty(Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER);

    // Setting camera parameters for 3D Scene
    mSkybox.GetActor().SetProperty( Actor::Property::POSITION, CAMERA_DEFAULT_POSITION );
    CameraActor camera3d = window.GetRenderTaskList().GetTask(0).GetCameraActor();
    camera3d.SetInvertYAxis( true );
    camera3d.SetProperty( Actor::Property::POSITION, CAMERA_DEFAULT_POSITION );
    camera3d.SetNearClippingPlane( CAMERA_DEFAULT_NEAR );
    camera3d.SetFarClippingPlane( CAMERA_DEFAULT_FAR );
    camera3d.SetFieldOfView( Radian( Degree( CAMERA_DEFAULT_FOV ) ) );

    window.Add( cameraUi );
    window.Add( mUiRoot );
    window.Add( m3dRoot );

    m3dRoot.Add( mSkybox.GetActor() );
    m3dRoot.Add( mModel[0].GetActor() );
    m3dRoot.Add( mModel[1].GetActor() );


    if( (windowSize.x > 360.0f) && (windowSize.y > 360.0f) )
    {
      mUiRoot.Add( mLabel );
    }

  }

  /**
   * Creates a shader using file path
   */
  void CreateModelShader()
  {
    const std::string mCurrentVShaderFile( VERTEX_SHADER_URL );
    const std::string mCurrentFShaderFile( FRAGMENT_SHADER_URL );

    mShader = LoadShaders( mCurrentVShaderFile, mCurrentFShaderFile );

    // Initialise shader uniforms
    // Level 8 because the environment texture has 6 levels plus 2 are missing (2x2 and 1x1)
    mShader.RegisterProperty( "uMaxLOD", 8.0f );
    mShader.RegisterProperty( "uRoughness", 1.0f );
    mShader.RegisterProperty( "uMetallic" , 0.0f );
  }

  /**
   * Create Textures
   */
  void CreateTexture()
  {
    PixelData albeldoPixelData = SyncImageLoader::Load( ALBEDO_METAL_TEXTURE_URL );
    Texture textureAlbedoMetal = Texture::New( TextureType::TEXTURE_2D, albeldoPixelData.GetPixelFormat(), albeldoPixelData.GetWidth(), albeldoPixelData.GetHeight() );
    textureAlbedoMetal.Upload( albeldoPixelData, 0, 0, 0, 0, albeldoPixelData.GetWidth(), albeldoPixelData.GetHeight() );

    PixelData normalPixelData = SyncImageLoader::Load( NORMAL_ROUGH_TEXTURE_URL );
    Texture textureNormalRough = Texture::New( TextureType::TEXTURE_2D, normalPixelData.GetPixelFormat(), normalPixelData.GetWidth(), normalPixelData.GetHeight() );
    textureNormalRough.Upload( normalPixelData, 0, 0, 0, 0, normalPixelData.GetWidth(), normalPixelData.GetHeight() );

    // This texture should have 6 faces and only one mipmap
    PbrDemo::CubeData diffuse;
    PbrDemo::LoadCubeMapFromKtxFile( CUBEMAP_DIFFUSE_TEXTURE_URL, diffuse );

    Texture diffuseTexture = Texture::New( TextureType::TEXTURE_CUBE, diffuse.img[0][0].GetPixelFormat(), diffuse.img[0][0].GetWidth(), diffuse.img[0][0].GetHeight() );
    for( unsigned int midmapLevel = 0; midmapLevel < diffuse.img[0].size(); ++midmapLevel )
    {
      for( unsigned int i = 0; i < diffuse.img.size(); ++i )
      {
        diffuseTexture.Upload( diffuse.img[i][midmapLevel], CubeMapLayer::POSITIVE_X + i, midmapLevel, 0, 0, diffuse.img[i][midmapLevel].GetWidth(), diffuse.img[i][midmapLevel].GetHeight() );
      }
    }

    // This texture should have 6 faces and 6 mipmaps
    PbrDemo::CubeData specular;
    PbrDemo::LoadCubeMapFromKtxFile( CUBEMAP_SPECULAR_TEXTURE_URL, specular);

    Texture specularTexture = Texture::New( TextureType::TEXTURE_CUBE, specular.img[0][0].GetPixelFormat(), specular.img[0][0].GetWidth(), specular.img[0][0].GetHeight() );
    for( unsigned int midmapLevel = 0; midmapLevel < specular.img[0].size(); ++midmapLevel )
    {
      for( unsigned int i = 0; i < specular.img.size(); ++i )
      {
        specularTexture.Upload( specular.img[i][midmapLevel], CubeMapLayer::POSITIVE_X + i, midmapLevel, 0, 0, specular.img[i][midmapLevel].GetWidth(), specular.img[i][midmapLevel].GetHeight() );
      }
    }

    mModel[0].InitTexture( textureAlbedoMetal, textureNormalRough, diffuseTexture, specularTexture );
    mModel[1].InitTexture( textureAlbedoMetal, textureNormalRough, diffuseTexture, specularTexture );
    mSkybox.InitTexture( specularTexture );
  }

  /**
  * @brief Load a shader source file
  * @param[in] The path of the source file
  * @param[out] The contents of file
  * @return True if the source was read successfully
  */
  bool LoadShaderCode( const std::string& fullpath, std::vector<char>& output )
  {
    Dali::FileStream fileStream( fullpath, FileStream::READ | FileStream::BINARY );
    FILE* file = fileStream.GetFile();
    if( NULL == file )
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

    return retValue;
  }

  /**
  * @brief Load vertex and fragment shader source
  * @param[in] shaderVertexFileName is the filepath of Vertex shader
  * @param[in] shaderFragFileName is the filepath of Fragment shader
  * @return the Dali::Shader object
  */
  Shader LoadShaders( const std::string& shaderVertexFileName, const std::string& shaderFragFileName )
  {
    Shader shader;
    std::vector<char> bufV, bufF;

    if( LoadShaderCode( shaderVertexFileName.c_str(), bufV ) )
    {
      if( LoadShaderCode( shaderFragFileName.c_str(), bufF ) )
      {
        shader = Shader::New( bufV.data() , bufF.data() );
      }
    }
    return shader;
  }

private:
  Application& mApplication;
  TextLabel mLabel;
  Actor m3dRoot;
  Actor mUiRoot;
  Shader mShader;
  Animation mAnimation;
  Timer mDoubleTapTime;

  ModelSkybox mSkybox;
  ModelPbr mModel[2];

  Vector2 mPointZ;
  Vector2 mStartTouch;

  Quaternion mModelOrientation;
  float mRoughness;
  float mMetalness;
  bool mDoubleTap;
  bool mTeapotView;

};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv);
  BasicPbrController test( application );
  application.MainLoop();
  return 0;
}
