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
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali/dali.h>
#include <string>
#include "shared/view.h"

using namespace Dali;
using namespace Toolkit;

namespace
{
const char* NEXT_BUTTON_IMAGE(DEMO_IMAGE_DIR "DarkStyleGreenArrowButton.png");
const char* NEXT_BUTTON_PRESSED_IMAGE(DEMO_IMAGE_DIR "DarkStyleGreyArrowButton.png");
const char* NEXT_BUTTON_DISABLED_IMAGE(DEMO_IMAGE_DIR "DarkStyleDisabledArrowButton.png");
const char* OK_IMAGE_IMAGE(DEMO_IMAGE_DIR "FontStyleButton_OK_02.png");
const char* LOADING_IMAGE(DEMO_IMAGE_DIR "animatedLoading.gif");

const char* IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "gallery-small-23.jpg",
  DEMO_IMAGE_DIR "woodEffect.jpg",
  DEMO_IMAGE_DIR "heartsframe.9.png",
  DEMO_IMAGE_DIR "keyboard-Landscape.jpg",
  DEMO_IMAGE_DIR "keyboard-LandscapeCopy.jpg",
};

/**
 * Enums that refer to the row in the main table view.
 * Aids in placement of content so easy to see which type of content belongs to each row.
 */
enum TableRowPlacement
{
  TITLE,
  INSTRUCTIONS,
  IMAGE,
  NEXT_BUTTON,
  LOADING_STATUS,
  NUMBER_OF_ROWS
};

} // namespace

/**
 * Examples showing the various polices of ImageVisual in use.
 * image release polcy, image loading policy and exif data are currently demonstrated.
 * Large images are used to cause loading time to be long enough to show differences.
 * If hardware causes loading time improve then remote images or larger images may be required in future.
 */
