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

#include <sstream>
#include "shared/view.h"

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const int NUM_FLEX_ITEMS = 8;

const char* BACKGROUND_IMAGE( DEMO_IMAGE_DIR "background-default.png" );
const char* TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );

const DemoHelper::ViewStyle VIEW_STYLE( 0.08f, 0.45f, 80.f, 4.f );

const std::string FLEX_DIRECTION[] = {
    "column",
    "columnReverse",
    "row",
    "rowReverse"
};

const unsigned int NUM_FLEX_DIRECTION = sizeof(FLEX_DIRECTION) / sizeof(std::string);

const std::string FLEX_WRAP[] = {
    "noWrap",
    "Wrap"
};

const unsigned int NUM_FLEX_WRAP = sizeof(FLEX_WRAP) / sizeof(std::string);

const std::string CONTENT_DIRECTION[] = {
    "inherit",
    "LTR",
    "RTL"
};

const unsigned int NUM_CONTENT_DIRECTION = sizeof(CONTENT_DIRECTION) / sizeof(std::string);

const std::string JUSTIFY_CONTENT[] = {
    "flexStart",
    "center",
    "flexEnd",
    "spaceBetween",
    "spaceAround"
};

const unsigned int NUM_JUSTIFY_CONTENT = sizeof(JUSTIFY_CONTENT) / sizeof(std::string);

const std::string ALIGN_ITEMS[] = {
    "flexStart",
    "center",
    "flexEnd",
    "stretch"
};

const unsigned int NUM_ALIGN_ITEMS = sizeof(ALIGN_ITEMS) / sizeof(std::string);

const std::string ALIGN_CONTENT[] = {
    "flexStart",
    "center",
    "flexEnd",
    "stretch"
};

const unsigned int NUM_ALIGN_CONTENT = sizeof(ALIGN_CONTENT) / sizeof(std::string);

} // unnamed namespace

/**
 * This example demonstrates a proof of concept for FlexContainer UI control.
 * The flexbox properties can be changed by pressing different buttons in the
 * toolbar.
 */
