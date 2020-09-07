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

/**
 * @file text-field-example.cpp
 * @brief Basic usage of TextField control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include <iostream>

// INTERNAL INCLUDES
#include "shared/multi-language-strings.h"
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace MultiLanguageStrings;

namespace
{

  const char* const FOLDER_ICON_IMAGE = DEMO_IMAGE_DIR "folder_appicon_empty_bg.png";
  const char* const FOLDER_OPEN_ICON_IMAGE = DEMO_IMAGE_DIR "folder_appicon_empty_open_bg.png";

  const float BORDER_WIDTH = 4.0f;

} // unnamed namespace

/**
 * @brief The main class of the demo.
 */
class TextFieldExample : public ConnectionTracker
{
public:

  TextFieldExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextFieldExample::Create );
  }

  ~TextFieldExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Window window = application.GetWindow();

    window.SetBackgroundColor( Vector4( 0.04f, 0.345f, 0.392f, 1.0f ) );
    window.KeyEventSignal().Connect(this, &TextFieldExample::OnKeyEvent);

    mButton = CreateFolderButton();
    mButton.ClickedSignal().Connect( this, &TextFieldExample::OnButtonClicked );
    window.Add( mButton );
  }

  PushButton CreateFolderButton()
  {
    PushButton button = PushButton::New();
    button.SetProperty( Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, FOLDER_ICON_IMAGE );
    button.SetProperty( Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, FOLDER_OPEN_ICON_IMAGE );
    button.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    return button;
  }

  bool OnButtonClicked( Toolkit::Button button )
  {
    Window window = mApplication.GetWindow();
    Vector2 windowSize = window.GetSize();

    // Remove previously hidden pop-up
    UnparentAndReset(mPopup);

    // Launch a pop-up containing TextField
    mField = CreateTextField( windowSize, mButtonLabel );
    mPopup = CreatePopup();
    mPopup.Add( mField );
    mPopup.OutsideTouchedSignal().Connect( this, &TextFieldExample::OnPopupOutsideTouched );
    window.Add( mPopup );
    mPopup.SetDisplayState( Popup::SHOWN );

    return true;
  }

  TextField CreateTextField( const Vector2& windowSize, const std::string& text )
  {
    TextField field = TextField::New();
    field.SetProperty( Dali::Actor::Property::NAME,"textField");
    field.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    field.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    field.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
    field.SetProperty( TextField::Property::TEXT, text );
    field.SetProperty( TextField::Property::TEXT_COLOR, Vector4( 0.0f, 1.0f, 1.0f, 1.0f ) ); // CYAN
    field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Unnamed folder" );
    field.SetProperty( TextField::Property::PLACEHOLDER_TEXT_FOCUSED, "Enter folder name." );
    field.SetProperty( TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>( BORDER_WIDTH, BORDER_WIDTH, windowSize.width - BORDER_WIDTH*2, windowSize.height - BORDER_WIDTH*2 ) );

    return field;
  }

  Popup CreatePopup()
  {
    Popup popup = Popup::New();
    popup.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    popup.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    popup.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::WIDTH );
    popup.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );
    popup.TouchSignal().Connect( this, &TextFieldExample::OnPopupTouched );

    return popup;
  }

  void OnPopupOutsideTouched()
  {
    // Update the folder text
    if( mButton && mField )
    {
      Property::Value text = mField.GetProperty( TextField::Property::TEXT );
      mButtonLabel = text.Get< std::string >();
      mButton.SetProperty( Toolkit::Button::Property::LABEL, mButtonLabel );
    }

    // Hide & discard the pop-up
    if( mPopup )
    {
      mPopup.SetDisplayState( Popup::HIDDEN );
    }
    mField.Reset();
  }

  bool OnPopupTouched( Actor actor, const TouchEvent& event )
  {
    // End edit mode for TextField if parent Popup touched.
    if((event.GetPointCount() > 0) && (mPopup == event.GetHitActor(0)))
    {
      switch( event.GetState( 0 ) )
      {
        case PointState::DOWN:
        {
          // Update the folder text and lose focus for Key events
          if( mButton && mField )
          {
            Property::Value text = mField.GetProperty( TextField::Property::TEXT );
            mButtonLabel = text.Get< std::string >();
            mButton.SetProperty( Toolkit::Button::Property::LABEL, mButtonLabel );
            mField.ClearKeyInputFocus();
          }
          break;
        }
        default:
        {
          break;
        }
      } // end switch
    }

    return false;
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

  // This button launches a pop-up containing TextField
  PushButton mButton;
  std::string mButtonLabel;

  // Pop-up contents
  TextField mField;
  Popup mPopup;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  // DALI_DEMO_THEME_PATH not passed to Application so TextField example uses default Toolkit style sheet.
  Application application = Application::New( &argc, &argv );
  TextFieldExample test( application );
  application.MainLoop();
  return 0;
}
