/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/bubble-effect/bubble-emitter.h>

#include <cstdio>
#include <iostream>

// INTERNAL INCLUDES
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

class NewWindowController;

namespace
{
const char * const BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-2.jpg" );
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char * const LOSE_CONTEXT_IMAGE( DALI_IMAGE_DIR "icon-cluster-wobble.png" );
const char * const LOSE_CONTEXT_IMAGE_SELECTED( DALI_IMAGE_DIR "icon-cluster-wobble-selected.png" );
const char * const BASE_IMAGE( DALI_IMAGE_DIR "gallery-large-14.jpg" );
const char * const EFFECT_IMAGE( DALI_IMAGE_DIR "gallery-large-18.jpg" );
const char * const LOGO_IMAGE(DALI_IMAGE_DIR "dali-logo.png");

const float EXPLOSION_DURATION(1.2f);
const unsigned int EMIT_INTERVAL_IN_MS(40);
const float TRACK_DURATION_IN_MS(970);

Application gApplication;
NewWindowController* gNewWindowController(NULL);

#define MAKE_SHADER(A)#A

const char* VERTEX_COLOR_MESH = MAKE_SHADER(
attribute mediump vec3  aPosition;\n
attribute lowp    vec3  aColor;\n
uniform   mediump mat4  uMvpMatrix;\n
uniform   mediump vec3  uSize;\n
varying   lowp    vec3  vColor;\n
\n
void main()\n
{\n
  gl_Position = uMvpMatrix * vec4( aPosition*uSize, 1.0 );\n
  vColor = aColor;\n
}\n
);

const char* FRAGMENT_COLOR_MESH = MAKE_SHADER(
uniform lowp vec4  uColor;\n
varying lowp vec3  vColor;\n
\n
void main()\n
{\n
  gl_FragColor = vec4(vColor,1.0)*uColor;
}\n
);

const char* VERTEX_TEXTURE_MESH = MAKE_SHADER(
attribute mediump vec3  aPosition;\n
attribute highp   vec2  aTexCoord;\n
uniform   mediump mat4  uMvpMatrix;\n
uniform   mediump vec3  uSize;\n
varying   mediump vec2  vTexCoord;\n
\n
void main()\n
{\n
  gl_Position = uMvpMatrix * vec4( aPosition*uSize, 1.0 );\n
  vTexCoord = aTexCoord;\n
}\n
);

const char* FRAGMENT_TEXTURE_MESH = MAKE_SHADER(
varying mediump vec2  vTexCoord;\n
uniform lowp    vec4  uColor;\n
uniform sampler2D     sTexture;\n
\n
void main()\n
{\n
  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;
}\n
);

const char* FRAGMENT_BLEND_SHADER = MAKE_SHADER(
uniform mediump float alpha;\n
\n
void main()\n
{\n
  mediump vec4 fragColor = texture2D(sTexture, vTexCoord);\n
  mediump vec4 fxColor   = texture2D(sEffect, vTexCoord);\n
  gl_FragColor = mix(fragColor,fxColor, alpha);\n
}\n
);

}; // anonymous namespace


class NewWindowController : public ConnectionTracker
{
public:
  NewWindowController( Application& app );
  void Create( Application& app );
  void Destroy( Application& app );

  void AddBubbles( Actor& parentActor, const Vector2& stageSize);
  void AddMeshActor( Actor& parentActor );
  void AddBlendingImageActor( Actor& parentActor );
  void AddTextLabel( Actor& parentActor );

  ImageView CreateBlurredMirrorImage(const char* imageName);
  FrameBufferImage CreateFrameBufferForImage( const char* imageName, Property::Map& shaderEffect, const Vector3& rgbDelta );
  void SetUpBubbleEmission( const Vector2& emitPosition, const Vector2& direction );
  Geometry CreateMeshGeometry();
  Dali::Property::Map CreateColorModifierer();

  static void NewWindow(void);

  bool OnTrackTimerTick();
  void OnKeyEvent(const KeyEvent& event);
  bool OnLoseContextButtonClicked( Toolkit::Button button );
  void OnContextLost();
  void OnContextRegained();

private:
  Application                mApplication;
  TextLabel                  mTextActor;

  Toolkit::Control           mView;                              ///< The View instance.
  Toolkit::ToolBar           mToolBar;                           ///< The View's Toolbar.
  TextLabel                  mTitleActor;                        ///< The Toolbar's Title.
  Layer                      mContentLayer;                      ///< Content layer (scrolling cluster content)
  Toolkit::PushButton        mLoseContextButton;

