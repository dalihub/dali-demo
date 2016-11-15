/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali/devel-api/images/texture-set-image.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali-toolkit/dali-toolkit.h>

#include "shared/view.h"
#include "shared/utility.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * const BACKGROUND_IMAGE( DEMO_IMAGE_DIR "background-2.jpg" );
const char * const TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );

const float GRAVITY_X(0);
const float GRAVITY_Y(-0.09);
}

#define METABALL_NUMBER 6


const char*const METABALL_VERTEX_SHADER = DALI_COMPOSE_SHADER (
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


const char*const METABALL_FRAG_SHADER = DALI_COMPOSE_SHADER (
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

const char*const REFRACTION_FRAG_SHADER = DALI_COMPOSE_SHADER (
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

const char*const FRAG_SHADER = DALI_COMPOSE_SHADER (
  precision mediump float;\n
  void main()\n
  {\n
    gl_FragColor = texture2D(sTexture, vTexCoord);\n
  }\n
);


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


/**************************************************************************/
/* Demo using Metaballs                                         ***********/
/* When the metaball is clicked it explodes in different balls  ***********/
/**************************************************************************/
class MetaballExplosionController : public ConnectionTracker
{
public:
  MetaballExplosionController( Application& application );
  ~MetaballExplosionController();

  /**
   * Main create function, it creates the metaballs and all the related data
   */
  void Create( Application& app );

  /**
   * Touch event function
   */
  bool OnTouch( Actor actor, const TouchData& touch );

  /**
   * Key event function
   */
  void OnKeyEvent(const KeyEvent& event);


private:
  Application&      mApplication;
  Vector2           mScreenSize;

  Layer             mContentLayer;

  Image             mBackImage;
  FrameBufferImage  mMetaballFBO;

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

  int               mDispersion;
  Animation         mDispersionAnimation[METABALL_NUMBER];

  Timer             mTimerDispersion;

  float             mTimeMult;

  //Private functions

  /**
   * Create a mesh data with the geometry for the metaball rendering
   */
  Geometry          CreateGeometry();

  /**
   * Create a mesh data with the geometry for the final composition
   */
  Geometry          CreateGeometryComposition();

  /**
   * Create a mesh actor for the metaballs
   */
  void              CreateMetaballActors();

  /**
   * Create the render task and FBO to render the metaballs into a texture
   */
  void              CreateMetaballImage();

  /**
   * Create a mesh image to render the final composition
   */
  void              AddRefractionImage();

  /**
   * Function to create animations for the small variations of position inside the metaball
   */
  void              CreateAnimations();

  /**
   * Function to reset metaball state
   */
  void              ResetMetaballs(bool resetAnims);

  /**
   * Function to create disperse each of the ball that compose the metaball when exploding
   */
  void              DisperseBallAnimation(int ball);

  /**
   * Function to make metaballs come back to reset position
   */
  void              LaunchResetMetaballPosition(Animation &source);

  /**
   * Function to set things at the end of the animation
   */
  void              EndDisperseAnimation(Animation &source);

  /**
   * Function to init dispersion of the metaballs one by one using a timer
   * (so not all the balls begin moving at the same time)
   */
  bool              OnTimerDispersionTick();

  /**
   * Function to set the actual position of the metaballs when the user clicks the screen
   */
  void              SetPositionToMetaballs(Vector2 & metaballCenter);
};


//-----------------------------------------------------------------------------------------------
//
//  IMPLEMENTATION
//
//----------------

MetaballExplosionController::MetaballExplosionController( Application& application )
  : mApplication( application )
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

  stage.KeyEventSignal().Connect(this, &MetaballExplosionController::OnKeyEvent);

  mScreenSize = stage.GetSize();

  mTimeMult = 1.0f;

  stage.SetBackgroundColor(Color::BLACK);

  //Set background image for the view
  mBackImage = DemoHelper::LoadImage( BACKGROUND_IMAGE );

  srand((unsigned)time(0));

  //Create internal data
  CreateMetaballActors();
  CreateMetaballImage();
  AddRefractionImage();

  CreateAnimations();

  mDispersion = 0;
  mTimerDispersion = Timer::New( 150 );
  mTimerDispersion.TickSignal().Connect(this, &MetaballExplosionController::OnTimerDispersionTick);

  // Connect the callback to the touch signal on the mesh actor
  stage.GetRootLayer().TouchSignal().Connect( this, &MetaballExplosionController::OnTouch );
}

Geometry MetaballExplosionController::CreateGeometry()
{
  float aspect = (float)mScreenSize.y / (float)mScreenSize.x;

  // Create vertices and specify their color
  float xsize = mScreenSize.x * 0.5;

  //We create the meshdata for the metaballs
  struct VertexPosition { Vector2 position; };
  struct VertexTexture { Vector2 texture; };
  struct VertexNormal { Vector3 normal; };

  VertexPosition vertices[] = {
    { Vector2( -xsize, -xsize * aspect) },
    { Vector2(  xsize, -xsize * aspect) },
    { Vector2( -xsize,  xsize * aspect) },
    { Vector2(  xsize,  xsize * aspect) }
  };

  VertexTexture textures[] = {
    { Vector2(0.0f, 0.0f) },
    { Vector2(1.0f, 0.0f) },
    { Vector2(0.0f, 1.0f * aspect) },
    { Vector2(1.0f, 1.0f * aspect) }
  };

  unsigned int numberOfVertices = sizeof(vertices)/sizeof(VertexPosition);

  //Vertices
  Property::Map positionVertexFormat;
  positionVertexFormat["aPosition"] = Property::VECTOR2;
  PropertyBuffer positionVertices = PropertyBuffer::New( positionVertexFormat );
  positionVertices.SetData( vertices, numberOfVertices );

  //Textures
  Property::Map textureVertexFormat;
  textureVertexFormat["aTexture"] = Property::VECTOR2;
  PropertyBuffer textureVertices = PropertyBuffer::New( textureVertexFormat );
  textureVertices.SetData( textures, numberOfVertices );

  //Indices
  unsigned short indices[] = { 0, 3, 1, 0, 2, 3 };

  // Create the geometry object
  Geometry texturedQuadGeometry = Geometry::New();
  texturedQuadGeometry.AddVertexBuffer( positionVertices );
  texturedQuadGeometry.AddVertexBuffer( textureVertices );

  texturedQuadGeometry.SetIndexBuffer ( &indices[0], sizeof( indices )/ sizeof( indices[0] ) );

  return texturedQuadGeometry;
}

Geometry MetaballExplosionController::CreateGeometryComposition()
{
  float aspect = (float)mScreenSize.y / (float)mScreenSize.x;

  // Create vertices and specify their color
  float xsize = mScreenSize.x * 0.5;

  //We create the meshdata for the metaballs
  struct VertexPosition { Vector2 position; };
  struct VertexTexture { Vector2 texture; };
  struct VertexNormal { Vector3 normal; };

  VertexPosition vertices[] = {
    { Vector2( -xsize, -xsize * aspect) },
    { Vector2(  xsize, -xsize * aspect) },
    { Vector2( -xsize,  xsize * aspect) },
    { Vector2(  xsize,  xsize * aspect) }
  };

  VertexTexture textures[] = {
    { Vector2(0.0f, 0.0f) },
    { Vector2(1.0f, 0.0f) },
    { Vector2(0.0f, 1.0f) },
    { Vector2(1.0f, 1.0f) }
  };

  unsigned int numberOfVertices = sizeof(vertices)/sizeof(VertexPosition);

  //Vertices
  Property::Map positionVertexFormat;
  positionVertexFormat["aPosition"] = Property::VECTOR2;
  PropertyBuffer positionVertices = PropertyBuffer::New( positionVertexFormat );
  positionVertices.SetData( vertices, numberOfVertices );

  //Textures
  Property::Map textureVertexFormat;
  textureVertexFormat["aTexture"] = Property::VECTOR2;
  PropertyBuffer textureVertices = PropertyBuffer::New( textureVertexFormat );
  textureVertices.SetData( textures, numberOfVertices );

  //Indices
  unsigned short indices[] = { 0, 3, 1, 0, 2, 3 };

  // Create the geometry object
  Geometry texturedQuadGeometry = Geometry::New();
  texturedQuadGeometry.AddVertexBuffer( positionVertices );
  texturedQuadGeometry.AddVertexBuffer( textureVertices );

  texturedQuadGeometry.SetIndexBuffer ( &indices[0], sizeof( indices )/ sizeof( indices[0] ) );

  return texturedQuadGeometry;
}

float randomNumber(float lowest, float highest)
{
  float range=(highest-lowest);
  return lowest+range*rand()/RAND_MAX;
}

void MetaballExplosionController::CreateMetaballActors()
{
  //Create the shader for the metaballs
  Shader shader = Shader::New( METABALL_VERTEX_SHADER, METABALL_FRAG_SHADER );

  Geometry metaballGeom = CreateGeometry();
  Renderer renderer = Renderer::New( metaballGeom, shader );
  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_SRC_RGB,    BlendFactor::ONE );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_RGB,   BlendFactor::ONE );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_SRC_ALPHA,  BlendFactor::ONE );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_ALPHA, BlendFactor::ONE  );

  //Initialization of each of the metaballs
  for( int i = 0; i < METABALL_NUMBER; i++ )
  {
    mMetaballs[i].position = Vector2(0.0f, 0.0f);
    mMetaballs[i].radius = mMetaballs[i].initRadius = randomNumber(0.05f,0.07f);

    mMetaballs[i].actor = Actor::New( );
    mMetaballs[i].actor.SetName("Metaball");
    mMetaballs[i].actor.SetScale( 1.0f );
    mMetaballs[i].actor.SetParentOrigin( ParentOrigin::CENTER );
    mMetaballs[i].actor.AddRenderer( renderer );

    mMetaballs[i].positionIndex = mMetaballs[i].actor.RegisterProperty( "uPositionMetaball", mMetaballs[i].position );

    mMetaballs[i].positionVarIndex = mMetaballs[i].actor.RegisterProperty( "uPositionVar", Vector2(0.f,0.f) );

    mMetaballs[i].actor.RegisterProperty( "uGravityVector", Vector2(randomNumber(-0.2,0.2),randomNumber(-0.2,0.2)) );

    mMetaballs[i].actor.RegisterProperty( "uRadius", mMetaballs[i].radius );

    mMetaballs[i].actor.RegisterProperty( "uRadiusVar", 0.f );

    mMetaballs[i].actor.SetSize(400, 400);
  }

  //Root creation
  mMetaballRoot = Actor::New();
  mMetaballRoot.SetParentOrigin( ParentOrigin::CENTER );
  for( int i = 0; i < METABALL_NUMBER; i++ )
  {
    mMetaballRoot.Add( mMetaballs[i].actor );
  }

  //Initialization of variables related to metaballs
  mMetaballPosVariation = Vector2(0,0);
  mMetaballPosVariationFrom = Vector2(0,0);
  mMetaballPosVariationTo = Vector2(0,0);
  mCurrentTouchPosition = Vector2(0,0);
}

