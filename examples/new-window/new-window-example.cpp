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

#include <dali-toolkit/dali-toolkit.h>
#include "shared/view.h"
#include <cstdio>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

class NewWindowController;

namespace
{
const char * const BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-2.jpg" );
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char * const LOSE_CONTEXT_IMAGE( DALI_IMAGE_DIR "icon-cluster-wobble.png" );
const char * const BASE_IMAGE( DALI_IMAGE_DIR "gallery-large-14.jpg" );
const char * const EFFECT_IMAGE( DALI_IMAGE_DIR "gallery-large-18.jpg" );

const float EXPLOSION_DURATION(1.2f);
const unsigned int EMIT_INTERVAL_IN_MS(80);
const float TRACK_DURATION_IN_MS(970);

Application gApplication;
NewWindowController* gNewWindowController(NULL);

const char*const FRAG_SHADER=
  "uniform mediump float alpha;\n"
  "\n"
  "void main()\n"
  "{\n"
  "  mediump vec4 fragColor = texture2D(sTexture, vTexCoord);\n"
  "  mediump vec4 fxColor   = texture2D(sEffect, vTexCoord);\n"
  "  gl_FragColor   = mix(fragColor,fxColor, alpha);\n"
  "}\n";

}; // anonymous namespace


class NewWindowController : public ConnectionTracker
{
public:
  NewWindowController( Application& app );
  void Create( Application& app );
  void Destroy( Application& app );
  void OnKeyEvent(const KeyEvent& event);
  bool OnLoseContextButtonClicked( Toolkit::Button button );
  static void NewWindow(void);

  void OnContextLost();
  void OnContextRegained();
  void CreateMeshActor();
  Mesh CreateMesh(bool, Material);
  void CreateBubbles(Vector2 stageSize);
  void CreateBlending();
  void CreateText();
  bool OnTrackTimerTick();
  bool OnExplodeTimerTick();
  void SetUpAnimation( Vector2 emitPosition, Vector2 direction );
  FrameBufferImage CreateMirrorImage(const char* imageName);
  ImageActor CreateBlurredMirrorImage(const char* imageName);
  FrameBufferImage CreateFrameBufferForImage(const char* imageName, Image image, ShaderEffect shaderEffect);


private:
  Application                mApplication;
  Actor                      mCastingLight;
  TextActor                  mTextActor;
  ImageActor                 mImageActor;
  ImageActor                 mBlendActor;
  Image                      mEffectImage;
  Image                      mBaseImage;
  MeshActor                  mMeshActor;
  MeshActor                  mAnimatedMeshActor;

  Toolkit::View              mView;                              ///< The View instance.
  Toolkit::ToolBar           mToolBar;                           ///< The View's Toolbar.
  TextView                   mTitleActor;                        ///< The Toolbar's Title.
  Layer                      mContentLayer;                      ///< Content layer (scrolling cluster content)
  Toolkit::PushButton        mLoseContextButton;
  Vector3                    mHSVDelta;
  Toolkit::BubbleEmitter     mEmitter;

  Timer                      mEmitTrackTimer;
  Timer                      mExplodeTimer;
  bool                       mNeedNewAnimation;

  unsigned int               mAnimateComponentCount;
  Animation                  mEmitAnimation;
};


