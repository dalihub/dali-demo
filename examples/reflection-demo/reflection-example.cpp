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

#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/actors/camera-actor-devel.h>
#include <dali/devel-api/adaptor-framework/file-stream.h>

#include <map>

#include "gltf-scene.h"

using namespace Dali;
using Dali::Toolkit::TextLabel;

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec3 aPosition;\n
  attribute mediump vec3 aNormal;\n
  attribute mediump vec2 aTexCoord;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump mat3 uNormalMatrix;\n
  uniform mediump vec3 uSize;\n
  \n
  varying mediump vec2 vTexCoord; \n
  varying mediump vec3 vNormal; \n
  varying mediump vec3 vPosition; \n
  void main()\n
{\n
  mediump vec4 vertexPosition = vec4(aPosition, 1.0);\n
  vertexPosition.xyz *= uSize;\n
  vTexCoord = aTexCoord;\n
  vNormal = normalize(uNormalMatrix * aNormal);\n
  vPosition = aPosition; \n
  gl_Position = uMvpMatrix * vertexPosition;\n
}\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  uniform lowp vec4 uColor;\n
  uniform sampler2D sTexture; \n
  varying mediump vec3 vNormal;\n
  varying mediump vec3 vPosition; \n
  varying mediump vec2 vTexCoord; \n
  \n
  void main()\n
{\n
  gl_FragColor = texture2D(sTexture, vTexCoord) * 50.0;\n
}\n
);

const char* FRAGMENT_SIMPLE_SHADER = DALI_COMPOSE_SHADER(
        uniform lowp vec4 uColor;\n
        uniform sampler2D sTexture; \n
        varying mediump vec3 vNormal;\n
        varying mediump vec3 vPosition; \n
        varying mediump vec2 vTexCoord; \n
        \n
        void main()\n
{\n
        gl_FragColor = texture2D(sTexture, vTexCoord) * 2.0;\n
}\n
);

const char* TEXTURED_FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  uniform lowp vec4 uColor;\n
  uniform sampler2D sTexture; \n
  uniform mediump vec2 uScreenSize;\n

  uniform mediump vec3 eyePos;\n
  uniform mediump vec3 lightDir;\n

  varying mediump vec3 vNormal;\n
  varying mediump vec3 vPosition; \n
  varying mediump vec2 vTexCoord; \n
  \n
  void main()\n
{\n
  mediump vec3 n = normalize( vNormal );\n
  mediump vec3 l = normalize( lightDir );\n
  mediump vec3 e = normalize( eyePos );\n
  mediump float intensity = max(dot(n,l), 0.0);\n
  gl_FragColor = texture2D(sTexture, vTexCoord) * intensity;\n
}\n
);

const char* PLASMA_FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  precision mediump float;\n
  uniform sampler2D sTexture; \n

  uniform float uTime;
  uniform float uKFactor;
  uniform mediump vec3 eyePos;\n
  uniform mediump vec3 lightDir;\n
  varying mediump vec3 vNormal;\n
  varying mediump vec3 vPosition; \n
  varying mediump vec2 vTexCoord; \n
  \n
  void main()\n
{\n
  mediump vec3 n = normalize( vNormal );\n
  mediump vec3 l = normalize( lightDir );\n
  mediump vec3 e = normalize( eyePos );\n
  mediump float intensity = max(dot(n,l), 0.0);\n
\n
  const mediump float PI = 3.1415926535897932384626433832795;\n
  mediump float v = 0.0;\n
  mediump vec2 c = vTexCoord * uKFactor - uKFactor/2.0;\n
  v += sin((c.x+uTime));\n
  v += sin((c.y+uTime)/2.0);\n
  v += sin((c.x+c.y+uTime)/2.0);\n
  c += uKFactor/2.0 * vec2(sin(uTime/3.0), cos(uTime/2.0));\n
  v += sin(sqrt(c.x*c.x+c.y*c.y+1.0)+uTime);\n
  v = v/2.0;\n
  mediump vec3 col = vec3(1, sin(PI*v), cos(PI*v));\n
  gl_FragColor = (texture2D(sTexture, vTexCoord)) * (((col.r+col.g+col.b)/3.0)+1.0+intensity);\n
}\n
);

