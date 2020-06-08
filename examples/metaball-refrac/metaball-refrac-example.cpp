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

// INTERNAL INCLUDES
#include "shared/utility.h" // DemoHelper::LoadTexture

using namespace Dali;

namespace // unnamed namespace for constants
{
const char * const BACKGROUND_IMAGE( DEMO_IMAGE_DIR "background-2.jpg" );
const float GRAVITY_X(0);
const float GRAVITY_Y(-0.09);

// number of metaballs
constexpr uint32_t METABALL_NUMBER = 6;

/**
 * Vertex shader for metaballs
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
      mediump vec4 vertexPosition = vec4(aPosition.x, aPosition.y, 0.0, 1.0);\n
      vertexPosition = uMvpMatrix * vertexPosition;\n
      gl_Position = vertexPosition;\n
      vTexCoord = aTexture;\n
    }\n
);

/**
 * Fragment shader for metaballs
 */
const char* const METABALL_FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision mediump float;\n
  varying vec2 vTexCoord;\n
  uniform vec2 uPositionMetaball;\n
  uniform vec2 uPositionVar;\n
  uniform vec2 uGravityVector;\n
  uniform float uRadius;\n
  uniform float uRadiusVar;\n
  uniform float uAspect;\n
  void main()\n
  {\n
    vec2 adjustedCoords = vTexCoord * 2.0 - 1.0;\n
    vec2 finalMetaballPosition = uPositionMetaball + uGravityVector + uPositionVar;\n

    float distance = (adjustedCoords.x - finalMetaballPosition.x) * (adjustedCoords.x - finalMetaballPosition.x) +
                     (adjustedCoords.y - finalMetaballPosition.y) * (adjustedCoords.y - finalMetaballPosition.y);\n
    float finalRadius = uRadius + uRadiusVar;\n
    float color = finalRadius / sqrt( distance );\n
    vec2 bordercolor = vec2(0.0,0.0);\n
    if (vTexCoord.x < 0.1)\n
    {\n
      bordercolor.x = (0.1 - vTexCoord.x) * 0.8;\n
    }\n
    if (vTexCoord.x > 0.9)\n
    {\n
      bordercolor.x = (vTexCoord.x - 0.9) * 0.8;\n
    }\n
    if (vTexCoord.y < 0.1)\n
    {\n
      bordercolor.y = (0.1 - vTexCoord.y) * 0.8;\n
    }\n
    if (vTexCoord.y > (0.9 * uAspect))\n
    {\n
      bordercolor.y = (vTexCoord.y - (0.9 * uAspect)) * 0.8;\n
    }\n
    float border = (bordercolor.x + bordercolor.y) * 0.5;\n
    gl_FragColor = vec4(color + border,color + border,color + border,1.0);\n
  }\n
);

/**
 * Fragment shader code for metaball and background composition with refraction effect
 */
const char* const REFRACTION_FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision mediump float;\n
  varying vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform sampler2D sEffect;\n
  void main()\n
  {\n
    vec4 metaColor = texture2D(sEffect, vTexCoord);\n
    vec2 zoomCoords;\n
    float bright = 1.0;\n
    if (metaColor.r > 0.85)\n
    {\n
      zoomCoords = ((vTexCoord - 0.5) * 0.95) + 0.5;\n
    }\n
    else if (metaColor.r > 0.78)\n
    {\n
      float interpolation = mix(0.95, 1.05, (0.85 - metaColor.r) * 50.0);\n
      zoomCoords = ((vTexCoord - 0.5) * interpolation) + 0.5;\n
      bright = 1.2;\n
    }\n
    else\n
    {\n
      zoomCoords = vTexCoord;\n
    }\n

    gl_FragColor = texture2D(sTexture, zoomCoords) * bright;\n
  }\n
 );

/**
 * Fragment shader code when there's no effect
 */
const char* const FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision mediump float;\n
  varying vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  void main()\n
  {\n
    gl_FragColor = texture2D(sTexture, vTexCoord);\n
  }\n
);

/**
 * Metadata for each ball
 */
struct MetaballInfo
{
  Actor   actor;
  Vector2 position;
  float   radius;
  float   initRadius;

