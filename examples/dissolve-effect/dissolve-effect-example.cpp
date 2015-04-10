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
 *
 */

// EXTERNAL INCLUDES
#include <math.h>

// INTERNAL INCLUDES
#include "shared/view.h"

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;

using Dali::Toolkit::TextLabel;

// LOCAL STUFF
namespace
{

const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char * const APPLICATION_TITLE_HIGHP( "Dissolve Effect(highp)" );
const char * const APPLICATION_TITLE_MEDIUMP( "Dissolve Effect(mediump)" );
const char * const EFFECT_HIGHP_IMAGE( DALI_IMAGE_DIR "icon-highp.png" );
const char * const EFFECT_MEDIUMP_IMAGE( DALI_IMAGE_DIR "icon-mediump.png" );
const char * const PLAY_ICON( DALI_IMAGE_DIR "icon-play.png" );
const char * const STOP_ICON( DALI_IMAGE_DIR "icon-stop.png" );

const char* IMAGES[] =
{
  DALI_IMAGE_DIR "gallery-large-1.jpg",
  DALI_IMAGE_DIR "gallery-large-2.jpg",
  DALI_IMAGE_DIR "gallery-large-3.jpg",
  DALI_IMAGE_DIR "gallery-large-4.jpg",
  DALI_IMAGE_DIR "gallery-large-5.jpg",
  DALI_IMAGE_DIR "gallery-large-6.jpg",
  DALI_IMAGE_DIR "gallery-large-7.jpg",
  DALI_IMAGE_DIR "gallery-large-8.jpg",
  DALI_IMAGE_DIR "gallery-large-9.jpg",
  DALI_IMAGE_DIR "gallery-large-10.jpg",
  DALI_IMAGE_DIR "gallery-large-11.jpg",
  DALI_IMAGE_DIR "gallery-large-12.jpg",
  DALI_IMAGE_DIR "gallery-large-13.jpg",
  DALI_IMAGE_DIR "gallery-large-14.jpg",
  DALI_IMAGE_DIR "gallery-large-15.jpg",
  DALI_IMAGE_DIR "gallery-large-16.jpg",
  DALI_IMAGE_DIR "gallery-large-17.jpg",
  DALI_IMAGE_DIR "gallery-large-18.jpg",
  DALI_IMAGE_DIR "gallery-large-19.jpg",
  DALI_IMAGE_DIR "gallery-large-20.jpg",
  DALI_IMAGE_DIR "gallery-large-21.jpg",
};

const int NUM_IMAGES( sizeof(IMAGES) / sizeof(IMAGES[0]) );

// The duration of the current image staying on screen when slideshow is on
const int VIEWINGTIME = 2000; // 2 seconds

const float TRANSITION_DURATION = 2.5f; //2.5 second

const float INITIAL_DEPTH = -10.0f;

/**
 * @brief Load an image, scaled-down to no more than the stage dimensions.
 *
 * Uses image scaling mode ImageAttributes::ScaleToFill to resize the image at
 * load time to cover the entire stage with pixels with no borders,
 * and filter mode ImageAttributes::BoxThenLinear to sample the image with
 * maximum quality.
 */
ResourceImage LoadStageFillingImage( const char * const imagePath )
{
  Size stageSize = Stage::GetCurrent().GetSize();
  ImageAttributes attributes;
  attributes.SetSize( stageSize.x, stageSize.y );
  attributes.SetFilterMode( ImageAttributes::BoxThenLinear );
  attributes.SetScalingMode( ImageAttributes::ScaleToFill );
  return ResourceImage::New( imagePath, attributes );
}

} // namespace

class DissolveEffectApp : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  DissolveEffectApp( Application& application );

  ~DissolveEffectApp();