class ImagePolicies : public ConnectionTracker
{
public:
  /**
   * Constructor
   */
  ImagePolicies(Application& application)
  : mApplication(application),
    mExampleIndex(0)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ImagePolicies::Create);
  }

  /**
   * To prevent the next button being pressed before an Image has loaded the Button can br disabled.
   * This function allows the control (Image view in this case) to attached to the Image loading signal
   * and re-enable the button after Image has loaded.
   */
  void ResourceReadySignal(Control control)
  {
    mNextButton.SetProperty(Button::Property::DISABLED, false);
  }

  /**
   * Helper function to create ImageViews used by this example, preventing the duplication of code.
   * param[in] correctionEnabled  Set true if Exif orientation correction should be applied.
   * param[in] loadPolicy Which LoadPolicy to use.
   * param[in] releasePolicy Which ReleasePolicy to use
   * param[in] synchronousLoading If the Image should be loaded synchronously
   * param[in] imageFilenameId Which image to load, referring to the array of filenames for this example.
   * return An ImageView with the required set up
   */
  ImageView CreateImageView(bool correctionEnabled, ImageVisual::LoadPolicy::Type loadPolicy, ImageVisual::ReleasePolicy::Type releasePolicy, bool synchronousLoading, unsigned int imageFilenameId)
  {
    ImageView     imageView = ImageView::New();
    Property::Map imagePropertyMap;
    imagePropertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
    imagePropertyMap.Insert(ImageVisual::Property::URL, IMAGE_PATH[imageFilenameId]);
    imagePropertyMap.Insert(ImageVisual::Property::ORIENTATION_CORRECTION, correctionEnabled);
    imagePropertyMap.Insert(ImageVisual::Property::LOAD_POLICY, loadPolicy);
    imagePropertyMap.Insert(ImageVisual::Property::RELEASE_POLICY, releasePolicy);
    if(synchronousLoading)
    {
      imagePropertyMap.Insert(DevelImageVisual::Property::SYNCHRONOUS_LOADING, true);
    }
    imageView.SetProperty(ImageView::Property::IMAGE, imagePropertyMap);

    imageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    imageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    imageView.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

    return imageView;
  }

  /**
   * To prevent the next button being pressed before an Image has loaded the Button can br disabled.
   * This function will disable the next button.
   * Connecting to the ResourceReady signal with ( ResourceReadySignal( Control control ) ) will allow enabling of the button again.
   */

  void DisableButtonWhilstLoading()
  {
    mNextButton.SetProperty(Button::Property::DISABLED, true);
  }

  /**
   * Example shows loading an Image with exif orientation data but not applying automatic orientation correction
   */
  void OrientationCorrectionExampleNoCorrection()
  {
    mTitle.SetProperty(Toolkit::TextLabel::Property::TEXT, "Orientation Correction");
    mInstructions.SetProperty(TextLabel::Property::TEXT, "Orientation Correction not applied");
    mTable.RemoveChildAt(TableView::CellPosition(TableRowPlacement::IMAGE, 0));
    DisableButtonWhilstLoading();
    ImageView imageView01 = CreateImageView(false, ImageVisual::LoadPolicy::ATTACHED, ImageVisual::ReleasePolicy::DESTROYED, false, 3);
    imageView01.ResourceReadySignal().Connect(this, &ImagePolicies::ResourceReadySignal);

    mTable.AddChild(imageView01, TableView::CellPosition(TableRowPlacement::IMAGE, 0));
  }

  /**
   * Example shows loading an Image with exif orientation data and automatically correcting the orientation to match the exif data.
   */
  void OrientationCorrectionExampleWithCorrection()
  {
    mInstructions.SetProperty(TextLabel::Property::TEXT, "Orientation Correction applied based on Exif data, now shown in landscape");
    mTable.RemoveChildAt(TableView::CellPosition(TableRowPlacement::IMAGE, 0));
    DisableButtonWhilstLoading();
    ImageView imageView01 = CreateImageView(true, ImageVisual::LoadPolicy::ATTACHED, ImageVisual::ReleasePolicy::DESTROYED, false, 3);
    imageView01.ResourceReadySignal().Connect(this, &ImagePolicies::ResourceReadySignal);
    mTable.AddChild(imageView01, TableView::CellPosition(TableRowPlacement::IMAGE, 0));
  }

  /**
   * Part One of the Immediate loading example, displays instructions on what will be shown and starts loading of the Immediate image before it
   * is windowd.
   */
  void LoadPolicyImmediateExampleInstructions()
  {
    mTitle.SetProperty(Toolkit::TextLabel::Property::TEXT, "Immediate Loading Policy");
    mTable.RemoveChildAt(TableView::CellPosition(TableRowPlacement::IMAGE, 0));
    mInstructions.SetProperty(TextLabel::Property::TEXT, "Loading Image before staging, press next to see it in right column");
    TableView dualImageViewTable = TableView::New(1, 2);
    dualImageViewTable.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    dualImageViewTable.SetProperty(Dali::Actor::Property::NAME, "dualTable");
    dualImageViewTable.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    dualImageViewTable.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    dualImageViewTable.SetCellPadding(Vector2(6.0f, 0.0f));
    TextLabel attached = TextLabel::New("ATTACHED loaded image \nWill appear here");
    attached.SetProperty(TextLabel::Property::MULTI_LINE, true);
    TextLabel immediate = TextLabel::New("IMMEDIATE loaded image \nWill appear here");
    immediate.SetProperty(TextLabel::Property::MULTI_LINE, true);

    dualImageViewTable.AddChild(attached, TableView::CellPosition(0, 0));
    dualImageViewTable.AddChild(immediate, TableView::CellPosition(0, 1));
    mTable.AddChild(dualImageViewTable, TableView::CellPosition(TableRowPlacement::IMAGE, 0));

    DisableButtonWhilstLoading();
    mPersistantImageView = CreateImageView(true, ImageVisual::LoadPolicy::IMMEDIATE, ImageVisual::ReleasePolicy::DESTROYED, false, 4);
    mPersistantImageView.ResourceReadySignal().Connect(this, &ImagePolicies::ResourceReadySignal);
  }

  /**
   * Part Two of the Immediate loading example and here the Image is windowd, shown to be instant (if previously step gave enough time to load).
   */
  void LoadPolicyImmediateExample()
  {
    mInstructions.SetProperty(TextLabel::Property::TEXT, "Immediate loading policy on only second column hence image load was almost instant.");
    DisableButtonWhilstLoading();
    mTable.RemoveChildAt(TableView::CellPosition(TableRowPlacement::IMAGE, 0));

    TableView dualImageViewTable = TableView::New(2, 2);
    dualImageViewTable.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    dualImageViewTable.SetProperty(Dali::Actor::Property::NAME, "dualTable");
    dualImageViewTable.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    dualImageViewTable.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    dualImageViewTable.SetCellPadding(Vector2(6.0f, 0.0f));

    TextLabel attached  = TextLabel::New("ATTACHED");
    TextLabel immediate = TextLabel::New("IMMEDIATE");
    dualImageViewTable.AddChild(attached, TableView::CellPosition(1, 0));
    dualImageViewTable.AddChild(immediate, TableView::CellPosition(1, 1));
    dualImageViewTable.SetFitHeight(1);

    mTable.AddChild(dualImageViewTable, TableView::CellPosition(TableRowPlacement::IMAGE, 0));

    ImageView imageView02 = CreateImageView(true, ImageVisual::LoadPolicy::ATTACHED, ImageVisual::ReleasePolicy::DESTROYED, false, 3);
    imageView02.ResourceReadySignal().Connect(this, &ImagePolicies::ResourceReadySignal);
    dualImageViewTable.AddChild(imageView02, TableView::CellPosition(0, 0));
    dualImageViewTable.AddChild(mPersistantImageView, TableView::CellPosition(0, 1));
  }

  /**
   * Part one of an example of loading time when an Image is destroyed with the ReleasePolicy DESTROYED.
   */
  void LoadPolicyDestroyedExample()
  {
    mTitle.SetProperty(Toolkit::TextLabel::Property::TEXT, "Release Policy DESTROYED");
    mTable.RemoveChildAt(TableView::CellPosition(TableRowPlacement::IMAGE, 0));
    mPersistantImageView.Reset();
    mInstructions.SetProperty(TextLabel::Property::TEXT, "ReleasePolicy::DESTROYED shown in first column, press next to destroy it.");
    DisableButtonWhilstLoading();
    TableView dualImageViewTable = TableView::New(1, 2);
    dualImageViewTable.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    dualImageViewTable.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    dualImageViewTable.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    dualImageViewTable.SetCellPadding(Vector2(6.0f, 0.0f));

    mTable.AddChild(dualImageViewTable, TableView::CellPosition(TableRowPlacement::IMAGE, 0));

    ImageView imageView01 = CreateImageView(true, ImageVisual::LoadPolicy::ATTACHED, ImageVisual::ReleasePolicy::DESTROYED, false, 3);
    imageView01.ResourceReadySignal().Connect(this, &ImagePolicies::ResourceReadySignal);

    dualImageViewTable.AddChild(imageView01, TableView::CellPosition(0, 0));
  }

  /**
   * Part two of the Destroyed example, the image is windowd again but to the second column, it shows the loading takes the same amount of time as
   * when first loaded.
   */
  void LoadPolicyDestroyedExample02()
  {
    mInstructions.SetProperty(TextLabel::Property::TEXT, "Destroyed first image and reloaded in second column (loading took some time)");
    mTable.RemoveChildAt(TableView::CellPosition(TableRowPlacement::IMAGE, 0));
    DisableButtonWhilstLoading();
    TableView dualImageViewTable = TableView::New(1, 2);
    dualImageViewTable.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    dualImageViewTable.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    dualImageViewTable.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    dualImageViewTable.SetCellPadding(Vector2(6.0f, 0.0f));

    mTable.AddChild(dualImageViewTable, TableView::CellPosition(TableRowPlacement::IMAGE, 0));

    ImageView imageView01 = CreateImageView(true, ImageVisual::LoadPolicy::ATTACHED, ImageVisual::ReleasePolicy::DESTROYED, false, 3);
    imageView01.ResourceReadySignal().Connect(this, &ImagePolicies::ResourceReadySignal);

    dualImageViewTable.AddChild(imageView01, TableView::CellPosition(0, 1));
  }

  /**
   * Part one the second Release policy example showing detachment of a visual with the Destroyed policy and loading instantly when re-used.
   */
  void ReleasePolicyDestroyedExample03()
  {
    mTitle.SetProperty(Toolkit::TextLabel::Property::TEXT, "Detaching with DESTROYED Policy");
    mTable.RemoveChildAt(TableView::CellPosition(TableRowPlacement::IMAGE, 0));
    mInstructions.SetProperty(TextLabel::Property::TEXT, "Image with ReleasePolicy::DESTROYED shown in first column, Image will be detached, reusing it will be fast");
    DisableButtonWhilstLoading();
    TableView dualImageViewTable = TableView::New(1, 2);
    dualImageViewTable.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    dualImageViewTable.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    dualImageViewTable.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    dualImageViewTable.SetCellPadding(Vector2(6.0f, 0.0f));

    mTable.AddChild(dualImageViewTable, TableView::CellPosition(TableRowPlacement::IMAGE, 0));

    ImageView imageView01 = CreateImageView(true, ImageVisual::LoadPolicy::ATTACHED, ImageVisual::ReleasePolicy::DESTROYED, false, 3);
    imageView01.ResourceReadySignal().Connect(this, &ImagePolicies::ResourceReadySignal);

    dualImageViewTable.AddChild(imageView01, TableView::CellPosition(0, 0));
  }

  /**
   * Second part of the second Release policy example, the detached visual is used again in the second column and shown nearly instantly.
   */
  void ReleasePolicyDestroyedExample04()
  {
    mInstructions.SetProperty(TextLabel::Property::TEXT, "Detached first image and reloaded in second column, loading should have seemed instant");
    DisableButtonWhilstLoading();
    TableView dualImageViewTable = TableView::DownCast(mTable.GetChildAt(TableView::CellPosition(TableRowPlacement::IMAGE, 0)));
    ImageView imageViewDetached  = ImageView::DownCast(dualImageViewTable.GetChildAt(TableView::CellPosition(0, 0)));
    dualImageViewTable.RemoveChildAt(TableView::CellPosition(0, 0));

    ImageView imageView01 = CreateImageView(true, ImageVisual::LoadPolicy::ATTACHED, ImageVisual::ReleasePolicy::DESTROYED, false, 3);
    imageView01.ResourceReadySignal().Connect(this, &ImagePolicies::ResourceReadySignal);

    dualImageViewTable.AddChild(imageView01, TableView::CellPosition(0, 1));
  }

  /**
   * Part one of an example of loading time when an Image is detached with the ReleasePolicy Detached.
   */
  void ReleasePolicyDestroyedExample05()
  {
    mTitle.SetProperty(Toolkit::TextLabel::Property::TEXT, "Detaching with DETACHED Policy");
    mTable.RemoveChildAt(TableView::CellPosition(TableRowPlacement::IMAGE, 0));
    mInstructions.SetProperty(TextLabel::Property::TEXT, "Image with ReleasePolicy::DETACHED shown in first column, will be detached and reloaded");
    DisableButtonWhilstLoading();
    TableView dualImageViewTable = TableView::New(1, 2);
    dualImageViewTable.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    dualImageViewTable.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    dualImageViewTable.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    dualImageViewTable.SetCellPadding(Vector2(6.0f, 0.0f));

    mTable.AddChild(dualImageViewTable, TableView::CellPosition(TableRowPlacement::IMAGE, 0));

    ImageView imageView01 = CreateImageView(true, ImageVisual::LoadPolicy::ATTACHED, ImageVisual::ReleasePolicy::DETACHED, false, 3);
    imageView01.ResourceReadySignal().Connect(this, &ImagePolicies::ResourceReadySignal);

    dualImageViewTable.AddChild(imageView01, TableView::CellPosition(0, 0));
  }

  /**
   * Part two of the Detached example, the image is windowd again but to the second column, it shows the loading takes the same amount of time as
   * when first loaded.
   */
  void ReleasePolicyDestroyedExample06()
  {
    mInstructions.SetProperty(TextLabel::Property::TEXT, "Detached first image and reloaded in second column, loading took some time");
    DisableButtonWhilstLoading();
    TableView dualImageViewTable = TableView::DownCast(mTable.GetChildAt(TableView::CellPosition(TableRowPlacement::IMAGE, 0)));
    ImageView imageViewDetached  = ImageView::DownCast(dualImageViewTable.GetChildAt(TableView::CellPosition(0, 0)));
    dualImageViewTable.RemoveChildAt(TableView::CellPosition(0, 0));

    ImageView imageView01 = CreateImageView(true, ImageVisual::LoadPolicy::ATTACHED, ImageVisual::ReleasePolicy::DESTROYED, false, 3);
    imageView01.ResourceReadySignal().Connect(this, &ImagePolicies::ResourceReadySignal);

    dualImageViewTable.AddChild(imageView01, TableView::CellPosition(0, 1));
  }

  /**
   * Created a gradient property map that will produce a Gradient Visual
   * param[out] gradientMap the output property map
   */
  void CreateGradient(Property::Map& gradientMap)
  {
    gradientMap.Insert(Toolkit::Visual::Property::TYPE, Visual::GRADIENT);

    Property::Array stopOffsets;
    stopOffsets.PushBack(0.0f);
    stopOffsets.PushBack(0.6f);
    stopOffsets.PushBack(1.0f);
    gradientMap.Insert(GradientVisual::Property::STOP_OFFSET, stopOffsets);

    Property::Array stopColors;
    stopColors.PushBack(Vector4(54.f, 140.f, 207.f, 223.f) / 255.f);
    stopColors.PushBack(Vector4(54.f, 170.f, 207.f, 123.f) / 255.f);
    stopColors.PushBack(Vector4(54.f, 189.f, 207.f, 123.f) / 255.f);
    gradientMap.Insert(GradientVisual::Property::STOP_COLOR, stopColors);

    gradientMap.Insert(GradientVisual::Property::START_POSITION, Vector2(0.5f, 0.5f));
    gradientMap.Insert(GradientVisual::Property::END_POSITION, Vector2(-0.5f, -0.5f));
  }

  /**
   * Start of this example, called once when the application is initiated
   */
  void Create(Application& application)
  {
    Property::Map gradientBackground;
    CreateGradient(gradientBackground);

    // Get a handle to the window
    Window window = application.GetWindow();

    // Create default View.
    Toolkit::Control view = Toolkit::Control::New();
    view.SetProperty(Actor::Property::ANCHOR_POINT, Dali::AnchorPoint::CENTER);
    view.SetProperty(Actor::Property::PARENT_ORIGIN, Dali::ParentOrigin::CENTER);
    view.SetResizePolicy(Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::ALL_DIMENSIONS);
    view.SetProperty(Toolkit::Control::Property::BACKGROUND, gradientBackground);
    window.Add(view);

    // Create a table view to show a pair of buttons above each image.
    mTable = TableView::New(TableRowPlacement::NUMBER_OF_ROWS, 1);
    mTable.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mTable.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mTable.SetResizePolicy(ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS);
    Vector3 offset(0.9f, 0.90f, 0.0f);
    mTable.SetProperty(Actor::Property::SIZE_MODE_FACTOR, offset);
    mTable.SetFitHeight(TableRowPlacement::NEXT_BUTTON);
    mTable.SetFitHeight(TableRowPlacement::LOADING_STATUS);
    view.Add(mTable);

    // Create Next button
    mNextButton = PushButton::New();
    Property::Map imagePropertyMap;
    imagePropertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
    imagePropertyMap.Insert(ImageVisual::Property::URL, NEXT_BUTTON_IMAGE);
    mNextButton.SetProperty(Button::Property::UNSELECTED_BACKGROUND_VISUAL, imagePropertyMap);
    imagePropertyMap.Clear();
    imagePropertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
    imagePropertyMap.Insert(ImageVisual::Property::URL, NEXT_BUTTON_PRESSED_IMAGE);
    mNextButton.SetProperty(Button::Property::SELECTED_BACKGROUND_VISUAL, imagePropertyMap);
    mNextButton.SetProperty(Button::Property::DISABLED_UNSELECTED_BACKGROUND_VISUAL, LOADING_IMAGE);
    mNextButton.SetProperty(Button::Property::DISABLED_SELECTED_BACKGROUND_VISUAL, NEXT_BUTTON_DISABLED_IMAGE);
    mNextButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
    mNextButton.SetProperty(Actor::Property::POSITION_Y, -50.0f);
    mNextButton.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
    mNextButton.ClickedSignal().Connect(this, &ImagePolicies::ChangeImageClicked);
    mTable.AddChild(mNextButton, TableView::CellPosition(TableRowPlacement::NEXT_BUTTON, 0));
    mTable.SetCellPadding(Vector2(2.0f, 2.0f));

    window.KeyEventSignal().Connect(this, &ImagePolicies::OnKeyEvent);

    // Outline Map for Labels
    Property::Map outlineMap;
    outlineMap["color"] = Color::BLACK;
    outlineMap["width"] = 1.0f;

    // Create Title Label
    mTitle = TextLabel::New("Image Polices");
    mTitle.SetProperty(TextLabel::Property::TEXT_COLOR, Color::CYAN);
    mTable.AddChild(mTitle, TableView::CellPosition(TableRowPlacement::TITLE, 0));
    mTable.SetFitHeight(TableRowPlacement::TITLE);

    // Create Instructions label
    mInstructions = TextLabel::New("This is an explaination of each example");
    mInstructions.SetProperty(TextLabel::Property::MULTI_LINE, true);
    mInstructions.SetProperty(TextLabel::Property::TEXT_COLOR, Color::WHITE);
    mInstructions.SetProperty(TextLabel::Property::OUTLINE, outlineMap);
    mInstructions.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    mTable.AddChild(mInstructions, TableView::CellPosition(TableRowPlacement::INSTRUCTIONS, 0));
    float value = mInstructions.GetProperty<float>(TextLabel::Property::PIXEL_SIZE);
    mTable.SetFixedHeight(TableRowPlacement::INSTRUCTIONS, value * 5); // Space allocated for example instructions

    ChangeImageClicked(mNextButton); // Start examples ( 0 )
  }