  //Properties needed for animations
  Property::Index positionIndex;
  Property::Index positionVarIndex;
  Property::Index gravityIndex;
  Property::Index radiusIndex;
  Property::Index radiusVarIndex;
  Property::Index aspectIndex;
};

} // unnamed namespace

/**
 * Demo using Metaballs
 *
 * When the metaball is clicked it starts to grow and fuses into the closest edge of screen
 */
class MetaballRefracController : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application
   */
  MetaballRefracController( Application& application );

  /**
   * Destructor
   */
  virtual ~MetaballRefracController();

  /**
   * Creates the metaballs and initializes the scene
   */
  void Create( Application& app );

  /**
   * Touch handler, start the grow animation and creates additional metaballs
   */
  bool OnTouch( Actor actor, const TouchData& touch );

  /**
   * Key event callback to quit the application on escape or back key
   */
  void OnKeyEvent( const KeyEvent& event );

private: // Data

  Application&      mApplication;
  Vector2           mScreenSize;

  Texture           mBackgroundTexture;
  FrameBuffer       mMetaballFBO;

  Actor             mMetaballRoot;
  MetaballInfo      mMetaballs[METABALL_NUMBER];

  Actor             mCompositionActor;

  //Motion
  Vector2           mCurrentTouchPosition;
  Vector2           mMetaballPosVariation;
  Vector2           mMetaballPosVariationFrom;
  Vector2           mMetaballPosVariationTo;
  Vector2           mMetaballCenter;

  Vector2           mGravity;
  Vector2           mGravityVar;

  Renderer          mRendererRefraction;
  TextureSet        mTextureSetRefraction;
  Shader            mShaderRefraction;
  TextureSet        mTextureSetNormal;
  Shader            mShaderNormal;

  // Animations
  Animation         mGravityAnimation[METABALL_NUMBER];
  Animation         mRadiusDecAnimation[METABALL_NUMBER];
  Animation         mRadiusIncFastAnimation[METABALL_NUMBER];
  Animation         mRadiusIncSlowAnimation[METABALL_NUMBER];
  Animation         mRadiusVarAnimation[METABALL_NUMBER];
  Animation         mPositionVarAnimation[METABALL_NUMBER];

  // Private Helper functions

  /**
   * Create a mesh data with the geometry for the metaball rendering
   * @param aspectMappedTexture whether texture coords should be mapped based on aspect ratio
   */
  Geometry CreateGeometry( bool aspectMappedTexture = true );

  /**
   * Create a actor for the metaballs
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
   * Create all the metaballs animations (gravity, movement, size, etc.)
   */
  void CreateAnimations();

  /**
   * Function to launch the grow slow radius for the metaballs, and also the small variations for metaball[2] and [3]
   */
  void LaunchRadiusIncSlowAnimations( Animation& source );

  /**
   * Function to launch the animation to get the metaball[1] back to the center
   */
  void LaunchGetBackToPositionAnimation( Animation& source );

  /**
   * Function to stop all animations related to the click of the user in the screen
   */
  void StopClickAnimations();

  /**
   * Function to stop all animations related to the after click of the user in the screen
   */
  void StopAfterClickAnimations();

  /**
   * Function that resets the sate of the different Metaballs
   */
  void ResetMetaballsState();

  /**
   * Function to set the actual position of the metaballs when the user clicks the screen
   */
  void SetPositionToMetaballs( const Vector2& metaballCenter );

};

/**
 * Implementation
 */

MetaballRefracController::MetaballRefracController( Application& application )
  : mApplication( application )
{
  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &MetaballRefracController::Create );
}

MetaballRefracController::~MetaballRefracController()
{
  // Nothing to do here;
}

void MetaballRefracController::Create( Application& app )
{
  Stage stage = Stage::GetCurrent();

  stage.KeyEventSignal().Connect( this, &MetaballRefracController::OnKeyEvent );

  mScreenSize = stage.GetSize();

  stage.SetBackgroundColor(Color::BLACK);

  // Load background texture
  mBackgroundTexture = DemoHelper::LoadTexture( BACKGROUND_IMAGE );

  mGravity = Vector2(GRAVITY_X,GRAVITY_Y);
  mGravityVar = Vector2(0,0);

  CreateMetaballActors();
  CreateMetaballImage();
  CreateComposition();
  CreateAnimations();

  // Connect the callback to the touch signal on the mesh actor
  stage.GetRootLayer().TouchSignal().Connect( this, &MetaballRefracController::OnTouch );
}

