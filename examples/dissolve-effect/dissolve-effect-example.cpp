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

// EXTERNAL INCLUDES
#include <math.h>

// INTERNAL INCLUDES
#include "shared/view.h"

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>
#include <dali-toolkit/devel-api/shader-effects/dissolve-effect.h>

using namespace Dali;

using Dali::Toolkit::TextLabel;

// LOCAL STUFF
namespace
{

const char * const TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char * const APPLICATION_TITLE_HIGHP( "Dissolve Effect(highp)" );
const char * const APPLICATION_TITLE_MEDIUMP( "Dissolve Effect(mediump)" );
const char * const EFFECT_HIGHP_IMAGE( DEMO_IMAGE_DIR "icon-highp.png" );
const char * const EFFECT_HIGHP_IMAGE_SELECTED( DEMO_IMAGE_DIR "icon-highp-selected.png" );
const char * const EFFECT_MEDIUMP_IMAGE( DEMO_IMAGE_DIR "icon-mediump.png" );
const char * const EFFECT_MEDIUMP_IMAGE_SELECTED( DEMO_IMAGE_DIR "icon-mediump-selected.png" );
const char * const PLAY_ICON( DEMO_IMAGE_DIR "icon-play.png" );
const char * const PLAY_ICON_SELECTED( DEMO_IMAGE_DIR "icon-play-selected.png" );
const char * const STOP_ICON( DEMO_IMAGE_DIR "icon-stop.png" );
const char * const STOP_ICON_SELECTED( DEMO_IMAGE_DIR "icon-stop-selected.png" );

const char* IMAGES[] =
{
  DEMO_IMAGE_DIR "gallery-large-1.jpg",
  DEMO_IMAGE_DIR "gallery-large-2.jpg",
  DEMO_IMAGE_DIR "gallery-large-3.jpg",
  DEMO_IMAGE_DIR "gallery-large-4.jpg",
  DEMO_IMAGE_DIR "gallery-large-5.jpg",
  DEMO_IMAGE_DIR "gallery-large-6.jpg",
  DEMO_IMAGE_DIR "gallery-large-7.jpg",
  DEMO_IMAGE_DIR "gallery-large-8.jpg",
  DEMO_IMAGE_DIR "gallery-large-9.jpg",
  DEMO_IMAGE_DIR "gallery-large-10.jpg",
  DEMO_IMAGE_DIR "gallery-large-11.jpg",
  DEMO_IMAGE_DIR "gallery-large-12.jpg",
  DEMO_IMAGE_DIR "gallery-large-13.jpg",
  DEMO_IMAGE_DIR "gallery-large-14.jpg",
  DEMO_IMAGE_DIR "gallery-large-15.jpg",
  DEMO_IMAGE_DIR "gallery-large-16.jpg",
  DEMO_IMAGE_DIR "gallery-large-17.jpg",
  DEMO_IMAGE_DIR "gallery-large-18.jpg",
  DEMO_IMAGE_DIR "gallery-large-19.jpg",
  DEMO_IMAGE_DIR "gallery-large-20.jpg",
  DEMO_IMAGE_DIR "gallery-large-21.jpg",
};

const int NUM_IMAGES( sizeof(IMAGES) / sizeof(IMAGES[0]) );

// The duration of the current image staying on screen when slideshow is on
const int VIEWINGTIME = 2000; // 2 seconds

const float TRANSITION_DURATION = 2.5f; //2.5 second

const float INITIAL_DEPTH = 10.0f;

/**
 * @brief Create an image view with an image which would be scaled-down to no more than the stage dimensions.
 *
 * Uses image scaling mode SCALE_TO_FILL to resize the image at
 * load time to cover the entire stage with pixels with no borders,
 * and filter mode BOX_THEN_LINEAR to sample the image with maximum quality.
 */
Toolkit::ImageView CreateStageFillingImageView( const char * const imagePath )
{
  Size stageSize = Stage::GetCurrent().GetSize();
  Toolkit::ImageView imageView = Toolkit::ImageView::New();
  Property::Map map;
  map[Toolkit::Visual::Property::TYPE] = Toolkit::Visual::IMAGE;
  map[Toolkit::ImageVisual::Property::URL] = imagePath;
  map[Toolkit::ImageVisual::Property::DESIRED_WIDTH] = stageSize.x;
  map[Toolkit::ImageVisual::Property::DESIRED_HEIGHT] = stageSize.y;
  map[Toolkit::ImageVisual::Property::FITTING_MODE] = FittingMode::SCALE_TO_FILL;
  map[Toolkit::ImageVisual::Property::SAMPLING_MODE] = SamplingMode::BOX_THEN_LINEAR;
  map[Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING] = true;
  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, map );

  return imageView;
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
  Toolkit::Control                mView;
  Toolkit::ToolBar                mToolBar;
  Layer                           mContent;
  Toolkit::TextLabel              mTitleActor;
  Actor                           mParent;

  Toolkit::ImageView              mCurrentImage;
  Toolkit::ImageView              mNextImage;
  unsigned int                    mIndex;

  Property::Map                   mDissolveEffect;
  Property::Map                   mEmptyEffect;

  bool                            mUseHighPrecision;
  Animation                       mAnimation;

  PanGestureDetector              mPanGestureDetector;
  bool                            mIsTransiting;

  bool                            mSlideshow;
  Timer                           mViewTimer;
  bool                            mTimerReady;
  unsigned int                    mCentralLineIndex;

  Toolkit::PushButton             mPlayStopButton;
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
  Stage::GetCurrent().KeyEventSignal().Connect(this, &DissolveEffectApp::OnKeyEvent);

