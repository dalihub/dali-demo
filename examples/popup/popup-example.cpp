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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali/dali.h>
#include "shared/view.h"

using namespace Dali;

using Dali::Toolkit::TextLabel;

struct ButtonItem
{
  const char* name;
  const char* text;
};

namespace
{
const char* const BACKGROUND_IMAGE = DEMO_IMAGE_DIR "background-default.png";
const char* const TOOLBAR_IMAGE    = DEMO_IMAGE_DIR "top-bar.png";

const char* const TOOLBAR_TITLE = "Popup";

const char* CONTEXT_DISABLED_ICON_IMAGE = DEMO_IMAGE_DIR "icon-scroll-view-carousel.png";
const char* CONTEXT_ENABLED_ICON_IMAGE  = DEMO_IMAGE_DIR "icon-scroll-view-spiral.png";
const char* ANIMATION_FADE_ICON_IMAGE   = DEMO_IMAGE_DIR "icon-effects-off.png";
const char* ANIMATION_ZOOM_ICON_IMAGE   = DEMO_IMAGE_DIR "icon-effects-on.png";

const char* const POPUP_BUTTON_TITLE_ID                       = "POPUP_BUTTON_TITLE";
const char* const POPUP_BUTTON_BUTTONS_1_ID                   = "POPUP_BUTTON_BUTTONS_1";
const char* const POPUP_BUTTON_BUTTONS_2_ID                   = "POPUP_BUTTON_BUTTONS_2";
const char* const POPUP_BUTTON_TOAST_ID                       = "POPUP_BUTTON_TOAST";
const char* const POPUP_BUTTON_TITLE_CONTENT_BUTTONS_ID       = "POPUP_BUTTON_TITLE_CONTENT_BUTTONS";
const char* const POPUP_BUTTON_CONTENT_TEXT_ID                = "POPUP_BUTTON_CONTENT_TEXT";
const char* const POPUP_BUTTON_CONTENT_IMAGE_ID               = "POPUP_BUTTON_CONTENT_IMAGE";
const char* const POPUP_BUTTON_TITLE_CONTENT_TEXT_ID          = "POPUP_BUTTON_TITLE_CONTENT_TEXT";
const char* const POPUP_BUTTON_TITLE_LARGE_CONTENT_BUTTONS_ID = "POPUP_BUTTON_TITLE_LARGE_CONTENT_BUTTONS";
const char* const POPUP_BUTTON_FIXED_SIZE_ID                  = "POPUP_BUTTON_FIXED_SIZE_ID";
const char* const POPUP_BUTTON_COMPLEX_ID                     = "POPUP_BUTTON_COMPLEX";
const char* const POPUP_BUTTON_CUSTOM_STYLE                   = "POPUP_BUTTON_CUSTOM_STYLE";

// Names to give Popup PushButton controls.
const char* const POPUP_CONTROL_OK_NAME     = "controlOk";
const char* const POPUP_CONTROL_CANCEL_NAME = "controlCancel";

const char* const CONTENT_TEXT = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
const char* const IMAGE1       = DEMO_IMAGE_DIR "gallery-medium-5.jpg";
const char* const IMAGE2       = DEMO_IMAGE_DIR "background-magnifier.jpg";

// Control area image.
const char* DEFAULT_CONTROL_AREA_IMAGE_PATH = DEMO_IMAGE_DIR "popup_button_background.9.png"; ///< Control area image for the popup.

const ButtonItem POPUP_BUTTON_ITEMS[] = {
  {POPUP_BUTTON_COMPLEX_ID, "Complex"},
  {POPUP_BUTTON_TOAST_ID, "Toast Popup"},
  {POPUP_BUTTON_TITLE_ID, "Title"},
  {POPUP_BUTTON_BUTTONS_1_ID, "1 Button"},
  {POPUP_BUTTON_BUTTONS_2_ID, "2 Buttons"},
  {POPUP_BUTTON_FIXED_SIZE_ID, "Fixed Size"},
  {POPUP_BUTTON_TITLE_CONTENT_BUTTONS_ID, "Title + Content + Buttons"},
  {POPUP_BUTTON_CONTENT_TEXT_ID, "Content Text"},
  {POPUP_BUTTON_CONTENT_IMAGE_ID, "Content Image"},
  {POPUP_BUTTON_TITLE_CONTENT_TEXT_ID, "Title + Content"},
  {POPUP_BUTTON_TITLE_LARGE_CONTENT_BUTTONS_ID, "Title + Large Content + Buttons"},
  {POPUP_BUTTON_CUSTOM_STYLE, "Custom Styled Popup"}};

const int POPUP_BUTTON_ITEMS_COUNT = sizeof(POPUP_BUTTON_ITEMS) / sizeof(POPUP_BUTTON_ITEMS[0]);

const char* const CUSTOM_POPUP_STYLE_NAME = "CustomPopupStyle"; ///< Custom popup style name
} // anonymous namespace