class FlexContainerExample : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  FlexContainerExample( Application& application )
  : mApplication( application ),
    mCurrentFlexDirection( FlexContainer::ROW ),
    mCurrentFlexWrap( FlexContainer::WRAP ),
    mCurrentContentDirection( FlexContainer::INHERIT),
    mCurrentJustifyContent( FlexContainer::JUSTIFY_FLEX_START ),
    mCurrentAlignItems( FlexContainer::ALIGN_FLEX_START ),
    mCurrentAlignContent( FlexContainer::ALIGN_FLEX_START )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &FlexContainerExample::OnInit);
  }

  /**
   * This method gets called once the main loop of application is up and running
   */
  void OnInit(Application& app)
  {
    Stage stage = Dali::Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &FlexContainerExample::OnKeyEvent);
    stage.GetRootLayer().SetBehavior(Layer::LAYER_3D);

    Vector2 stageSize = Stage::GetCurrent().GetSize();

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Layer contents = DemoHelper::CreateView( mApplication,
                                             mView,
                                             mToolBar,
                                             BACKGROUND_IMAGE,
                                             TOOLBAR_IMAGE,
                                             "" );

    // Create a flex direction toggle button. (left of toolbar)
    mFlexDirectionButton = Toolkit::PushButton::New();
    mFlexDirectionButton.SetName("mFlexDirectionButton");
    mFlexDirectionButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, DEMO_IMAGE_DIR "icon-change.png" );
    mFlexDirectionButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, DEMO_IMAGE_DIR "icon-change-selected.png" );
    mFlexDirectionButton.ClickedSignal().Connect( this, &FlexContainerExample::OnFlexDirectionButtonClicked);
    mFlexDirectionButton.SetLeaveRequired( true );
    mToolBar.AddControl( mFlexDirectionButton, VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalLeft, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

    // Create a flex wrap toggle button. (left of toolbar)
    mFlexWrapButton = Toolkit::PushButton::New();
    mFlexWrapButton.SetName("mFlexWrapButton");
    mFlexWrapButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, DEMO_IMAGE_DIR "icon-edit.png" );
    mFlexWrapButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, DEMO_IMAGE_DIR "icon-edit-selected.png" );
    mFlexWrapButton.ClickedSignal().Connect( this, &FlexContainerExample::OnFlexWrapButtonClicked);
    mFlexWrapButton.SetLeaveRequired( true );
    mToolBar.AddControl( mFlexWrapButton, VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalLeft, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

    // Create a content direction toggle button. (left of toolbar)
    mContentDirectionButton = Toolkit::PushButton::New();
    mContentDirectionButton.SetName("mContentDirectionButton");
    mContentDirectionButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, DEMO_IMAGE_DIR "icon-replace.png" );
    mContentDirectionButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, DEMO_IMAGE_DIR "icon-replace-selected.png" );
    mContentDirectionButton.ClickedSignal().Connect( this, &FlexContainerExample::OnContentDirectionButtonClicked);
    mContentDirectionButton.SetLeaveRequired( true );
    mToolBar.AddControl( mContentDirectionButton, VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalLeft, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

    // Create a justify content toggle button. (right of toolbar)
    mJustifyContentButton = Toolkit::PushButton::New();
    mJustifyContentButton.SetName("mJustifyContentButton");
    mJustifyContentButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, DEMO_IMAGE_DIR "icon-reset.png" );
    mJustifyContentButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, DEMO_IMAGE_DIR "icon-reset-selected.png" );
    mJustifyContentButton.ClickedSignal().Connect( this, &FlexContainerExample::OnJustifyContentButtonClicked);
    mJustifyContentButton.SetLeaveRequired( true );
    mToolBar.AddControl( mJustifyContentButton, VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

    // Create a align items toggle button. (right of toolbar)
    mAlignItemsButton = Toolkit::PushButton::New();
    mAlignItemsButton.SetName("mAlignItemsButton");
    mAlignItemsButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, DEMO_IMAGE_DIR "icon-highp.png" );
    mAlignItemsButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, DEMO_IMAGE_DIR "icon-highp-selected.png" );
    mAlignItemsButton.ClickedSignal().Connect( this, &FlexContainerExample::OnAlignItemsButtonClicked);
    mAlignItemsButton.SetLeaveRequired( true );
    mToolBar.AddControl( mAlignItemsButton, VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

    // Create a align content toggle button. (right of toolbar)
    mAlignContentButton = Toolkit::PushButton::New();
    mAlignContentButton.SetName("mAlignContentButton");
    mAlignContentButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, DEMO_IMAGE_DIR "icon-effect-cross.png" );
    mAlignContentButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, DEMO_IMAGE_DIR "icon-effect-cross-selected.png" );
    mAlignContentButton.ClickedSignal().Connect( this, &FlexContainerExample::OnAlignContentButtonClicked);
    mAlignContentButton.SetLeaveRequired( true );
    mToolBar.AddControl( mAlignContentButton, VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

    // Create the base flex container
    mFlexContainer = FlexContainer::New();
    mFlexContainer.SetParentOrigin(ParentOrigin::TOP_LEFT);
    mFlexContainer.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    mFlexContainer.SetSize( stageSize.width, stageSize.height - VIEW_STYLE.mToolBarHeight );
    mFlexContainer.SetY(VIEW_STYLE.mToolBarHeight);
    mFlexContainer.SetProperty(FlexContainer::Property::FLEX_DIRECTION, FlexContainer::COLUMN); // column as main axis
    contents.Add(mFlexContainer);

    // Add a text label to the container for showing the recently updated flexbox property value
    mFlexPropertyLabel = TextLabel::New( FLEX_DIRECTION[mCurrentFlexDirection] );
    mFlexPropertyLabel.SetProperty(FlexContainer::ChildProperty::FLEX_MARGIN, Vector4(10.0f, 10.0f, 10.0f, 10.0f));
    mFlexPropertyLabel.SetProperty(FlexContainer::ChildProperty::FLEX, 0.05f); // 5 pecent of the container size in the main axis
    mFlexPropertyLabel.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    mFlexPropertyLabel.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
    mFlexContainer.Add( mFlexPropertyLabel );

    // Create the flex container for the flex items and add it to the base flex container
    mFlexItemContainer = FlexContainer::New();
    mFlexItemContainer.SetParentOrigin(ParentOrigin::TOP_LEFT);
    mFlexItemContainer.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    mFlexItemContainer.SetBackgroundColor( Color::YELLOW );
    mFlexItemContainer.SetProperty(FlexContainer::Property::FLEX_DIRECTION, mCurrentFlexDirection);
    mFlexItemContainer.SetProperty(FlexContainer::Property::FLEX_WRAP, mCurrentFlexWrap);
    mFlexItemContainer.SetProperty(FlexContainer::ChildProperty::FLEX, 0.95f); // 95 pecent of the container size in the main axis
    mFlexContainer.Add(mFlexItemContainer);

    // Create flex items and add them to the container
    for (int i = 0; i < NUM_FLEX_ITEMS; i++)
    {
      PushButton flexItem = PushButton::New();
      flexItem.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      flexItem.SetParentOrigin( ParentOrigin::TOP_LEFT );

      // Set different background colour to help to identify different items
      flexItem.SetBackgroundColor(Vector4(static_cast<float>(i) / NUM_FLEX_ITEMS, static_cast<float>(NUM_FLEX_ITEMS - i) / NUM_FLEX_ITEMS, 1.0f, 1.0f));
      flexItem.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, "" );
      flexItem.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, "" );

      // Add a label to the button so that we can identify each item more easily
      std::ostringstream index;
      index << i + 1;
      flexItem.SetProperty( Toolkit::Button::Property::LABEL, index.str() );
      flexItem.SetName("FlexItem " + index.str());

      // Set a fixed size to the items so that we can wrap the line and test these
      // flex properties that only work when there are multiple lines in the layout
      flexItem.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
      // Make sure there are still extra space in the line after wrapping
      flexItem.SetSize(stageSize.width / NUM_FLEX_ITEMS * 1.25f, (stageSize.height - VIEW_STYLE.mToolBarHeight) * 0.95f / NUM_FLEX_ITEMS * 1.25f);

      mFlexItemContainer.Add( flexItem );
    }

    // Update the title
    SetTitle( "Flex direction", FLEX_DIRECTION[mCurrentFlexDirection] );
  }

  bool OnFlexDirectionButtonClicked( Toolkit::Button button )
  {
    mCurrentFlexDirection = static_cast<FlexContainer::FlexDirection>( ( mCurrentFlexDirection + 1 ) % NUM_FLEX_DIRECTION );
    mFlexItemContainer.SetProperty(FlexContainer::Property::FLEX_DIRECTION, mCurrentFlexDirection);
    SetTitle( "Flex direction", FLEX_DIRECTION[mCurrentFlexDirection] );

    return true;
  }

  bool OnFlexWrapButtonClicked( Toolkit::Button button )
  {
    mCurrentFlexWrap = static_cast<FlexContainer::WrapType>( ( mCurrentFlexWrap + 1 ) % NUM_FLEX_WRAP );
    mFlexItemContainer.SetProperty(FlexContainer::Property::FLEX_WRAP, mCurrentFlexWrap);
    SetTitle( "Flex wrap", FLEX_WRAP[mCurrentFlexWrap] );

    return true;
  }

  bool OnContentDirectionButtonClicked( Toolkit::Button button )
  {
    mCurrentContentDirection = static_cast<FlexContainer::ContentDirection>( ( mCurrentContentDirection + 1 ) % NUM_CONTENT_DIRECTION );
    mFlexItemContainer.SetProperty(FlexContainer::Property::CONTENT_DIRECTION, mCurrentContentDirection);
    SetTitle( "Content direction", CONTENT_DIRECTION[mCurrentContentDirection] );

    return true;
  }

  bool OnJustifyContentButtonClicked( Toolkit::Button button )
  {
    mCurrentJustifyContent = static_cast<FlexContainer::Justification>( ( mCurrentJustifyContent + 1 ) % NUM_JUSTIFY_CONTENT );
    mFlexItemContainer.SetProperty(FlexContainer::Property::JUSTIFY_CONTENT, mCurrentJustifyContent);
    SetTitle( "Justify content", JUSTIFY_CONTENT[mCurrentJustifyContent] );

    return true;
  }

  bool OnAlignItemsButtonClicked( Toolkit::Button button )
  {
    mCurrentAlignItems = static_cast<FlexContainer::Alignment>( ( mCurrentAlignItems + 1 ) % ( NUM_ALIGN_ITEMS + 1 ) );
    mCurrentAlignItems = mCurrentAlignItems < FlexContainer::ALIGN_FLEX_START ? FlexContainer::ALIGN_FLEX_START : mCurrentAlignItems; // skip auto as it is invalid for alignItems property
    mFlexItemContainer.SetProperty(FlexContainer::Property::ALIGN_ITEMS, mCurrentAlignItems );
    SetTitle( "Align Items", ALIGN_ITEMS[mCurrentAlignItems - 1] );

    return true;
  }

  bool OnAlignContentButtonClicked( Toolkit::Button button )
  {
    mCurrentAlignContent = static_cast<FlexContainer::Alignment>( ( mCurrentAlignContent + 1 ) % (NUM_ALIGN_CONTENT + 1 ) );
    mCurrentAlignContent = mCurrentAlignContent < FlexContainer::ALIGN_FLEX_START ? FlexContainer::ALIGN_FLEX_START : mCurrentAlignContent; // skip auto as it is invalid for alignContent property
    mFlexItemContainer.SetProperty(FlexContainer::Property::ALIGN_CONTENT, mCurrentAlignContent);
    SetTitle( "Align content", ALIGN_CONTENT[mCurrentAlignContent - 1] );

    return true;
  }

