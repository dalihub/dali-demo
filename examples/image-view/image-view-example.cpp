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
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali/dali.h>
#include <string>
#include "shared/view.h"

using namespace Dali;

namespace
{
const char* BACKGROUND_IMAGE(DEMO_IMAGE_DIR "background-gradient.jpg");
const char* TOOLBAR_IMAGE(DEMO_IMAGE_DIR "top-bar.png");
const char* APPLICATION_TITLE("Image view");

const char* IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "gallery-small-23.jpg",
  DEMO_IMAGE_DIR "woodEffect.jpg",
  DEMO_IMAGE_DIR "wood.png", // 32bits image
  DEMO_IMAGE_DIR "heartsframe.9.png",
  DEMO_IMAGE_DIR "World.svg"};

const unsigned int NUMBER_OF_IMAGES = 3;

enum CellPlacement
{
  TOP_BUTTON,
  MID_BUTTON,
  LOWER_BUTTON,
  IMAGE,
  NUMBER_OF_ROWS
};

unsigned int GetButtonIndex(Toolkit::Button button)
{
  std::string  buttonName = button.GetProperty<std::string>(Dali::Actor::Property::NAME);
  unsigned int index      = 0;

  if(buttonName != "")
  {
    index = std::stoul(buttonName);
  }

  return index;
}

const unsigned int NUMBER_OF_RESOURCES = sizeof(IMAGE_PATH) / sizeof(char*);

std::string EXAMPLE_INSTRUCTIONS =
  "Instructions: Change button cycles through different image visuals, "
  "on/off takes the ImageView and it's current visual on or off window.";

const float CORNER_RADIUS_VALUE(20.0f);
const float BORDERLINE_WIDTH_VALUE(10.0f);
const float BORDERLINE_OFFSET_VALUE(-1.0f); ///< draw borderline inside of imageview.
enum {
  STATUS_NORMAL             = 0,
  STATUS_ROUND              = 1,
  STATUS_BORDERLINE         = 2,
  STATUS_ROUNDED_BORDERLINE = STATUS_ROUND | STATUS_BORDERLINE,
  NUMBER_OF_STATUS,
};
const char* BUTTON_LABEL[NUMBER_OF_STATUS] = {
  "Normal",
  "Round",
  "Borderline",
  "RoundBorderline",
};

} // namespace

class ImageViewController : public ConnectionTracker
{
public:
  ImageViewController(Application& application)
  : mApplication(application),
    mCurrentPositionToggle(0, 0),
    mCurrentPositionImage(0, 0)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ImageViewController::Create);
  }

  ~ImageViewController()
  {
    // Nothing to do here
  }

  void Create(Application& application)
  {
    // The Init signal is received once (only) during the Application lifetime

    // Creates a default view with a default tool bar.
    // The view is added to the window.
    mContentLayer = DemoHelper::CreateView(application,
                                           mView,
                                           mToolBar,
                                           BACKGROUND_IMAGE,
                                           TOOLBAR_IMAGE,
                                           APPLICATION_TITLE);

    // Create a table view to show a pair of buttons above each image.
    mTable = Toolkit::TableView::New(CellPlacement::NUMBER_OF_ROWS, NUMBER_OF_IMAGES);
    mTable.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mTable.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mTable.SetResizePolicy(ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS);
    Vector3 offset(0.9f, 0.70f, 0.0f);
    mTable.SetProperty(Actor::Property::SIZE_MODE_FACTOR, offset);
    mTable.SetFitHeight(CellPlacement::TOP_BUTTON);
    mTable.SetFitHeight(CellPlacement::MID_BUTTON);
    mTable.SetFitHeight(CellPlacement::LOWER_BUTTON);
    mContentLayer.Add(mTable);

    Toolkit::TextLabel instructions = Toolkit::TextLabel::New(EXAMPLE_INSTRUCTIONS);
    instructions.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    instructions.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
    instructions.SetProperty(Actor::Property::POSITION_Y, -50.0f);
    instructions.SetProperty(Toolkit::TextLabel::Property::ENABLE_AUTO_SCROLL, true);
    instructions.SetProperty(Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 10);
    mContentLayer.Add(instructions);

    for(unsigned int x = 0; x < NUMBER_OF_IMAGES; x++)
    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetProperty(Toolkit::Button::Property::LABEL, "on/off");
      button.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
      button.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
      button.ClickedSignal().Connect(this, &ImageViewController::ToggleImageOnWindow);
      button.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      button.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
      std::string s = std::to_string(x);
      button.SetProperty(Dali::Actor::Property::NAME, s);
      mTable.AddChild(button, Toolkit::TableView::CellPosition(CellPlacement::TOP_BUTTON, x));

      Toolkit::PushButton button2 = Toolkit::PushButton::New();
      button2.SetProperty(Toolkit::Button::Property::LABEL, "Change");
      button2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
      button2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
      button2.ClickedSignal().Connect(this, &ImageViewController::ChangeImageClicked);
      button2.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      button2.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
      button2.SetProperty(Dali::Actor::Property::NAME, s);
      mTable.AddChild(button2, Toolkit::TableView::CellPosition(CellPlacement::MID_BUTTON, x));

      Toolkit::PushButton button3 = Toolkit::PushButton::New();
      button3.SetProperty(Toolkit::Button::Property::LABEL, BUTTON_LABEL[STATUS_NORMAL]);
      button3.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
      button3.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
      button3.ClickedSignal().Connect(this, &ImageViewController::RoundedCornerClicked);
      button3.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      button3.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
      button3.SetProperty(Dali::Actor::Property::NAME, s);
      mTable.AddChild(button3, Toolkit::TableView::CellPosition(CellPlacement::LOWER_BUTTON, x));

      mImageViews[x] = Toolkit::ImageView::New();
      Property::Map imagePropertyMap;
      imagePropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
      imagePropertyMap.Insert(Toolkit::ImageVisual::Property::URL, IMAGE_PATH[0]);
      mImageViews[x].SetProperty(Toolkit::ImageView::Property::IMAGE, imagePropertyMap);

      mImageViews[x].SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      mImageViews[x].SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      mImageViews[x].SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
      mTable.AddChild(mImageViews[x], Toolkit::TableView::CellPosition(CellPlacement::IMAGE, x));

      // Set changeable counter and toggle for each ImageView
      mImageViewImageIndexStatus[x]    = 0;
      mImageViewToggleStatus[x]        = true;
      mImageViewRoundedCornerStatus[x] = STATUS_NORMAL;
    }

    application.GetWindow().KeyEventSignal().Connect(this, &ImageViewController::OnKeyEvent);
  }

