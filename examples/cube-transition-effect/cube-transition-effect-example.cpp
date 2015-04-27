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
const char * const APPLICATION_TITLE_WAVE( "Cube Transition: Wave" );
const char * const APPLICATION_TITLE_CROSS( "Cube Transition: Cross" );
const char * const APPLICATION_TITLE_FOLD( "Cube Transition: Fold" );
const char * const EFFECT_WAVE_IMAGE( DALI_IMAGE_DIR "icon-effect-wave.png" );
const char * const EFFECT_CROSS_IMAGE( DALI_IMAGE_DIR "icon-effect-cross.png" );
const char * const EFFECT_FOLD_IMAGE( DALI_IMAGE_DIR "icon-effect-fold.png" );
const char * const SLIDE_SHOW_START_ICON( DALI_IMAGE_DIR "icon-play.png" );
const char * const SLIDE_SHOW_STOP_ICON( DALI_IMAGE_DIR "icon-stop.png" );

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

// the number of cubes: NUM_COLUMNS*NUM_ROWS
// better choose the numbers that can divide viewAreaSize.x
const int NUM_COLUMNS_WAVE(16);
const int NUM_COLUMNS_CROSS(8);
const int NUM_COLUMNS_FOLD(8);
// better choose the numbers that can divide viewAreaSize.y
const int NUM_ROWS_WAVE(20);
const int NUM_ROWS_CROSS(10);
const int NUM_ROWS_FOLD(10);
//transition effect duration
const float ANIMATION_DURATION_WAVE(1.5f);
const float ANIMATION_DURATION_CROSS(1.f);
const float ANIMATION_DURATION_FOLD(1.f);
//transition effect displacement
const float CUBE_DISPLACEMENT_WAVE(70.f);
const float CUBE_DISPLACEMENT_CROSS(30.f);

// The duration of the current image staying on screen when slideshow is on
const int VIEWINGTIME = 2000; // 2 seconds

/**
 * @brief Load an image, scaled-down to no more than the stage dimensions.
 *
 * Uses image scaling mode SCALE_TO_FILL to resize the image at
 * load time to cover the entire stage with pixels with no borders,
 * and filter mode BOX_THEN_LINEAR to sample the image with
 * maximum quality.
 */
ResourceImage LoadStageFillingImage( const char * const imagePath )
{
  Size stageSize = Stage::GetCurrent().GetSize();
  return ResourceImage::New( imagePath, ImageDimensions( stageSize.x, stageSize.y ), Dali::FittingMode::SCALE_TO_FILL, Dali::SamplingMode::BOX_THEN_LINEAR );
}

} // namespace

class CubeTransitionApp : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  CubeTransitionApp( Application& application );

  ~CubeTransitionApp();

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
   * Load the next image and start the transition;
   */
  void GoToNextImage();
  /**
   * Callback function of image resource loading succeed
   * Start the transition
   * @param[in] image The image content of the imageActor for transition
   */
  void OnImageLoaded(ResourceImage image);
  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event);
  /**
   * Callback function of effect-switch button
   * Change the effect when the effect button is clicked
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
  void OnTransitionCompleted(Toolkit::CubeTransitionEffect effect, ImageActor imageActor);
  /**
   * Callback function of timer tick
   * The timer is used to count the image display duration in slideshow,
   */
  bool OnTimerTick();

private:
  Application&                    mApplication;
  Toolkit::Control                mView;
  Toolkit::ToolBar                mToolBar;
  Layer                           mContent;
  Toolkit::TextLabel              mTitleActor;
  Actor                           mParent;

  Vector2                         mViewSize;

  ImageActor                      mCurrentImage;
  ImageActor                      mNextImage;
  unsigned int                    mIndex;
  bool                            mIsImageLoading;

  PanGestureDetector              mPanGestureDetector;

  Toolkit::CubeTransitionEffect   mCubeWaveEffect;
  Toolkit::CubeTransitionEffect   mCubeCrossEffect;
  Toolkit::CubeTransitionEffect   mCubeFoldEffect;
  Toolkit::CubeTransitionEffect   mCurrentEffect;

  bool                            mSlideshow;
  Timer                           mViewTimer;
  Toolkit::PushButton             mSlideshowButton;
  Image                           mIconSlideshowStart;
  Image                           mIconSlideshowStop;

  Vector2                         mPanPosition;
  Vector2                         mPanDisplacement;

  Image                           mImageWave;
  Image                           mImageCross;
  Image                           mImageFold;
  Toolkit::PushButton             mEffectChangeButton;
};