Geometry MetaballRefracController::CreateGeometry( bool aspectMappedTexture )
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

void MetaballRefracController::CreateMetaballActors()
{
  const float aspect = mScreenSize.y / mScreenSize.x;

  // Create the renderer for the metaballs
  Shader shader = Shader::New( METABALL_VERTEX_SHADER, METABALL_FRAG_SHADER, Shader::Hint::MODIFIES_GEOMETRY );
  Geometry metaballGeometry = CreateGeometry();
  Renderer renderer = Renderer::New( metaballGeometry, shader );
  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_SRC_RGB,    BlendFactor::ONE );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_RGB,   BlendFactor::ONE );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_SRC_ALPHA,  BlendFactor::ONE );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_ALPHA, BlendFactor::ONE );

  // Each metaball has a different radius
  mMetaballs[0].radius = mMetaballs[0].initRadius = 0.0145f;
  mMetaballs[1].radius = mMetaballs[1].initRadius = 0.012f;
  mMetaballs[2].radius = mMetaballs[2].initRadius = 0.0135f;
  mMetaballs[3].radius = mMetaballs[3].initRadius = 0.0135f;

  // Initialization of each of the metaballs
  for( uint32_t i = 0 ; i < METABALL_NUMBER ; i++ )
  {
    mMetaballs[i].position = Vector2(0.0f, 0.0f);

    mMetaballs[i].actor = Actor::New();
    mMetaballs[i].actor.SetProperty( Dali::Actor::Property::NAME, "Metaball" );
    mMetaballs[i].actor.SetProperty( Actor::Property::SCALE, 1.0f );
    mMetaballs[i].actor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );


    mMetaballs[i].actor.AddRenderer( renderer );

    mMetaballs[i].positionIndex = mMetaballs[i].actor.RegisterProperty( "uPositionMetaball", mMetaballs[i].position );
    mMetaballs[i].positionVarIndex = mMetaballs[i].actor.RegisterProperty( "uPositionVar", Vector2(0.f,0.f) );
    mMetaballs[i].gravityIndex = mMetaballs[i].actor.RegisterProperty( "uGravityVector", Vector2(0.f,0.f) );
    mMetaballs[i].radiusIndex = mMetaballs[i].actor.RegisterProperty( "uRadius", mMetaballs[i].radius );
    mMetaballs[i].radiusVarIndex = mMetaballs[i].actor.RegisterProperty( "uRadiusVar", 0.f );
    mMetaballs[i].aspectIndex = mMetaballs[i].actor.RegisterProperty( "uAspect", aspect );
  }

  //Root creation
  mMetaballRoot = Actor::New();
  mMetaballRoot.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  for( uint32_t i = 0 ; i < METABALL_NUMBER ; i++ )
  {
    mMetaballRoot.Add( mMetaballs[i].actor );
  }
}

void MetaballRefracController::CreateMetaballImage()
{
  // Create an FBO and a render task to create to render the metaballs with a fragment shader
  Stage stage = Stage::GetCurrent();
  mMetaballFBO = FrameBuffer::New( mScreenSize.x, mScreenSize.y );

  stage.Add(mMetaballRoot);

  //Creation of the render task used to render the metaballs
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.CreateTask();
  task.SetRefreshRate( RenderTask::REFRESH_ALWAYS );
  task.SetSourceActor( mMetaballRoot );
  task.SetExclusive( true );
  task.SetClearColor( Color::BLACK );
  task.SetClearEnabled( true );
  task.SetFrameBuffer( mMetaballFBO );
}

