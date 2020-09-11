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
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali/devel-api/object/handle-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* const IMAGE_NAME = DEMO_IMAGE_DIR "gallery-medium-1.jpg"; ///< The image to use.
const Vector3     IMAGE_SIZE = Vector3(300, 200, 0);                  ///< The size of the image-views.

const float           BORDER_SIZE = 2.0f; ///< The size of the border.
const Property::Value BORDER              ///< The border to use for each image-view.
  {
    {Visual::Property::TYPE, Visual::BORDER},
    {BorderVisual::Property::COLOR, Color::RED},
    {BorderVisual::Property::SIZE, BORDER_SIZE}};
const Extents LARGE_PADDING(100.0f, 100.0f, 2.0f, 2.0f);                               ///< The large padding extents.
const Extents BORDER_ONLY_PADDING(BORDER_SIZE, BORDER_SIZE, BORDER_SIZE, BORDER_SIZE); ///< The border only padding extents.

const std::string INSTRUCTIONS_TEXT = "\n(Tap or Key Press To Change)";                 ///< Instructions on how to change the padding mode.
const std::string LARGE_PADDING_TEXT("Padding: Left/Right Large" + INSTRUCTIONS_TEXT);  ///< Label to shown when large padding enabled.
const std::string BORDER_ONLY_PADDING_TEXT("Padding: Border Only" + INSTRUCTIONS_TEXT); ///< Label to shown when border-only padding enabled.
const std::string FILL_LABEL("FILL");
const std::string FIT_KEEP_ASPECT_LABEL("FIT\nKEEP ASPECT");

const Property::Map TEXT_LABEL_PROPERTIES ///< All the properties of the Large Text Label shown in the example.
  {
    {Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER},
    {Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER},
    {Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT},
    {Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT},
    {Control::Property::BACKGROUND,
     {{Toolkit::Visual::Property::TYPE, Visual::GRADIENT},
      {GradientVisual::Property::STOP_COLOR, Property::Array{Vector4(167.0f, 207.0f, 223.0f, 255.0f) / 255.0f, Vector4(0.0f, 64.0f, 137.0f, 255.0f) / 255.0f}},
      {GradientVisual::Property::START_POSITION, Vector2(0.0f, -0.5f)},
      {GradientVisual::Property::END_POSITION, Vector2(0.0f, 0.5f)}}},
    {TextLabel::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::CENTER},
    {TextLabel::Property::VERTICAL_ALIGNMENT, VerticalAlignment::CENTER},
    {TextLabel::Property::MULTI_LINE, true}};

const Property::Map FILL_IMAGE_PROPERTIES ///< The basic properties of the Fill image view.
  {
    {Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER},
    {Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER},
    {Actor::Property::SIZE, IMAGE_SIZE},
    {Control::Property::BACKGROUND, BORDER}};

const Property::Map FIT_KEEP_ASPECT_RATIO_IMAGE_BASIC_PROPERTIES ///< The basic properties of the Fit Keep Aspect image view.
  {
    {Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER},
    {Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER},
    {Actor::Property::SIZE, IMAGE_SIZE},
    {Control::Property::BACKGROUND, BORDER}};

const Property::Map OVERLAY_LABEL_PROPERTIES ///< The image view overlay label properties
  {
    {TextLabel::Property::TEXT_COLOR, Color::WHITE},
    {TextLabel::Property::OUTLINE, {{"color", Color::BLACK}, {"width", 1.0f}}},
    {TextLabel::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::CENTER},
    {TextLabel::Property::VERTICAL_ALIGNMENT, VerticalAlignment::CENTER},
    {TextLabel::Property::MULTI_LINE, true},
  };
} // unnamed namespace

/**
 * @brief This example shows how to use the Dali::Toolkit::DevelVisual::Property::VISUAL_FITTING_MODE property.
 */
class VisualFittingModeController : public ConnectionTracker
{
public:
  /**
   * @brief Constructor.
   * @param[in]  application A reference to the Application class.
   */
  VisualFittingModeController(Application& application)
  : mApplication(application),
    mLargePadding(true)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &VisualFittingModeController::Create);
  }