private:
  /**
   * Callback to the button clicked signal and starts the next example.
   */
  bool ChangeImageClicked(Button button)
  {
    switch(mExampleIndex++)
    {
      case 0:
      {
        OrientationCorrectionExampleNoCorrection();
        break;
      }
      case 1:
      {
        OrientationCorrectionExampleWithCorrection();
        break;
      }
      case 2:
      {
        LoadPolicyImmediateExampleInstructions();
        break;
      }
      case 3:
      {
        LoadPolicyImmediateExample();
        break;
      }
      case 4:
      {
        LoadPolicyDestroyedExample();
        break;
      }
      case 5:
      {
        LoadPolicyDestroyedExample02();
        break;
      }
      case 6:
      {
        ReleasePolicyDestroyedExample03();
        break;
      }
      case 7:
      {
        ReleasePolicyDestroyedExample04();
        break;
      }
      case 8:
      {
        ReleasePolicyDestroyedExample05();
        break;
      }
      case 9:
      {
        ReleasePolicyDestroyedExample06();
        // Change Next button to complete button ( will quit app once pressed )
        button.SetProperty(Button::Property::UNSELECTED_BACKGROUND_VISUAL, OK_IMAGE_IMAGE);
        button.SetProperty(Button::Property::SELECTED_BACKGROUND_VISUAL, OK_IMAGE_IMAGE);
        break;
      }
      default:
      {
        mApplication.Quit();
        break;
      }
    }
    return true;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication;

  TableView  mTable;
  TextLabel  mInstructions;
  TextLabel  mTitle;
  PushButton mNextButton;
  ImageView  mPersistantImageView;

  unsigned int mExampleIndex;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application   application = Application::New(&argc, &argv, DEMO_THEME_PATH);
  ImagePolicies test(application);
  application.MainLoop();
  return 0;
}