  // Creates a default view with a default tool bar, the view is added to the stage.
  mContent = DemoHelper::CreateView( application, mView,mToolBar, "", TOOLBAR_IMAGE, "" );

  // Add an effect-changing button on the right of the tool bar.
  mEffectChangeButton = Toolkit::PushButton::New();
  mEffectChangeButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, EFFECT_HIGHP_IMAGE );
  mEffectChangeButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, EFFECT_HIGHP_IMAGE_SELECTED );
  mEffectChangeButton.ClickedSignal().Connect( this, &DissolveEffectApp::OnEffectButtonClicked );
  mToolBar.AddControl( mEffectChangeButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

  // Add title to the tool bar.
  mTitleActor = DemoHelper::CreateToolBarLabel( APPLICATION_TITLE_HIGHP );
  mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Toolkit::Alignment::HorizontalCenter );

  // Add an slide-show button on the right of the title
  mPlayStopButton = Toolkit::PushButton::New();
  mPlayStopButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, PLAY_ICON );
  mPlayStopButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, PLAY_ICON_SELECTED );
  mPlayStopButton.ClickedSignal().Connect( this, &DissolveEffectApp::OnSildeshowButtonClicked );
  mToolBar.AddControl( mPlayStopButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalCenter, DemoHelper::DEFAULT_PLAY_PADDING );

  // use pan gesture to detect the cursor or finger movement
  mPanGestureDetector = PanGestureDetector::New();
  mPanGestureDetector.DetectedSignal().Connect( this, &DissolveEffectApp::OnPanGesture );

  mViewTimer = Timer::New( VIEWINGTIME );
  mViewTimer.TickSignal().Connect( this, &DissolveEffectApp::OnTimerTick );
  mTimerReady = true;

  // Set size to stage size to avoid seeing a black border on transition
  mParent = Actor::New();
  mParent.SetSize( Stage::GetCurrent().GetSize() );
  mParent.SetParentOrigin( ParentOrigin::CENTER );
  mContent.Add( mParent );

  // show the first image
  mCurrentImage = CreateStageFillingImageView( IMAGES[mIndex] );
  mCurrentImage.SetParentOrigin( ParentOrigin::CENTER );
  mCurrentImage.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  mCurrentImage.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );
  mParent.Add( mCurrentImage );

  mPanGestureDetector.Attach( mCurrentImage );

  mDissolveEffect = Dali::Toolkit::CreateDissolveEffect( mUseHighPrecision );
  Property::Map emptyShaderMap;
  mEmptyEffect.Insert( "shader", emptyShaderMap );
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

    mNextImage = CreateStageFillingImageView( IMAGES[ mIndex ] );
    mNextImage.SetParentOrigin( ParentOrigin::CENTER );
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

  Dali::Toolkit::DissolveEffectSetCentralLine( mCurrentImage, position, displacement, 0.0f );
  mCurrentImage.SetProperty( Toolkit::ImageView::Property::IMAGE, mDissolveEffect );
  mAnimation.AnimateTo( Property( mCurrentImage, "uPercentage" ), 1.0f, AlphaFunction::LINEAR );

  mNextImage.SetOpacity(0.0f);
  mAnimation.AnimateTo( Property( mNextImage, Actor::Property::COLOR_ALPHA ), 1.0f, AlphaFunction::LINEAR );

  if(mUseHighPrecision)
  {
    Dali::Toolkit::DissolveEffectSetCentralLine( mNextImage, position, displacement, 1.0f );
    mNextImage.SetProperty( Toolkit::ImageView::Property::IMAGE, mDissolveEffect );
    mAnimation.AnimateTo( Property( mNextImage, "uPercentage" ), 0.0f, AlphaFunction::LINEAR );
  }
  else
  {
    mAnimation.AnimateTo( Property( mNextImage, Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), AlphaFunction::LINEAR );
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
  mDissolveEffect = Dali::Toolkit::CreateDissolveEffect(mUseHighPrecision);
  if(mUseHighPrecision)
  {
    mTitleActor.SetProperty( TextLabel::Property::TEXT, std::string(APPLICATION_TITLE_HIGHP) );
    mEffectChangeButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, EFFECT_HIGHP_IMAGE );
    mEffectChangeButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, EFFECT_HIGHP_IMAGE_SELECTED );
  }
  else
  {
    mTitleActor.SetProperty( TextLabel::Property::TEXT, std::string(APPLICATION_TITLE_MEDIUMP) );
    mEffectChangeButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, EFFECT_MEDIUMP_IMAGE );
    mEffectChangeButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, EFFECT_MEDIUMP_IMAGE_SELECTED );
  }

  return true;
}

bool DissolveEffectApp::OnSildeshowButtonClicked( Toolkit::Button button )
{
  mSlideshow = !mSlideshow;
  if( mSlideshow )
  {
    mPlayStopButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, STOP_ICON );
    mPlayStopButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, STOP_ICON_SELECTED );
    mPanGestureDetector.Detach( mParent );
    mViewTimer.Start();
    mTimerReady = false;
  }
  else
  {
    mPlayStopButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, PLAY_ICON );
    mPlayStopButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, PLAY_ICON_SELECTED );
    mTimerReady = true;
    mPanGestureDetector.Attach( mParent );
  }
  return true;
}

void DissolveEffectApp::OnTransitionCompleted( Animation& source )
{
  if(mUseHighPrecision)
  {
    mNextImage.SetProperty( Toolkit::ImageView::Property::IMAGE, mEmptyEffect );
  }
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
    mNextImage = CreateStageFillingImageView( IMAGES[ mIndex ] );
    mNextImage.SetParentOrigin( ParentOrigin::CENTER );
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

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  DissolveEffectApp test( application );
  application.MainLoop();

  return 0;
}