CubeTransitionApp::CubeTransitionApp( Application& application )
: mApplication( application ),
  mIndex( 0 ),
  mIsImageLoading( false ),
  mSlideshow( false )
{
  mApplication.InitSignal().Connect( this, &CubeTransitionApp::OnInit );
}

CubeTransitionApp::~CubeTransitionApp()
{
  //Nothing to do
}

void CubeTransitionApp::OnInit( Application& application )
{
  DemoHelper::RequestThemeChange();

  Stage::GetCurrent().KeyEventSignal().Connect(this, &CubeTransitionApp::OnKeyEvent);

  // Creates a default view with a default tool bar, the view is added to the stage.
  mContent = DemoHelper::CreateView( application, mView, mToolBar, "", TOOLBAR_IMAGE, "" );

  // Add an effect-changing button on the right of the tool bar.
  mImageWave = ResourceImage::New( EFFECT_WAVE_IMAGE );
  mImageCross = ResourceImage::New( EFFECT_CROSS_IMAGE );
  mImageFold = ResourceImage::New( EFFECT_FOLD_IMAGE );
  mEffectChangeButton = Toolkit::PushButton::New();
  mEffectChangeButton.SetBackgroundImage(mImageWave);
  mEffectChangeButton.ClickedSignal().Connect( this, &CubeTransitionApp::OnEffectButtonClicked );
  mToolBar.AddControl( mEffectChangeButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

  // Add title to the tool bar.
  mTitleActor = DemoHelper::CreateToolBarLabel( APPLICATION_TITLE_WAVE );
  mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Toolkit::Alignment::HorizontalCenter );

  //Add an slideshow icon on the right of the title
  mIconSlideshowStart = ResourceImage::New( SLIDE_SHOW_START_ICON );
  mIconSlideshowStop = ResourceImage::New( SLIDE_SHOW_STOP_ICON );
  mSlideshowButton = Toolkit::PushButton::New();
  mSlideshowButton.SetBackgroundImage( mIconSlideshowStart );
  mSlideshowButton.ClickedSignal().Connect( this, &CubeTransitionApp::OnSildeshowButtonClicked );
  mToolBar.AddControl( mSlideshowButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalCenter, DemoHelper::DEFAULT_PLAY_PADDING );

  // Set size to stage size to avoid seeing a black border on transition
  mViewSize = Stage::GetCurrent().GetSize();

  mParent = Actor::New();
  mParent.SetSize( mViewSize );
  mParent.SetPositionInheritanceMode( USE_PARENT_POSITION );
  mContent.Add( mParent );

  // use pan gesture to detect the cursor or finger movement
  mPanGestureDetector = PanGestureDetector::New();
  mPanGestureDetector.DetectedSignal().Connect( this, &CubeTransitionApp::OnPanGesture );
  mPanGestureDetector.Attach( mParent );

  //use small cubes
  mCubeWaveEffect = Toolkit::CubeTransitionWaveEffect::New(NUM_ROWS_WAVE, NUM_COLUMNS_WAVE, mViewSize);
  mCubeWaveEffect.SetTransitionDuration( ANIMATION_DURATION_WAVE );
  mCubeWaveEffect.SetCubeDisplacement( CUBE_DISPLACEMENT_WAVE );
  mCubeWaveEffect.TransitionCompletedSignal().Connect(this, &CubeTransitionApp::OnTransitionCompleted);
  mParent.Add(mCubeWaveEffect.GetRoot());
  // use big cubes
  mCubeCrossEffect = Toolkit::CubeTransitionCrossEffect::New(NUM_ROWS_CROSS, NUM_COLUMNS_CROSS, mViewSize);
  mCubeCrossEffect.SetTransitionDuration( ANIMATION_DURATION_CROSS);
  mCubeCrossEffect.SetCubeDisplacement( CUBE_DISPLACEMENT_CROSS );
  mCubeCrossEffect.TransitionCompletedSignal().Connect(this, &CubeTransitionApp::OnTransitionCompleted);
  mParent.Add(mCubeCrossEffect.GetRoot());

  mCubeFoldEffect = Toolkit::CubeTransitionFoldEffect::New(NUM_ROWS_FOLD, NUM_COLUMNS_FOLD, mViewSize);
  mCubeFoldEffect.SetTransitionDuration( ANIMATION_DURATION_FOLD);
  mCubeFoldEffect.TransitionCompletedSignal().Connect(this, &CubeTransitionApp::OnTransitionCompleted);
  mParent.Add(mCubeFoldEffect.GetRoot());

  mViewTimer = Timer::New( VIEWINGTIME );
  mViewTimer.TickSignal().Connect( this, &CubeTransitionApp::OnTimerTick );

  // show the first image
  mCurrentImage = ImageActor::New( LoadStageFillingImage( IMAGES[mIndex] ) );
  mCurrentImage.SetPositionInheritanceMode( USE_PARENT_POSITION );
  mCurrentImage.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  mCurrentImage.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );
  mParent.Add( mCurrentImage );

  mCurrentEffect = mCubeWaveEffect;
  mCurrentEffect.SetCurrentImage( mCurrentImage );
}