void MetaballExplosionController::CreateMetaballImage()
{
  //We create an FBO and a render task to create to render the metaballs with a fragment shader
  Stage stage = Stage::GetCurrent();
  mMetaballFBO = FrameBufferImage::New(mScreenSize.x, mScreenSize.y, Pixel::RGBA8888, RenderBuffer::COLOR_DEPTH);


  stage.Add(mMetaballRoot);

  //Creation of the render task used to render the metaballs
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.CreateTask();
  task.SetRefreshRate( RenderTask::REFRESH_ALWAYS );
  task.SetSourceActor( mMetaballRoot );
  task.SetExclusive(true);
  task.SetClearColor( Color::BLACK );
  task.SetClearEnabled( true );
  task.SetTargetFrameBuffer( mMetaballFBO );
}

void MetaballExplosionController::AddRefractionImage()
{
  //Create Gaussian blur for the rendered image
  FrameBufferImage fbo;
  fbo = FrameBufferImage::New( mScreenSize.x, mScreenSize.y, Pixel::RGBA8888, RenderBuffer::COLOR_DEPTH);

  GaussianBlurView gbv = GaussianBlurView::New(5, 2.0f, Pixel::RGBA8888, 0.5f, 0.5f, true);
  gbv.SetBackgroundColor(Color::TRANSPARENT);
  gbv.SetUserImageAndOutputRenderTarget( mMetaballFBO, fbo );
  gbv.SetSize(mScreenSize.x, mScreenSize.y);
  Stage::GetCurrent().Add(gbv);
  gbv.Activate();

  //Create new shader
  Shader shader = Shader::New( METABALL_VERTEX_SHADER, REFRACTION_FRAG_SHADER );

  //Create new texture set
  TextureSet textureSet = TextureSet::New();
  TextureSetImage( textureSet, 0u, mBackImage );
  TextureSetImage( textureSet, 1u, fbo );

  //Create geometry
  Geometry metaballGeom = CreateGeometryComposition();

  Renderer mRenderer = Renderer::New( metaballGeom, shader );
  mRenderer.SetTextures( textureSet );

  mCompositionActor = Actor::New( );
  mCompositionActor.SetParentOrigin(ParentOrigin::CENTER);
  mCompositionActor.SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  mCompositionActor.SetSize(mScreenSize.x, mScreenSize.y);

  mCompositionActor.AddRenderer( mRenderer );

  Vector2 metaballCenter(0.0,0);
  metaballCenter.x = metaballCenter.x * 0.5;
  metaballCenter.y = metaballCenter.y * 0.5;

  mPositionIndex = mCompositionActor.RegisterProperty( "uPositionMetaball", metaballCenter );

  SetPositionToMetaballs(metaballCenter);

  mCompositionActor.SetSize(mScreenSize.x, mScreenSize.y);

  Stage stage = Stage::GetCurrent();
  stage.Add( mCompositionActor );
}

