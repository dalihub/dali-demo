/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <cstdio>
#include <string>
#include <cstdint> // uint32_t, uint16_t etc

#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/math/random.h>

// INTERNAL INCLUDES
#include "shared/utility.h" // DemoHelper::LoadTexture

using namespace Dali;

namespace // unnamed namespace for constants
{
// background image
const char * const BACKGROUND_IMAGE( DEMO_IMAGE_DIR "background-2.jpg" );

// number of metaballs
constexpr uint32_t METABALL_NUMBER = 6;

/**
 * Vertex shader code for metaball
 */
const char* const METABALL_VERTEX_SHADER = DALI_COMPOSE_SHADER (
    attribute mediump vec2    aPosition;\n
    attribute mediump vec2    aTexture;\n
    uniform   mediump mat4    uMvpMatrix;\n
    uniform   mediump vec3    uSize;\n
    uniform   lowp    vec4    uColor;\n
    varying   mediump vec2    vTexCoord;\n

    void main()\n
    {\n
      vTexCoord = aTexture;\n
      mediump vec4 vertexPosition = vec4(aPosition.x, aPosition.y, 0.0, 1.0);\n
      gl_Position = uMvpMatrix * vertexPosition;\n
    }\n
);

/**
 * Fragment shader code for metaball
 */
const char* const METABALL_FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision mediump float;\n
  varying vec2 vTexCoord;\n
  uniform vec2 uPositionMetaball;\n
  uniform vec2 uPositionVar;\n
  uniform vec2 uGravityVector;\n
  uniform float uRadius;\n
  uniform float uRadiusVar;\n
  void main()\n
  {\n
    vec2 adjustedCoords = vTexCoord * 2.0 - 1.0;\n
    vec2 finalMetaballPosition = uPositionMetaball + uGravityVector + uPositionVar;\n
    \n
    float finalRadius = uRadius + uRadiusVar;\n
    vec2 distanceVec = adjustedCoords - finalMetaballPosition;\n
    float result = dot(distanceVec, distanceVec);\n
    float color = inversesqrt(result) * finalRadius;\n
    \n
    gl_FragColor = vec4(color,color,color,1.0);\n
  }\n
);

/**
 * Fragment shader code for metaball and background composition with refraction effect
 */
const char* const REFRACTION_FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision highp float;\n
  varying vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform sampler2D sEffect;\n
  uniform vec2 uPositionMetaball;\n
  void main()\n
  {\n
    vec2 zoomCoords;\n
    vec3 normal = vec3(0.0,0.0,1.0);\n
    vec2 fakePos = vec2(0.0,0.0);\n
    vec3 color = vec3(1.0, 1.0, 1.0);
    float ambient = 0.2;
    \n
    vec4 metaColor = texture2D(sEffect, vTexCoord);\n
    \n
    vec2 adjustedCoords = vTexCoord.xy * vec2(2.0) - vec2(1.0);\n
    fakePos = adjustedCoords.xy - vec2(uPositionMetaball.x, -uPositionMetaball.y);
    float len = length(fakePos) + 0.01;\n
    vec3 colorPos = vec3(0,0,1);
    \n
    if (metaColor.r > 0.85)\n
    {\n
      zoomCoords = ((vTexCoord - 0.5) * 0.9);\n
      zoomCoords = zoomCoords + 0.5;\n
      \n
      float interpNormal = mix(0.7, 1.0, (metaColor.r - 0.85) * 4.);\n
      normal.xyz = vec3(fakePos.x * (1.0 - interpNormal) / len, fakePos.y * (1.0 - interpNormal) / len, interpNormal);\n
      normal.xyz = normalize(normal.xyz);\n
      color = vec3(0.65, 1.0, 0);\n
      colorPos = vec3(fakePos.x,fakePos.y,0);
    }\n
    else if (metaColor.r > 0.75)\n
    {\n
      float interpolation = mix(0.9, 1.15, (0.85 - metaColor.r) * 10.0);\n
      zoomCoords = ((vTexCoord - 0.5) * interpolation);\n
      zoomCoords = zoomCoords + 0.5;\n
      \n
      float interpNormal = mix(0.7, 0.0, (0.85 - metaColor.r) * 10.0);\n
      normal.xyz = vec3(fakePos.x * (1.0 - interpNormal) / len, fakePos.y * (1.0 - interpNormal) / len, interpNormal);\n
      normal.xyz = normalize(normal.xyz);\n
      color = vec3(0.65, 1.0, 0);\n
      colorPos = vec3(fakePos.x,fakePos.y,0);
    }\n
    else\n
    {\n
      zoomCoords = vTexCoord;\n
      normal = vec3(0,0,0);\n
      ambient = 0.5;\n
    }\n
    \n
    vec3 lightPosition = vec3(-750.0,-1000.0,2000.0);\n
    vec3 vertex = vec3(adjustedCoords.x,adjustedCoords.y,0.0);\n
    \n
    vec3 vecToLight = normalize( lightPosition - vertex );\n
    \n
    float lightDiffuse = dot( vecToLight, normal );\n
    lightDiffuse = max(0.0,lightDiffuse);\n
    lightDiffuse = lightDiffuse * 0.5 + 0.5;
    \n
    vec3 vertexToEye = vec3(0,0,1) - vertex;\n
    vertexToEye = normalize(vertexToEye);
    vec3 lightReflect = normalize(reflect(-vecToLight, normal));\n
    float specularFactor = max(0.0,dot(vertexToEye, lightReflect));\n
    specularFactor = pow(specularFactor, 32.0) * 0.7;
    \n
    vec4 texColor = texture2D(sTexture, zoomCoords);\n
    gl_FragColor.rgb = texColor.rgb * ambient + color.rgb * texColor.rgb * lightDiffuse + vec3(specularFactor);\n
    gl_FragColor.a = 1.0;
  }\n
 );