private:

  /**
   * This method gets called once the main loop of application is up and running
   */
  void OnInit( Application& application );
  /**
   * PanGesture callback. This method gets called when the pan gesture is detected.
   * @param[in] actor The actor receiving the pan gesture.
   * @param[in] gesture The detected pan gesture.
   */
  void OnPanGesture( Actor actor, const PanGesture& gesture );

  /**
   * Set up the animations for transition
   * @param[in] position The point ( locates within rectange {(0,0),(0,1),(1,0),(1,1)} ) passing through the central line of the dissolve effect
   * @param[in] displacement The direction of the central line of the dissolve effect
   */
  void StartTransition(Vector2 position, Vector2 displacement);
  /**
   * Callback function of effect-switch button
   * Change the precision of the effect shader when the effect button is clicked
   * @param[in] button The handle of the clicked button
   */
  bool OnEffectButtonClicked( Toolkit::Button button );
  /**
   * Callback function of slideshow button
   * Start or stop the automatical image display when the slideshow button is clicked
   * @param[in] button The handle of the clicked button
   */
  bool OnSildeshowButtonClicked( Toolkit::Button button );
  /**
   * Callback function of cube transition completed signal
   * @param[in] effect The cube effect used for the transition
   * @param[in] imageActor The target imageActor of the completed transition
   */
  void OnTransitionCompleted(Animation& source);
  /**
   * Callback function of timer tick
   * The timer is used to count the image display duration after cube transition in slideshow,
   */
  bool OnTimerTick();

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event);

private:
  Application&                    mApplication;
  Toolkit::View                   mView;
  Toolkit::ToolBar                mToolBar;
  Layer                           mContent;
  Toolkit::TextLabel              mTitleActor;
  Actor                           mParent;

  ImageActor                      mCurrentImage;
  ImageActor                      mNextImage;
  unsigned int                    mIndex;

  Toolkit::DissolveEffect         mCurrentImageEffect;
  Toolkit::DissolveEffect         mNextImageEffect;
  bool                            mUseHighPrecision;
  Animation                       mAnimation;

  PanGestureDetector              mPanGestureDetector;
  bool                            mIsTransiting;

  bool                            mSlideshow;
  Timer                           mViewTimer;
  bool                            mTimerReady;
  unsigned int                    mCentralLineIndex;

  Image                           mIconPlay;
  Image                           mIconStop;
  Toolkit::PushButton             mPlayStopButton;

  Image                           mIconHighP;
  Image                           mIconMediumP;
  Toolkit::PushButton             mEffectChangeButton;
};

DissolveEffectApp::DissolveEffectApp( Application& application )
: mApplication( application ),
  mIndex( 0 ),
  mUseHighPrecision(true),
  mIsTransiting( false ),
  mSlideshow( false ),
  mTimerReady( false ),
  mCentralLineIndex( 0 )
{
  mApplication.InitSignal().Connect( this, &DissolveEffectApp::OnInit );
}

DissolveEffectApp::~DissolveEffectApp()
{
  //Nothing to do
}

