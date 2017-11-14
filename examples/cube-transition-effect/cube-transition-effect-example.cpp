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
#include "shared/utility.h"

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>
#include <dali-toolkit/devel-api/controls/buttons/toggle-button.h>
#include <dali-toolkit/devel-api/transition-effects/cube-transition-effect.h>
#include <dali-toolkit/devel-api/transition-effects/cube-transition-cross-effect.h>
#include <dali-toolkit/devel-api/transition-effects/cube-transition-fold-effect.h>
#include <dali-toolkit/devel-api/transition-effects/cube-transition-wave-effect.h>


using namespace Dali;

using Dali::Toolkit::TextLabel;

// LOCAL STUFF
namespace
{

const char * const TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char * const APPLICATION_TITLE_WAVE( "Cube Transition: Wave" );
const char * const APPLICATION_TITLE_CROSS( "Cube Transition: Cross" );
const char * const APPLICATION_TITLE_FOLD( "Cube Transition: Fold" );
const char * const EFFECT_WAVE_IMAGE( DEMO_IMAGE_DIR "icon-effect-wave.png" );
const char * const EFFECT_WAVE_IMAGE_SELECTED( DEMO_IMAGE_DIR "icon-effect-wave-selected.png" );
const char * const EFFECT_CROSS_IMAGE( DEMO_IMAGE_DIR "icon-effect-cross.png" );
const char * const EFFECT_CROSS_IMAGE_SELECTED( DEMO_IMAGE_DIR "icon-effect-cross-selected.png" );
const char * const EFFECT_FOLD_IMAGE( DEMO_IMAGE_DIR "icon-effect-fold.png" );
const char * const EFFECT_FOLD_IMAGE_SELECTED( DEMO_IMAGE_DIR "icon-effect-fold-selected.png" );
const char * const SLIDE_SHOW_START_ICON( DEMO_IMAGE_DIR "icon-play.png" );
const char * const SLIDE_SHOW_START_ICON_SELECTED( DEMO_IMAGE_DIR "icon-play-selected.png" );
const char * const SLIDE_SHOW_STOP_ICON( DEMO_IMAGE_DIR "icon-stop.png" );
const char * const SLIDE_SHOW_STOP_ICON_SELECTED( DEMO_IMAGE_DIR "icon-stop-selected.png" );

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
   * @param[in] texture The target Texture of the completed transition
   */
  void OnTransitionCompleted(Toolkit::CubeTransitionEffect effect, Texture image );
  /**
   * Callback function of timer tick
   * The timer is used to count the image display duration in slideshow,
   */
  bool OnTimerTick();

  /**
   * Loads image, resizes it to the size of stage and creates a textue out of it
   * @param[in] filepath Path to the image file
   * @return New texture object
   */
  Texture LoadStageFillingTexture( const char* filepath );

private:
  Application&                    mApplication;
  Toolkit::Control                mView;
  Toolkit::ToolBar                mToolBar;
  Layer                           mContent;
  Toolkit::TextLabel              mTitle;

  Vector2                         mViewSize;

  Texture                         mCurrentTexture;
  Texture                         mNextTexture;
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

  Vector2                         mPanPosition;
  Vector2                         mPanDisplacement;
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
  Stage::GetCurrent().KeyEventSignal().Connect(this, &CubeTransitionApp::OnKeyEvent);

  // Creates a default view with a default tool bar, the view is added to the stage.
  mContent = DemoHelper::CreateView( application, mView, mToolBar, "", TOOLBAR_IMAGE, "" );
  mContent.SetBehavior( Layer::LAYER_3D );


  // Add an effect changing toggle button
  Toolkit::ToggleButton effectChangeToggleButton = Toolkit::ToggleButton::ToggleButton::New();

  effectChangeToggleButton.SetProperty( Toolkit::ToggleButton::Property::STATE_VISUALS,
                                        Property::Array().Add( EFFECT_WAVE_IMAGE )
                                                         .Add( EFFECT_CROSS_IMAGE)
                                                         .Add( EFFECT_FOLD_IMAGE )
                                      );