/**
 * Metadata for each ball
 */
struct MetaballInfo
{
  Actor         actor;
  Vector2       position;
  float         radius;
  float         initRadius;

  //new shader stuff
  Property::Index positionIndex;
  Property::Index positionVarIndex;
};

} // unnamed namespace

/**
 * Demo using Metaballs
 *
 * When the metaball is clicked it explodes to smaller balls
 */
class MetaballExplosionController : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application
   */
  MetaballExplosionController( Application& application );

  /**
   * Destructor
   */
  virtual ~MetaballExplosionController();

  /**
   * Creates the metaballs and initializes the scene
   */
  void Create( Application& app );

  /**
   * Touch event handler to center metaballs at touch position
   * and start explosion animation on release
   */
  bool OnTouch( Actor actor, const TouchData& touch );

  /**
   * Key event handler to quit application on escape or back key
   */
  void OnKeyEvent(const KeyEvent& event);

private: // Data

  Application&      mApplication;
  Vector2           mScreenSize;

  Texture           mBackgroundTexture;
  FrameBuffer       mMetaballFBO;

  Actor             mMetaballRoot;
  MetaballInfo      mMetaballs[METABALL_NUMBER];

  Property::Index   mPositionIndex;
  Actor             mCompositionActor;

  //Motion
  Vector2           mCurrentTouchPosition;
  Vector2           mMetaballPosVariation;
  Vector2           mMetaballPosVariationFrom;
  Vector2           mMetaballPosVariationTo;
  Vector2           mMetaballCenter;

  //Animations
  Animation         mPositionVarAnimation[METABALL_NUMBER];

  uint32_t          mDispersion;
  Animation         mDispersionAnimation[METABALL_NUMBER];

  Timer             mTimerDispersion;

  float             mTimeMultiplier;

  // Private helper functions

  /**
   * Create a mesh data with the geometry for the metaball rendering
   * @param aspectMappedTexture whether texture coords should be mapped based on aspect ratio
   */
  Geometry CreateGeometry( bool aspectMappedTexture = true );

  /**
   * Create a actors and renderers for the metaballs
   */
  void CreateMetaballActors();

  /**
   * Create the render task and FBO to render the metaballs into a texture
   */
  void CreateMetaballImage();

  /**
   * Create the the final composition
   */
  void CreateComposition();

  /**
   * Function to create animations for the small variations of position inside the metaball
   */
  void CreateAnimations();

  /**
   * Function to reset metaball state
   */
  void ResetMetaballs( bool resetAnims );

  /**
   * Function to create disperse each of the ball that compose the metaball when exploding
   */
  void DisperseBallAnimation( uint32_t ball );

  /**
   * Function to make metaballs come back to reset position
   */
  void LaunchResetMetaballPosition( Animation& source );

  /**
   * Function to set things at the end of the animation
   */
  void EndDisperseAnimation( Animation& source );

  /**
   * Function to init dispersion of the metaballs one by one using a timer
   * (so not all the balls begin moving at the same time)
   */
  bool OnTimerDispersionTick();

  /**
   * Function to set the actual position of the metaballs when the user clicks the screen
   */
  void SetPositionToMetaballs( const Vector2& metaballCenter );
};

/**
 * Implementation
 */