void MetaballExplosionController::CreateAnimations()
{
  Vector2 direction;

  for( int i = 0; i < METABALL_NUMBER; i++ )
  {
    float key;
    KeyFrames keySinCosVariation = KeyFrames::New();
    Vector2 sinCosVariation(0,0);

    direction.x = randomNumber(-100.f,100.f);
    direction.y = randomNumber(-100.f,100.f);

    direction.Normalize();
    direction *= 0.1f;

    for( int j = 0; j < 360; j++ )
    {
      sinCosVariation.x = sin(j * Math::PI/180.f) * direction.x;
      sinCosVariation.y = cos(j * Math::PI/180.f) * direction.y;
      key = j/360.f;
      keySinCosVariation.Add(key, sinCosVariation);
    }

    mPositionVarAnimation[i] = Animation::New(3.f);
    mPositionVarAnimation[i].AnimateBetween(Property( mMetaballs[i].actor, mMetaballs[i].positionVarIndex ), keySinCosVariation);
    mPositionVarAnimation[i].SetLooping( true );
    mPositionVarAnimation[i].Play();
  }
}

void MetaballExplosionController::ResetMetaballs(bool resetAnims)
{
  for( int i = 0; i < METABALL_NUMBER; i++ )
  {
    if (mDispersionAnimation[i])
      mDispersionAnimation[i].Clear();

    mMetaballs[i].position = Vector2(0.0f, 0.0f);
    mMetaballs[i].actor.SetProperty(mMetaballs[i].positionIndex, mMetaballs[i].position);
  }
  mTimerDispersion.Stop();
  mDispersion = 0;

  mCompositionActor.SetProperty( mPositionIndex, Vector2(0,0) );
}