private:
  void ImmediateLoadImage(const char* urlToLoad)
  {
    Property::Map imagePropertyMap;
    imagePropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
    imagePropertyMap.Insert(Toolkit::ImageVisual::Property::URL, urlToLoad);
    Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual(imagePropertyMap);
    visual.Reset();
  }

  bool ToggleImageOnWindow(Toolkit::Button button)
  {
    unsigned int buttonIndex = GetButtonIndex(button);

    Toolkit::ImageView imageView = mImageViews[buttonIndex];

    if(mImageViewToggleStatus[buttonIndex])
    {
      imageView.Unparent();
    }
    else
    {
      mTable.AddChild(imageView, Toolkit::TableView::CellPosition(CellPlacement::IMAGE, GetButtonIndex(button)));
    }

    mImageViewToggleStatus[buttonIndex] = !mImageViewToggleStatus[buttonIndex];

    return true;
  }

  bool ChangeImageClicked(Toolkit::Button button)
  {
    unsigned int buttonIndex = GetButtonIndex(button);

    if(mImageViews[buttonIndex].GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
    {
      ++mImageViewImageIndexStatus[buttonIndex];

      if(mImageViewImageIndexStatus[buttonIndex] == NUMBER_OF_RESOURCES)
      {
        mImageViewImageIndexStatus[buttonIndex] = 0;
      }

      Property::Map imagePropertyMap;
      imagePropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
      imagePropertyMap.Insert(Toolkit::ImageVisual::Property::URL, IMAGE_PATH[mImageViewImageIndexStatus[buttonIndex]]);
      if(mImageViewRoundedCornerStatus[buttonIndex] & STATUS_ROUND)
      {
        imagePropertyMap.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, CORNER_RADIUS_VALUE);
      }
      if(mImageViewRoundedCornerStatus[buttonIndex] & STATUS_BORDERLINE)
      {
        imagePropertyMap.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, BORDERLINE_WIDTH_VALUE);
        imagePropertyMap.Insert(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, BORDERLINE_OFFSET_VALUE);
      }

      mImageViews[buttonIndex].SetProperty(Toolkit::ImageView::Property::IMAGE, imagePropertyMap);
    }
    return true;
  }

  bool RoundedCornerClicked(Toolkit::Button button)
  {
    unsigned int buttonIndex = GetButtonIndex(button);

    if(mImageViews[buttonIndex].GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
    {
      mImageViewRoundedCornerStatus[buttonIndex] = (mImageViewRoundedCornerStatus[buttonIndex] + 1) % NUMBER_OF_STATUS;

      button.SetProperty(Toolkit::Button::Property::LABEL, BUTTON_LABEL[mImageViewRoundedCornerStatus[buttonIndex]]);

      Property::Map imagePropertyMap;
      imagePropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
      imagePropertyMap.Insert(Toolkit::ImageVisual::Property::URL, IMAGE_PATH[mImageViewImageIndexStatus[buttonIndex]]);
      if(mImageViewRoundedCornerStatus[buttonIndex] & STATUS_ROUND)
      {
        imagePropertyMap.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, CORNER_RADIUS_VALUE);
      }
      if(mImageViewRoundedCornerStatus[buttonIndex] & STATUS_BORDERLINE)
      {
        imagePropertyMap.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, BORDERLINE_WIDTH_VALUE);
        imagePropertyMap.Insert(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, BORDERLINE_OFFSET_VALUE);
      }

      mImageViews[buttonIndex].SetProperty(Toolkit::ImageView::Property::IMAGE, imagePropertyMap);
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

  Toolkit::Control   mView;         ///< The View instance.
  Toolkit::ToolBar   mToolBar;      ///< The View's Toolbar.
  Layer              mContentLayer; ///< Content layer
  Toolkit::TableView mTable;
  Toolkit::ImageView mImageViews[NUMBER_OF_IMAGES];
  bool               mImageViewToggleStatus[NUMBER_OF_IMAGES];
  unsigned int       mImageViewRoundedCornerStatus[NUMBER_OF_IMAGES];
  unsigned int       mImageViewImageIndexStatus[NUMBER_OF_IMAGES];

  Toolkit::TableView::CellPosition mCurrentPositionToggle;
  Toolkit::TableView::CellPosition mCurrentPositionImage;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application         application = Application::New(&argc, &argv, DEMO_THEME_PATH);
  ImageViewController test(application);
  application.MainLoop();
  return 0;
}
