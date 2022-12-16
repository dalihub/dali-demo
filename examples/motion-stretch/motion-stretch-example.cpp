/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <iomanip>
#include <sstream>

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/shader-effects/motion-stretch-effect.h>
#include <dali/dali.h>
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace // unnamed namespace
{
////////////////////////////////////////////////////
//
// Demo setup parameters
//

const float MOTION_STRETCH_ACTOR_WIDTH  = 256; // actor size on screen
const float MOTION_STRETCH_ACTOR_HEIGHT = 256; // ""

const int   MOTION_STRETCH_NUM_ACTOR_IMAGES = 5;
const char* MOTION_STRETCH_ACTOR_IMAGE1(DEMO_IMAGE_DIR "image-with-border-1.jpg");
const char* MOTION_STRETCH_ACTOR_IMAGE2(DEMO_IMAGE_DIR "image-with-border-2.jpg");
const char* MOTION_STRETCH_ACTOR_IMAGE3(DEMO_IMAGE_DIR "image-with-border-3.jpg");
const char* MOTION_STRETCH_ACTOR_IMAGE4(DEMO_IMAGE_DIR "image-with-border-4.jpg");
const char* MOTION_STRETCH_ACTOR_IMAGE5(DEMO_IMAGE_DIR "image-with-border-5.jpg");

const char* MOTION_STRETCH_ACTOR_IMAGES[] = {
  MOTION_STRETCH_ACTOR_IMAGE1,
  MOTION_STRETCH_ACTOR_IMAGE2,
  MOTION_STRETCH_ACTOR_IMAGE3,
  MOTION_STRETCH_ACTOR_IMAGE4,
  MOTION_STRETCH_ACTOR_IMAGE5,
};

const int NUM_ACTOR_ANIMATIONS  = 4;
const int NUM_CAMERA_ANIMATIONS = 2;

const char* BACKGROUND_IMAGE_PATH = DEMO_IMAGE_DIR "background-default.png";

const char* TOOLBAR_IMAGE(DEMO_IMAGE_DIR "top-bar.png");
const char* LAYOUT_IMAGE(DEMO_IMAGE_DIR "icon-change.png");
const char* LAYOUT_IMAGE_SELECTED(DEMO_IMAGE_DIR "icon-change-selected.png");
const char* APPLICATION_TITLE("Motion Stretch");
const char* EFFECTS_OFF_ICON(DEMO_IMAGE_DIR "icon-effects-off.png");
const char* EFFECTS_OFF_ICON_SELECTED(DEMO_IMAGE_DIR "icon-effects-off-selected.png");
const char* EFFECTS_ON_ICON(DEMO_IMAGE_DIR "icon-effects-on.png");
const char* EFFECTS_ON_ICON_SELECTED(DEMO_IMAGE_DIR "icon-effects-on-selected.png");

// These values depend on the button background image
const Vector4 BUTTON_IMAGE_BORDER(Vector4::ONE * 3.0f);

const float UI_MARGIN = 4.0f; ///< Screen Margin for placement of UI buttons

const Vector3 BUTTON_SIZE_CONSTRAINT(0.24f, 0.09f, 1.0f);

// move this button down a bit so it is visible on target and not covered up by toolbar
const float BUTTON_TITLE_LABEL_Y_OFFSET = 0.05f;

const float ORIENTATION_DURATION = 0.5f; ///< Time to rotate to new orientation.
} // unnamed namespace

//
class MotionStretchExampleApp : public ConnectionTracker
{
public:
  /**
     * DeviceOrientation describes the four different
     * orientations the device can be in based on accelerometer reports.
     */
  enum DeviceOrientation
  {
    PORTRAIT          = 0,
    LANDSCAPE         = 90,
    PORTRAIT_INVERSE  = 180,
    LANDSCAPE_INVERSE = 270
  };

  /**
   * Constructor
   * @param application class, stored as reference
   */
  MotionStretchExampleApp(Application& app)
  : mApplication(app),
    mActorEffectsEnabled(false),
    mCurrentActorAnimation(0),
    mCurrentImage(0),
    mOrientation(PORTRAIT)
  {
    // Connect to the Application's Init signal
    app.InitSignal().Connect(this, &MotionStretchExampleApp::OnInit);
  }

  ~MotionStretchExampleApp()
  {
    // Nothing to do here; everything gets deleted automatically
  }

