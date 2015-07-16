/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include "shared/view.h"
#include "radial-sweep-view.h"
#include "radial-sweep-view-impl.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TEST_OUTER_RING_FILENAME = DALI_IMAGE_DIR "layer2.png"; // Image to be masked
const char* TEST_INNER_RING_FILENAME = DALI_IMAGE_DIR "layer1.png"; // Image to be masked
const char* TEST_MENU_FILENAME = DALI_IMAGE_DIR "layer3.png"; // Image to be masked
const char* TEST_DIAL_FILENAME = DALI_IMAGE_DIR "layer4.png"; // Image to be masked
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" ); // Background for toolbar
const char* APPLICATION_TITLE( "Radial Menu" );
const char * const PLAY_ICON( DALI_IMAGE_DIR "icon-play.png" );
const char * const PLAY_ICON_SELECTED( DALI_IMAGE_DIR "icon-play-selected.png" );
const char * const STOP_ICON( DALI_IMAGE_DIR "icon-stop.png" );
const char * const STOP_ICON_SELECTED( DALI_IMAGE_DIR "icon-stop-selected.png" );
}


/********************************************************************************
 * Application controller class
 */

// This example shows how to create a mesh actor for use as a stencil buffer
class RadialMenuExample : public ConnectionTracker
{
public:
  /**
   * Constructor
   * @param[in] app The application handle
   */
  RadialMenuExample(Application app);

  /**
   * Destructor
   */
  ~RadialMenuExample();

private:

  /**
   * Initialization signal handler - all actor initialization should happen here
   * @param[in] app The application handle
   */
  void OnInit(Application& app);

  /**
   * Create a sweep view with the given image and parameters
   */
  RadialSweepView CreateSweepView( std::string imageName, Degree initial, Degree final );

  /**
   * Start the sweep animation on the menu
   */
  void StartAnimation();

  /**
   * Play or pause the animation when the button is clicked
   */
  bool OnButtonClicked( Toolkit::Button button );

  /**
   * Update the state flag and change the button icon when the animation is finished
   */
  void OnAnimationFinished( Animation& source );

  /**
   * Main key event handler
   *
   * @param[in] event The key event to respond to
   */
  void OnKeyEvent(const KeyEvent& event);

private: // Member variables
  enum AnimState
  {
    STOPPED,
    PAUSED,
    PLAYING
  };

  Application     mApplication; ///< The application handle
  Toolkit::Control mView;        ///< The toolbar view
  Layer           mContents;    ///< The toolbar contents pane
  ImageView       mImageView;  ///< Image view shown by stencil mask
  Animation       mAnimation;
  AnimState       mAnimationState;

  Toolkit::PushButton mPlayStopButton;
  ImageView       mDialView;
  RadialSweepView mRadialSweepView1;
  RadialSweepView mRadialSweepView2;
  RadialSweepView mRadialSweepView3;
};

RadialMenuExample::RadialMenuExample(Application app)
: mApplication( app ),
  mAnimationState(STOPPED)
{
  // Connect to the Application's Init signal
  app.InitSignal().Connect(this, &RadialMenuExample::OnInit);
}

RadialMenuExample::~RadialMenuExample()
{
  // Nothing to do here; actor handles will clean up themselves.
}

void RadialMenuExample::OnInit(Application& app)
{
  Stage stage = Dali::Stage::GetCurrent();

  // The Init signal is received once (only) during the Application lifetime
  stage.KeyEventSignal().Connect(this, &RadialMenuExample::OnKeyEvent);

  // Create toolbar & view
  Toolkit::ToolBar toolBar;
  mContents = DemoHelper::CreateView( mApplication,
                                      mView,
                                      toolBar,
                                      "",
                                      TOOLBAR_IMAGE,
                                      APPLICATION_TITLE );

  mPlayStopButton = Toolkit::PushButton::New();
  mPlayStopButton.SetUnselectedImage( STOP_ICON );
  mPlayStopButton.SetSelectedImage( STOP_ICON_SELECTED );

  mPlayStopButton.ClickedSignal().Connect( this, &RadialMenuExample::OnButtonClicked );

  toolBar.AddControl( mPlayStopButton,
                      DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage,
                      Toolkit::Alignment::HorizontalRight,
                      DemoHelper::DEFAULT_PLAY_PADDING );


  const ImageDimensions intImgSize = ResourceImage::GetImageSize(TEST_OUTER_RING_FILENAME);
  Vector2 imgSize = Vector2( intImgSize.GetWidth(), intImgSize.GetHeight() );
  Vector2 stageSize = stage.GetSize();
  float scale = stageSize.width / imgSize.width;
  float availableHeight = stageSize.height - DemoHelper::DEFAULT_VIEW_STYLE.mToolBarHeight * 2.0f;
  if(availableHeight <= stageSize.width)
  {
    scale = availableHeight / imgSize.width;
  }

  mRadialSweepView1 = CreateSweepView( TEST_OUTER_RING_FILENAME, Degree(-90.0f), Degree(-90.0f));
  mRadialSweepView2 = CreateSweepView( TEST_INNER_RING_FILENAME, Degree(90.0f),  Degree(0.0f));
  mRadialSweepView3 = CreateSweepView( TEST_MENU_FILENAME, Degree(100.0f), Degree(0.0f));
  mRadialSweepView3.SetInitialActorAngle(Degree(-110));
  mRadialSweepView3.SetFinalActorAngle(Degree(0));

  Image dial = ResourceImage::New( TEST_DIAL_FILENAME );
  mDialView = ImageView::New( dial );
  mDialView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  mDialView.SetPositionInheritanceMode(USE_PARENT_POSITION);
  mDialView.SetScale(scale);
  Layer dialLayer = Layer::New();

  dialLayer.Add( mDialView );
  dialLayer.SetPositionInheritanceMode(USE_PARENT_POSITION);
  dialLayer.SetSize(stage.GetSize());
  mContents.Add(dialLayer);

  mRadialSweepView1.SetScale(scale);
  mRadialSweepView2.SetScale(scale);
  mRadialSweepView3.SetScale(scale);

  StartAnimation();
}