void MetaballRefracController::CreateComposition()
{
  // Create Refraction shader and renderer
  mShaderRefraction = Shader::New( METABALL_VERTEX_SHADER, REFRACTION_FRAG_SHADER );

  // Create new texture set
  mTextureSetRefraction = TextureSet::New();
  mTextureSetRefraction.SetTexture( 0u, mBackgroundTexture  );
  mTextureSetRefraction.SetTexture( 1u, mMetaballFBO.GetColorTexture() );

  // Create normal shader
  mShaderNormal = Shader::New( METABALL_VERTEX_SHADER, FRAG_SHADER );

  // Create new texture set
  mTextureSetNormal = TextureSet::New();
  mTextureSetNormal.SetTexture( 0u, mBackgroundTexture );

  // Create actor
  mCompositionActor = Actor::New( );
  mCompositionActor.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER);
  mCompositionActor.SetProperty( Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  mCompositionActor.SetProperty( Actor::Property::SIZE, Vector2(mScreenSize.x, mScreenSize.y) );

  // Create geometry
  Geometry metaballGeometry = CreateGeometry( false );
  mRendererRefraction = Renderer::New( metaballGeometry, mShaderNormal );
  mRendererRefraction.SetTextures( mTextureSetNormal );
  mCompositionActor.AddRenderer( mRendererRefraction );

  Stage stage = Stage::GetCurrent();
  stage.Add( mCompositionActor );
}

void MetaballRefracController::CreateAnimations()
{
  uint32_t i = 0;
  float key;

  mPositionVarAnimation[1] = Animation::New( 2.f );
  mPositionVarAnimation[1].SetLooping( false );
  mPositionVarAnimation[1].Pause();
  mPositionVarAnimation[1].FinishedSignal().Connect( this, &MetaballRefracController::LaunchGetBackToPositionAnimation );

  KeyFrames keySinCosVariation = KeyFrames::New();
  Vector2 sinCosVariation(0,0);
  for( i = 0 ; i < 360; i++ )
  {
    sinCosVariation.x = 0.05f * ( -sinf(i * Math::PI_OVER_180) + cosf(i * Math::PI_OVER_180) );
    sinCosVariation.y = 0.05f * ( sinf(i * Math::PI_OVER_180) - cosf(i * Math::PI_OVER_180) );
    key = i/360.f;
    keySinCosVariation.Add(key, sinCosVariation);
  }

  mPositionVarAnimation[2] = Animation::New(6.f);
  mPositionVarAnimation[2].AnimateBetween(Property( mMetaballs[2].actor, mMetaballs[2].positionVarIndex ), keySinCosVariation);
  mPositionVarAnimation[2].SetLooping( true );
  mPositionVarAnimation[2].Pause();

  KeyFrames keyCosSinVariation = KeyFrames::New();
  Vector2 cosSinVariation(0,0);
  for( i = 0 ; i < 360; i++ )
  {
    cosSinVariation.x = 0.05f * ( -sinf(i * Math::PI_OVER_180) - cosf(i * Math::PI_OVER_180) );
    cosSinVariation.y = 0.05f * ( sinf(i * Math::PI_OVER_180) + cosf(i * Math::PI_OVER_180) );
    key = i/360.f;
    keyCosSinVariation.Add(key, cosSinVariation);
  }

  mPositionVarAnimation[3] = Animation::New(6.f);
  mPositionVarAnimation[3].AnimateBetween(Property( mMetaballs[3].actor, mMetaballs[3].positionVarIndex ), keyCosSinVariation);
  mPositionVarAnimation[3].SetLooping( true );
  mPositionVarAnimation[3].Pause();

  //Animations for gravity
  for( i = 0 ; i < METABALL_NUMBER; i++ )
  {
    mGravityAnimation[i] = Animation::New( 25.f );
    mGravityAnimation[i].AnimateBy( Property( mMetaballs[i].actor, mMetaballs[i].gravityIndex ), mGravity * 25.f * 3.f );
    mGravityAnimation[i].SetLooping( false );
    mGravityAnimation[i].Pause();
  }

  //Animation to decrease size of metaballs when there is no click
  for( i = 0 ; i < METABALL_NUMBER; i++ )
  {
    mRadiusDecAnimation[i] = Animation::New( 25.f );
    mRadiusDecAnimation[i].AnimateBy( Property( mMetaballs[i].actor, mMetaballs[i].radiusIndex ), -0.004f * 25.f * 3.f );
    mRadiusDecAnimation[i].SetLooping( false );
    mRadiusDecAnimation[i].Pause();
  }

  // Animation to grow the size of the metaballs the first second of the click
  for( i = 0 ; i < METABALL_NUMBER; i++ )
  {
    mRadiusIncFastAnimation[i] = Animation::New( 0.3f );
    mRadiusIncFastAnimation[i].AnimateBy( Property( mMetaballs[i].actor, mMetaballs[i].radiusIndex ), 0.06f );
    mRadiusIncFastAnimation[i].SetLooping( false );
    mRadiusIncFastAnimation[i].Pause();
  }
  mRadiusIncFastAnimation[0].FinishedSignal().Connect( this, &MetaballRefracController::LaunchRadiusIncSlowAnimations );

  // Animation to grow the size of the metaballs afterwards
  for( i = 0 ; i < METABALL_NUMBER; i++ )
  {
    mRadiusIncSlowAnimation[i] = Animation::New( 20.f );
    mRadiusIncSlowAnimation[i].AnimateBy( Property( mMetaballs[i].actor, mMetaballs[i].radiusIndex ), 0.04f );
    mRadiusIncSlowAnimation[i].SetLooping( false );
    mRadiusIncSlowAnimation[i].Pause();
  }

  // Keyframes of a sin function
  KeyFrames keySin = KeyFrames::New();
  float val;
  for( i = 0 ; i < 360; i++ )
  {
    val = 0.01f * sin(i * Math::PI/180.f);
    key = i/360.f;
    keySin.Add(key, val);
  }

  //Animation to change the size of the metaball
  mRadiusVarAnimation[2] = Animation::New( 8.f );
  mRadiusVarAnimation[2].AnimateBetween( Property( mMetaballs[2].actor, mMetaballs[2].radiusVarIndex ), keySin );
  mRadiusVarAnimation[2].SetLooping( true );

  // Keyframes of a cos function
  KeyFrames keyCos = KeyFrames::New();
  for( i = 0 ; i < 360; i++ )
  {
    val = 0.01f * cos(i * Math::PI/180.f);
    key = i/360.f;
    keyCos.Add(key, val);
  }

  //Animation to change the size of the metaball
  mRadiusVarAnimation[3] = Animation::New( 8.f );
  mRadiusVarAnimation[3].AnimateBetween( Property( mMetaballs[3].actor, mMetaballs[3].radiusVarIndex ), keyCos );
  mRadiusVarAnimation[3].SetLooping( true );
}