  /**
   * This method gets called once the main loop of application is up and running
   */
  void OnInit(Application& app)
  {
    // The Init signal is received once (only) during the Application lifetime
    Window window = app.GetWindow();

    window.KeyEventSignal().Connect(this, &MotionStretchExampleApp::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the window.
    mContentLayer = DemoHelper::CreateView(mApplication,
                                           mView,
                                           mToolBar,
                                           BACKGROUND_IMAGE_PATH,
                                           TOOLBAR_IMAGE,
                                           APPLICATION_TITLE);

    // Ensure the content layer is a square so the touch area works in all orientations
    Vector2 windowSize = window.GetSize();
    float   size       = std::max(windowSize.width, windowSize.height);
    mContentLayer.SetProperty(Actor::Property::SIZE, Vector2(size, size));

    //Add an slideshow icon on the right of the title
    mActorEffectsButton = Toolkit::PushButton::New();
    mActorEffectsButton.SetProperty(Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, EFFECTS_OFF_ICON);
    mActorEffectsButton.SetProperty(Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, EFFECTS_OFF_ICON_SELECTED);
    mActorEffectsButton.ClickedSignal().Connect(this, &MotionStretchExampleApp::OnEffectButtonClicked);
    mToolBar.AddControl(mActorEffectsButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HORIZONTAL_CENTER, DemoHelper::DEFAULT_PLAY_PADDING);

    // Creates a mode button.
    // Create a effect toggle button. (right of toolbar)
    Toolkit::PushButton layoutButton = Toolkit::PushButton::New();
    layoutButton.SetProperty(Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, LAYOUT_IMAGE);
    layoutButton.SetProperty(Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, LAYOUT_IMAGE_SELECTED);
    layoutButton.ClickedSignal().Connect(this, &MotionStretchExampleApp::OnLayoutButtonClicked);
    layoutButton.SetProperty(Actor::Property::LEAVE_REQUIRED, true);
    mToolBar.AddControl(layoutButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HORIZONTAL_RIGHT, DemoHelper::DEFAULT_MODE_SWITCH_PADDING);

    // Input
    mTapGestureDetector = TapGestureDetector::New();
    mTapGestureDetector.Attach(mContentLayer);
    mTapGestureDetector.DetectedSignal().Connect(this, &MotionStretchExampleApp::OnTap);

    // set initial orientation
    Dali::Window winHandle = app.GetWindow();
    winHandle.AddAvailableOrientation(Dali::WindowOrientation::PORTRAIT);
    winHandle.AddAvailableOrientation(Dali::WindowOrientation::LANDSCAPE);
    winHandle.AddAvailableOrientation(Dali::WindowOrientation::PORTRAIT_INVERSE);
    winHandle.AddAvailableOrientation(Dali::WindowOrientation::LANDSCAPE_INVERSE);
    winHandle.ResizeSignal().Connect(this, &MotionStretchExampleApp::OnWindowResized);

    // set initial orientation
    Rotate(PORTRAIT);

    ///////////////////////////////////////////////////////
    //
    // Motion stretched actor
    //
    mMotionStretchEffect        = Toolkit::CreateMotionStretchEffect();
    mMotionStretchEffect["url"] = MOTION_STRETCH_ACTOR_IMAGE1;
    mMotionStretchImageView     = ImageView::New();
    mMotionStretchImageView.SetProperty(Toolkit::ImageView::Property::IMAGE, mMotionStretchEffect);
    mMotionStretchImageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mMotionStretchImageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mMotionStretchImageView.SetProperty(Actor::Property::SIZE, Vector2(MOTION_STRETCH_ACTOR_WIDTH, MOTION_STRETCH_ACTOR_HEIGHT));
    // Add stretch padding

    mMotionStretchImageView.SetProperty(Actor::Property::UPDATE_AREA_HINT, Vector4(0, 0, MOTION_STRETCH_ACTOR_WIDTH, MOTION_STRETCH_ACTOR_HEIGHT)*1.25f);

    mContentLayer.Add(mMotionStretchImageView);

    // Create shader used for doing motion stretch
    Toolkit::SetMotionStretchProperties(mMotionStretchImageView);
  }

  //////////////////////////////////////////////////////////////
  //
  // Device Orientation Support
  //
  //

  void OnWindowResized(Window window, Window::WindowSize size)
  {
    Rotate(size.GetWidth() > size.GetHeight() ? LANDSCAPE : PORTRAIT);
  }

  void Rotate(DeviceOrientation orientation)
  {
    // Resize the root actor
    const Vector2 targetSize = mApplication.GetWindow().GetSize();

    if(mOrientation != orientation)
    {
      mOrientation = orientation;

      // check if actor is on window
      if(mView.GetParent())
      {
        // has parent so we expect it to be on window, start animation
        mRotateAnimation = Animation::New(ORIENTATION_DURATION);
        mRotateAnimation.AnimateTo(Property(mView, Actor::Property::SIZE_WIDTH), targetSize.width);
        mRotateAnimation.AnimateTo(Property(mView, Actor::Property::SIZE_HEIGHT), targetSize.height);
        mRotateAnimation.Play();
      }
      else
      {
        mView.SetProperty(Actor::Property::SIZE, targetSize);
      }
    }
    else
    {
      // for first time just set size
      mView.SetProperty(Actor::Property::SIZE, targetSize);
    }
  }

  //////////////////////////////////////////////////////////////
  //
  // Actor Animation
  //
  //

  // move to point on screen that was tapped
  void OnTap(Actor actor, const TapGesture& tapGesture)
  {
    Vector3 destPos;
    float   originOffsetX, originOffsetY;

    // rotate offset (from top left origin to centre) into actor space
    Vector2 windowSize = mApplication.GetWindow().GetSize();
    actor.ScreenToLocal(originOffsetX, originOffsetY, windowSize.width * 0.5f, windowSize.height * 0.5f);

    // get dest point in local actor space
    const Vector2& localPoint = tapGesture.GetLocalPoint();
    destPos.x                 = localPoint.x - originOffsetX;
    destPos.y                 = localPoint.y - originOffsetY;
    destPos.z                 = 0.0f;

    float tapMovementAnimDuration = 0.5f;
    mActorTapMovementAnimation    = Animation::New(tapMovementAnimDuration);
    if(mMotionStretchImageView)
    {
      mActorTapMovementAnimation.AnimateTo(Property(mMotionStretchImageView, Actor::Property::POSITION), destPos, AlphaFunction::EASE_IN_OUT_SINE, TimePeriod(tapMovementAnimDuration));
    }
    mActorTapMovementAnimation.SetEndAction(Animation::BAKE);
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
          mActorAnimation    = Animation::New(animDuration);
          mActorAnimation.AnimateBy(Property(mMotionStretchImageView, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(360.0f)), Vector3::YAXIS), AlphaFunction::EASE_IN_OUT);
          mActorAnimation.SetEndAction(Animation::BAKE);
          mActorAnimation.Play();
        }
        break;