// signal handler, called when the pan gesture is detected
void CubeTransitionApp::OnPanGesture( Actor actor, const PanGesture& gesture )
{
  // does not response when the transition has not finished
  if( mIsImageLoading || mCubeWaveEffect.IsTransiting() || mCubeCrossEffect.IsTransiting() || mCubeFoldEffect.IsTransiting() || mSlideshow )
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

    mPanPosition = gesture.position;
    mPanDisplacement = gesture.displacement;
    GoToNextImage();
  }
}

void CubeTransitionApp::GoToNextImage()
{
  ResourceImage image = LoadStageFillingImage( IMAGES[ mIndex ] );
  mNextImage = ImageActor::New( image );

  mNextImage.SetPositionInheritanceMode(USE_PARENT_POSITION);
  mNextImage.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  mNextImage.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );
  mCurrentEffect.SetTargetImage(mNextImage);
  if( image.GetLoadingState() == ResourceLoadingSucceeded )
  {
    mIsImageLoading = false;
    OnImageLoaded( image );
  }
  else
  {
    mIsImageLoading = true;
    image.LoadingFinishedSignal().Connect( this, &CubeTransitionApp::OnImageLoaded );
  }
}

void CubeTransitionApp::OnImageLoaded(ResourceImage image)
{
   mIsImageLoading = false;
   mCurrentEffect.StartTransition( mPanPosition, mPanDisplacement );
   mParent.Remove(mCurrentImage);
   mParent.Add(mNextImage);
   mCurrentImage = mNextImage;
}

bool CubeTransitionApp::OnEffectButtonClicked( Toolkit::Button button )
{
  if(mCurrentEffect == mCubeWaveEffect)
  {
    mCurrentEffect = mCubeCrossEffect;
    mTitleActor.SetProperty( TextLabel::Property::TEXT, std::string(APPLICATION_TITLE_CROSS) );
    mEffectChangeButton.SetBackgroundImage(mImageCross);

  }
  else if(mCurrentEffect == mCubeCrossEffect)
  {
    mCurrentEffect = mCubeFoldEffect;
    mTitleActor.SetProperty( TextLabel::Property::TEXT, std::string(APPLICATION_TITLE_FOLD) );
    mEffectChangeButton.SetBackgroundImage(mImageFold);
  }
  else
  {
    mCurrentEffect = mCubeWaveEffect;
    mTitleActor.SetProperty( TextLabel::Property::TEXT, std::string(APPLICATION_TITLE_WAVE) );
    mEffectChangeButton.SetBackgroundImage(mImageWave);
  }

  // Set the current image to cube transition effect
  // only need to set at beginning or change from another effect
  mCurrentEffect.SetCurrentImage(mCurrentImage);
  return true;
}

bool CubeTransitionApp::OnSildeshowButtonClicked( Toolkit::Button button )
{
  mSlideshow = !mSlideshow;
  if( mSlideshow )
  {
    mPanGestureDetector.Detach( mParent );
    mSlideshowButton.SetBackgroundImage( mIconSlideshowStop );
    mPanPosition = Vector2( mViewSize.width, mViewSize.height*0.5f );
    mPanDisplacement = Vector2( -10.f, 0.f );
    mViewTimer.Start();
  }
  else
  {
    mPanGestureDetector.Attach( mParent );
    mSlideshowButton.SetBackgroundImage( mIconSlideshowStart );
    mViewTimer.Stop();
  }
  return true;
}

void CubeTransitionApp::OnTransitionCompleted(Toolkit::CubeTransitionEffect effect, ImageActor imageActor)
{
  if( mSlideshow )
  {
    mViewTimer.Start();
  }
}

bool CubeTransitionApp::OnTimerTick()
{
  if(mSlideshow)
  {
    mIndex = (mIndex + 1)%NUM_IMAGES;
    GoToNextImage();
  }

  //return false to stop the timer
  return false;
}

void CubeTransitionApp::OnKeyEvent(const KeyEvent& event)
{
  if(event.state == KeyEvent::Down)
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
      mApplication.Quit();
    }
  }
}

// Entry point for Linux & Tizen applications
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  CubeTransitionApp test( application );
  application.MainLoop();

  return 0;
}