MetaballExplosionController::MetaballExplosionController( Application& application )
: mApplication( application ),
  mScreenSize(),
  mBackgroundTexture(),
  mMetaballFBO(),
  mMetaballRoot(),
  mMetaballs(),
  mPositionIndex(),
  mCompositionActor(),
  mCurrentTouchPosition(),
  mMetaballPosVariation(),
  mMetaballPosVariationFrom(),
  mMetaballPosVariationTo(),
  mMetaballCenter(),
  mPositionVarAnimation(),
  mDispersion( 0 ),
  mDispersionAnimation(),
  mTimerDispersion(),
  mTimeMultiplier( 1.0f )
{
  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &MetaballExplosionController::Create );
}

MetaballExplosionController::~MetaballExplosionController()
{
  // Nothing to do here;
}

void MetaballExplosionController::Create( Application& app )
{
  Stage stage = Stage::GetCurrent();

  stage.KeyEventSignal().Connect( this, &MetaballExplosionController::OnKeyEvent );

  mScreenSize = stage.GetSize();

  mTimeMultiplier = 1.0f;

  stage.SetBackgroundColor(Color::BLACK);

  // Load background texture
  mBackgroundTexture = DemoHelper::LoadTexture( BACKGROUND_IMAGE );

  srand( static_cast<uint32_t>( time(0) ) );

  //Create internal data
  CreateMetaballActors();
  CreateMetaballImage();
  CreateComposition();

  CreateAnimations();

  mDispersion = 0;
  mTimerDispersion = Timer::New( 150 );
  mTimerDispersion.TickSignal().Connect( this, &MetaballExplosionController::OnTimerDispersionTick );

  // Connect the callback to the touch signal on the mesh actor
  stage.GetRootLayer().TouchSignal().Connect( this, &MetaballExplosionController::OnTouch );
}

Geometry MetaballExplosionController::CreateGeometry( bool aspectMappedTexture )
{
  const float aspect = mScreenSize.y / mScreenSize.x;

  // Create vertices and specify their color
  const float xsize = mScreenSize.x * 0.5;

  // Create the meshdata for the metaballs
  struct VertexPosition { Vector2 position; };
  struct VertexTexture { Vector2 texture; };

  VertexPosition vertices[] =
  {
    { Vector2( -xsize, -xsize * aspect ) },
    { Vector2(  xsize, -xsize * aspect ) },
    { Vector2( -xsize,  xsize * aspect ) },
    { Vector2(  xsize,  xsize * aspect ) }
  };

  const float textureAspect = (aspectMappedTexture) ? aspect : 1.0f;
  VertexTexture textures[] =
  {
    { Vector2( 0.0f, 0.0f ) },
    { Vector2( 1.0f, 0.0f ) },
    { Vector2( 0.0f, 1.0f * textureAspect ) },
    { Vector2( 1.0f, 1.0f * textureAspect ) }
  };

  uint32_t numberOfVertices = sizeof(vertices)/sizeof(VertexPosition);

  // Vertices
  Property::Map positionVertexFormat;
  positionVertexFormat["aPosition"] = Property::VECTOR2;
  PropertyBuffer positionVertices = PropertyBuffer::New( positionVertexFormat );
  positionVertices.SetData( vertices, numberOfVertices );

  // Textures
  Property::Map textureVertexFormat;
  textureVertexFormat["aTexture"] = Property::VECTOR2;
  PropertyBuffer textureVertices = PropertyBuffer::New( textureVertexFormat );
  textureVertices.SetData( textures, numberOfVertices );

  // Indices
  const uint16_t indices[] = { 0, 3, 1, 0, 2, 3 };

  // Create the geometry object
  Geometry texturedQuadGeometry = Geometry::New();
  texturedQuadGeometry.AddVertexBuffer( positionVertices );
  texturedQuadGeometry.AddVertexBuffer( textureVertices );

  texturedQuadGeometry.SetIndexBuffer ( &indices[0], sizeof( indices )/ sizeof( indices[0] ) );

  return texturedQuadGeometry;
}