NewWindowController::NewWindowController( Application& application )
: mApplication(application),
  mHSVDelta(0.5f, 0.0f, 0.5f),
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
                                          BACKGROUND_IMAGE,
                                          TOOLBAR_IMAGE,
                                          "Context recovery" );

  // Point the default render task at the view
  RenderTaskList taskList = stage.GetRenderTaskList();
  RenderTask defaultTask = taskList.GetTask( 0u );
  if ( defaultTask )
  {
    defaultTask.SetSourceActor( mView );
  }

  mLoseContextButton = Toolkit::PushButton::New();
  mLoseContextButton.SetBackgroundImage( ResourceImage::New( LOSE_CONTEXT_IMAGE ) );
  mLoseContextButton.ClickedSignal().Connect( this, &NewWindowController::OnLoseContextButtonClicked );
  mToolBar.AddControl( mLoseContextButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );


  Actor logoLayoutActor = Actor::New();
  logoLayoutActor.SetParentOrigin(ParentOrigin::CENTER);
  logoLayoutActor.SetPosition(0.0f, -200.0f, 0.0f);
  logoLayoutActor.SetScale(0.5f);
  mContentLayer.Add(logoLayoutActor);

  Image image = ResourceImage::New(DALI_IMAGE_DIR "dali-logo.png");
  mImageActor = ImageActor::New(image);
  mImageActor.SetName("dali-logo");
  mImageActor.SetParentOrigin(ParentOrigin::CENTER);
  mImageActor.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER);
  logoLayoutActor.Add(mImageActor);

  ImageActor mirrorImageActor = CreateBlurredMirrorImage(DALI_IMAGE_DIR "dali-logo.png");
  mirrorImageActor.SetParentOrigin(ParentOrigin::CENTER);
  mirrorImageActor.SetAnchorPoint(AnchorPoint::TOP_CENTER);
  logoLayoutActor.Add(mirrorImageActor);

  CreateBubbles(stage.GetSize());
  CreateMeshActor();
  CreateBlending();
  CreateText();

  stage.ContextLostSignal().Connect(this, &NewWindowController::OnContextLost);
  stage.ContextRegainedSignal().Connect(this, &NewWindowController::OnContextRegained);
}

void NewWindowController::Destroy( Application& app )
{
  UnparentAndReset(mTextActor);
}

bool NewWindowController::OnLoseContextButtonClicked( Toolkit::Button button )
{
  // Add as an idle callback to avoid ProcessEvents being recursively called.
  mApplication.AddIdle( MakeCallback( NewWindowController::NewWindow ) );
  return true;
}

void NewWindowController::CreateMeshActor()
{
  mEffectImage = ResourceImage::New(EFFECT_IMAGE);

  Material baseMaterial = Material::New( "Material1" );
  Dali::MeshActor meshActor = MeshActor::New( CreateMesh(true, baseMaterial) );
  meshActor.SetScale( 100.0f );
  meshActor.SetParentOrigin( ParentOrigin::CENTER );
  meshActor.SetPosition(Vector3( -150.0f, 200.0f, 0.0f ));
  meshActor.SetName("MeshActor");
  mContentLayer.Add( meshActor );

  Material orchidMaterial = Material::New( "Material2" );
  orchidMaterial.SetDiffuseTexture(mEffectImage);

  Dali::MeshActor meshActor2 = MeshActor::New( CreateMesh(false, orchidMaterial) );
  meshActor2.SetScale( 100.0f );
  meshActor2.SetParentOrigin( ParentOrigin::CENTER );
  meshActor2.SetPosition(Vector3( -150.0f, 310.0f, 0.0f ));
  meshActor2.SetName("MeshActor");
  mContentLayer.Add( meshActor2 );
}

FrameBufferImage NewWindowController::CreateMirrorImage(const char* imageName)
{
  FrameBufferImage fbo;
  Image image = ResourceImage::New(imageName);
  fbo = CreateFrameBufferForImage(imageName, image, ShaderEffect());
  return fbo;
}

ImageActor NewWindowController::CreateBlurredMirrorImage(const char* imageName)
{
  FrameBufferImage fbo;
  Image image = ResourceImage::New( imageName );
  Vector2 FBOSize = ResourceImage::GetImageSize(imageName);
  fbo = FrameBufferImage::New( FBOSize.width, FBOSize.height, Pixel::RGBA8888);
  GaussianBlurView gbv = GaussianBlurView::New(5, 2.0f, Pixel::RGBA8888, 0.5f, 0.5f, true);
  gbv.SetBackgroundColor(Color::TRANSPARENT);
  gbv.SetUserImageAndOutputRenderTarget( image, fbo );
  gbv.SetSize(FBOSize);
  Stage::GetCurrent().Add(gbv);
  gbv.ActivateOnce();

  ImageActor blurredActor = ImageActor::New(fbo);
  blurredActor.SetSize(FBOSize);
  blurredActor.SetScale(1.0f, -1.0f, 1.0f);
  return blurredActor;
}

