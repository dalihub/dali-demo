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
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali-toolkit/devel-api/visuals/animated-image-visual-actions-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* const PLAY_ICON_UNSELECTED(DEMO_IMAGE_DIR "icon-play.png");
const char* const PLAY_ICON_SELECTED(DEMO_IMAGE_DIR "icon-play-selected.png");

const unsigned int ANIMATED_IMAGE_COUNT = 2;

const char* ANIMATED_IMAGE_URLS[ANIMATED_IMAGE_COUNT] =
  {
    DEMO_IMAGE_DIR "dog-anim.webp",
    DEMO_IMAGE_DIR "dali-logo-anim.gif"};

const char* ANIMATED_ARRAY_URL_FORMATS[ANIMATED_IMAGE_COUNT] =
  {
    DEMO_IMAGE_DIR "dog-anim-%03d.png",      // Images are named dog-anim-001.png, dog-anim-002.png, etc.
    DEMO_IMAGE_DIR "dali-logo-anim-%03d.png" // Images are named dali-logo-anim-001.png, dali-logo-anim-002.png, etc.
};

int ANIMATED_ARRAY_NUMBER_OF_FRAMES[ANIMATED_IMAGE_COUNT] =
  {
    8,
    15};

const char* ANIMATION_RADIO_BUTTON_NAME("Animation Image");
const char* ARRAY_RADIO_BUTTON_NAME("Array");

/// Structure to specify the layout information for the animated images views.
struct ImageLayoutInfo
{
  Vector3 anchorPoint;
  Vector3 parentOrigin;
  float   yPosition;
};

ImageLayoutInfo IMAGE_LAYOUT_INFO[ANIMATED_IMAGE_COUNT] =
  {
    {AnchorPoint::BOTTOM_CENTER, ParentOrigin::CENTER, -80.0f},
    {AnchorPoint::TOP_CENTER, ParentOrigin::CENTER, 80.0f}};

} // unnamed namespace

/**
 * @brief This demonstrates how to display and control Animated Images.
 *
 * - It displays two animated images, an animated dog and an animated DALi logo.
 * - The images are loaded paused, a play button is overlayed on top of the images to play the animated image.
 * - Radio buttons at the bottom allow the user to change between Animated Images and a collection of Image Arrays.
 */
class AnimatedImageController : public ConnectionTracker
{
public:
  /**
   * @brief Constructor.
   * @param[in]  application  A reference to the Application class
   */
  AnimatedImageController(Application& application)
  : mApplication(application),
    mImageType(ImageType::ANIMATED_IMAGE)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &AnimatedImageController::Create);
  }