  Toolkit::BubbleEmitter     mEmitter;
  Timer                      mEmitTrackTimer;
  bool                       mNeedNewAnimation;
  unsigned int               mAnimateComponentCount;
  Animation                  mEmitAnimation;
};


NewWindowController::NewWindowController( Application& application )
: mApplication(application),
  mNeedNewAnimation(true)
{
  mApplication.InitSignal().Connect(this, &NewWindowController::Create);
  mApplication.TerminateSignal().Connect(this, &NewWindowController::Destroy);
}

void NewWindowController::Create( Application& app )
{
  Stage stage = Stage::GetCurrent();
  stage.SetBackgroundColor(Color::YELLOW);

  stage.KeyEventSignal().Connect(this, &NewWindowController::OnKeyEvent);

  // The Init signal is received once (only) during the Application lifetime

  // Hide the indicator bar
  mApplication.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

  mContentLayer = DemoHelper::CreateView( app,
                                          mView,
                                          mToolBar,
                                          "",
                                          TOOLBAR_IMAGE,
                                          "Context recovery" );

  Size stageSize = stage.GetSize();
  Image backgroundImage = ResourceImage::New( BACKGROUND_IMAGE, Dali::ImageDimensions( stageSize.x, stageSize.y ), Dali::FittingMode::SCALE_TO_FILL, Dali::SamplingMode::BOX_THEN_LINEAR );
  ImageView backgroundActor = ImageView::New( backgroundImage );
  backgroundActor.SetParentOrigin( ParentOrigin::CENTER );
  mContentLayer.Add(backgroundActor);

  // Point the default render task at the view
  RenderTaskList taskList = stage.GetRenderTaskList();
  RenderTask defaultTask = taskList.GetTask( 0u );
  if ( defaultTask )
  {
    defaultTask.SetSourceActor( mView );
  }

  mLoseContextButton = Toolkit::PushButton::New();
  mLoseContextButton.SetUnselectedImage( LOSE_CONTEXT_IMAGE );
  mLoseContextButton.SetSelectedImage( LOSE_CONTEXT_IMAGE_SELECTED );
  mLoseContextButton.ClickedSignal().Connect( this, &NewWindowController::OnLoseContextButtonClicked );
  mToolBar.AddControl( mLoseContextButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

  Actor logoLayoutActor = Actor::New();
  logoLayoutActor.SetParentOrigin(ParentOrigin::CENTER);
  logoLayoutActor.SetPosition(0.0f, -200.0f, 0.0f);
  logoLayoutActor.SetScale(0.5f);
  backgroundActor.Add(logoLayoutActor);

  Image image = ResourceImage::New(LOGO_IMAGE);
  ImageView imageView = ImageView::New(image);
  imageView.SetName("dali-logo");
  imageView.SetParentOrigin(ParentOrigin::CENTER);
  imageView.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER);
  logoLayoutActor.Add(imageView);

  ImageView mirrorImageView = CreateBlurredMirrorImage(LOGO_IMAGE);
  mirrorImageView.SetParentOrigin(ParentOrigin::CENTER);
  mirrorImageView.SetAnchorPoint(AnchorPoint::TOP_CENTER);
  logoLayoutActor.Add(mirrorImageView);

  AddBubbles( backgroundActor, stage.GetSize());
  AddMeshActor( backgroundActor );
  AddBlendingImageActor( backgroundActor );
  AddTextLabel( backgroundActor );

  stage.ContextLostSignal().Connect(this, &NewWindowController::OnContextLost);
  stage.ContextRegainedSignal().Connect(this, &NewWindowController::OnContextRegained);
}

void NewWindowController::Destroy( Application& app )
{
  UnparentAndReset(mTextActor);
}

void NewWindowController::AddBubbles( Actor& parentActor, const Vector2& stageSize)
{
  mEmitter = Toolkit::BubbleEmitter::New( stageSize,
                                          ResourceImage::New( DALI_IMAGE_DIR "bubble-ball.png" ),
                                          200, Vector2( 5.0f, 5.0f ) );

  Image background = ResourceImage::New(BACKGROUND_IMAGE);
  mEmitter.SetBackground( background, Vector3(0.5f, 0.f,0.5f) );
  mEmitter.SetBubbleDensity( 9.f );
  Actor bubbleRoot = mEmitter.GetRootActor();
  parentActor.Add( bubbleRoot );
  bubbleRoot.SetParentOrigin(ParentOrigin::CENTER);
  bubbleRoot.SetZ(0.1f);

  mEmitTrackTimer = Timer::New( EMIT_INTERVAL_IN_MS );
  mEmitTrackTimer.TickSignal().Connect(this, &NewWindowController::OnTrackTimerTick);
  mEmitTrackTimer.Start();
}