private:
  /**
   * @brief Called to initialise the application content
   * @param[in] application A reference to the Application class.
   */
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Text Label filling the entire screen, with a background
    mTextLabel = Handle::New<TextLabel>(TEXT_LABEL_PROPERTIES);
    window.Add(mTextLabel);

    // We want to change the padding when tapping
    mTapDetector = TapGestureDetector::New();
    mTapDetector.Attach(mTextLabel);
    mTapDetector.DetectedSignal().Connect(this, &VisualFittingModeController::OnTap);

    // Create an ImageView with the default behaviour, i.e. image fills to control size
    mFillImage = ImageView::New(IMAGE_NAME);
    mFillImage.SetProperties(FILL_IMAGE_PROPERTIES);
    window.Add(mFillImage);

    // Create an ImageView that Keeps the aspect ratio while fitting within the given size
    mFitKeepAspectRatioImage = Handle::New<ImageView>(FIT_KEEP_ASPECT_RATIO_IMAGE_BASIC_PROPERTIES);
    mFitKeepAspectRatioImage.SetProperty(ImageView::Property::IMAGE,
                                         Property::Map{
                                           {Visual::Property::TYPE, Visual::IMAGE},
                                           {ImageVisual::Property::URL, IMAGE_NAME},
                                           {DevelVisual::Property::VISUAL_FITTING_MODE, DevelVisual::FIT_KEEP_ASPECT_RATIO}});
    window.Add(mFitKeepAspectRatioImage);

    // Create an overlay label for fill image
    Actor fillLabel = TextLabel::New(FILL_LABEL);
    fillLabel.SetProperties(FILL_IMAGE_PROPERTIES);
    fillLabel.SetProperties(OVERLAY_LABEL_PROPERTIES);
    window.Add(fillLabel);

    // Create an overlay label for the Fit/Keep Aspect image
    Actor fitLabel = TextLabel::New(FIT_KEEP_ASPECT_LABEL);
    fitLabel.SetProperties(FIT_KEEP_ASPECT_RATIO_IMAGE_BASIC_PROPERTIES);
    fitLabel.SetProperties(OVERLAY_LABEL_PROPERTIES);
    window.Add(fitLabel);

    // Respond to key events, exit if ESC/Back, change the padding if anything else
    window.KeyEventSignal().Connect(this, &VisualFittingModeController::OnKeyEvent);

    // Set the initial padding
    ChangePadding();
  }

  /**
   * @brief Changes the padding of both image-views to show how the VisualFittingMode is applied.
   */
  void ChangePadding()
  {
    mLargePadding = !mLargePadding;

    const Extents padding(mLargePadding ? LARGE_PADDING : BORDER_ONLY_PADDING);
    mFitKeepAspectRatioImage.SetProperty(Control::Property::PADDING, padding);
    mFillImage.SetProperty(Control::Property::PADDING, padding);

    mTextLabel.SetProperty(TextLabel::Property::TEXT, mLargePadding ? LARGE_PADDING_TEXT : BORDER_ONLY_PADDING_TEXT);
  }

  /**
   * @brief Called when the main text-label is tapped.
   *
   * We just want to change the padding when this happens.
   */
  void OnTap(Actor /* actor */, const TapGesture& /* tap */)
  {
    ChangePadding();
  }

  /**
   * @brief Called when any key event is received
   *
   * Will use this to quit the application if we receive the Back or the Escape key & change
   * the padding if any other key.
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
      else
      {
        ChangePadding();
      }
    }
  }

private:
  Application&       mApplication;             ///< Reference to the application class.
  Actor              mFillImage;               ///< An image-view that fills to the control size.
  Actor              mFitKeepAspectRatioImage; ///< An image-view that fits within the given size & keeps the aspect ratio.
  Actor              mTextLabel;               ///< A text label to show the current padding mode.
  TapGestureDetector mTapDetector;             ///< A tap detector to change the padding mode.
  bool               mLargePadding;            ///< If true, the large padding values are used. When false, only the border padding is applied.
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                 application = Application::New(&argc, &argv);
  VisualFittingModeController visualFittingModeController(application);
  application.MainLoop();
  return 0;
}