void RadialMenuExample::StartAnimation()
{
  mDialView.SetOpacity(0.0f);
  mRadialSweepView1.SetOpacity(0.0f);
  mAnimation = Animation::New(6.0f);
  mRadialSweepView1.Activate(mAnimation, 0.0f, 3.0f);
  mRadialSweepView2.Activate(mAnimation, 1.5f, 3.0f);
  mRadialSweepView3.Activate(mAnimation, 3.0f, 3.0f);
  mAnimation.AnimateTo( Property( mDialView, Actor::Property::COLOR_ALPHA ), 1.0f, AlphaFunction::EASE_IN, TimePeriod( 0.0f, 0.8f ) );
  mAnimation.AnimateTo( Property( mRadialSweepView1, Actor::Property::COLOR_ALPHA ), 1.0f, AlphaFunction::EASE_IN, TimePeriod( 0.0f, 0.5f ) );
  mAnimation.FinishedSignal().Connect( this, &RadialMenuExample::OnAnimationFinished );

  mAnimationState = PLAYING;
  mAnimation.Play();
}

bool RadialMenuExample::OnButtonClicked( Toolkit::Button button )
{
  switch( mAnimationState )
  {
    case PLAYING:
    {
      mAnimation.Pause();
      mAnimationState = PAUSED;
      mPlayStopButton.SetUnselectedImage( PLAY_ICON );
      mPlayStopButton.SetSelectedImage( PLAY_ICON_SELECTED );
    }
    break;

    case PAUSED:
    {
      mAnimation.Play();
      mAnimationState = PLAYING;
      mPlayStopButton.SetUnselectedImage( STOP_ICON );
      mPlayStopButton.SetSelectedImage( STOP_ICON_SELECTED );
    }
    break;

    case STOPPED:
    {
      mPlayStopButton.SetUnselectedImage( STOP_ICON );
      mPlayStopButton.SetSelectedImage( STOP_ICON_SELECTED );
      mRadialSweepView1.Deactivate();
      mRadialSweepView2.Deactivate();
      mRadialSweepView3.Deactivate();
      StartAnimation();
    }
  }
  return false;
}

void RadialMenuExample::OnAnimationFinished( Animation& source )
{
  mAnimationState = STOPPED;
  mPlayStopButton.SetUnselectedImage( PLAY_ICON );
  mPlayStopButton.SetSelectedImage( PLAY_ICON_SELECTED );
}

RadialSweepView RadialMenuExample::CreateSweepView( std::string imageName,
                                                    Degree initialAngle,
                                                    Degree finalAngle)
{
  // Create the image
  Image image = ResourceImage::New(imageName);
  mImageView = ImageView::New(image);
  mImageView.SetParentOrigin(ParentOrigin::CENTER);
  mImageView.SetAnchorPoint(AnchorPoint::CENTER);
  mImageView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

  // Create the stencil
  const ImageDimensions imageSize = ResourceImage::GetImageSize(imageName);
  float diameter = std::max(imageSize.GetWidth(), imageSize.GetHeight());
  RadialSweepView radialSweepView = RadialSweepView::New();
  radialSweepView.SetDiameter( diameter );
  radialSweepView.SetInitialAngle( initialAngle );
  radialSweepView.SetFinalAngle( finalAngle );
  radialSweepView.SetInitialSector( Degree(0.0f) );
  radialSweepView.SetFinalSector( Degree(359.999f) );
  radialSweepView.SetSize( Stage::GetCurrent().GetSize());
  radialSweepView.SetEasingFunction( Dali::AlphaFunction::EASE_IN_OUT );
  radialSweepView.SetPositionInheritanceMode(USE_PARENT_POSITION);
  mContents.Add(radialSweepView);
  radialSweepView.Add( mImageView );
  mImageView.SetPositionInheritanceMode(USE_PARENT_POSITION);

  return radialSweepView;
}


void RadialMenuExample::OnKeyEvent(const KeyEvent& event)
{
  if(event.state == KeyEvent::Down)
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
      mApplication.Quit();
    }
  }
}

void RunTest(Application app)
{
  RadialMenuExample test(app);

  app.MainLoop();
}

// Entry point for Linux & Tizen applications
int main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv, DALI_DEMO_THEME_PATH);

  RunTest(app);

  return 0;
}