void NewWindowController::AddMeshActor( Actor& parentActor )
{
  Geometry meshGeometry = CreateMeshGeometry();

  // Create a coloured mesh
  Shader shaderColorMesh = Shader::New( VERTEX_COLOR_MESH, FRAGMENT_COLOR_MESH );
  Material colorMeshmaterial = Material::New( shaderColorMesh );
  Renderer colorMeshRenderer = Renderer::New( meshGeometry, colorMeshmaterial );

  Actor colorMeshActor = Actor::New();
  colorMeshActor.AddRenderer( colorMeshRenderer );
  colorMeshActor.SetSize( 175.f,175.f );
  colorMeshActor.SetParentOrigin( ParentOrigin::CENTER );
  colorMeshActor.SetAnchorPoint(AnchorPoint::TOP_CENTER);
  colorMeshActor.SetPosition(Vector3(0.0f, 50.0f, 0.0f));
  colorMeshActor.SetOrientation( Degree(75.f), Vector3::XAXIS );
  colorMeshActor.SetName("ColorMeshActor");
  parentActor.Add( colorMeshActor );

 // Create a textured mesh
  Image effectImage = ResourceImage::New(EFFECT_IMAGE);
  Shader shaderTextureMesh = Shader::New( VERTEX_TEXTURE_MESH, FRAGMENT_TEXTURE_MESH );
  Material textureMeshMaterial = Material::New( shaderTextureMesh );
  textureMeshMaterial.AddTexture(effectImage, "sTexture");
  Renderer textureMeshRenderer = Renderer::New( meshGeometry, textureMeshMaterial );

  Actor textureMeshActor = Actor::New();
  textureMeshActor.AddRenderer( textureMeshRenderer );
  textureMeshActor.SetSize( 175.f,175.f );
  textureMeshActor.SetParentOrigin( ParentOrigin::CENTER );
  textureMeshActor.SetAnchorPoint(AnchorPoint::TOP_CENTER);
  textureMeshActor.SetPosition(Vector3(0.0f, 200.0f, 0.0f));
  textureMeshActor.SetOrientation( Degree(75.f), Vector3::XAXIS );
  textureMeshActor.SetName("TextureMeshActor");
  parentActor.Add( textureMeshActor );
}

void NewWindowController::AddBlendingImageActor( Actor& parentActor )
{
  Property::Map colorModifier = CreateColorModifierer();

  FrameBufferImage fb2 = CreateFrameBufferForImage( EFFECT_IMAGE, colorModifier, Vector3( 0.5f, 0.5f, 0.5f ) );

  ImageView tmpActor = ImageView::New(fb2);
  parentActor.Add(tmpActor);
  tmpActor.SetParentOrigin(ParentOrigin::CENTER_RIGHT);
  tmpActor.SetAnchorPoint(AnchorPoint::TOP_RIGHT);
  tmpActor.SetPosition(Vector3(0.0f, 150.0f, 0.0f));
  tmpActor.SetScale(0.25f);

  // create blending shader effect
  ShaderEffect blendShader = ShaderEffect::New( "", FRAGMENT_BLEND_SHADER );
  blendShader.SetEffectImage( fb2 );
  blendShader.SetUniform("alpha", 0.5f);

  Image baseImage = ResourceImage::New(BASE_IMAGE);
  ImageActor blendActor = ImageActor::New( baseImage );
  blendActor.SetParentOrigin(ParentOrigin::CENTER_RIGHT);
  blendActor.SetAnchorPoint(AnchorPoint::BOTTOM_RIGHT);
  blendActor.SetPosition(Vector3(0.0f, 100.0f, 0.0f));
  blendActor.SetSize(140, 140);
  blendActor.SetShaderEffect( blendShader );
  parentActor.Add(blendActor);
}