FrameBufferImage NewWindowController::CreateFrameBufferForImage(const char* imageName, Image image, ShaderEffect shaderEffect)
{
  Stage stage = Stage::GetCurrent();
  Vector2 FBOSize = ResourceImage::GetImageSize(imageName);

  FrameBufferImage framebuffer = FrameBufferImage::New(FBOSize.x, FBOSize.y );

  RenderTask renderTask = stage.GetRenderTaskList().CreateTask();

  ImageActor imageActor = ImageActor::New(image);
  imageActor.SetName("Source image actor");
  if(shaderEffect)
  {
    imageActor.SetShaderEffect(shaderEffect);
  }
  imageActor.SetParentOrigin(ParentOrigin::CENTER);
  imageActor.SetAnchorPoint(AnchorPoint::CENTER);
  imageActor.SetScale(1.0f, -1.0f, 1.0f);
  stage.Add(imageActor); // Not in default image view

  CameraActor cameraActor = CameraActor::New(FBOSize);
  cameraActor.SetParentOrigin(ParentOrigin::CENTER);
  cameraActor.SetFieldOfView(Math::PI*0.25f);
  cameraActor.SetNearClippingPlane(1.0f);
  cameraActor.SetAspectRatio(FBOSize.width / FBOSize.height);
  cameraActor.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor
  cameraActor.SetRotation(Quaternion(M_PI, Vector3::YAXIS));
  cameraActor.SetPosition(0.0f, 0.0f, ((FBOSize.height * 0.5f) / tanf(Math::PI * 0.125f)));
  stage.Add(cameraActor);

  renderTask.SetSourceActor(imageActor);
  renderTask.SetInputEnabled(false);
  renderTask.SetTargetFrameBuffer(framebuffer);
  renderTask.SetCameraActor( cameraActor );
  renderTask.SetClearColor( Color::TRANSPARENT );
  renderTask.SetClearEnabled( true );
  renderTask.SetRefreshRate(RenderTask::REFRESH_ONCE);

  return framebuffer;
}

void NewWindowController::CreateBubbles(Vector2 stageSize)
{
  mEmitter = Toolkit::BubbleEmitter::New( stageSize,
                                          ResourceImage::New( DALI_IMAGE_DIR "bubble-ball.png" ),
                                          1000, Vector2( 5.0f, 5.0f ) );

  Image background = ResourceImage::New(BACKGROUND_IMAGE);
  mEmitter.SetBackground( background, mHSVDelta );
  Actor bubbleRoot = mEmitter.GetRootActor();
  mContentLayer.Add( bubbleRoot );
  bubbleRoot.SetParentOrigin(ParentOrigin::CENTER);
  bubbleRoot.SetZ(0.1f);

  mEmitTrackTimer = Timer::New( EMIT_INTERVAL_IN_MS );
  mEmitTrackTimer.TickSignal().Connect(this, &NewWindowController::OnTrackTimerTick);
  mEmitTrackTimer.Start();

  //mExplodeTimer = Timer::New( Random::Range(4000.f, 8000.f) );
  //mExplodeTimer.TickSignal().Connect(this, &NewWindowController::OnExplodeTimerTick);
  //mExplodeTimer.Start();
}

bool NewWindowController::OnExplodeTimerTick()
{
  mEmitter.StartExplosion( EXPLOSION_DURATION, 5.0f );

  mExplodeTimer = Timer::New( Random::Range(4.f, 8.f) );
  mExplodeTimer.TickSignal().Connect(this, &NewWindowController::OnExplodeTimerTick);
  return false;
}