void MetaballRefracController::LaunchGetBackToPositionAnimation( Animation& source )
{
  mMetaballPosVariationTo = Vector2(0,0);

  mPositionVarAnimation[1] = Animation::New( 1.f );
  mPositionVarAnimation[1].SetLooping( false );
  mPositionVarAnimation[1].AnimateTo(Property( mMetaballs[1].actor, mMetaballs[1].positionVarIndex ), Vector2(0,0) );
  mPositionVarAnimation[1].Play();
}

void MetaballRefracController::LaunchRadiusIncSlowAnimations( Animation& source )
{
  for( uint32_t i = 0 ; i < METABALL_NUMBER; i++ )
  {
    mRadiusIncSlowAnimation[i].Play();
  }
  mPositionVarAnimation[2].Play();
  mPositionVarAnimation[3].Play();
}

void MetaballRefracController::StopClickAnimations()
{
  for( uint32_t i = 0 ; i < METABALL_NUMBER; i++ )
  {
    mRadiusIncSlowAnimation[i].Stop();
    mRadiusIncFastAnimation[i].Stop();
  }
  mPositionVarAnimation[1].Stop();
  mPositionVarAnimation[2].Stop();
  mPositionVarAnimation[3].Stop();
}

void MetaballRefracController::StopAfterClickAnimations()
{
  for( uint32_t i = 0 ; i < METABALL_NUMBER; i++ )
  {
    mGravityAnimation[i].Stop();
    mRadiusDecAnimation[i].Stop();

    mMetaballs[i].radius = mMetaballs[i].initRadius;

    mMetaballs[i].actor.SetProperty( mMetaballs[i].gravityIndex, Vector2(0,0) );
    mMetaballs[i].actor.SetProperty( mMetaballs[i].radiusIndex, mMetaballs[i].radius );
    mMetaballs[i].actor.SetProperty( mMetaballs[i].radiusVarIndex, 0.f );
  }
  mRadiusVarAnimation[2].Stop();
  mRadiusVarAnimation[3].Stop();
}