void NewWindowController::AddTextLabel( Actor& parentActor )
{
  mTextActor = TextLabel::New("Some text");
  mTextActor.SetParentOrigin(ParentOrigin::CENTER);
  mTextActor.SetColor(Color::RED);
  mTextActor.SetName("PushMe text");
  parentActor.Add( mTextActor );
}

ImageView NewWindowController::CreateBlurredMirrorImage(const char* imageName)
{
  Image image = ResourceImage::New(imageName);

  Uint16Pair intFboSize = ResourceImage::GetImageSize(imageName);
  Vector2 FBOSize = Vector2( intFboSize.GetWidth(), intFboSize.GetHeight() );
  FrameBufferImage fbo = FrameBufferImage::New( FBOSize.width, FBOSize.height, Pixel::RGBA8888);

  GaussianBlurView gbv = GaussianBlurView::New(5, 2.0f, Pixel::RGBA8888, 0.5f, 0.5f, true);
  gbv.SetBackgroundColor(Color::TRANSPARENT);
  gbv.SetUserImageAndOutputRenderTarget( image, fbo );
  gbv.SetSize(FBOSize);
  Stage::GetCurrent().Add(gbv);
  gbv.ActivateOnce();

  ImageView blurredActor = ImageView::New(fbo);
  blurredActor.SetSize(FBOSize);
  blurredActor.SetScale(1.0f, -1.0f, 1.0f);
  return blurredActor;
}

FrameBufferImage NewWindowController::CreateFrameBufferForImage(const char* imageName, Property::Map& shaderEffect, const Vector3& rgbDelta )
{
  Stage stage = Stage::GetCurrent();
  Uint16Pair intFboSize = ResourceImage::GetImageSize( imageName );
  Vector2 FBOSize = Vector2(intFboSize.GetWidth(), intFboSize.GetHeight());

  FrameBufferImage framebuffer = FrameBufferImage::New(FBOSize.x, FBOSize.y );

  RenderTask renderTask = stage.GetRenderTaskList().CreateTask();

  ImageView imageView = ImageView::New( imageName );
  imageView.SetName("Source image actor");
  imageView.SetProperty( ImageView::Property::IMAGE, shaderEffect );
  imageView.RegisterProperty( "uRGBDelta", rgbDelta );

  imageView.SetParentOrigin(ParentOrigin::CENTER);
  imageView.SetAnchorPoint(AnchorPoint::CENTER);
  imageView.SetScale(1.0f, -1.0f, 1.0f);
  stage.Add(imageView); // Not in default image view

  CameraActor cameraActor = CameraActor::New(FBOSize);
  cameraActor.SetParentOrigin(ParentOrigin::CENTER);
  cameraActor.SetFieldOfView(Math::PI*0.25f);
  cameraActor.SetNearClippingPlane(1.0f);
  cameraActor.SetAspectRatio(FBOSize.width / FBOSize.height);
  cameraActor.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor
  cameraActor.SetPosition(0.0f, 0.0f, ((FBOSize.height * 0.5f) / tanf(Math::PI * 0.125f)));
  stage.Add(cameraActor);

  renderTask.SetSourceActor(imageView);
  renderTask.SetInputEnabled(false);
  renderTask.SetTargetFrameBuffer(framebuffer);
  renderTask.SetCameraActor( cameraActor );
  renderTask.SetClearColor( Color::TRANSPARENT );
  renderTask.SetClearEnabled( true );
  renderTask.SetRefreshRate(RenderTask::REFRESH_ONCE);

  return framebuffer;
}

void NewWindowController::SetUpBubbleEmission( const Vector2& emitPosition, const Vector2& direction)
{
  if( mNeedNewAnimation )
  {
    float duration = Random::Range(1.f, 1.5f);
    mEmitAnimation = Animation::New( duration );
    mNeedNewAnimation = false;
    mAnimateComponentCount = 0;
  }

  mEmitter.EmitBubble( mEmitAnimation, emitPosition, direction, Vector2(10,10) );

  mAnimateComponentCount++;

  if( mAnimateComponentCount % 6 ==0 )
  {
    mEmitAnimation.Play();
    mNeedNewAnimation = true;
  }
}