void DissolveEffectApp::OnInit( Application& application )
{
  DemoHelper::RequestThemeChange();

  Stage::GetCurrent().KeyEventSignal().Connect(this, &DissolveEffectApp::OnKeyEvent);

  // Creates a default view with a default tool bar, the view is added to the stage.
  mContent = DemoHelper::CreateView( application, mView,mToolBar, "", TOOLBAR_IMAGE, "" );

  // Add an effect-changing button on the right of the tool bar.
  mIconHighP = ResourceImage::New( EFFECT_HIGHP_IMAGE );
  mIconMediumP = ResourceImage::New( EFFECT_MEDIUMP_IMAGE );
  mEffectChangeButton = Toolkit::PushButton::New();
  mEffectChangeButton.SetBackgroundImage(mIconHighP);
  mEffectChangeButton.ClickedSignal().Connect( this, &DissolveEffectApp::OnEffectButtonClicked );
  mToolBar.AddControl( mEffectChangeButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

  // Add title to the tool bar.
  mTitleActor = DemoHelper::CreateToolBarLabel( APPLICATION_TITLE_HIGHP );
  mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Toolkit::Alignment::HorizontalCenter );

  // Add an slide-show button on the right of the title
  mIconPlay = ResourceImage::New( PLAY_ICON );
  mIconStop = ResourceImage::New( STOP_ICON );
  mPlayStopButton = Toolkit::PushButton::New();
  mPlayStopButton.SetBackgroundImage( mIconPlay );
  mPlayStopButton.ClickedSignal().Connect( this, &DissolveEffectApp::OnSildeshowButtonClicked );
  mToolBar.AddControl( mPlayStopButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalCenter, DemoHelper::DEFAULT_PLAY_PADDING );

  // use pan gesture to detect the cursor or finger movement
  mPanGestureDetector = PanGestureDetector::New();
  mPanGestureDetector.DetectedSignal().Connect( this, &DissolveEffectApp::OnPanGesture );

  // create the dissolve effect object
  mCurrentImageEffect = Toolkit::DissolveEffect::New(mUseHighPrecision);
  mNextImageEffect = Toolkit::DissolveEffect::New(mUseHighPrecision);

  mViewTimer = Timer::New( VIEWINGTIME );
  mViewTimer.TickSignal().Connect( this, &DissolveEffectApp::OnTimerTick );
  mTimerReady = true;

  // Set size to stage size to avoid seeing a black border on transition
  mParent = Actor::New();
  mParent.SetSize( Stage::GetCurrent().GetSize() );
  mParent.SetPositionInheritanceMode( USE_PARENT_POSITION );
  mContent.Add( mParent );

  // show the first image
  mCurrentImage = ImageActor::New( LoadStageFillingImage( IMAGES[mIndex] ) );
  mCurrentImage.SetRelayoutEnabled( false );
  mCurrentImage.SetPositionInheritanceMode(USE_PARENT_POSITION_PLUS_LOCAL_POSITION);
  mCurrentImage.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  mCurrentImage.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );
  mParent.Add( mCurrentImage );

  mPanGestureDetector.Attach( mCurrentImage );
}

// signal handler, called when the pan gesture is detected
void DissolveEffectApp::OnPanGesture( Actor actor, const PanGesture& gesture )
{
  // does not response when the animation has not finished
  if( mIsTransiting || mSlideshow )
  {
    return;
  }

  if( gesture.state == Gesture::Continuing )
  {
    if( gesture.displacement.x < 0)
    {
      mIndex = (mIndex + 1)%NUM_IMAGES;
    }
    else
    {
      mIndex = (mIndex + NUM_IMAGES -1)%NUM_IMAGES;
    }

    Image image = LoadStageFillingImage( IMAGES[ mIndex ] );
    mNextImage = ImageActor::New( image );
    mNextImage.SetRelayoutEnabled( false );
    mNextImage.SetPositionInheritanceMode(USE_PARENT_POSITION_PLUS_LOCAL_POSITION);
    mNextImage.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mNextImage.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );
    mNextImage.SetZ(INITIAL_DEPTH);
    mParent.Add( mNextImage );
    Vector2 size = Vector2( mCurrentImage.GetCurrentSize() );
    StartTransition( gesture.position / size, gesture.displacement * Vector2(1.0, size.x/size.y));
  }
}

void DissolveEffectApp::StartTransition(Vector2 position, Vector2 displacement)
{
  mAnimation = Animation::New(TRANSITION_DURATION);

  mCurrentImageEffect.SetCentralLine(position,displacement);
  mCurrentImageEffect.SetDistortion(0.0f);
  mCurrentImage.SetShaderEffect(mCurrentImageEffect);
  mAnimation.AnimateTo( Property(mCurrentImageEffect, mCurrentImageEffect.GetDistortionPropertyName()), 1.0f, AlphaFunctions::Linear );

  mNextImage.SetOpacity(0.0f);
  mAnimation.AnimateTo( Property( mNextImage, Actor::Property::COLOR_ALPHA ), 1.0f, AlphaFunctions::Linear );

  if(mUseHighPrecision)
  {
    mNextImageEffect.SetCentralLine(position,-displacement);
    mNextImageEffect.SetDistortion(1.0f);
    mNextImage.SetShaderEffect(mNextImageEffect);
    mAnimation.AnimateTo( Property(mNextImageEffect, mNextImageEffect.GetDistortionPropertyName()), 0.0f, AlphaFunctions::Linear );
  }
  else
  {
    mAnimation.AnimateTo( Property( mNextImage, Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), AlphaFunctions::Linear );
  }

  mAnimation.FinishedSignal().Connect( this, &DissolveEffectApp::OnTransitionCompleted );
  mAnimation.Play();
  mIsTransiting = true;
}