private:
  /**
   * @brief The image types supported by the application.
   */
  enum class ImageType
  {
    ANIMATED_IMAGE, ///< Displays Animated Image Files.
    IMAGE_ARRAY     ///< Displays an array of URLs that are used as an animated image.
  };

  /**
   * @brief Called to initialise the application content.
   * @param[in]  application  A reference to the Application class
   */
  void Create(Application& application)
  {
    // Set the window background color and connect to the window's key signal to allow Back and Escape to exit.
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);
    window.KeyEventSignal().Connect(this, &AnimatedImageController::OnKeyEvent);

    // Create the animated image-views
    CreateAnimatedImageViews(window);

    // Create radio buttons to change between Animated images and Image Arrays
    CreateRadioButtonLayout(window);

    // Create a tap gesture detector to use to pause the animated images
    mTapDetector = TapGestureDetector::New();
    mTapDetector.DetectedSignal().Connect(this, &AnimatedImageController::OnTap);
  }

  /**
   * @brief Creates and lays out radio buttons to allow changing between the different image types.
   */
  void CreateRadioButtonLayout(Window& window)
  {
    mAnimatedImageButton = CreateRadioButton(ANIMATION_RADIO_BUTTON_NAME, true);
    mArrayButton         = CreateRadioButton(ARRAY_RADIO_BUTTON_NAME, false);

    Toolkit::TableView radioButtonLayout = Toolkit::TableView::New(1, 2);
    radioButtonLayout.SetProperty(Dali::Actor::Property::NAME, "RadioButtonsLayout");
    radioButtonLayout.SetResizePolicy(ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT);
    radioButtonLayout.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    radioButtonLayout.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
    radioButtonLayout.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
    radioButtonLayout.SetFitHeight(0);
    radioButtonLayout.AddChild(mAnimatedImageButton, TableView::CellPosition(0, 0));
    radioButtonLayout.AddChild(mArrayButton, TableView::CellPosition(0, 1));
    radioButtonLayout.SetCellAlignment(TableView::CellPosition(0, 0),
                                       HorizontalAlignment::CENTER,
                                       VerticalAlignment::CENTER);
    radioButtonLayout.SetCellAlignment(TableView::CellPosition(0, 1),
                                       HorizontalAlignment::CENTER,
                                       VerticalAlignment::CENTER);
    radioButtonLayout.SetProperty(Actor::Property::POSITION_Y, -10.0f);

    window.Add(radioButtonLayout);
  }

  /**
   * @brief Creates a radio button.
   * @param[in]  name      The name of the button
   * @param[in]  selected  Whether the button is selected
   * @return The created radio-button
   */
  RadioButton CreateRadioButton(const char* const name, bool selected)
  {
    RadioButton radioButton = Toolkit::RadioButton::New(name);
    radioButton.SetProperty(Button::Property::SELECTED, selected);
    radioButton.ClickedSignal().Connect(this, &AnimatedImageController::OnRadioButtonClicked);
    return radioButton;
  }

  /**
   * @brief Creates the required animated image views.
   */
  void CreateAnimatedImageViews(Window window)
  {
    for(unsigned int index = 0; index < ANIMATED_IMAGE_COUNT; ++index)
    {
      Control& control = (index == 0) ? mActorDog : mActorLogo;
      if(control)
      {
        // Remove the previous control from the window, it's resources (and children) will be deleted automatically
        control.Unparent();
      }

      // Create and lay out the image view according to the index
      control = Toolkit::ImageView::New();
      control.SetProperty(Toolkit::ImageView::Property::IMAGE, SetupViewProperties(mImageType, index));
      control.SetProperty(Actor::Property::ANCHOR_POINT, IMAGE_LAYOUT_INFO[index].anchorPoint);
      control.SetProperty(Actor::Property::PARENT_ORIGIN, IMAGE_LAYOUT_INFO[index].parentOrigin);
      control.SetProperty(Actor::Property::POSITION_Y, IMAGE_LAYOUT_INFO[index].yPosition);

      control.SetProperty(Actor::Property::SIZE, Vector2(300, 300));

      // We do not want the animated image playing when it's added to the window.
      PauseAnimatedImage(control);

      window.Add(control);
    }
  }

  /**
   * @brief Plays the passed in animated image.
   * @details Also sets up the control so it can be paused when tapped.
   * @param[in]  control  The animated image to play
   */
  void PlayAnimatedImage(Control& control)
  {
    DevelControl::DoAction(control,
                           ImageView::Property::IMAGE,
                           DevelAnimatedImageVisual::Action::PLAY,
                           Property::Value());

    if(mTapDetector)
    {
      mTapDetector.Attach(control);
    }
  }

  /**
   * @brief Pauses the animated image.
   * @details Adds a Play button to the control and sets both up so that the animated image can be played again when
   *          the button is tapped.
   * @param[in]  control  The animated image to pause
   */
  void PauseAnimatedImage(Control& control)
  {
    DevelControl::DoAction(control,
                           ImageView::Property::IMAGE,
                           DevelAnimatedImageVisual::Action::PAUSE,
                           Property::Value());

    // Create a push button, and add it as child of the control
    Toolkit::PushButton animateButton = Toolkit::PushButton::New();
    animateButton.SetProperty(Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, PLAY_ICON_UNSELECTED);
    animateButton.SetProperty(Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, PLAY_ICON_SELECTED);
    animateButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    animateButton.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    animateButton.ClickedSignal().Connect(this, &AnimatedImageController::OnPlayButtonClicked);
    control.Add(animateButton);

    if(mTapDetector)
    {
      mTapDetector.Detach(control);
    }
  }

  /**
   * @brief Called when the play button is clicked.
   * @details This method is used to start playing the parent image-view of the clicked button.
   * @param[in]  button  The button that has been clicked
   * @return We return true to state that we handled the event
   */
  bool OnPlayButtonClicked(Toolkit::Button button)
  {
    Control control = (button.GetParent() == mActorDog) ? mActorDog : mActorLogo;
    PlayAnimatedImage(control);

    button.Unparent();

    return true;
  }

  /**
   * @brief Called when the animated image views are tapped.
   * @details This method is used to pause the tapped animated image view.
   * @param[in]  actor  The actor that's tapped
   */
  void OnTap(Dali::Actor actor, const Dali::TapGesture& /* tap */)
  {
    Control control = (actor == mActorDog) ? mActorDog : mActorLogo;
    PauseAnimatedImage(control);
  }

  /**
   * @brief Called when a radio button is clicked.
   * @details This method is used to change between the different image types.
   * @param[in]  button  The clicked radio-button
   * @return We return true to state that we handled the event.
   *
   */
  bool OnRadioButtonClicked(Toolkit::Button button)
  {
    mImageType = (button == mAnimatedImageButton) ? ImageType::ANIMATED_IMAGE : ImageType::IMAGE_ARRAY;

    CreateAnimatedImageViews(mApplication.GetWindow());
    return true;
  }

  /**
   * @brief Called when any key event is received.
   *
   * Will use this to quit the application if Back or the Escape key is received
   * @param[in] event The key event information
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

  /**
   * @brief Sets up the view properties appropriately.
   * @param[in]  type   The Image type
   * @param[in]  index  The index
   * @return The set up property value
   */
  Property::Value SetupViewProperties(ImageType type, int index)
  {
    Property::Map map;

    AddUrl(map, type, index);
    AddCache(map, type, index);
    return Property::Value(map);
  }

  /**
   * @brief Adds the URL to the given map appropriately.
   * @param[in/out]  map    The map to add the URL details to
   * @param[in]      type   The Image type
   * @param[in]      index  The index
   */
  void AddUrl(Property::Map& map, ImageType type, int index)
  {
    if(type == ImageType::ANIMATED_IMAGE)
    {
      map.Add(Toolkit::ImageVisual::Property::URL, Property::Value(ANIMATED_IMAGE_URLS[index]));
    }
    else
    {
      Property::Array frameUrls;
      for(int i = 1; i <= ANIMATED_ARRAY_NUMBER_OF_FRAMES[index]; ++i)
      {
        char* buffer;
        int   len = asprintf(&buffer, ANIMATED_ARRAY_URL_FORMATS[index], i);
        if(len > 0)
        {
          std::string frameUrl(buffer);
          free(buffer);
          frameUrls.Add(Property::Value(frameUrl));
        }
      }
      map.Add(Toolkit::ImageVisual::Property::URL, Property::Value(frameUrls));
    }
  }

  /**
   * @brief Adds the cache properties, if required to the map.
   * @param[in/out]  map    The map to add the URL details to
   * @param[in]      type   The Image type
   * @param[in]      index  The index
   */
  void AddCache(Property::Map& map, ImageType type, int index)
  {
    if(type == ImageType::IMAGE_ARRAY)
    {
      map
        .Add(Toolkit::ImageVisual::Property::BATCH_SIZE, 4)
        .Add(Toolkit::ImageVisual::Property::CACHE_SIZE, 10)
        .Add(Toolkit::ImageVisual::Property::FRAME_DELAY, 150);
    }
  }

private:
  Application& mApplication; ///< A reference to the application.

  Toolkit::ImageView mActorDog;  ///< The current dog image view.
  Toolkit::ImageView mActorLogo; ///< The current logo image view.

  Toolkit::RadioButton mAnimatedImageButton; ///< The Animated Image Radio Button.
  Toolkit::RadioButton mArrayButton;         ///< The Array Radio Button.

  TapGestureDetector mTapDetector; ///< The tap detector.

  ImageType mImageType; ///< The current Image type.
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  AnimatedImageController test(application);

  application.MainLoop();

  return 0;
}
