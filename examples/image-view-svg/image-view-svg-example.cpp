/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <string.h>

using namespace Dali;

namespace
{
const float MIN_SCALE = 0.6f;
const float MAX_SCALE = 6.f;

const char* SVG_IMAGES[] =
  {
    DEMO_IMAGE_DIR "Camera.svg",
    DEMO_IMAGE_DIR "Contacts.svg",
    DEMO_IMAGE_DIR "Mail.svg",
    DEMO_IMAGE_DIR "Message.svg",
    DEMO_IMAGE_DIR "Phone.svg",
    DEMO_IMAGE_DIR "Settings.svg",
    DEMO_IMAGE_DIR "World.svg",
    DEMO_IMAGE_DIR "Kid1.svg"};
const unsigned int NUM_SVG_IMAGES(sizeof(SVG_IMAGES) / sizeof(SVG_IMAGES[0]));
const unsigned int NUM_IMAGES_DISPLAYED = 4u;
} // unnamed namespace

// This example shows how to display svg images with ImageView.
//
class ImageSvgController : public ConnectionTracker
{
public:
  ImageSvgController(Application& application)
  : mApplication(application),
    mScale(1.f),
    mIndex(0)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ImageSvgController::Create);
  }

  ~ImageSvgController()
  {
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);
    Vector2 windowSize = window.GetSize();
    mActorSize         = windowSize / 2.f;

    window.KeyEventSignal().Connect(this, &ImageSvgController::OnKeyEvent);

    // Background, for receiving gestures
    mWindowBackground = Actor::New();
    mWindowBackground.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    mWindowBackground.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
    mWindowBackground.SetProperty(Actor::Property::SIZE, Vector2(windowSize.x, windowSize.y));
    window.Add(mWindowBackground);

    // Push button,  for changing the image set for displaying
    Toolkit::PushButton changeButton = Toolkit::PushButton::New();
    changeButton.SetProperty(Toolkit::Button::Property::LABEL, "Next");
    changeButton.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_RIGHT);
    changeButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_RIGHT);
    window.Add(changeButton);
    changeButton.ClickedSignal().Connect(this, &ImageSvgController::OnChangeButtonClicked);

    // Push button, for resetting the actor size and position
    Toolkit::PushButton resetButton = Toolkit::PushButton::New();
    resetButton.SetProperty(Toolkit::Button::Property::LABEL, "Reset");
    resetButton.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    resetButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    window.Add(resetButton);
    resetButton.ClickedSignal().Connect(this, &ImageSvgController::OnResetButtonClicked);

    // Create and put imageViews to window
    for(unsigned int i = 0; i < NUM_IMAGES_DISPLAYED; i++)
    {
      mSvgActor[i] = Toolkit::ImageView::New(SVG_IMAGES[mIndex + i]);
      mSvgActor[i].SetProperty(Actor::Property::SIZE, mActorSize);
      mSvgActor[i].TranslateBy(Vector3(0.0, windowSize.height * 0.05, 0.0f));
      window.Add(mSvgActor[i]);
    }
    mSvgActor[0].SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mSvgActor[0].SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_RIGHT);
    mSvgActor[1].SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mSvgActor[1].SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_LEFT);
    mSvgActor[2].SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mSvgActor[2].SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_RIGHT);
    mSvgActor[3].SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mSvgActor[3].SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

    // Connect pan gesture for moving the actors
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect(this, &ImageSvgController::OnPanGesture);
    mPanGestureDetector.Attach(mWindowBackground);

    // Connect pinch gesture for resizing the actors
    mPinchGestureDetector = PinchGestureDetector::New();
    mPinchGestureDetector.Attach(mWindowBackground);
    mPinchGestureDetector.DetectedSignal().Connect(this, &ImageSvgController::OnPinch);

    changeButton.RaiseToTop();
    resetButton.RaiseToTop();
  }

  // Callback of push button, for changing image set
  bool OnChangeButtonClicked(Toolkit::Button button)
  {
    mIndex = (mIndex + NUM_IMAGES_DISPLAYED) % NUM_SVG_IMAGES;
    for(unsigned int i = 0; i < NUM_IMAGES_DISPLAYED; i++)
    {
      mSvgActor[i].SetImage(SVG_IMAGES[mIndex + i]);
    }

    return true;
  }

  // Callback of push button, for resetting image size and position
  bool OnResetButtonClicked(Toolkit::Button button)
  {
    for(unsigned int i = 0; i < NUM_IMAGES_DISPLAYED; i++)
    {
      mSvgActor[i].SetProperty(Actor::Property::SIZE, mActorSize);
      mSvgActor[i].SetProperty(Actor::Property::POSITION, Vector3::ZERO);
      mScale = 1.f;
    }

    return true;
  }

  // Callback of pan gesture, for moving the actors
  void OnPanGesture(Actor actor, const PanGesture& gesture)
  {
    if(gesture.GetState() == GestureState::CONTINUING)
    {
      for(unsigned int i = 0; i < NUM_IMAGES_DISPLAYED; i++)
      {
        mSvgActor[i].TranslateBy(Vector3(gesture.GetDisplacement()));
      }
    }
  }

  // Callback of pinch gesture, for resizing the actors
  void OnPinch(Actor actor, const PinchGesture& gesture)
  {
    switch(gesture.GetState())
    {
        // Only scale the image when we start or continue pinching

      case GestureState::STARTED:
      case GestureState::CONTINUING:
      {
        float scale = std::max(gesture.GetScale(), MIN_SCALE / mScale);
        scale       = std::min(MAX_SCALE / mScale, scale);

        for(unsigned int i = 0; i < NUM_IMAGES_DISPLAYED; i++)
        {
          mSvgActor[i].SetProperty(Actor::Property::SCALE, scale);
        }
        break;
      }

      case GestureState::FINISHED:
      {
        // Resize the image when pinching is complete, this will rasterize the SVG to the new size

        mScale = mScale * gesture.GetScale();
        mScale = mScale > MAX_SCALE ? MAX_SCALE : mScale;
        mScale = mScale < MIN_SCALE ? MIN_SCALE : mScale;
        for(unsigned int i = 0; i < NUM_IMAGES_DISPLAYED; i++)
        {
          mSvgActor[i].SetProperty(Actor::Property::SIZE, mActorSize * mScale);
          mSvgActor[i].SetProperty(Actor::Property::SCALE, 1.0f);
        }
        break;
      }

      case GestureState::CANCELLED:
      case GestureState::CLEAR:
      case GestureState::POSSIBLE:
        break;
    }
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
      else
      {
        const char* keyName = event.GetKeyName().c_str();
        if(strcmp(keyName, "Left") == 0)
        {
          if(mScale > MIN_SCALE)
          {
            mScale /= 1.1f;
          }
          for(unsigned int i = 0; i < NUM_IMAGES_DISPLAYED; i++)
          {
            mSvgActor[i].SetProperty(Actor::Property::SIZE, mActorSize * mScale);
          }
        }
        else if(strcmp(keyName, "Right") == 0)
        {
          if(mScale < MAX_SCALE)
          {
            mScale *= 1.1f;
          }
          for(unsigned int i = 0; i < NUM_IMAGES_DISPLAYED; i++)
          {
            mSvgActor[i].SetProperty(Actor::Property::SIZE, mActorSize * mScale);
          }
        }
      }
    }
  }

private:
  Application&         mApplication;
  Actor                mWindowBackground;
  PanGestureDetector   mPanGestureDetector;
  PinchGestureDetector mPinchGestureDetector;

  Toolkit::ImageView mSvgActor[4];
  Vector2            mActorSize;
  float              mScale;
  unsigned int       mIndex;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application        application = Application::New(&argc, &argv);
  ImageSvgController test(application);
  application.MainLoop();
  return 0;
}