void DissolveEffectApp::OnKeyEvent(const KeyEvent& event)
{
  if(event.state == KeyEvent::Down)
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
      mApplication.Quit();
    }
  }
}

bool DissolveEffectApp::OnEffectButtonClicked( Toolkit::Button button )
{
  mUseHighPrecision = !mUseHighPrecision;
  mCurrentImageEffect = Toolkit::DissolveEffect::New(mUseHighPrecision);
  if(mUseHighPrecision)
  {
    mTitleActor.SetProperty( TextLabel::Property::TEXT, std::string(APPLICATION_TITLE_HIGHP) );
    mEffectChangeButton.SetBackgroundImage(mIconHighP);
  }
  else
  {
    mTitleActor.SetProperty( TextLabel::Property::TEXT, std::string(APPLICATION_TITLE_MEDIUMP) );
    mEffectChangeButton.SetBackgroundImage(mIconMediumP);
  }

  return true;
}

bool DissolveEffectApp::OnSildeshowButtonClicked( Toolkit::Button button )
{
  mSlideshow = !mSlideshow;
  if( mSlideshow )
  {
    mPlayStopButton.SetBackgroundImage( mIconStop );
    mPanGestureDetector.Detach( mParent );
    mViewTimer.Start();
    mTimerReady = false;
  }
  else
  {
    mPlayStopButton.SetBackgroundImage( mIconPlay );
    mTimerReady = true;
    mPanGestureDetector.Attach( mParent );
  }
  return true;
}

void DissolveEffectApp::OnTransitionCompleted( Animation& source )
{
  mCurrentImage.RemoveShaderEffect();
  mNextImage.RemoveShaderEffect();
  mParent.Remove( mCurrentImage );
  mPanGestureDetector.Detach( mCurrentImage );
  mCurrentImage = mNextImage;
  mPanGestureDetector.Attach( mCurrentImage );
  mIsTransiting = false;

  if( mSlideshow)
  {
    mViewTimer.Start();
    mTimerReady = false;
  }
}

bool DissolveEffectApp::OnTimerTick()
{
  mTimerReady = true;
  if(mSlideshow)
  {
    mIndex = (mIndex + 1)%NUM_IMAGES;
    Image image = LoadStageFillingImage( IMAGES[ mIndex ] );
    mNextImage = ImageActor::New( image );
    mNextImage.SetPositionInheritanceMode(USE_PARENT_POSITION_PLUS_LOCAL_POSITION);
    mNextImage.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mNextImage.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );
    mNextImage.SetZ(INITIAL_DEPTH);
    mParent.Add( mNextImage );
    switch( mCentralLineIndex%4 )
    {
      case 0:
      {
        StartTransition(Vector2(1.0f,0.5f), Vector2(-1.0f, 0.0f));
        break;
      }
      case 1:
      {
        StartTransition(Vector2(0.5f,0.0f), Vector2(0.0f, 1.0f));
        break;
      }
      case 2:
      {
        StartTransition(Vector2(0.0f,0.5f), Vector2(1.0f, 0.0f));
        break;
      }
      default:
      {
        StartTransition(Vector2(0.5f,1.0f), Vector2(0.0f, -1.0f));
        break;
      }

    }
    mCentralLineIndex++;
  }
  return false;   //return false to stop the timer
}

// Entry point for Linux & Tizen applications
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  DissolveEffectApp test( application );
  application.MainLoop();

  return 0;
}