void MetaballRefracController::ResetMetaballsState()
{
  mRendererRefraction.SetTextures( mTextureSetNormal );
  mRendererRefraction.SetShader( mShaderNormal );

  for( uint32_t i = 0 ; i < METABALL_NUMBER; i++ )
  {
    mMetaballs[i].radius = mMetaballs[i].initRadius;
  }

  mMetaballPosVariationTo = Vector2(0,0);
  mMetaballPosVariationFrom = Vector2(0,0);
  mMetaballPosVariation = Vector2(0,0);
  mGravityVar = Vector2(0,0);
}

void MetaballRefracController::SetPositionToMetaballs( const Vector2& metaballCenter )
{
  //We set the position for the metaballs based on click position
  for( uint32_t i = 0 ; i < METABALL_NUMBER; i++ )
  {
    mMetaballs[i].position = metaballCenter;
    mMetaballs[i].actor.SetProperty( mMetaballs[i].positionIndex, mMetaballs[i].position );
  }
}

bool MetaballRefracController::OnTouch( Actor actor, const TouchData& touch )
{
  const float aspect = mScreenSize.y / mScreenSize.x;
  switch( touch.GetState( 0 ) )
  {
    case PointState::DOWN:
    {
      StopAfterClickAnimations();
      for( uint32_t i = 0 ; i < METABALL_NUMBER; i++ )
      {
        mRadiusIncFastAnimation[i].Play();
      }
      mRadiusVarAnimation[2].Play();
      mRadiusVarAnimation[3].Play();

      //We draw with the refraction-composition shader
      mRendererRefraction.SetTextures( mTextureSetRefraction );
      mRendererRefraction.SetShader( mShaderRefraction );
      mCurrentTouchPosition = touch.GetScreenPosition( 0 );

      //we use the click position for the metaballs
      Vector2 metaballCenter = Vector2( (mCurrentTouchPosition.x / mScreenSize.x) - 0.5f,
                                        (aspect * (mScreenSize.y - mCurrentTouchPosition.y) / mScreenSize.y) - 0.5f ) * 2.0f;
      SetPositionToMetaballs(metaballCenter);
      break;
    }
    case PointState::MOTION:
    {
      Vector2 screen = touch.GetScreenPosition( 0 );
      Vector2 displacement = screen - mCurrentTouchPosition;
      mCurrentTouchPosition = screen;

      mMetaballPosVariationTo.x += ( displacement.x / mScreenSize.x ) * 2.2f;
      mMetaballPosVariationTo.y += (-displacement.y / mScreenSize.y ) * 2.2f;

      if (mPositionVarAnimation[1])
      {
        mPositionVarAnimation[1].FinishedSignal().Disconnect( this, &MetaballRefracController::LaunchGetBackToPositionAnimation );
        mPositionVarAnimation[1].Stop();
      }
      mPositionVarAnimation[1] = Animation::New( 1.f );
      mPositionVarAnimation[1].SetLooping( false );
      mPositionVarAnimation[1].AnimateTo(Property( mMetaballs[1].actor, mMetaballs[1].positionVarIndex ), mMetaballPosVariationTo );
      mPositionVarAnimation[1].FinishedSignal().Connect( this, &MetaballRefracController::LaunchGetBackToPositionAnimation );
      mPositionVarAnimation[1].Play();

      //we use the click position for the metaballs
      Vector2 metaballCenter = Vector2( (screen.x / mScreenSize.x) - 0.5f,
                                        (aspect * (mScreenSize.y - screen.y) / mScreenSize.y) - 0.5f) * 2.0f;
      SetPositionToMetaballs(metaballCenter);
      break;
    }
    case PointState::UP:
    case PointState::LEAVE:
    case PointState::INTERRUPTED:
    {
      //Stop click animations
      StopClickAnimations();

      //Launch out of screen animations
      for( uint32_t i = 0 ; i < METABALL_NUMBER; i++ )
      {
        mGravityAnimation[i].Play();
      }

      for( uint32_t i = 0 ; i < METABALL_NUMBER; i++ )
      {
        mRadiusDecAnimation[i].Play();
      }
      break;
    }
    default:
      break;
    }
  return true;
}

void MetaballRefracController::OnKeyEvent(const KeyEvent& event)
{
  if( event.state == KeyEvent::Down )
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

  MetaballRefracController test( application );
  application.MainLoop();

  return 0;
}