Geometry NewWindowController::CreateMeshGeometry()
{
  // Create vertices and specify their color
  struct Vertex
  {
    Vector3 position;
    Vector2 textureCoordinates;
    Vector3 color;
  };

  Vertex vertexData[5] = {
    { Vector3(  0.0f,  0.0f, 0.5f ), Vector2(0.5f, 0.5f), Vector3(1.0f, 1.0f, 1.0f) },
    { Vector3( -0.5f, -0.5f, 0.0f ), Vector2(0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f) },
    { Vector3(  0.5f, -0.5f, 0.0f ), Vector2(1.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f) },
    { Vector3( -0.5f,  0.5f, 0.0f ), Vector2(0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f) },
    { Vector3(  0.5f,  0.5f, 0.0f ), Vector2(1.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f) }  };

  Property::Map vertexFormat;
  vertexFormat["aPosition"] = Property::VECTOR3;
  vertexFormat["aTexCoord"] = Property::VECTOR2;
  vertexFormat["aColor"] = Property::VECTOR3;
  PropertyBuffer vertices = PropertyBuffer::New( vertexFormat, 5 );
  vertices.SetData( vertexData );

  // Specify all the faces
  unsigned int indexData[12] = { 0,1,3,0,2,4,0,3,4,0,2,1 };
  Property::Map indexFormat;
  indexFormat["indices"] = Property::INTEGER;
  PropertyBuffer indices = PropertyBuffer::New( indexFormat, 12 );
  indices.SetData( indexData );

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertices );
  geometry.SetIndexBuffer( indices );

  return geometry;
}

Dali::Property::Map NewWindowController::CreateColorModifierer()
{
 const char* fragmentShader ( DALI_COMPOSE_SHADER (
   precision highp float;\n
   uniform vec3 uRGBDelta;\n
   uniform float uIgnoreAlpha;\n
   \n
   varying mediump vec2 vTexCoord;\n
   uniform sampler2D sTexture;\n
   \n
   float rand(vec2 co) \n
   {\n
     return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); \n}
   \n
   void main() {\n
     vec4 color = texture2D(sTexture, vTexCoord); \n
     // modify the hsv Value
     color.rgb += uRGBDelta * rand(vTexCoord); \n
     // if the new vale exceeds one, then decrease it
     color.rgb -= max(color.rgb*2.0 - vec3(2.0), 0.0);\n
     // if the new vale drops below zero, then increase it
     color.rgb -= min(color.rgb*2.0, 0.0);\n
     gl_FragColor = color; \n
   }\n
 ) );

 Property::Map map;
 Property::Map customShader;
 customShader[ "fragment-shader" ] = fragmentShader;
 map[ "shader" ] = customShader;

 return map;
}

void NewWindowController::NewWindow(void)
{
  PositionSize posSize(0, 0, 720, 1280);
  gApplication.ReplaceWindow(posSize, "NewWindow"); // Generates a new window
}

bool NewWindowController::OnLoseContextButtonClicked( Toolkit::Button button )
{
  // Add as an idle callback to avoid ProcessEvents being recursively called.
  mApplication.AddIdle( MakeCallback( NewWindowController::NewWindow ) );
  return true;
}

bool NewWindowController::OnTrackTimerTick()
{
  static int time=0;
  const float radius(250.0f);

  time += EMIT_INTERVAL_IN_MS;
  float modTime = time / TRACK_DURATION_IN_MS;
  float angle = 2.0f*Math::PI*modTime;

  Vector2 position(radius*cosf(angle), radius*-sinf(angle));
  Vector2 aimPos(radius*2*sinf(angle), radius*2*-cosf(angle));
  Vector2 direction = aimPos-position;
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  SetUpBubbleEmission( stageSize*0.5f+position, direction );
  SetUpBubbleEmission( stageSize*0.5f+position*0.75f, direction );
  SetUpBubbleEmission( stageSize*0.5f+position*0.7f, direction );

  return true;
}

void NewWindowController::OnKeyEvent(const KeyEvent& event)
{
  if(event.state == KeyEvent::Down)
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
      mApplication.Quit();
    }
  }
}

void NewWindowController::OnContextLost()
{
  printf("Stage reporting context loss\n");
}

void NewWindowController::OnContextRegained()
{
  printf("Stage reporting context regain\n");
}

void RunTest(Application& app)
{
  gNewWindowController = new NewWindowController(app);
  app.MainLoop(Configuration::APPLICATION_DOES_NOT_HANDLE_CONTEXT_LOSS);
}

// Entry point for Linux & Tizen applications
//
int main(int argc, char **argv)
{
  gApplication = Application::New(&argc, &argv, DALI_DEMO_THEME_PATH);
  RunTest(gApplication);

  return 0;
}
