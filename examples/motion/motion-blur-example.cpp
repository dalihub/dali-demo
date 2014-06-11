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

#include <sstream>
#include <iomanip>

#include "../shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;



namespace // unnamed namespace
{

////////////////////////////////////////////////////
//
// Demo setup parameters
//

//#define MULTIPLE_MOTION_BLURRED_ACTORS
#ifndef MULTIPLE_MOTION_BLURRED_ACTORS

const float MOTION_BLUR_ACTOR_WIDTH = 256;                                          // actor size on screen
const float MOTION_BLUR_ACTOR_HEIGHT = 256;                                         // ""

#else //#ifndef MULTIPLE_MOTION_BLURRED_ACTORS

const float MOTION_BLUR_ACTOR_WIDTH = 150;                                          // actor size on screen
const float MOTION_BLUR_ACTOR_HEIGHT = 112;                                         // ""

#endif //#ifndef MULTIPLE_MOTION_BLURRED_ACTORS


const unsigned int MOTION_BLUR_NUM_SAMPLES = 8;

const int MOTION_BLUR_NUM_ACTOR_IMAGES = 5;
const char* MOTION_BLUR_ACTOR_IMAGE1( DALI_IMAGE_DIR "image-with-border-1.jpg" );
const char* MOTION_BLUR_ACTOR_IMAGE2( DALI_IMAGE_DIR "image-with-border-2.jpg" );
const char* MOTION_BLUR_ACTOR_IMAGE3( DALI_IMAGE_DIR "image-with-border-3.jpg" );
const char* MOTION_BLUR_ACTOR_IMAGE4( DALI_IMAGE_DIR "image-with-border-4.jpg" );
const char* MOTION_BLUR_ACTOR_IMAGE5( DALI_IMAGE_DIR "image-with-border-1.jpg" );

const char* MOTION_BLUR_ACTOR_IMAGES[] = {
  MOTION_BLUR_ACTOR_IMAGE1,
  MOTION_BLUR_ACTOR_IMAGE2,
  MOTION_BLUR_ACTOR_IMAGE3,
  MOTION_BLUR_ACTOR_IMAGE4,
  MOTION_BLUR_ACTOR_IMAGE5,
};

const int NUM_ACTOR_ANIMATIONS = 4;
const int NUM_CAMERA_ANIMATIONS = 2;


const char* BACKGROUND_IMAGE_PATH = DALI_IMAGE_DIR "background-default.png";

const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* LAYOUT_IMAGE( DALI_IMAGE_DIR "icon-change.png" );
const char* APPLICATION_TITLE( "Motion Blur" );
const char* EFFECTS_OFF_ICON( DALI_IMAGE_DIR "icon-effects-off.png" );
const char* EFFECTS_ON_ICON( DALI_IMAGE_DIR "icon-effects-on.png" );

const float UI_MARGIN = 4.0f;                              ///< Screen Margin for placement of UI buttons

const Vector3 BUTTON_SIZE_CONSTRAINT( 0.24f, 0.09f, 1.0f );
const Vector3 BUTTON_TITLE_LABEL_TAP_HERE_SIZE_CONSTRAINT( 0.55f, 0.06f, 1.0f );
const Vector3 BUTTON_TITLE_LABEL_INSTRUCTIONS_POPUP_SIZE_CONSTRAINT( 1.0f, 1.0f, 1.0f );

// move this button down a bit so it is visible on target and not covered up by toolbar
const float BUTTON_TITLE_LABEL_Y_OFFSET = 0.05f;

const float ORIENTATION_DURATION = 0.5f;                  ///< Time to rotate to new orientation.
} // unnamed namespace




//
class MotionBlurExampleApp : public ConnectionTracker
{
public:

  /**
     * DeviceOrientation describes the four different
     * orientations the device can be in based on accelerometer reports.
     */
  enum DeviceOrientation
  {
    PORTRAIT = 0,
    LANDSCAPE = 90,
    PORTRAIT_INVERSE = 180,
    LANDSCAPE_INVERSE = 270
  };

