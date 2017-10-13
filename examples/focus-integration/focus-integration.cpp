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

#include "shared/view.h"
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* const BACKGROUND_IMAGE = DEMO_IMAGE_DIR "background-gradient.jpg";
const char* const TOOLBAR_IMAGE = DEMO_IMAGE_DIR "top-bar.png";
const char* const TOOLBAR_TITLE = "Focus Integration";
const Vector4 BACKGROUND_COLOUR( 1.0f, 1.0f, 1.0f, 0.15f );

// Layout sizes
const int MARGIN_SIZE = 10;
const int TOP_MARGIN = 85;
const std::string ITEMNAME[] = { "TextLabel", "TextField", "TextEditor", "PushButton", "RadioButton", "CheckBoxButton" };

}  // namespace

/**
 * @brief Shows how integrated DALi Focus works.
 */
class FocusIntegrationExample: public ConnectionTracker
{
public:

  FocusIntegrationExample( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &FocusIntegrationExample::Create );
  }

  void Create( Application& application )
  {
    mStage = Stage::GetCurrent();
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            TOOLBAR_TITLE );

    TableView contentTable = TableView::New(2, 1);
    contentTable.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    contentTable.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
    contentTable.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    contentTable.SetParentOrigin(ParentOrigin::TOP_LEFT);
    contentTable.SetCellPadding(Size(MARGIN_SIZE, MARGIN_SIZE * 0.5f));
    contentTable.SetKeyboardFocusable(true);

    for( unsigned int i = 0; i < contentTable.GetRows(); ++i )
    {
      contentTable.SetFitHeight( i );
    }
    contentTable.SetPosition( 0.0f, TOP_MARGIN );
    mContentLayer.Add( contentTable );

    // Create label to display which control's KeyEvent callback is called
    mEventLabel = TextLabel::New("Controls don't get KeyEvent yet");
    mEventLabel.SetSize( mStage.GetSize().width, mStage.GetSize().height*0.1f );
    mEventLabel.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    mEventLabel.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mEventLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mEventLabel.SetBackgroundColor( Color::WHITE );
    contentTable.Add( mEventLabel );

    mContainer = TableView::New( 4, 3 );
    mContainer.SetSize( mStage.GetSize().width, mStage.GetSize().height*0.4f );
    mContainer.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    mContainer.SetBackgroundColor( BACKGROUND_COLOUR );
    mContainer.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );
    mContainer.SetRelativeHeight( 0, 0.2f);
    mContainer.SetRelativeHeight( 1, 0.3f);
    mContainer.SetRelativeHeight( 2, 0.2f);
    mContainer.SetRelativeHeight( 3, 0.3f);
    mContainer.SetKeyboardFocusable(true);
    contentTable.Add( mContainer );

    // Make name label for each controls
    for(int i = 0; i < 6; i++)
    {
      TextLabel itemLabel = TextLabel::New( ITEMNAME[i] );
      itemLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      itemLabel.SetBackgroundColor( BACKGROUND_COLOUR );
      itemLabel.SetProperty( TextLabel::Property::POINT_SIZE, 14.0f );
      itemLabel.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
      itemLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
      mContainer.AddChild( itemLabel, TableView::CellPosition( (i/3)*2, i%3 ) );
    }

    TextLabel textLabel = TextLabel::New("TextLabel");
    mContainer.AddChild( textLabel, TableView::CellPosition( 1, 0 ) );

    TextField textField = TextField::New();
    textField.SetBackgroundColor( Color::WHITE );
    textField.SetProperty( TextField::Property::TEXT, "Text" );
    mContainer.AddChild( textField, TableView::CellPosition( 1, 1 ) );

    TextEditor textEditor = TextEditor::New();
    textEditor.SetBackgroundColor( Color::WHITE );
    textEditor.SetProperty( TextEditor::Property::TEXT, "Text\nText" );
    mContainer.AddChild( textEditor, TableView::CellPosition( 1, 2 ) );

    PushButton pushButton = PushButton::New();
    mContainer.AddChild( pushButton, TableView::CellPosition( 3, 0 ) );

    RadioButton radioButton = RadioButton::New();
    mContainer.AddChild( radioButton, TableView::CellPosition( 3, 1 ) );

    CheckBoxButton checkBoxButton = CheckBoxButton::New();
    mContainer.AddChild( checkBoxButton, TableView::CellPosition( 3, 2 ) );

    // Set name and keyboard focusable for each controls
    for(int i = 0; i<6; i++)
    {
      Control control = Control::DownCast( mContainer.GetChildAt( TableView::CellPosition( (i/3)*2+1, i%3 ) ) );
      control.SetKeyboardFocusable(true);
      control.SetName(ITEMNAME[i]);
      control.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      control.KeyEventSignal().Connect( this, &FocusIntegrationExample::OnControlKeyEvent );
    }

    KeyboardFocusManager::Get().PreFocusChangeSignal().Connect( this, &FocusIntegrationExample::OnPreFocusChange );

    // Respond to key events
    mStage.KeyEventSignal().Connect( this, &FocusIntegrationExample::OnKeyEvent );
  }

  // Callback for KeyboardFocusManager
  Actor OnPreFocusChange( Actor current, Actor next, Control::KeyboardFocus::Direction direction )
  {
    if( !current && !next )
    {
      next = mContainer.GetChildAt( TableView::CellPosition( 1, 0 ) );
    }
    return next;
  }

  // Callback for each controls.
  // Display current control name.
  bool OnControlKeyEvent( Control control, const KeyEvent& event )
  {
    std::string controlName = control.GetName();
    mEventLabel.SetProperty( TextLabel::Property::TEXT, controlName+"'s KeyEvent works\n" );

    return false;
  }

private:

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

  Application&  mApplication;
  Stage mStage;
  TableView mContainer;
  TextLabel mEventLabel;
  Toolkit::Control  mView;                              ///< The View instance.
  Toolkit::ToolBar  mToolBar;                           ///< The View's Toolbar.
  Layer             mContentLayer;                      ///< Content layer.
};

//
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  FocusIntegrationExample test( application );

  application.MainLoop();

  return 0;
}