void MetaballExplosionController::DisperseBallAnimation(int ball)
{
  Vector2 position;
  position.x = randomNumber(-1.5f,1.5f);
  position.y = randomNumber(-1.5f,1.5f);

  mDispersionAnimation[ball] = Animation::New(2.0f * mTimeMult);
  mDispersionAnimation[ball].AnimateTo( Property(mMetaballs[ball].actor, mMetaballs[ball].positionIndex), position);
  mDispersionAnimation[ball].Play();

  if( ball == METABALL_NUMBER - 1 )
    mDispersionAnimation[ball].FinishedSignal().Connect( this, &MetaballExplosionController::LaunchResetMetaballPosition );
}

void MetaballExplosionController::LaunchResetMetaballPosition(Animation &source)
{
  for( int i = 0; i < METABALL_NUMBER; i++ )
  {
    mDispersionAnimation[i] = Animation::New(1.5f + i*0.25f*mTimeMult);
    mDispersionAnimation[i].AnimateTo(Property(mMetaballs[i].actor, mMetaballs[i].positionIndex), Vector2(0,0));
    mDispersionAnimation[i].Play();

    if( i == METABALL_NUMBER - 1 )
      mDispersionAnimation[i].FinishedSignal().Connect( this, &MetaballExplosionController::EndDisperseAnimation );
  }
}

void MetaballExplosionController::EndDisperseAnimation(Animation &source)
{
  mCompositionActor.SetProperty( mPositionIndex, Vector2(0,0) );
}

bool MetaballExplosionController::OnTimerDispersionTick()
{
  if( mDispersion < METABALL_NUMBER )
  {
    DisperseBallAnimation(mDispersion);
    mDispersion++;
  }
  return true;
}

void MetaballExplosionController::SetPositionToMetaballs(Vector2 & metaballCenter)
{
  //We set the position for the metaballs based on click position
  for( int i = 0; i < METABALL_NUMBER; i++ )
  {
    mMetaballs[i].position = metaballCenter;
    mMetaballs[i].actor.SetProperty(mMetaballs[i].positionIndex, mMetaballs[i].position);
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
      Vector2 metaballCenter = Vector2((screen.x / mScreenSize.x) - 0.5, (aspectR * (mScreenSize.y - screen.y) / mScreenSize.y) - 0.5) * 2.0;
      SetPositionToMetaballs(metaballCenter);

      break;
    }
    case PointState::MOTION:
    {
      const Vector2 screen = touch.GetScreenPosition( 0 );
      Vector2 metaballCenter = Vector2((screen.x / mScreenSize.x) - 0.5, (aspectR * (mScreenSize.y - screen.y) / mScreenSize.y) - 0.5) * 2.0;
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


//-----------------------------------------------------------------------------------------------
//
//  Main functions
//
//-----------------------------------------------------------------------------------------------

void RunTest( Application& application )
{
  MetaballExplosionController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