/**
 * This example shows the usage of the Popup class.
 */
class PopupExample : public ConnectionTracker, public Toolkit::ItemFactory
{
public:
  PopupExample(Application& application)
  : mApplication(application),
    mContextual(false),
    mAnimationFade(true)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &PopupExample::Create);
  }

  ~PopupExample()
  {
    // Nothing to do here
  }

  void Create(Application& application)
  {
    // The Init signal is received once (only) during the Application lifetime
    Window window = application.GetWindow();

    // Respond to key events if not handled
    window.KeyEventSignal().Connect(this, &PopupExample::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the window.
    mContentLayer = DemoHelper::CreateView(application,
                                           mView,
                                           mToolBar,
                                           BACKGROUND_IMAGE,
                                           TOOLBAR_IMAGE,
                                           std::string(""));

    mTitleActor = DemoHelper::CreateToolBarLabel("CUSTOM_TOOLBAR_TITLE");
    mTitleActor.SetProperty(Toolkit::TextLabel::Property::TEXT, TOOLBAR_TITLE);

    // Add title to the tool bar.
    const float padding(DemoHelper::DEFAULT_VIEW_STYLE.mToolBarPadding);
    mToolBar.AddControl(mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Toolkit::Alignment::HORIZONTAL_CENTER, Toolkit::Alignment::Padding(padding, padding, padding, padding));

    // Create animation button.
    mAnimationButton = Toolkit::PushButton::New();
    mAnimationButton.SetProperty(Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, ANIMATION_FADE_ICON_IMAGE);
    mAnimationButton.SetProperty(Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, ANIMATION_ZOOM_ICON_IMAGE);
    mAnimationButton.SetProperty(Toolkit::Button::Property::TOGGLABLE, true);
    mAnimationButton.ClickedSignal().Connect(this, &PopupExample::OnAnimationClicked);
    mToolBar.AddControl(mAnimationButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HORIZONTAL_LEFT, DemoHelper::DEFAULT_MODE_SWITCH_PADDING);

    // Create context button.
    mContextButton = Toolkit::PushButton::New();
    mContextButton.SetProperty(Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, CONTEXT_DISABLED_ICON_IMAGE);
    mContextButton.SetProperty(Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, CONTEXT_ENABLED_ICON_IMAGE);
    mContextButton.SetProperty(Toolkit::Button::Property::TOGGLABLE, true);
    mContextButton.ClickedSignal().Connect(this, &PopupExample::OnContextClicked);
    mToolBar.AddControl(mContextButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HORIZONTAL_LEFT, DemoHelper::DEFAULT_MODE_SWITCH_PADDING);

    // Add title to the tool bar.
    mItemView = Toolkit::ItemView::New(*this);
    mItemView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mItemView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mItemView.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

    // Use a grid layout for tests
    Vector2                windowSize = window.GetSize();
    Toolkit::ItemLayoutPtr gridLayout = Toolkit::DefaultItemLayout::New(Toolkit::DefaultItemLayout::LIST);
    Vector3                itemSize;
    gridLayout->GetItemSize(0, Vector3(windowSize), itemSize);
    itemSize.height = windowSize.y / 10;
    gridLayout->SetItemSize(itemSize);
    mItemView.AddLayout(*gridLayout);

    mItemView.ActivateLayout(0, Vector3(windowSize.x, windowSize.y, windowSize.x), 0.0f);

    mContentLayer.Add(mItemView);
  }

  bool OnContextClicked(Toolkit::Button button)
  {
    mContextual = button.GetProperty(Toolkit::Button::Property::SELECTED).Get<bool>();
    return true;
  }

  bool OnAnimationClicked(Toolkit::Button button)
  {
    mAnimationFade = !(button.GetProperty(Toolkit::Button::Property::SELECTED).Get<bool>());
    return true;
  }

  /**
   * This function is designed as a shortcut to convert any resize policies set for a popup to
   * ones that will work for contextual mode (for demo purposes).
   * Note that in a real-use case example the policies would be set to something appropriate
   * manually, but in the case of this demo, the popup is parented from the popup-opening buttons
   * and (incorrectly) have their policies as "SIZE_RELATIVE_TO_PARENT". This would create a tiny
   * popup that would not be able to contain it's contents, so to illustrate contextual behaviour
   * this function converts the old policies and size to new ones that would give the popup the
   * same visual appearance.
   * @param[in] popup The popup whose policies should be modified.
   */
  void SetupContextualResizePolicy(Toolkit::Popup& popup)
  {
    Vector2 windowSize = mApplication.GetWindow().GetSize();
    // Some defaults when creating a new fixed size.
    // This is NOT a Vector2 so we can modify each dimension in a for-loop.
    float newSize[Dimension::DIMENSION_COUNT] = {windowSize.x * 0.75f, windowSize.y * 0.75f};
    bool  modifySize                          = false;

    // Loop through each of two dimensions to process them.
    for(unsigned int dimension = 0; dimension < 2; ++dimension)
    {
      float           windowDimensionSize, sizeModeFactor;
      Dimension::Type policyDimension = dimension == 0 ? Dimension::WIDTH : Dimension::HEIGHT;

      // Setup information related to the current dimension we are processing.
      if(policyDimension == Dimension::WIDTH)
      {
        windowDimensionSize = windowSize.x;
        sizeModeFactor      = popup.GetProperty<Vector3>(Actor::Property::SIZE_MODE_FACTOR).x;
      }
      else
      {
        windowDimensionSize = windowSize.y;
        sizeModeFactor      = popup.GetProperty<Vector3>(Actor::Property::SIZE_MODE_FACTOR).y;
      }

      bool               modifyPolicy = false;
      ResizePolicy::Type policy       = popup.GetResizePolicy(policyDimension);
      ResizePolicy::Type newPolicy(policy);

      // Switch on each policy type to determine the new behaviour.
      switch(policy)
      {
        case ResizePolicy::FIXED:
        case ResizePolicy::USE_ASSIGNED_SIZE:
        {
          break;
        }

        case ResizePolicy::USE_NATURAL_SIZE:
        case ResizePolicy::FIT_TO_CHILDREN:
        case ResizePolicy::DIMENSION_DEPENDENCY:
        {
          // Set size to 0 so the policy determines size.
          // If a non-zero size is set, policy is converted to fixed.
          newSize[dimension] = 0.0f;
          modifySize         = true;
          break;
        }

        // The following cases emulate the three size-mode related resize policies.
        case ResizePolicy::FILL_TO_PARENT:
        {
          newPolicy          = ResizePolicy::FIXED;
          newSize[dimension] = windowDimensionSize;
          modifyPolicy       = true;
          break;
        }

        case ResizePolicy::SIZE_RELATIVE_TO_PARENT:
        {
          newPolicy          = ResizePolicy::FIXED;
          newSize[dimension] = windowDimensionSize * sizeModeFactor;
          modifyPolicy       = true;
          break;
        }

        case ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT:
        {
          newPolicy          = ResizePolicy::FIXED;
          newSize[dimension] = windowDimensionSize + sizeModeFactor;
          modifyPolicy       = true;
          break;
        }
      }

      if(modifyPolicy)
      {
        // Set the new policy for this dimension, if it has been modified.
        popup.SetResizePolicy(newPolicy, policyDimension);
        modifySize = true;
      }
    }

    if(modifySize)
    {
      // The size is set once at the end.
      popup.SetProperty(Actor::Property::SIZE, Vector2(newSize[0], newSize[1]));
    }
  }

  void SetupPopup(Toolkit::Popup popup, Actor parent)
  {
    if(mAnimationFade)
    {
      popup.SetProperty(Toolkit::Popup::Property::ANIMATION_MODE, "FADE");
    }
    else
    {
      popup.SetProperty(Toolkit::Popup::Property::ANIMATION_MODE, "ZOOM");
    }

    if(mContextual)
    {
      popup.SetProperty(Toolkit::Popup::Property::CONTEXTUAL_MODE, "BELOW");

      // Modify the preset demo resize policies (and size) to contextual ones.
      SetupContextualResizePolicy(popup);

      parent.Add(popup);
    }
    else
    {
      mApplication.GetWindow().Add(popup);
    }

    mPopup.SetDisplayState(Toolkit::Popup::SHOWN);
  }

  void HidePopup()
  {
    if(mPopup)
    {
      mPopup.SetDisplayState(Toolkit::Popup::HIDDEN);
    }
  }

  void PopupHidden()
  {
    if(mPopup)
    {
      mPopup.Unparent();
      mPopup.Reset();
    }
  }

  Toolkit::Popup CreatePopup()
  {
    Window      window         = mApplication.GetWindow();
    const float POPUP_WIDTH_DP = window.GetSize().GetWidth() * 0.75f;

    Toolkit::Popup popup = Toolkit::Popup::New();
    popup.SetProperty(Dali::Actor::Property::NAME, "popup");
    popup.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    popup.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    popup.SetProperty(Actor::Property::SIZE, Vector2(POPUP_WIDTH_DP, 0.0f));
    popup.SetProperty(Toolkit::Popup::Property::TAIL_VISIBILITY, false);

    popup.OutsideTouchedSignal().Connect(this, &PopupExample::HidePopup);
    popup.HiddenSignal().Connect(this, &PopupExample::PopupHidden);

    return popup;
  }

  Toolkit::Popup CreateConfirmationPopup(int numberOfButtons)
  {
    Toolkit::Popup confirmationPopup = Toolkit::Popup::New();
    confirmationPopup.SetProperty(Dali::Actor::Property::NAME, "MAIN-POPUP-SELF");

    if(numberOfButtons > 0)
    {
      // Start with a control area image.
      Toolkit::ImageView footer = Toolkit::ImageView::New(DEFAULT_CONTROL_AREA_IMAGE_PATH);
      footer.SetProperty(Dali::Actor::Property::NAME, "controlAreaImage");
      // Set up the container's layout.
      footer.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      footer.SetResizePolicy(ResizePolicy::FIXED, Dimension::HEIGHT);
      footer.SetProperty(Actor::Property::SIZE, Vector2(0.0f, 130.0f));
      footer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      footer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

      Actor okButton = CreateOKButton();
      okButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      okButton.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      okButton.SetResizePolicy(ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS);
      okButton.SetProperty(Actor::Property::SIZE_MODE_FACTOR, Vector3(-20.0f, -20.0f, 0.0));

      if(numberOfButtons > 1)
      {
        Toolkit::TableView controlLayout = Toolkit::TableView::New(1, 2);
        controlLayout.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
        controlLayout.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
        controlLayout.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

        Actor cancelButton = CreateCancelButton();
        cancelButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
        cancelButton.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
        cancelButton.SetResizePolicy(ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS);
        cancelButton.SetProperty(Actor::Property::SIZE_MODE_FACTOR, Vector3(-20.0f, -20.0f, 0.0));

        controlLayout.SetCellPadding(Size(10.0f, 10.0f));

        controlLayout.SetRelativeWidth(0, 0.5f);
        controlLayout.SetRelativeWidth(1, 0.5f);

        controlLayout.SetCellAlignment(Toolkit::TableView::CellPosition(0, 0), HorizontalAlignment::CENTER, VerticalAlignment::CENTER);
        controlLayout.SetCellAlignment(Toolkit::TableView::CellPosition(0, 1), HorizontalAlignment::CENTER, VerticalAlignment::CENTER);
        controlLayout.AddChild(okButton, Toolkit::TableView::CellPosition(0, 0));
        controlLayout.AddChild(cancelButton, Toolkit::TableView::CellPosition(0, 1));

        footer.Add(controlLayout);
      }
      else
      {
        footer.Add(okButton);
      }

      confirmationPopup.SetFooter(footer);
    }

    confirmationPopup.OutsideTouchedSignal().Connect(this, &PopupExample::HidePopup);

    return confirmationPopup;
  }

  Actor CreateTitle(std::string title)
  {
    Toolkit::TextLabel titleActor = Toolkit::TextLabel::New(title);
    titleActor.SetProperty(Dali::Actor::Property::NAME, "titleActor");
    titleActor.SetProperty(Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE);
    titleActor.SetProperty(Toolkit::TextLabel::Property::MULTI_LINE, true);
    titleActor.SetProperty(Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");

    return titleActor;
  }

  Toolkit::PushButton CreateOKButton()
  {
    Toolkit::PushButton okayButton = Toolkit::PushButton::New();
    okayButton.SetProperty(Dali::Actor::Property::NAME, POPUP_CONTROL_OK_NAME);
    okayButton.SetProperty(Toolkit::Button::Property::LABEL, "OK!");

    okayButton.ClickedSignal().Connect(this, &PopupExample::OnPopupButtonClicked);

    return okayButton;
  }

  Toolkit::PushButton CreateCancelButton()
  {
    Toolkit::PushButton cancelButton = Toolkit::PushButton::New();
    cancelButton.SetProperty(Dali::Actor::Property::NAME, POPUP_CONTROL_CANCEL_NAME);
    cancelButton.SetProperty(Toolkit::Button::Property::LABEL, "Cancel");

    cancelButton.ClickedSignal().Connect(this, &PopupExample::OnPopupButtonClicked);

    return cancelButton;
  }

  bool OnPopupButtonClicked(Toolkit::Button button)
  {
    // Handle Popup pushbuttons being clicked.
    HidePopup();
    return true;
  }

  bool OnButtonClicked(Toolkit::Button button)
  {
    // Handle menu items that create popups.
    if(button.GetProperty<std::string>(Dali::Actor::Property::NAME) == POPUP_BUTTON_TITLE_ID)
    {
      mPopup = CreatePopup();
      mPopup.SetTitle(CreateTitle("Popup!"));

      SetupPopup(mPopup, button);
    }
    else if(button.GetProperty<std::string>(Dali::Actor::Property::NAME) == POPUP_BUTTON_BUTTONS_1_ID)
    {
      mPopup = CreateConfirmationPopup(1);
      mPopup.SetTitle(CreateTitle("Title"));

      SetupPopup(mPopup, button);
    }
    else if(button.GetProperty<std::string>(Dali::Actor::Property::NAME) == POPUP_BUTTON_BUTTONS_2_ID)
    {
      mPopup = CreateConfirmationPopup(2);
      mPopup.SetTitle(CreateTitle("Title"));

      SetupPopup(mPopup, button);
    }
    else if(button.GetProperty<std::string>(Dali::Actor::Property::NAME) == POPUP_BUTTON_TOAST_ID)
    {
      // Create a toast popup via the type registry (as it is a named-type).
      TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo("PopupToast");
      if(typeInfo)
      {
        BaseHandle baseHandle = typeInfo.CreateInstance();
        if(baseHandle)
        {
          mPopup = Toolkit::Popup::DownCast(baseHandle);
          mPopup.SetTitle(CreateTitle("This is a Toast Popup.\nIt will auto-hide itself"));

          mApplication.GetWindow().Add(mPopup);
          mPopup.SetDisplayState(Toolkit::Popup::SHOWN);
        }
      }
    }
    else if(button.GetProperty<std::string>(Dali::Actor::Property::NAME) == POPUP_BUTTON_TITLE_CONTENT_BUTTONS_ID)
    {
      mPopup = CreateConfirmationPopup(2);
      mPopup.SetTitle(CreateTitle("Erase image"));

      Toolkit::TextLabel text = Toolkit::TextLabel::New("This will erase the image permanently. Are you sure?");
      text.SetProperty(Dali::Actor::Property::NAME, "POPUP_CONTENT_TEXT");
      text.SetProperty(Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE);
      text.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      text.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);
      text.SetProperty(TextLabel::Property::MULTI_LINE, true);
      text.SetProperty(Actor::Property::PADDING, Padding(10.0f, 10.0f, 20.0f, 0.0f));
      mPopup.SetContent(text);

      SetupPopup(mPopup, button);
    }
    else if(button.GetProperty<std::string>(Dali::Actor::Property::NAME) == POPUP_BUTTON_CONTENT_TEXT_ID)
    {
      mPopup = CreatePopup();

      TextLabel text = TextLabel::New(CONTENT_TEXT);
      text.SetProperty(Dali::Actor::Property::NAME, "POPUP_CONTENT_TEXT");
      text.SetProperty(Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE);
      text.SetProperty(TextLabel::Property::MULTI_LINE, true);
      text.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
      text.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
      text.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      text.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);
      text.SetProperty(Actor::Property::PADDING, Padding(20.0f, 20.0f, 20.0f, 20.0f));

      mPopup.Add(text);

      SetupPopup(mPopup, button);
    }
    else if(button.GetProperty<std::string>(Dali::Actor::Property::NAME) == POPUP_BUTTON_CONTENT_IMAGE_ID)
    {
      mPopup                   = CreatePopup();
      Toolkit::ImageView image = Toolkit::ImageView::New(IMAGE2);
      image.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      image.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);
      image.SetProperty(Actor::Property::PADDING, Padding(20.0f, 20.0f, 20.0f, 20.0f));

      mPopup.Add(image);

      SetupPopup(mPopup, button);
    }
    else if(button.GetProperty<std::string>(Dali::Actor::Property::NAME) == POPUP_BUTTON_TITLE_CONTENT_TEXT_ID)
    {
      mPopup = CreatePopup();
      mPopup.SetTitle(CreateTitle("Popup!"));

      Toolkit::TextLabel text = Toolkit::TextLabel::New(CONTENT_TEXT);
      text.SetProperty(Dali::Actor::Property::NAME, "POPUP_CONTENT_TEXT");
      text.SetProperty(Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE);
      text.SetProperty(TextLabel::Property::MULTI_LINE, true);
      text.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      text.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);
      text.SetProperty(Actor::Property::PADDING, Padding(20.0f, 20.0f, 20.0f, 20.0f));

      mPopup.Add(text);

      SetupPopup(mPopup, button);
    }
    else if(button.GetProperty<std::string>(Dali::Actor::Property::NAME) == POPUP_BUTTON_FIXED_SIZE_ID)
    {
      mPopup = CreatePopup();
      mPopup.SetTitle(CreateTitle("Popup!"));

      Toolkit::TextLabel text = Toolkit::TextLabel::New("Fixed size popup");
      text.SetProperty(Dali::Actor::Property::NAME, "POPUP_CONTENT_TEXT");
      text.SetProperty(Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE);
      text.SetProperty(TextLabel::Property::MULTI_LINE, true);
      text.SetProperty(Actor::Property::PADDING, Padding(20.0f, 20.0f, 20.0f, 20.0f));

      mPopup.Add(text);

      // Fix the popup's size.
      mPopup.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 400.0f));
      mPopup.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);

      SetupPopup(mPopup, button);
    }
    else if(button.GetProperty<std::string>(Dali::Actor::Property::NAME) == POPUP_BUTTON_TITLE_LARGE_CONTENT_BUTTONS_ID)
    {
      mPopup = CreateConfirmationPopup(2);
      mPopup.SetTitle(CreateTitle("Popup!"));

      Toolkit::TextLabel text = Toolkit::TextLabel::New(CONTENT_TEXT);
      text.SetProperty(Dali::Actor::Property::NAME, "POPUP_CONTENT_TEXT");
      text.SetProperty(Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE);
      text.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      text.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);
      text.SetProperty(TextLabel::Property::MULTI_LINE, true);
      text.SetProperty(Actor::Property::PADDING, Padding(10.0f, 10.0f, 20.0f, 0.0f));

      mPopup.Add(text);

      SetupPopup(mPopup, button);
    }
    else if(button.GetProperty<std::string>(Dali::Actor::Property::NAME) == POPUP_BUTTON_COMPLEX_ID)
    {
      mPopup = CreateConfirmationPopup(2);
      mPopup.SetTitle(CreateTitle("Warning"));

      // Content
      Toolkit::TableView content = Toolkit::TableView::New(2, 2);
      content.SetProperty(Dali::Actor::Property::NAME, "COMPLEX_TABLEVIEW");
      content.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      content.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
      content.SetFitHeight(0);
      content.SetFitHeight(1);
      content.SetProperty(Actor::Property::PADDING, Padding(20.0f, 20.0f, 20.0f, 0.0f));

      // Text
      {
        Toolkit::TextLabel text = Toolkit::TextLabel::New("Do you really want to quit?");
        text.SetProperty(Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE);
        text.SetProperty(Toolkit::TextLabel::Property::MULTI_LINE, true);
        text.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
        text.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);

        content.AddChild(text, Toolkit::TableView::CellPosition(0, 0));
      }

      // Image
      {
        Toolkit::ImageView image = Toolkit::ImageView::New(IMAGE1);
        image.SetProperty(Dali::Actor::Property::NAME, "COMPLEX_IMAGE");
        image.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
        image.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);
        image.SetProperty(Actor::Property::PADDING, Padding(20.0f, 0.0f, 0.0f, 0.0f));
        content.AddChild(image, Toolkit::TableView::CellPosition(0, 1));
      }

      // Text 2
      {
        Toolkit::TableView root = Toolkit::TableView::New(1, 2);
        root.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
        root.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
        root.SetFitHeight(0);
        root.SetFitWidth(0);
        root.SetProperty(Actor::Property::PADDING, Padding(0.0f, 0.0f, 0.0f, 20.0f));

        Toolkit::CheckBoxButton checkBox = Toolkit::CheckBoxButton::New();
        checkBox.SetProperty(Actor::Property::SIZE, Vector2(48, 48));
        root.AddChild(checkBox, Toolkit::TableView::CellPosition(0, 0));

        Toolkit::TextLabel text = Toolkit::TextLabel::New("Don't show again");
        text.SetProperty(Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE);
        Actor textActor = text;
        textActor.SetProperty(Actor::Property::PADDING, Padding(20.0f, 0.0f, 0.0f, 10.0f));

        root.AddChild(text, Toolkit::TableView::CellPosition(0, 1));

        content.AddChild(root, Toolkit::TableView::CellPosition(1, 0));
      }

      mPopup.SetContent(content);

      SetupPopup(mPopup, button);
    }
    else if(button.GetProperty<std::string>(Dali::Actor::Property::NAME) == POPUP_BUTTON_CUSTOM_STYLE)
    {
      mPopup = CreateConfirmationPopup(2);

      Toolkit::TextLabel titleActor = Toolkit::TextLabel::New("Styled Popup");
      titleActor.SetProperty(Dali::Actor::Property::NAME, "titleActor");
      titleActor.SetProperty(Toolkit::TextLabel::Property::TEXT_COLOR, Color::RED);
      titleActor.SetProperty(Toolkit::TextLabel::Property::MULTI_LINE, true);
      titleActor.SetProperty(Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
      mPopup.SetTitle(titleActor);

      SetupPopup(mPopup, button);

      mPopup.SetStyleName(CUSTOM_POPUP_STYLE_NAME);
    }

    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        // Exit application when click back or escape.
        mApplication.Quit();
      }
    }
  }