void NewWindowController::SetUpAnimation( Vector2 emitPosition, Vector2 direction )
{
  if( mNeedNewAnimation )
  {
    float duration = Random::Range(1.f, 1.5f);
    mEmitAnimation = Animation::New( duration );
    mNeedNewAnimation = false;
    mAnimateComponentCount = 0;
  }

  mEmitter.EmitBubble( mEmitAnimation, emitPosition, direction, Vector2(1, 1) );

  mAnimateComponentCount++;

  if( mAnimateComponentCount % 20 ==0 )
  {
    mEmitAnimation.Play();
    mNeedNewAnimation = true;
  }
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

  for(int i=0; i<20; i++)
  {
    SetUpAnimation( stageSize*0.5f+position, direction );
  }

  return true;
}


void NewWindowController::CreateBlending()
{
  Toolkit::ColorAdjuster colorAdjuster = ColorAdjuster::New(mHSVDelta);
  FrameBufferImage fb2 = CreateFrameBufferForImage( EFFECT_IMAGE, mEffectImage, colorAdjuster );

  ImageActor tmpActor = ImageActor::New(fb2);
  mContentLayer.Add(tmpActor);
  tmpActor.SetParentOrigin(ParentOrigin::BOTTOM_RIGHT);
  tmpActor.SetAnchorPoint(AnchorPoint::BOTTOM_RIGHT);
  tmpActor.SetScale(0.25f);

  // create blending shader effect
  ShaderEffect blendShader = ShaderEffect::New( "", FRAG_SHADER );
  blendShader.SetEffectImage( fb2 );
  blendShader.SetUniform("alpha", 0.5f);

  mBaseImage = ResourceImage::New(BASE_IMAGE);
  mBlendActor = ImageActor::New( mBaseImage );
  mBlendActor.SetParentOrigin(ParentOrigin::CENTER);
  mBlendActor.SetPosition(Vector3(150.0f, 200.0f, 0.0f));
  mBlendActor.SetSize(140, 140);
  mBlendActor.SetShaderEffect( blendShader );
  mContentLayer.Add(mBlendActor);
}

void NewWindowController::CreateText()
{
  mTextActor = TextActor::New("Some text");
  mTextActor.SetParentOrigin(ParentOrigin::CENTER);
  mTextActor.SetColor(Color::RED);
  mTextActor.SetName("PushMe text");
  mContentLayer.Add( mTextActor );
}

Mesh NewWindowController::CreateMesh(bool hasColor, Material material)
{
  // Create vertices and specify their color
  MeshData::VertexContainer vertices(4);
  vertices[ 0 ] = MeshData::Vertex( Vector3( -0.5f, -0.5f, 0.0f ), Vector2(0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f) );
  vertices[ 1 ] = MeshData::Vertex( Vector3(  0.5f, -0.5f, 0.0f ), Vector2(1.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f) );
  vertices[ 2 ] = MeshData::Vertex( Vector3( -0.5f,  0.5f, 0.0f ), Vector2(0.0f, 1.0f), Vector3(0.0f,1.0f,0.0f) );
  vertices[ 3 ] = MeshData::Vertex( Vector3(  0.5f,  0.5f, 0.0f ), Vector2(1.0f, 1.0f), Vector3(0.0f,0.0f,1.0f) );

  // Specify all the faces
  MeshData::FaceIndices faces;
  faces.reserve( 6 ); // 2 triangles in Quad
  faces.push_back( 0 ); faces.push_back( 3 ); faces.push_back( 1 );
  faces.push_back( 0 ); faces.push_back( 2 ); faces.push_back( 3 );

  // Create the mesh data from the vertices and faces
  MeshData meshData;
  meshData.SetHasColor( hasColor );
  meshData.SetMaterial( material );
  meshData.SetVertices( vertices );
  meshData.SetFaceIndices( faces );

  // Create a mesh from the data
  Dali::Mesh mesh = Mesh::New( meshData );
  return mesh;
}

void NewWindowController::NewWindow(void)
{
  PositionSize posSize(0, 0, 720, 1280);
  gApplication.ReplaceWindow(posSize, "NewWindow"); // Generates a new window
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
  gApplication = Application::New(&argc, &argv);
  RunTest(gApplication);

  return 0;
}