private:

  /**
   * Sets/Updates the title of the View and the value of the recently updated
   * flexbox property.
   *
   * @param[in] title The new title for the view.
   * @param[in] propertyValue The value of the flexbox property.
   */
  void SetTitle(const std::string& title, const std::string& propertyValue)
  {
    if(!mTitleActor)
    {
      mTitleActor = DemoHelper::CreateToolBarLabel( "" );
      // Add title to the tool bar.
      mToolBar.AddControl( mTitleActor, VIEW_STYLE.mToolBarTitlePercentage, Alignment::HorizontalCenter );
    }

    // Update the title and property value
    mTitleActor.SetProperty( TextLabel::Property::TEXT, title );
    mFlexPropertyLabel.SetProperty( TextLabel::Property::TEXT, propertyValue );
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  Application& mApplication;

  Toolkit::Control mView;
  Toolkit::ToolBar mToolBar;
  TextLabel mTitleActor;             ///< The Toolbar's Title.

  FlexContainer mFlexContainer;
  FlexContainer mFlexItemContainer;
  TextLabel mFlexPropertyLabel;

  FlexContainer::FlexDirection mCurrentFlexDirection;
  FlexContainer::WrapType mCurrentFlexWrap;
  FlexContainer::ContentDirection mCurrentContentDirection;
  FlexContainer::Justification mCurrentJustifyContent;
  FlexContainer::Alignment mCurrentAlignItems;
  FlexContainer::Alignment mCurrentAlignContent;

  Toolkit::PushButton mFlexDirectionButton;
  Toolkit::PushButton mFlexWrapButton;
  Toolkit::PushButton mContentDirectionButton;
  Toolkit::PushButton mJustifyContentButton;
  Toolkit::PushButton mAlignItemsButton;
  Toolkit::PushButton mAlignContentButton;
};

int DALI_EXPORT_API main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv, DEMO_THEME_PATH);
  FlexContainerExample test(app);
  app.MainLoop();
  return 0;
}