        // spin around z
        case 1:
        {
          float animDuration = 1.0f;
          mActorAnimation    = Animation::New(animDuration);
          mActorAnimation.AnimateBy(Property(mMotionStretchImageView, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(360.0f)), Vector3::ZAXIS), AlphaFunction::EASE_IN_OUT);
          mActorAnimation.SetEndAction(Animation::BAKE);
          mActorAnimation.Play();
        }
        break;

        // spin around y and z
        case 2:
        {
          float animDuration = 1.0f;
          mActorAnimation    = Animation::New(animDuration);
          mActorAnimation.AnimateBy(Property(mMotionStretchImageView, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(360.0f)), Vector3::YAXIS), AlphaFunction::EASE_IN_OUT);
          mActorAnimation.AnimateBy(Property(mMotionStretchImageView, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(360.0f)), Vector3::ZAXIS), AlphaFunction::EASE_IN_OUT);
          mActorAnimation.SetEndAction(Animation::BAKE);
          mActorAnimation.Play();
        }
        break;

        // scale
        case 3:
        {
          float animDuration = 1.0f;
          mActorAnimation    = Animation::New(animDuration);
          mActorAnimation.AnimateBy(Property(mMotionStretchImageView, Actor::Property::SCALE), Vector3(2.0f, 2.0f, 2.0f), AlphaFunction::BOUNCE, TimePeriod(0.0f, 1.0f));
          mActorAnimation.SetEndAction(Animation::BAKE);
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
      mActorEffectsButton.SetProperty(Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, EFFECTS_ON_ICON);
      mActorEffectsButton.SetProperty(Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, EFFECTS_ON_ICON_SELECTED);
    }
    else
    {
      mActorEffectsEnabled = false;
      mActorEffectsButton.SetProperty(Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, EFFECTS_OFF_ICON);
      mActorEffectsButton.SetProperty(Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, EFFECTS_OFF_ICON_SELECTED);
    }
  }

  //////////////////////////////////////////////////////////////
  //
  // Input handlers
  //
  //

  bool OnLayoutButtonClicked(Toolkit::Button button)
  {
    ChangeImage();
    return true;
  }

  bool OnEffectButtonClicked(Toolkit::Button button)
  {
    ToggleActorEffects();
    return true;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
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
    if(MOTION_STRETCH_NUM_ACTOR_IMAGES == mCurrentImage)
    {
      mCurrentImage = 0;
    }

    mMotionStretchEffect["url"] = MOTION_STRETCH_ACTOR_IMAGES[mCurrentImage];
    mMotionStretchImageView.SetProperty(Toolkit::ImageView::Property::IMAGE, mMotionStretchEffect);
  }

private:
  Application&     mApplication; ///< Application instance
  Toolkit::Control mView;
  Toolkit::ToolBar mToolBar;
  Layer            mContentLayer; ///< Content layer (contains actor for this stretch demo)

  PushButton mActorEffectsButton; ///< The actor effects toggling Button.

  // Motion stretch
  Property::Map mMotionStretchEffect;
  ImageView     mMotionStretchImageView;

  // animate actor to position where user taps screen
  Animation mActorTapMovementAnimation;

  // show different animations to demonstrate stretch effect working on an object only movement basis
  bool      mActorEffectsEnabled;
  Animation mActorAnimation;
  int       mCurrentActorAnimation;

  // offer a selection of images that user can cycle between
  int mCurrentImage;

  TapGestureDetector mTapGestureDetector;

  DeviceOrientation mOrientation;     ///< Current Device orientation
  Animation         mRotateAnimation; ///< Animation for rotating between landscape and portrait.
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application             app = Application::New(&argc, &argv, DEMO_THEME_PATH);
  MotionStretchExampleApp test(app);
  app.MainLoop();
  return 0;
}