  effectChangeToggleButton.ClickedSignal().Connect( this, &CubeTransitionApp::OnEffectButtonClicked );
  mToolBar.AddControl( effectChangeToggleButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

  // Add title to the tool bar.
  mTitle = DemoHelper::CreateToolBarLabel( APPLICATION_TITLE_WAVE );
  mToolBar.AddControl( mTitle, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Toolkit::Alignment::HorizontalCenter );

  //Add an slideshow icon on the right of the title
  mSlideshowButton = Toolkit::PushButton::New();
  mSlideshowButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, SLIDE_SHOW_START_ICON );
  mSlideshowButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, SLIDE_SHOW_START_ICON_SELECTED );
  mSlideshowButton.ClickedSignal().Connect( this, &CubeTransitionApp::OnSildeshowButtonClicked );
  mToolBar.AddControl( mSlideshowButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalCenter, DemoHelper::DEFAULT_PLAY_PADDING );

  // Set size to stage size to avoid seeing a black border on transition
  mViewSize = Stage::GetCurrent().GetSize();

  // show the first image
  mCurrentTexture = LoadStageFillingTexture( IMAGES[mIndex] );

  //use small cubes
  mCubeWaveEffect = Toolkit::CubeTransitionWaveEffect::New( NUM_ROWS_WAVE, NUM_COLUMNS_WAVE );
  mCubeWaveEffect.SetTransitionDuration( ANIMATION_DURATION_WAVE );
  mCubeWaveEffect.SetCubeDisplacement( CUBE_DISPLACEMENT_WAVE );
  mCubeWaveEffect.TransitionCompletedSignal().Connect(this, &CubeTransitionApp::OnTransitionCompleted);

  mCubeWaveEffect.SetSize( mViewSize );
  mCubeWaveEffect.SetParentOrigin( ParentOrigin::CENTER );
  mCubeWaveEffect.SetCurrentTexture( mCurrentTexture );

  // use big cubes
  mCubeCrossEffect = Toolkit::CubeTransitionCrossEffect::New(NUM_ROWS_CROSS, NUM_COLUMNS_CROSS );
  mCubeCrossEffect.SetTransitionDuration( ANIMATION_DURATION_CROSS );
  mCubeCrossEffect.SetCubeDisplacement( CUBE_DISPLACEMENT_CROSS );
  mCubeCrossEffect.TransitionCompletedSignal().Connect(this, &CubeTransitionApp::OnTransitionCompleted);

  mCubeCrossEffect.SetSize( mViewSize );
  mCubeCrossEffect.SetParentOrigin( ParentOrigin::CENTER );
  mCubeCrossEffect.SetCurrentTexture( mCurrentTexture );

  mCubeFoldEffect = Toolkit::CubeTransitionFoldEffect::New( NUM_ROWS_FOLD, NUM_COLUMNS_FOLD );
  mCubeFoldEffect.SetTransitionDuration( ANIMATION_DURATION_FOLD );
  mCubeFoldEffect.TransitionCompletedSignal().Connect(this, &CubeTransitionApp::OnTransitionCompleted);

  mCubeFoldEffect.SetSize( mViewSize );
  mCubeFoldEffect.SetParentOrigin( ParentOrigin::CENTER );
  mCubeFoldEffect.SetCurrentTexture( mCurrentTexture );

  mViewTimer = Timer::New( VIEWINGTIME );
  mViewTimer.TickSignal().Connect( this, &CubeTransitionApp::OnTimerTick );


  mCurrentEffect = mCubeWaveEffect;
  mContent.Add( mCurrentEffect );

  // use pan gesture to detect the cursor or finger movement
  mPanGestureDetector = PanGestureDetector::New();
  mPanGestureDetector.DetectedSignal().Connect( this, &CubeTransitionApp::OnPanGesture );
  mPanGestureDetector.Attach( mContent );
}

// signal handler, called when the pan gesture is detected
void CubeTransitionApp::OnPanGesture( Actor actor, const PanGesture& gesture )
{
  // does not response when the transition has not finished
  if( mIsImageLoading || mCubeWaveEffect.IsTransitioning() || mCubeCrossEffect.IsTransitioning() || mCubeFoldEffect.IsTransitioning() || mSlideshow )
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
  mNextTexture = LoadStageFillingTexture( IMAGES[ mIndex ] );
  mCurrentEffect.SetTargetTexture( mNextTexture );
  mIsImageLoading = false;
  mCurrentEffect.StartTransition( mPanPosition, mPanDisplacement );
  mCurrentTexture = mNextTexture;
}

bool CubeTransitionApp::OnEffectButtonClicked( Toolkit::Button button )
{
  mContent.Remove( mCurrentEffect );
  if(mCurrentEffect == mCubeWaveEffect)
  {
    mCurrentEffect = mCubeCrossEffect;
    mTitle.SetProperty( TextLabel::Property::TEXT, std::string(APPLICATION_TITLE_CROSS) );
  }
  else if(mCurrentEffect == mCubeCrossEffect)
  {
    mCurrentEffect = mCubeFoldEffect;
    mTitle.SetProperty( TextLabel::Property::TEXT, std::string(APPLICATION_TITLE_FOLD) );
  }
  else
  {
    mCurrentEffect = mCubeWaveEffect;
    mTitle.SetProperty( TextLabel::Property::TEXT, std::string(APPLICATION_TITLE_WAVE) );
  }
  mContent.Add( mCurrentEffect );

  // Set the current image to cube transition effect
  // only need to set at beginning or change from another effect
  mCurrentEffect.SetCurrentTexture( mCurrentTexture );
  return true;
}

bool CubeTransitionApp::OnSildeshowButtonClicked( Toolkit::Button button )
{
  mSlideshow = !mSlideshow;
  if( mSlideshow )
  {
    mPanGestureDetector.Detach( mContent );
    mSlideshowButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, SLIDE_SHOW_STOP_ICON );
    mSlideshowButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, SLIDE_SHOW_STOP_ICON_SELECTED );
    mPanPosition = Vector2( mViewSize.width, mViewSize.height*0.5f );
    mPanDisplacement = Vector2( -10.f, 0.f );
    mViewTimer.Start();
  }
  else
  {
    mPanGestureDetector.Attach( mContent );
    mSlideshowButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, SLIDE_SHOW_START_ICON );
    mSlideshowButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, SLIDE_SHOW_START_ICON_SELECTED );
    mViewTimer.Stop();
  }
  return true;
}

void CubeTransitionApp::OnTransitionCompleted(Toolkit::CubeTransitionEffect effect, Texture texture )
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

Texture CubeTransitionApp::LoadStageFillingTexture( const char* filepath )
{
  ImageDimensions dimensions( Stage::GetCurrent().GetSize().x, Stage::GetCurrent().GetSize().y );
  Devel::PixelBuffer pixelBuffer = LoadImageFromFile( filepath, dimensions, FittingMode::SCALE_TO_FILL );
  PixelData pixelData = Devel::PixelBuffer::Convert(pixelBuffer);

  Texture texture = Texture::New( TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight() );
  texture.Upload( pixelData );
  return texture;
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

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  CubeTransitionApp test( application );
  application.MainLoop();

  return 0;
}
