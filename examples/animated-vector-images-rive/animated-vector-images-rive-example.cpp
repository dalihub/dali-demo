/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/visuals/animated-vector-image-visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/animated-vector-image-visual-signals-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali/dali.h>
#include <string>
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* BACKGROUND_IMAGE(DEMO_IMAGE_DIR "background-gradient.jpg");
const char* TOOLBAR_IMAGE(DEMO_IMAGE_DIR "top-bar.png");
const char* APPLICATION_TITLE("Animated Vector Images Rive");

const char* IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "juice.riv"
  };

const unsigned int NUMBER_OF_IMAGES = 1;

enum CellPlacement
{
  TOP_BUTTON,
  LOWER_BUTTON,
  IMAGE,
  NUMBER_OF_ROWS
};

unsigned int GetControlIndex(Control control)
{
  std::string  controlName = control.GetProperty<std::string>(Dali::Actor::Property::NAME);
  unsigned int index       = 0;

  if(controlName != "")
  {
    index = std::stoul(controlName);
  }

  return index;
}

} // namespace

// This example shows the usage of AnimatedVectorImageVisual(Rive).
// It doesn't work on Ubuntu because the visual uses the external library to render frames.
class AnimatedVectorImageViewController : public ConnectionTracker
{
public:
  AnimatedVectorImageViewController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &AnimatedVectorImageViewController::Create);
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
    mTable = TableView::New(CellPlacement::NUMBER_OF_ROWS, NUMBER_OF_IMAGES);
    mTable.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mTable.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mTable.SetResizePolicy(ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS);
    Vector3 offset(0.9f, 0.70f, 0.0f);
    mTable.SetProperty(Actor::Property::SIZE_MODE_FACTOR, offset);
    mTable.SetFitHeight(CellPlacement::TOP_BUTTON);
    mTable.SetFitHeight(CellPlacement::LOWER_BUTTON);
    mContentLayer.Add(mTable);

    for(unsigned int x = 0; x < NUMBER_OF_IMAGES; x++)
    {
      mPlayButtons[x] = PushButton::New();
      mPlayButtons[x].SetProperty(Button::Property::LABEL, "Play");
      mPlayButtons[x].SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
      mPlayButtons[x].SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
      mPlayButtons[x].ClickedSignal().Connect(this, &AnimatedVectorImageViewController::OnPlayButtonClicked);
      mPlayButtons[x].SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      mPlayButtons[x].SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
      std::string s = std::to_string(x);
      mPlayButtons[x].SetProperty(Dali::Actor::Property::NAME, s);
      mTable.AddChild(mPlayButtons[x], TableView::CellPosition(CellPlacement::TOP_BUTTON, x));

      mStopButtons[x] = PushButton::New();
      mStopButtons[x].SetProperty(Button::Property::LABEL, "Stop");
      mStopButtons[x].SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
      mStopButtons[x].SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
      mStopButtons[x].ClickedSignal().Connect(this, &AnimatedVectorImageViewController::OnStopButtonClicked);
      mStopButtons[x].SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      mStopButtons[x].SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
      mStopButtons[x].SetProperty(Dali::Actor::Property::NAME, s);
      mTable.AddChild(mStopButtons[x], TableView::CellPosition(CellPlacement::LOWER_BUTTON, x));

      mImageViews[x] = ImageView::New();
      Property::Map imagePropertyMap;
      imagePropertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
      imagePropertyMap.Insert(ImageVisual::Property::URL, IMAGE_PATH[x]);
      imagePropertyMap.Insert(DevelImageVisual::Property::LOOP_COUNT, 3);
      mImageViews[x].SetProperty(ImageView::Property::IMAGE, imagePropertyMap);

      mImageViews[x].SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      mImageViews[x].SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      mImageViews[x].SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
      mImageViews[x].SetProperty(Dali::Actor::Property::NAME, s);

      DevelControl::VisualEventSignal(mImageViews[x]).Connect(this, &AnimatedVectorImageViewController::OnVisualEvent);

      mTable.AddChild(mImageViews[x], TableView::CellPosition(CellPlacement::IMAGE, x));
    }

    application.GetWindow().KeyEventSignal().Connect(this, &AnimatedVectorImageViewController::OnKeyEvent);
  }

private:
  bool OnPlayButtonClicked(Button button)
  {
    unsigned int controlIndex = GetControlIndex(button);

    ImageView imageView = mImageViews[controlIndex];

    Property::Map    map   = imageView.GetProperty<Property::Map>(ImageView::Property::IMAGE);
    Property::Value* value = map.Find(DevelImageVisual::Property::PLAY_STATE);

    if(value)
    {
      if(value->Get<int>() != static_cast<int>(DevelImageVisual::PlayState::PLAYING))
      {
        mPlayButtons[controlIndex].SetProperty(Button::Property::LABEL, "Pause");

        DevelControl::DoAction(imageView, ImageView::Property::IMAGE, DevelAnimatedVectorImageVisual::Action::PLAY, Property::Value());
      }
      else
      {
        mPlayButtons[controlIndex].SetProperty(Button::Property::LABEL, "Play");

        DevelControl::DoAction(imageView, ImageView::Property::IMAGE, DevelAnimatedVectorImageVisual::Action::PAUSE, Property::Value());
      }
    }

    return true;
  }

  bool OnStopButtonClicked(Button button)
  {
    unsigned int controlIndex = GetControlIndex(button);
    ImageView    imageView    = mImageViews[controlIndex];
    DevelControl::DoAction(imageView, ImageView::Property::IMAGE, DevelAnimatedVectorImageVisual::Action::STOP, Property::Value());

    return true;
  }

  void OnVisualEvent(Control control, Dali::Property::Index visualIndex, Dali::Property::Index signalId)
  {
    unsigned int controlIndex = GetControlIndex(control);

    if(visualIndex == ImageView::Property::IMAGE && signalId == DevelAnimatedVectorImageVisual::Signal::ANIMATION_FINISHED)
    {
      mPlayButtons[controlIndex].SetProperty(Button::Property::LABEL, "Play");
    }
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

  Control    mView;         ///< The View instance.
  ToolBar    mToolBar;      ///< The View's Toolbar.
  Layer      mContentLayer; ///< Content layer
  TableView  mTable;
  ImageView  mImageViews[NUMBER_OF_IMAGES];
  PushButton mPlayButtons[NUMBER_OF_IMAGES];
  PushButton mStopButtons[NUMBER_OF_IMAGES];
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                       application = Application::New(&argc, &argv, DEMO_THEME_PATH);
  AnimatedVectorImageViewController test(application);
  application.MainLoop();
  return 0;
}