  /**
   * Constructor
   * @param application class, stored as reference
   */
  MotionBlurExampleApp(Application &app)
  : mApplication(app),
    mActorEffectsEnabled(false),
    mCurrentActorAnimation(0),
    mCurrentImage(0)
  {
    // Connect to the Application's Init signal
    app.InitSignal().Connect(this, &MotionBlurExampleApp::OnInit);
  }

  ~MotionBlurExampleApp()
  {
    // Nothing to do here; everything gets deleted automatically
  }

  /**
   * This method gets called once the main loop of application is up and running
   */
  void OnInit(Application& app)
  {
    // The Init signal is received once (only) during the Application lifetime

    Stage::GetCurrent().KeyEventSignal().Connect(this, &MotionBlurExampleApp::OnKeyEvent);


    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( mApplication,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE_PATH,
                                            TOOLBAR_IMAGE,
                                            APPLICATION_TITLE);

    //Add an effects icon on the right of the title
    mIconEffectsOff = Image::New( EFFECTS_OFF_ICON );
    mIconEffectsOn = Image::New( EFFECTS_ON_ICON );
    mActorEffectsButton = Toolkit::PushButton::New();
    mActorEffectsButton.SetBackgroundImage( mIconEffectsOff );
    mActorEffectsButton.ClickedSignal().Connect( this, &MotionBlurExampleApp::OnEffectButtonClicked );
    mToolBar.AddControl( mActorEffectsButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalCenter, DemoHelper::DEFAULT_PLAY_PADDING );

    // Creates a mode button.
    // Create a effect toggle button. (right of toolbar)
    Image imageLayout = Image::New( LAYOUT_IMAGE );
    Toolkit::PushButton layoutButton = Toolkit::PushButton::New();
    layoutButton.SetBackgroundImage(imageLayout);
    layoutButton.ClickedSignal().Connect( this, &MotionBlurExampleApp::OnLayoutButtonClicked);
    layoutButton.SetLeaveRequired( true );
    mToolBar.AddControl( layoutButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

    // Input
    mTapGestureDetector = TapGestureDetector::New();
    mTapGestureDetector.Attach( mContentLayer );
    mTapGestureDetector.DetectedSignal().Connect( this, &MotionBlurExampleApp::OnTap );

    Dali::Window winHandle = app.GetWindow();
    winHandle.AddAvailableOrientation( Dali::Window::PORTRAIT );
    winHandle.AddAvailableOrientation( Dali::Window::LANDSCAPE );
    winHandle.AddAvailableOrientation( Dali::Window::PORTRAIT_INVERSE  );
    winHandle.AddAvailableOrientation( Dali::Window::LANDSCAPE_INVERSE );

    // set initial orientation
    app.GetOrientation().ChangedSignal().Connect( this, &MotionBlurExampleApp::OnOrientationChanged );
    unsigned int degrees = app.GetOrientation().GetDegrees();
    Rotate( static_cast< DeviceOrientation >( degrees ) );


    ///////////////////////////////////////////////////////
    //
    // Motion blurred actor
    //

    Image image = Image::New( MOTION_BLUR_ACTOR_IMAGE1 );
    mMotionBlurImageActor = ImageActor::New(image);
    mMotionBlurImageActor.SetParentOrigin( ParentOrigin::CENTER );
    mMotionBlurImageActor.SetSize(MOTION_BLUR_ACTOR_WIDTH, MOTION_BLUR_ACTOR_HEIGHT);

    mContentLayer.Add( mMotionBlurImageActor );

    // Create shader used for doing motion blur
    mMotionBlurEffect = MotionBlurEffect::Apply(mMotionBlurImageActor);


#ifdef MULTIPLE_MOTION_BLURRED_ACTORS

    ///////////////////////////////////////////////////////
    //
    // Motion blurred actor 2
    //

    mMotionBlurImageActor2 = ImageActor::New(image);
    mMotionBlurImageActor2.SetParentOrigin( ParentOrigin::CENTER );
    mMotionBlurImageActor2.SetSize(MOTION_BLUR_ACTOR_WIDTH, MOTION_BLUR_ACTOR_HEIGHT);
    mMotionBlurImageActor2.SetPosition(MOTION_BLUR_ACTOR_WIDTH * 1.1f, 0.0f);
    mMotionBlurImageActor.Add( mMotionBlurImageActor2 );

    // Create shader used for doing motion blur
    mMotionBlurEffect2 = MotionBlurEffect::New(MOTION_BLUR_NUM_SAMPLES);

    // set actor shader to the blur one
    mMotionBlurImageActor2.SetShaderEffect( mMotionBlurEffect2 );


    ///////////////////////////////////////////////////////
    //
    // Motion blurred actor 3
    //

    mMotionBlurImageActor3 = ImageActor::New(image);
    mMotionBlurImageActor3.SetParentOrigin( ParentOrigin::CENTER );
    mMotionBlurImageActor3.SetSize(MOTION_BLUR_ACTOR_WIDTH, MOTION_BLUR_ACTOR_HEIGHT);
    mMotionBlurImageActor3.SetPosition(-MOTION_BLUR_ACTOR_WIDTH * 1.1f, 0.0f);
    mMotionBlurImageActor.Add( mMotionBlurImageActor3 );

    // Create shader used for doing motion blur
    mMotionBlurEffect3 = MotionBlurEffect::New(MOTION_BLUR_NUM_SAMPLES);

    // set actor shader to the blur one
    mMotionBlurImageActor3.SetShaderEffect( mMotionBlurEffect3 );


    ///////////////////////////////////////////////////////
    //
    // Motion blurred actor 4
    //

    mMotionBlurImageActor4 = ImageActor::New(image);
    mMotionBlurImageActor4.SetParentOrigin( ParentOrigin::CENTER );
    mMotionBlurImageActor4.SetSize(MOTION_BLUR_ACTOR_WIDTH, MOTION_BLUR_ACTOR_HEIGHT);
    mMotionBlurImageActor4.SetPosition(0.0f, MOTION_BLUR_ACTOR_HEIGHT * 1.1f);
    mMotionBlurImageActor.Add( mMotionBlurImageActor4 );

    // Create shader used for doing motion blur
    mMotionBlurEffect4 = MotionBlurEffect::New(MOTION_BLUR_NUM_SAMPLES);

    // set actor shader to the blur one
    mMotionBlurImageActor4.SetShaderEffect( mMotionBlurEffect4 );


    ///////////////////////////////////////////////////////
    //
    // Motion blurred actor 5
    //

    mMotionBlurImageActor5 = ImageActor::New(image);
    mMotionBlurImageActor5.SetParentOrigin( ParentOrigin::CENTER );
    mMotionBlurImageActor5.SetSize(MOTION_BLUR_ACTOR_WIDTH, MOTION_BLUR_ACTOR_HEIGHT);
    mMotionBlurImageActor5.SetPosition(0.0f, -MOTION_BLUR_ACTOR_HEIGHT * 1.1f);
    mMotionBlurImageActor.Add( mMotionBlurImageActor5 );

    // Create shader used for doing motion blur
    mMotionBlurEffect5 = MotionBlurEffect::New(MOTION_BLUR_NUM_SAMPLES);

    // set actor shader to the blur one
    mMotionBlurImageActor5.SetShaderEffect( mMotionBlurEffect5 );
#endif //#ifdef MULTIPLE_MOTION_BLURRED_ACTORS
  }

  //////////////////////////////////////////////////////////////
  //
  // Device Orientation Support
  //
  //

  void OnOrientationChanged( Orientation orientation )
  {
    unsigned int degrees = orientation.GetDegrees();
    Rotate( static_cast< DeviceOrientation >( degrees ) );
  }

  void Rotate( DeviceOrientation orientation )
  {
    // Resize the root actor
    Vector2 stageSize = Stage::GetCurrent().GetSize();
    Vector2 targetSize = stageSize;
    if( orientation == LANDSCAPE ||
        orientation == LANDSCAPE_INVERSE )
    {
      targetSize = Vector2( stageSize.y, stageSize.x );
    }

    if( mOrientation != orientation )
    {
      mOrientation = orientation;

      // check if actor is on stage
      if( mView.GetParent() )
      {
        // has parent so we expect it to be on stage, start animation
        mRotateAnimation = Animation::New( ORIENTATION_DURATION );
        mRotateAnimation.SetDestroyAction( Animation::Bake );
        mRotateAnimation.RotateTo( mView, Degree( -orientation ), Vector3::ZAXIS, AlphaFunctions::EaseOut );
        mRotateAnimation.Resize( mView, targetSize.width, targetSize.height );
        mRotateAnimation.Play();
      }
      else
      {
        // set the rotation to match the orientation
        mView.SetRotation( Degree( -orientation ), Vector3::ZAXIS );
        mView.SetSize( targetSize );
      }
    }
    else
    {
      // for first time just set size
      mView.SetSize( targetSize );
    }
  }


  //////////////////////////////////////////////////////////////
  //
  // Actor Animation
  //
  //

  // move to point on screen that was tapped
  void OnTap( Actor actor, TapGesture tapGesture )
  {
    Vector3 destPos;
    float originOffsetX, originOffsetY;

    // rotate offset (from top left origin to centre) into actor space
    Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
    actor.ScreenToLocal(originOffsetX, originOffsetY, stageSize.width * 0.5f, stageSize.height * 0.5f);

    // get dest point in local actor space
    destPos.x = tapGesture.localPoint.x - originOffsetX;
    destPos.y = tapGesture.localPoint.y - originOffsetY;
    destPos.z = 0.0f;

    float animDuration = 0.5f;
    mActorTapMovementAnimation = Animation::New( animDuration );
    if ( mMotionBlurImageActor )
    {
      mActorTapMovementAnimation.AnimateTo( Property(mMotionBlurImageActor, Actor::POSITION), destPos, AlphaFunctions::EaseInOutSine, TimePeriod(animDuration) );
    }
    mActorTapMovementAnimation.SetEndAction( Animation::Bake );
    mActorTapMovementAnimation.Play();


    // perform some spinning etc
    if(mActorEffectsEnabled)
    {
      switch(mCurrentActorAnimation)
      {
        // spin around y
        case 0:
        {
          float animDuration = 1.0f;
          mActorAnimation = Animation::New(animDuration);
          mActorAnimation.RotateBy(mMotionBlurImageActor, Degree(720), Vector3::YAXIS, AlphaFunctions::EaseInOut);
          mActorAnimation.SetEndAction( Animation::Bake );
          mActorAnimation.Play();
        }
        break;

        // spin around z
        case 1:
        {
          float animDuration = 1.0f;
          mActorAnimation = Animation::New(animDuration);
          mActorAnimation.RotateBy(mMotionBlurImageActor, Degree(720), Vector3::ZAXIS, AlphaFunctions::EaseInOut);
          mActorAnimation.SetEndAction( Animation::Bake );
          mActorAnimation.Play();
        }
        break;

        // spin around y and z
        case 2:
        {
          float animDuration = 1.0f;
          mActorAnimation = Animation::New(animDuration);
          mActorAnimation.RotateBy(mMotionBlurImageActor, Degree(360), Vector3::YAXIS, AlphaFunctions::EaseInOut);
          mActorAnimation.RotateBy(mMotionBlurImageActor, Degree(360), Vector3::ZAXIS, AlphaFunctions::EaseInOut);
          mActorAnimation.SetEndAction( Animation::Bake );
          mActorAnimation.Play();
        }
        break;

        // scale
        case 3:
        {
          float animDuration = 1.0f;
          mActorAnimation = Animation::New(animDuration);
          mActorAnimation.ScaleBy(mMotionBlurImageActor, Vector3(2.0f, 2.0f, 2.0f), AlphaFunctions::Bounce, 0.0f, 1.0f);
          mActorAnimation.SetEndAction( Animation::Bake );
          mActorAnimation.Play();
        }
        break;

        default:
          break;
      }

      mCurrentActorAnimation++;
      if(NUM_ACTOR_ANIMATIONS == mCurrentActorAnimation)
      {
        mCurrentActorAnimation = 0;
      }
    }
  }

  void ToggleActorEffects()
  {
    if(!mActorEffectsEnabled)
    {
      mActorEffectsEnabled = true;
      mActorEffectsButton.SetBackgroundImage( mIconEffectsOn );
    }
    else
    {
      mActorEffectsEnabled = false;
      mActorEffectsButton.SetBackgroundImage( mIconEffectsOff );
    }
  }

  //////////////////////////////////////////////////////////////
  //
  // Input handlers
  //
  //

  bool OnLayoutButtonClicked( Toolkit::Button button )
  {
    ChangeImage();
    return true;
  }

  bool OnEffectButtonClicked( Toolkit::Button button )
  {
    ToggleActorEffects();
    return true;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

  //////////////////////////////////////////////////////////////
  //
  // Misc
  //
  //


  void ChangeImage()
  {
    mCurrentImage++;
    if(MOTION_BLUR_NUM_ACTOR_IMAGES == mCurrentImage)
    {
      mCurrentImage = 0;
    }

    Image blurImage = Image::New( MOTION_BLUR_ACTOR_IMAGES[mCurrentImage] );
    mMotionBlurImageActor.SetImage(blurImage);
  }


private:
  Application&               mApplication;            ///< Application instance
  Toolkit::View              mView;
  Toolkit::ToolBar           mToolBar;
  TextView                   mTitleActor;             ///< The Toolbar's Title.
  Image                      mIconEffectsOff;
  Image                      mIconEffectsOn;

  Layer                      mContentLayer;           ///< Content layer (contains actor for this blur demo)

  PushButton                 mActorEffectsButton;     ///< The actor effects toggling Button.

  // Motion blur
  MotionBlurEffect mMotionBlurEffect;
  ImageActor mMotionBlurImageActor;

#ifdef MULTIPLE_MOTION_BLURRED_ACTORS
  MotionBlurEffect mMotionBlurEffect2;
  MotionBlurEffect mMotionBlurEffect3;
  MotionBlurEffect mMotionBlurEffect4;
  MotionBlurEffect mMotionBlurEffect5;

  ImageActor mMotionBlurImageActor2;
  ImageActor mMotionBlurImageActor3;
  ImageActor mMotionBlurImageActor4;
  ImageActor mMotionBlurImageActor5;
#endif //#ifdef MULTIPLE_MOTION_BLURRED_ACTORS

  // animate actor to position where user taps screen
  Animation mActorTapMovementAnimation;

  // show different animations to demonstrate blur effect working on an object only movement basis
  bool mActorEffectsEnabled;
  Animation mActorAnimation;
  int mCurrentActorAnimation;

  // offer a selection of images that user can cycle between
  int mCurrentImage;

  TapGestureDetector mTapGestureDetector;

  DeviceOrientation mOrientation;               ///< Current Device orientation
  Animation mRotateAnimation;                   ///< Animation for rotating between landscape and portrait.

  Popup mInstructionsPopup;                     ///< Info Popup
};

void RunTest(Application& app)
{
  MotionBlurExampleApp test(app);

  app.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv);

  RunTest(app);

  return 0;
}