const char* TEX_FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  uniform lowp vec4 uColor;\n
  uniform sampler2D sTexture0; \n
  uniform sampler2D sTexture1; \n
  uniform mediump vec3 eyePos;\n
  uniform mediump vec3 lightDir;\n
  uniform mediump vec2 uScreenSize;\n
  varying mediump vec3 vNormal;\n
  varying mediump vec3 vPosition; \n
  varying mediump vec2 vTexCoord; \n
  \n

  mediump float rand(mediump vec2 co){\n
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);\n
  }\n
  \n
  void main()\n
{\n
  mediump vec2 tx = (gl_FragCoord.xy / uScreenSize.xy);\n
  mediump vec3 n = normalize( vNormal );\n
  mediump vec3 l = normalize( lightDir );\n
  mediump vec3 e = normalize( eyePos );\n
  mediump float factor = gl_FragCoord.y / uScreenSize.y;\n
  mediump float intensity = max(dot(n,l), 0.0);\n
  mediump vec2 uv = tx;\n
  gl_FragColor = ((texture2D(sTexture0, vTexCoord) * factor ) + \n
                 (texture2D(sTexture1, uv))) * intensity;\n
}\n
);

struct Model
{
  Shader    shader;
  Geometry  geometry;
};

// This example shows how to create and display mirrored reflection using CameraActor
//
class ReflectionExample : public ConnectionTracker
{
public:

  ReflectionExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ReflectionExample::Create );
  }

  ~ReflectionExample() = default;