void MetaballExplosionController::CreateMetaballActors()
{
  // Create the shader for the metaballs, tell DALi that shader modifies geometry so we dont need to set a meaningless size
  Shader shader = Shader::New( METABALL_VERTEX_SHADER, METABALL_FRAG_SHADER, Shader::Hint::MODIFIES_GEOMETRY );

  Geometry metaballGeom = CreateGeometry();
  // Reuse same renderer for each actor
  Renderer renderer = Renderer::New( metaballGeom, shader );
  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_SRC_RGB,    BlendFactor::ONE );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_RGB,   BlendFactor::ONE );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_SRC_ALPHA,  BlendFactor::ONE );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_ALPHA, BlendFactor::ONE  );

  //Initialization of each of the metaballs
  for( uint32_t i = 0; i < METABALL_NUMBER; i++ )
  {
    mMetaballs[i].position = Vector2(0.0f, 0.0f);
    mMetaballs[i].radius = mMetaballs[i].initRadius = Random::Range(0.05f,0.07f);

    mMetaballs[i].actor = Actor::New( );
    mMetaballs[i].actor.SetProperty( Dali::Actor::Property::NAME, "Metaball" );
    mMetaballs[i].actor.SetProperty( Actor::Property::SCALE, 1.0f );
    mMetaballs[i].actor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mMetaballs[i].actor.AddRenderer( renderer );

    mMetaballs[i].positionIndex = mMetaballs[i].actor.RegisterProperty( "uPositionMetaball", mMetaballs[i].position );

    mMetaballs[i].positionVarIndex = mMetaballs[i].actor.RegisterProperty( "uPositionVar", Vector2(0.f,0.f) );

    mMetaballs[i].actor.RegisterProperty( "uGravityVector", Vector2(Random::Range(-0.2,0.2),Random::Range(-0.2,0.2)) );
    mMetaballs[i].actor.RegisterProperty( "uRadius", mMetaballs[i].radius );
    mMetaballs[i].actor.RegisterProperty( "uRadiusVar", 0.f );
  }

  // Root creation
  mMetaballRoot = Actor::New();
  mMetaballRoot.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  for( uint32_t i = 0; i < METABALL_NUMBER; i++ )
  {
    mMetaballRoot.Add( mMetaballs[i].actor );
  }

}

void MetaballExplosionController::CreateMetaballImage()
{
  // Create an FBO and a render task to create to render the metaballs with a fragment shader
  Stage stage = Stage::GetCurrent();

  mMetaballFBO = FrameBuffer::New( mScreenSize.x, mScreenSize.y );

  stage.Add(mMetaballRoot);

  // Create the render task used to render the metaballs
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.CreateTask();
  task.SetRefreshRate( RenderTask::REFRESH_ALWAYS );
  task.SetSourceActor( mMetaballRoot );
  task.SetExclusive( true );
  task.SetClearColor( Color::BLACK );
  task.SetClearEnabled( true );
  task.SetFrameBuffer( mMetaballFBO );
}

void MetaballExplosionController::CreateComposition()
{
  //Create new shader
  Shader shader = Shader::New( METABALL_VERTEX_SHADER, REFRACTION_FRAG_SHADER );

  // Create new texture set
  auto textureSet = TextureSet::New();
  textureSet.SetTexture( 0u, mBackgroundTexture  );
  textureSet.SetTexture( 1u, mMetaballFBO.GetColorTexture() );

  // Create geometry
  Geometry metaballGeom = CreateGeometry( false );

  Renderer mRenderer = Renderer::New( metaballGeom, shader );
  mRenderer.SetTextures( textureSet );

  // Create actor
  mCompositionActor = Actor::New( );
  mCompositionActor.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER);
  mCompositionActor.SetProperty( Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  mCompositionActor.SetProperty( Actor::Property::SIZE, Vector2(mScreenSize.x, mScreenSize.y) );
  mCompositionActor.AddRenderer( mRenderer );

  Vector2 metaballCenter(0.0,0);
  metaballCenter.x = metaballCenter.x * 0.5;
  metaballCenter.y = metaballCenter.y * 0.5;
  mPositionIndex = mCompositionActor.RegisterProperty( "uPositionMetaball", metaballCenter );

  SetPositionToMetaballs( metaballCenter );

  mCompositionActor.SetProperty( Actor::Property::SIZE, Vector2(mScreenSize.x, mScreenSize.y) );

  Stage stage = Stage::GetCurrent();
  stage.Add( mCompositionActor );
}

void MetaballExplosionController::CreateAnimations()
{
  Vector2 direction;

  for( uint32_t i = 0; i < METABALL_NUMBER; i++ )
  {
    KeyFrames keySinCosVariation = KeyFrames::New();
    Vector2 sinCosVariation( 0,0 );

    direction.x = Random::Range( -100.f,100.f );
    direction.y = Random::Range( -100.f,100.f );

    direction.Normalize();
    direction *= 0.1f;

    for( uint32_t j = 0; j < 360; j++ )
    {
      sinCosVariation.x = sinf( j * Math::PI/180.f ) * direction.x;
      sinCosVariation.y = cosf( j * Math::PI/180.f ) * direction.y;
      float key = j/360.f;
      keySinCosVariation.Add( key, sinCosVariation );
    }

    mPositionVarAnimation[i] = Animation::New( 3.f );
    mPositionVarAnimation[i].AnimateBetween( Property( mMetaballs[i].actor, mMetaballs[i].positionVarIndex ), keySinCosVariation );
    mPositionVarAnimation[i].SetLooping( true );
    mPositionVarAnimation[i].Play();
  }
}