public: // From ItemFactory
  /**
   * @brief Return the number of items to display in the item view
   *
   * @return Return the number of items to display
   */
  virtual unsigned int GetNumberOfItems()
  {
    return POPUP_BUTTON_ITEMS_COUNT;
  }

  /**
   * @brief Create a new item to populate the item view with
   *
   * @param[in] itemId The index of the item to create
   * @return Return the created actor for the given ID
   */
  virtual Actor NewItem(unsigned int itemId)
  {
    Toolkit::PushButton popupButton = Toolkit::PushButton::New();
    popupButton.SetProperty(Dali::Actor::Property::NAME, POPUP_BUTTON_ITEMS[itemId].name);
    popupButton.SetProperty(Toolkit::Button::Property::LABEL, POPUP_BUTTON_ITEMS[itemId].text);
    popupButton.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);

    popupButton.ClickedSignal().Connect(this, &PopupExample::OnButtonClicked);

    return popupButton;
  }

private:
  Application&        mApplication;
  Toolkit::Control    mView;            ///< The View instance.
  Toolkit::ToolBar    mToolBar;         ///< The View's Toolbar.
  Toolkit::PushButton mContextButton;   ///< For toggling contextual mode.
  Toolkit::PushButton mAnimationButton; ///< For toggling the fade animation.
  Layer               mContentLayer;    ///< Content layer

  Toolkit::TextLabel mTitleActor; ///< Title text

  bool mContextual;    ///< True if currently using the contextual popup mode.
  bool mAnimationFade; ///< True if currently using the fade animation.

  Toolkit::Popup mPopup; ///< The current example popup.

  Toolkit::ItemView mItemView; ///< ItemView to hold test images
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application  application = Application::New(&argc, &argv, DEMO_THEME_PATH);
  PopupExample test(application);
  application.MainLoop();
  return 0;
}