private:

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    uint32_t stageWidth  = uint32_t(stage.GetSize().x);
    uint32_t stageHeight = uint32_t(stage.GetSize().y);

    stage.GetRenderTaskList().GetTask(0).SetClearEnabled(false);
    mLayer3D = Layer::New();
    mLayer3D.SetSize( stageWidth, stageHeight );
    stage.Add(mLayer3D);

    mLayer3D.SetAnchorPoint( AnchorPoint::CENTER );
    mLayer3D.SetParentOrigin( ParentOrigin::CENTER );
    mLayer3D.SetBehavior( Layer::LAYER_3D );
    mLayer3D.SetDepthTestDisabled( false );


    auto gltf = glTF(DEMO_GAME_DIR "/reflection");

    // Define direction of light
    mLightDir = Vector3( 0.5, 0.5, -1 );

    /**
     * Instantiate texture sets
     */
    CreateTextureSetsFromGLTF( &gltf, DEMO_GAME_DIR );

    /**
     * Create models
     */
    CreateModelsFromGLTF( &gltf );

    /**
     * Create scene nodes
     */
    CreateSceneFromGLTF( stage, &gltf );

    auto planeActor = mLayer3D.FindChildByName( "Plane" );
    auto solarActor = mLayer3D.FindChildByName( "solar_root" );
    auto backgroundActor = mLayer3D.FindChildByName( "background" );
    ReplaceShader( backgroundActor, VERTEX_SHADER, FRAGMENT_SIMPLE_SHADER );
    mCenterActor = mLayer3D.FindChildByName( "center" );
    mCenterHorizActor = mLayer3D.FindChildByName( "center2" );

    // Prepare Sun
    auto sun = mLayer3D.FindChildByName( "sun" );
    ReplaceShader( sun, VERTEX_SHADER, PLASMA_FRAGMENT_SHADER );
    mSunTimeUniformIndex = sun.RegisterProperty( "uTime", 0.0f );
    mSunKFactorUniformIndex = sun.RegisterProperty( "uKFactor", 0.0f );

    mTickTimer = Timer::New( 16 );
    mTickTimer.TickSignal().Connect( this, &ReflectionExample::TickTimerSignal);

    // Milkyway
    auto milkyway = mLayer3D.FindChildByName( "milkyway" );
    ReplaceShader( milkyway, VERTEX_SHADER, FRAGMENT_SHADER );

    auto renderTaskSourceActor = mLayer3D.FindChildByName( "RenderTaskSource" );

    /**
     * Access camera (it's a child of "Camera" node)
     */
    auto camera = mLayer3D.FindChildByName( "Camera_Orientation" );
    auto cameraRef = mLayer3D.FindChildByName( "CameraReflection_Orientation" );

    auto cameraActor = CameraActor::DownCast( camera );
    mCamera3D = cameraActor;

    auto cameraRefActor = CameraActor::DownCast( cameraRef );
    cameraRefActor.SetProperty( DevelCameraActor::Property::REFLECTION_PLANE, Vector4(0.0f, -1.0f, 0.0f, 0.0f));
    mReflectionCamera3D = cameraRefActor;

    auto task3D = stage.GetRenderTaskList().CreateTask();
    task3D.SetSourceActor( mLayer3D );
    task3D.SetViewport( Rect<int>(0, 0, stageWidth, stageHeight ) );
    task3D.SetCameraActor( cameraActor );
    task3D.SetClearColor( Color::BLACK );
    task3D.SetClearEnabled( true );
    task3D.SetExclusive( false );
    task3D.SetCameraActor( cameraActor );

    /**
     * Change shader to textured
     */
    Shader texShader = CreateShader( VERTEX_SHADER, TEX_FRAGMENT_SHADER );
    planeActor.RegisterProperty( "uScreenSize", Vector2(stageWidth, stageHeight) );
    auto renderer = planeActor.GetRendererAt(0);
    auto textureSet = renderer.GetTextures();
    renderer.SetShader( texShader );

    Texture fbTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::Format::RGBA8888, stageWidth, stageHeight );
    textureSet.SetTexture( 1u, fbTexture );

    auto fb = FrameBuffer::New(stageWidth, stageHeight,
                               FrameBuffer::Attachment::DEPTH );

    fb.AttachColorTexture( fbTexture );

    auto renderTask = stage.GetRenderTaskList().CreateTask();
    renderTask.SetFrameBuffer( fb );
    renderTask.SetSourceActor( renderTaskSourceActor );
    renderTask.SetViewport( Rect<int>(0, 0, stageWidth, stageHeight ) );
    renderTask.SetCameraActor( cameraRefActor );
    renderTask.SetClearColor( Color::BLACK );
    renderTask.SetClearEnabled( true );
    renderTask.SetExclusive( false );

    mAnimation = Animation::New(30.0f );
    mAnimation.AnimateBy(Property(solarActor, Actor::Property::ORIENTATION ),
                         Quaternion( Degree(359), Vector3(0.0, 1.0, 0.0)));
    mAnimation.AnimateBy(Property(milkyway, Actor::Property::ORIENTATION ),
                         Quaternion( Degree(-359), Vector3(0.0, 1.0, 0.0)));
    mAnimation.SetLooping(true );
    mAnimation.Play();

    Actor panScreen = Actor::New();
    auto stageSize = stage.GetSize();
    panScreen.SetSize( stageSize.width, stageSize.height );
    panScreen.SetAnchorPoint( AnchorPoint::CENTER );
    panScreen.SetParentOrigin( ParentOrigin::CENTER );
    auto camera2d = stage.GetRenderTaskList().GetTask(0).GetCameraActor();
    panScreen.SetPosition( 0, 0, camera2d.GetNearClippingPlane() );
    camera2d.Add(panScreen);
    camera2d.RotateBy( Degree(180.0f), Vector3( 0.0, 1.0, 0.0 ) );
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( panScreen );
    mPanGestureDetector.DetectedSignal().Connect( this, &ReflectionExample::OnPan );

    // Respond to key events
    stage.KeyEventSignal().Connect( this, &ReflectionExample::OnKeyEvent );

    mTickTimer.Start();
  }

  void CreateSceneFromGLTF( Stage stage, glTF* gltf )
  {
    const auto& nodes = gltf->GetNodes();

    // for each node create nodes and children
    // resolve parents later
    std::vector<Actor> actors;
    actors.reserve( nodes.size() );
    for( const auto& node : nodes )
    {
      auto actor = node.cameraId != 0xffffffff ? CameraActor::New( stage.GetSize() ) : Actor::New();

      actor.SetSize( 1, 1, 1 );
      actor.SetName( node.name );
      actor.SetAnchorPoint( AnchorPoint::CENTER );
      actor.SetParentOrigin( ParentOrigin::CENTER );
      actor.SetPosition( node.translation[0], node.translation[1], node.translation[2] );
      actor.SetScale( node.scale[0], node.scale[1], node.scale[2] );
      actor.SetOrientation( Quaternion(node.rotationQuaternion[3],
                                       node.rotationQuaternion[0],
                                       node.rotationQuaternion[1],
                                       node.rotationQuaternion[2]));

      actors.emplace_back( actor );

      // Initially add each actor to the very first one
      if(actors.size() > 1)
      {
        actors[0].Add(actor);
      }

      // If mesh, create and add renderer
      if(node.meshId != 0xffffffff)
      {
        const auto& model = mModels[node.meshId].get();
        auto renderer = Renderer::New( model->geometry, model->shader );

        // if textured, add texture set
        auto materialId = gltf->GetMeshes()[node.meshId]->material;
        if( materialId != 0xffffffff )
        {
          if( gltf->GetMaterials()[materialId].pbrMetallicRoughness.enabled )
          {
            renderer.SetTextures( mTextureSets[materialId] );
          }
        }

        actor.AddRenderer( renderer );
      }

      // Reset and attach main camera
      if( node.cameraId != 0xffffffff )
      {
        mCameraPos = Vector3(node.translation[0], node.translation[1], node.translation[2]);
        auto quatY = Quaternion( Degree(180.0f), Vector3( 0.0, 1.0, 0.0) );
        auto cameraActor = CameraActor::DownCast( actor );
        cameraActor.SetOrientation( Quaternion(node.rotationQuaternion[3],
                                               node.rotationQuaternion[0],
                                               node.rotationQuaternion[1],
                                               node.rotationQuaternion[2] )
                                    * quatY
                                      );
        cameraActor.SetProjectionMode( Camera::PERSPECTIVE_PROJECTION );

        const auto camera = gltf->GetCameras()[node.cameraId];
        cameraActor.SetNearClippingPlane( camera->znear );
        cameraActor.SetFarClippingPlane( camera->zfar );
        cameraActor.SetFieldOfView( camera->yfov );

        cameraActor.SetProperty( CameraActor::Property::INVERT_Y_AXIS, true);
        cameraActor.SetAnchorPoint( AnchorPoint::CENTER );
        cameraActor.SetParentOrigin( ParentOrigin::CENTER );

        mCameras.emplace_back( cameraActor );
      }
    }

    // Resolve hierarchy dependencies
    auto i = 0u;
    for( const auto& node : nodes )
    {
      if(!node.children.empty())
      {
        for(const auto& childId : node.children)
        {
          actors[i].Add( actors[childId+1] );
        }
      }
      ++i;
    }

    mActors = std::move(actors);

    // Add root actor to the stage
    mLayer3D.Add( mActors[0] );

    for( auto& actor : mActors )
    {
      actor.RegisterProperty( "lightDir", mLightDir );
      actor.RegisterProperty( "eyePos", mCameraPos );
    }

  }

  /**
   * Creates models from glTF
   */
  void CreateModelsFromGLTF( glTF* gltf )
  {
    const auto& meshes = gltf->GetMeshes();
    for( const auto& mesh : meshes )
    {
      // change shader to use texture if material indicates that
      if(mesh->material != 0xffffffff && gltf->GetMaterials()[mesh->material].pbrMetallicRoughness.enabled)
      {
        mModels.emplace_back( CreateModel( *gltf, mesh, VERTEX_SHADER, TEXTURED_FRAGMENT_SHADER ) );
      }
      else
      {
        mModels.emplace_back( CreateModel( *gltf, mesh, VERTEX_SHADER, FRAGMENT_SHADER ) );
      }
    }
  }

  void CreateTextureSetsFromGLTF( glTF* gltf, const std::string& basePath )
  {
    const auto& materials = gltf->GetMaterials();
    const auto& textures = gltf->GetTextures();

    std::map<std::string, Texture> textureCache{};

    for(const auto& material : materials )
    {
      TextureSet textureSet;
      if(material.pbrMetallicRoughness.enabled)
      {
        textureSet = TextureSet::New();
        std::string filename( basePath );
        filename += '/';
        filename += textures[material.pbrMetallicRoughness.baseTextureColor.index].uri;
        Dali::PixelData pixelData = Dali::Toolkit::SyncImageLoader::Load( filename );

        auto cacheKey = textures[material.pbrMetallicRoughness.baseTextureColor.index].uri;
        auto iter = textureCache.find(cacheKey);
        Texture texture;
        if(iter == textureCache.end())
        {
          texture = Texture::New(TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(),
                                         pixelData.GetHeight());
          texture.Upload(pixelData);
          texture.GenerateMipmaps();
          textureCache[cacheKey] = texture;
        }
        else
        {
          texture = iter->second;
        }
        textureSet.SetTexture( 0, texture );
        Dali::Sampler sampler = Dali::Sampler::New();
        sampler.SetWrapMode( Dali::WrapMode::REPEAT, Dali::WrapMode::REPEAT, Dali::WrapMode::REPEAT );
        sampler.SetFilterMode( Dali::FilterMode::LINEAR_MIPMAP_LINEAR, Dali::FilterMode::LINEAR );
        textureSet.SetSampler( 0, sampler );
      }
      mTextureSets.emplace_back( textureSet );
    }
  }

  template<class T>
  bool LoadFile( const std::string& filename, std::vector<T>& bytes )
  {
    Dali::FileStream fileStream( filename, Dali::FileStream::READ | Dali::FileStream::BINARY );
    FILE* fin = fileStream.GetFile();

    if( fin )
    {
      if( fseek( fin, 0, SEEK_END ) )
      {
        return false;
      }
      bytes.resize( uint32_t(ftell( fin )) );
      std::fill( bytes.begin(), bytes.end(), 0 );
      if( fseek( fin, 0, SEEK_SET ) )
      {
        return false;
      }
      size_t result = fread( bytes.data(), 1, bytes.size(), fin );
      return ( result != 0 );
    }

    return false;
  }

  Shader CreateShader( const std::string& vsh, const std::string& fsh )
  {
    std::vector<char> vshShaderSource;
    std::vector<char> fshShaderSource;

    // VSH
    if(vsh[0] == '/')
    {
      std::string vshPath( DEMO_GAME_DIR );
      vshPath += '/';
      vshPath += vsh;
      LoadFile( vshPath, vshShaderSource );
    }
    else
    {
      vshShaderSource.insert(vshShaderSource.end(), vsh.begin(), vsh.end());
    }

    // FSH
    if(fsh[0] == '/')
    {
      std::string fshPath( DEMO_GAME_DIR );
      fshPath += '/';
      fshPath += fsh;
      LoadFile( fshPath, fshShaderSource );
    }
    else
    {
      fshShaderSource.insert(fshShaderSource.end(), fsh.begin(), fsh.end());
    }

    vshShaderSource.emplace_back(0);
    fshShaderSource.emplace_back(0);
    return Shader::New( std::string(vshShaderSource.data()), std::string(fshShaderSource.data()) );
  }

  std::unique_ptr<Model> CreateModel( glTF& gltf,
                                      const glTF_Mesh* mesh,
                                      const std::string& vertexShaderSource,
                                      const std::string& fragmentShaderSource )
  {
    /*
     * Obtain interleaved buffer for first mesh with position and normal attributes
     */
    auto positionBuffer = gltf.GetMeshAttributeBuffer( *mesh,
                                                       {
                                                         glTFAttributeType::POSITION,
                                                         glTFAttributeType::NORMAL,
                                                         glTFAttributeType::TEXCOORD_0
                                                       } );

    auto attributeCount = gltf.GetMeshAttributeCount( mesh );
    /**
     * Create matching property buffer
     */
    auto vertexBuffer = PropertyBuffer::New( Property::Map()
                                               .Add("aPosition", Property::VECTOR3 )
                                               .Add("aNormal", Property::VECTOR3)
                                               .Add("aTexCoord", Property::VECTOR2)
    );

    // set vertex data
    vertexBuffer.SetData( positionBuffer.data(), attributeCount );

    auto geometry = Geometry::New();
    geometry.AddVertexBuffer( vertexBuffer );
    auto indexBuffer = gltf.GetMeshIndexBuffer( mesh );
    geometry.SetIndexBuffer( indexBuffer.data(), indexBuffer.size() );
    geometry.SetType( Geometry::Type::TRIANGLES );
    std::unique_ptr<Model> retval( new Model() );
    retval->shader = CreateShader( vertexShaderSource, fragmentShaderSource );
    retval->geometry = geometry;
    return retval;
  }

  void ReplaceShader( Actor& actor, const std::string& vsh, const std::string& fsh )
  {
    auto renderer = actor.GetRendererAt(0);
    auto shader = CreateShader(vsh, fsh);
    renderer.SetShader( shader );
  }

  void OnPan( Actor actor, const PanGesture& panGesture )
  {
    auto displacement = panGesture.screenDisplacement;
    mCenterActor.RotateBy( Degree( displacement.y *0.1f ), Vector3( 0.0, 0.0, 1.0) );
    mCenterActor.RotateBy( Degree( displacement.x *0.1f ), Vector3( 0.0, 1.0, 0.0) );
    Quaternion q;
    mCenterActor.GetProperty( Actor::Property::ORIENTATION ).Get(q);
    Matrix m = Matrix::IDENTITY;
    m.SetTransformComponents( Vector3::ONE, q, Vector3::ZERO );
    auto yAxis = m.GetYAxis() * -1.0f;

    yAxis.Normalize();
    mReflectionCamera3D.SetProperty( DevelCameraActor::Property::REFLECTION_PLANE, Vector4(yAxis.x, yAxis.y, yAxis.z, 0.0f));
  }

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

  bool TickTimerSignal()
  {
    auto root = mLayer3D;
    static float rotationAngle = 0.0f;

    const auto ROTATION_ANGLE_STEP = 0.05f;
    const auto FRAME_DELTA_TIME = 0.016f;
    const auto PLASMA_K_FACTOR = 12.0f; // 'granularity' of plasma effect

    rotationAngle += ROTATION_ANGLE_STEP;
    mMockTime += FRAME_DELTA_TIME;
    mKFactor = PLASMA_K_FACTOR;

    auto sun = root.FindChildByName( "sun" );
    sun.SetProperty( mSunTimeUniformIndex, mMockTime );
    sun.SetProperty( mSunKFactorUniformIndex, mKFactor );
    sun.SetOrientation( Quaternion( Radian(Degree(rotationAngle)), Vector3(0.0, 1.0, 0.0)));
    return true;
  }

private:
  Application&  mApplication;

  Layer mLayer3D;

  std::vector<Actor>                      mActors;
  std::vector<CameraActor>                mCameras;
  std::vector<std::unique_ptr<Model>>     mModels;
  std::vector<TextureSet>                 mTextureSets;

  Animation mAnimation;
  float mMockTime = 0.0f;
  float mKFactor = 0.0f;
  Property::Index mSunTimeUniformIndex;
  Property::Index mSunKFactorUniformIndex;
  PanGestureDetector mPanGestureDetector;

  Vector3 mCameraPos;
  Vector3 mLightDir;
  Timer mTickTimer;

  CameraActor mCamera3D;
  CameraActor mReflectionCamera3D;
  Actor mCenterActor;
  Actor mCenterHorizActor;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  ReflectionExample test( application );
  application.MainLoop();
  return 0;
}