void MetaballExplosionController::ResetMetaballs( bool resetAnims )
{
  for( uint32_t i = 0; i < METABALL_NUMBER; i++ )
  {
    if( mDispersionAnimation[i] )
    {
      mDispersionAnimation[i].Clear();
    }

    mMetaballs[i].position = Vector2( 0.0f, 0.0f );
    mMetaballs[i].actor.SetProperty( mMetaballs[i].positionIndex, mMetaballs[i].position );
  }
  mTimerDispersion.Stop();
  mDispersion = 0;

  mCompositionActor.SetProperty( mPositionIndex, Vector2(0,0) );
}

void MetaballExplosionController::DisperseBallAnimation( uint32_t ball )
{
  Vector2 position;
  position.x = Random::Range(-1.5f,1.5f);
  position.y = Random::Range(-1.5f,1.5f);

  mDispersionAnimation[ball] = Animation::New(2.0f * mTimeMultiplier);
  mDispersionAnimation[ball].AnimateTo( Property(mMetaballs[ball].actor, mMetaballs[ball].positionIndex), position);
  mDispersionAnimation[ball].Play();

  if( ball == METABALL_NUMBER - 1 )
  {
    mDispersionAnimation[ball].FinishedSignal().Connect( this, &MetaballExplosionController::LaunchResetMetaballPosition );
  }
}

void MetaballExplosionController::LaunchResetMetaballPosition( Animation& source )
{
  for( uint32_t i = 0; i < METABALL_NUMBER; i++ )
  {
    mDispersionAnimation[i] = Animation::New( 1.5f + i * 0.25f * mTimeMultiplier );
    mDispersionAnimation[i].AnimateTo(Property( mMetaballs[i].actor, mMetaballs[i].positionIndex), Vector2(0,0) );
    mDispersionAnimation[i].Play();

    if( i == METABALL_NUMBER - 1 )
    {
      mDispersionAnimation[i].FinishedSignal().Connect( this, &MetaballExplosionController::EndDisperseAnimation );
    }
  }
}

void MetaballExplosionController::EndDisperseAnimation( Animation& source )
{
  mCompositionActor.SetProperty( mPositionIndex, Vector2(0,0) );
}

bool MetaballExplosionController::OnTimerDispersionTick()
{
  if( mDispersion < METABALL_NUMBER )
  {
    DisperseBallAnimation( mDispersion );
    mDispersion++;
  }
  return true;
}

void MetaballExplosionController::SetPositionToMetaballs( const Vector2& metaballCenter )
{
  //We set the position for the metaballs based on click position
  for( uint32_t i = 0; i < METABALL_NUMBER; i++ )
  {
    mMetaballs[i].position = metaballCenter;
    mMetaballs[i].actor.SetProperty( mMetaballs[i].positionIndex, mMetaballs[i].position );
  }

  mCompositionActor.SetProperty( mPositionIndex, metaballCenter );
}

bool MetaballExplosionController::OnTouch( Actor actor, const TouchData& touch )
{
  float aspectR = mScreenSize.y / mScreenSize.x;

  switch( touch.GetState( 0 ) )
  {
    case PointState::DOWN:
    {
      ResetMetaballs(true);

      const Vector2 screen = touch.GetScreenPosition( 0 );
      Vector2 metaballCenter = Vector2( (screen.x / mScreenSize.x) - 0.5f, (aspectR * (mScreenSize.y - screen.y) / mScreenSize.y) - 0.5f ) * 2.0f;
      SetPositionToMetaballs(metaballCenter);

      break;
    }
    case PointState::MOTION:
    {
      const Vector2 screen = touch.GetScreenPosition( 0 );
      Vector2 metaballCenter = Vector2( (screen.x / mScreenSize.x) - 0.5f, (aspectR * (mScreenSize.y - screen.y) / mScreenSize.y) - 0.5f ) * 2.0f;
      SetPositionToMetaballs(metaballCenter);
      break;
    }
    case PointState::UP:
    case PointState::LEAVE:
    case PointState::INTERRUPTED:
    {
      mTimerDispersion.Start();
      break;
    }
    default:
      break;
    }
  return true;
}

void MetaballExplosionController::OnKeyEvent(const KeyEvent& event)
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
 * Main entry point
 */
int32_t DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  MetaballExplosionController test( application );

  application.MainLoop();

  return 0;
}
