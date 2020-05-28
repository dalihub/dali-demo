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
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;

// Define this so that it is interchangeable
// "DP" stands for Device independent Pixels
#define DP(x) x


namespace
{

const char* const BACKGROUND_IMAGE = DEMO_IMAGE_DIR "background-gradient.jpg";
const char* const TOOLBAR_IMAGE = DEMO_IMAGE_DIR "top-bar.png";

const char* const TOOLBAR_TITLE = "Buttons";

const char* const SMALL_IMAGE_1 = DEMO_IMAGE_DIR "gallery-small-14.jpg";
const char* const BIG_IMAGE_1   = DEMO_IMAGE_DIR "gallery-large-4.jpg";

const char* const SMALL_IMAGE_2 = DEMO_IMAGE_DIR "gallery-small-20.jpg";
const char* const BIG_IMAGE_2   = DEMO_IMAGE_DIR "gallery-large-11.jpg";

const char* const SMALL_IMAGE_3 = DEMO_IMAGE_DIR "gallery-small-25.jpg";
const char* const BIG_IMAGE_3   = DEMO_IMAGE_DIR "gallery-large-13.jpg";

const char* const ENABLED_IMAGE = DEMO_IMAGE_DIR "item-select-check.png";

const Vector4 BACKGROUND_COLOUR( 1.0f, 1.0f, 1.0f, 0.15f );

// Layout sizes
const int RADIO_LABEL_THUMBNAIL_SIZE = 60;
const int RADIO_LABEL_THUMBNAIL_SIZE_SMALL = 40;
const int RADIO_IMAGE_SPACING = 8;
const int BUTTON_HEIGHT = 48;

const int MARGIN_SIZE = 10;
const int TOP_MARGIN = 85;
const int GROUP2_HEIGHT = 238;
const int GROUP1_HEIGHT = 120;
const int GROUP3_HEIGHT = 190;
const int GROUP4_HEIGHT = BUTTON_HEIGHT + MARGIN_SIZE * 2;

}  // namespace

/** This example shows how to create and use different buttons.
 *
 * 1. First group of radio buttons with image actor labels selects an image to load
 * 2. A push button loads the selected thumbnail image into the larger image pane
 * 3. Second group of radio buttons with a table view label containing a text view and image view, and a normal text view.
 *    Selecting one of these will enable/disable the image loading push button
 * 4. A group of check boxes
 */
class ButtonsController: public ConnectionTracker
{
 public:

  ButtonsController( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ButtonsController::Create );
  }

  ~ButtonsController()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    // Respond to key events
    Stage::GetCurrent().KeyEventSignal().Connect(this, &ButtonsController::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            TOOLBAR_TITLE );

    Toolkit::TableView contentTable = Toolkit::TableView::New( 4, 1 );
    contentTable.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    contentTable.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    contentTable.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    contentTable.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
    contentTable.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE * 0.5f ) );

    for( unsigned int i = 0; i < contentTable.GetRows(); ++i )
    {
      contentTable.SetFitHeight( i );
    }

    contentTable.SetPosition( 0.0f, TOP_MARGIN );

    mContentLayer.Add( contentTable );

    // Image selector radio group
    Toolkit::TableView radioGroup2Background = Toolkit::TableView::New( 2, 2 );
    radioGroup2Background.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    radioGroup2Background.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    radioGroup2Background.SetBackgroundColor( BACKGROUND_COLOUR );
    radioGroup2Background.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );
    radioGroup2Background.SetFitHeight( 0 );
    radioGroup2Background.SetFitHeight( 1 );
    radioGroup2Background.SetFitWidth( 0 );

    contentTable.Add( radioGroup2Background );

    Toolkit::TableView radioButtonsGroup2 = Toolkit::TableView::New( 3, 1 );
    radioButtonsGroup2.SetCellPadding( Size( 0.0f, MARGIN_SIZE * 0.5f ) );
    radioButtonsGroup2.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    for( unsigned int i = 0; i < radioButtonsGroup2.GetRows(); ++i )
    {
      radioButtonsGroup2.SetFitHeight( i );
    }
    radioButtonsGroup2.SetFitWidth( 0 );

    radioGroup2Background.AddChild( radioButtonsGroup2, Toolkit::TableView::CellPosition( 0, 0 ) );

    // TableView to lay out 3x Radio buttons on the left, and 3x Image thumbnails on the right.
    Toolkit::TableView imageSelectTableView = Toolkit::TableView::New( 3, 2 );
    imageSelectTableView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    imageSelectTableView.SetFitHeight( 0 );
    imageSelectTableView.SetFitHeight( 1 );
    imageSelectTableView.SetFitHeight( 2 );
    imageSelectTableView.SetFitWidth( 0 );
    imageSelectTableView.SetFitWidth( 1 );
    imageSelectTableView.SetCellPadding( Vector2( 6.0f, 0.0f ) );

    radioButtonsGroup2.Add( imageSelectTableView );

    int radioY = 0;

    // Radio 1
    {
      Toolkit::ImageView image = Toolkit::ImageView::New( SMALL_IMAGE_1 );
      image.SetSize( DP(RADIO_LABEL_THUMBNAIL_SIZE), DP(RADIO_LABEL_THUMBNAIL_SIZE) );

      mRadioButtonImage1 = Dali::Toolkit::RadioButton::New( "1" );
      mRadioButtonImage1.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
      mRadioButtonImage1.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
      mRadioButtonImage1.SetPosition( 0, DP(radioY) );
      mRadioButtonImage1.SetProperty( Toolkit::Button::Property::SELECTED, true );

      imageSelectTableView.AddChild( mRadioButtonImage1, Toolkit::TableView::CellPosition( 0, 0 ) );
      imageSelectTableView.AddChild( image, Toolkit::TableView::CellPosition( 0, 1 ) );
    }

    // Radio 2
    {
      radioY += RADIO_LABEL_THUMBNAIL_SIZE + RADIO_IMAGE_SPACING;

      Toolkit::ImageView image = Toolkit::ImageView::New( SMALL_IMAGE_2 );
      image.SetSize( DP(RADIO_LABEL_THUMBNAIL_SIZE), DP(RADIO_LABEL_THUMBNAIL_SIZE) );

      mRadioButtonImage2 = Dali::Toolkit::RadioButton::New( "2" );
      mRadioButtonImage2.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
      mRadioButtonImage2.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
      mRadioButtonImage2.SetPosition( 0, DP(radioY) );

      imageSelectTableView.AddChild( mRadioButtonImage2, Toolkit::TableView::CellPosition( 1, 0 ) );
      imageSelectTableView.AddChild( image, Toolkit::TableView::CellPosition( 1, 1 ) );
    }

    // Radio 3
    {
      radioY += RADIO_LABEL_THUMBNAIL_SIZE + RADIO_IMAGE_SPACING;

      Toolkit::ImageView image = Toolkit::ImageView::New( SMALL_IMAGE_3 );
      image.SetSize( DP(RADIO_LABEL_THUMBNAIL_SIZE), DP(RADIO_LABEL_THUMBNAIL_SIZE) );

      mRadioButtonImage3 = Dali::Toolkit::RadioButton::New( "3" );
      mRadioButtonImage3.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
      mRadioButtonImage3.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
      mRadioButtonImage3.SetPosition( 0, DP(radioY) );

      imageSelectTableView.AddChild( mRadioButtonImage3, Toolkit::TableView::CellPosition( 2, 0 ) );
      imageSelectTableView.AddChild( image, Toolkit::TableView::CellPosition( 2, 1 ) );
    }

    // Create select button
    mUpdateButton = Toolkit::PushButton::New();
    mUpdateButton.SetProperty( Toolkit::Button::Property::LABEL, "Select" );
    mUpdateButton.SetProperty( Dali::Actor::Property::NAME, "selectButton" );
    mUpdateButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

    mUpdateButton.ClickedSignal().Connect( this, &ButtonsController::OnButtonClicked );

    radioGroup2Background.AddChild( mUpdateButton, Toolkit::TableView::CellPosition( 1, 0 ) );

    // ImageView to display selected image
    mImage = Toolkit::ImageView::New( BIG_IMAGE_1 );
    mImage.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mImage.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );
    radioGroup2Background.AddChild( mImage, Toolkit::TableView::CellPosition( 0, 1, 2, 1 ) );

    // The enable/disable radio group
    Toolkit::TableView radioGroup1Background = Toolkit::TableView::New( 1, 1 );
    radioGroup1Background.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    radioGroup1Background.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    radioGroup1Background.SetBackgroundColor( BACKGROUND_COLOUR );
    radioGroup1Background.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );
    radioGroup1Background.SetFitHeight( 0 );

    contentTable.Add( radioGroup1Background );

    // Radio group
    Toolkit::TableView radioButtonsGroup1 = Toolkit::TableView::New( 2, 1 );
    radioButtonsGroup1.SetCellPadding( Size( 0.0f, MARGIN_SIZE * 0.5f ) );
    radioButtonsGroup1.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    for( unsigned int i = 0; i < radioButtonsGroup1.GetRows(); ++i )
    {
      radioButtonsGroup1.SetFitHeight( i );
    }
    radioButtonsGroup1.SetFitWidth( 0 );

    radioGroup1Background.Add( radioButtonsGroup1 );

    // TableView to lay out 2x Radio buttons on the left, and 1x Tick image on the right.
    Toolkit::TableView tableView = Toolkit::TableView::New( 2, 2 );
    tableView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    tableView.SetFitHeight( 0 );
    tableView.SetFitHeight( 1 );
    tableView.SetFitWidth( 0 );
    tableView.SetFitWidth( 1 );

    Toolkit::TextLabel textLabel = Toolkit::TextLabel::New( "Select enabled" );
    textLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::WIDTH );
    textLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
    textLabel.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

    Toolkit::ImageView image = Toolkit::ImageView::New( ENABLED_IMAGE );
    image.SetSize( DP(RADIO_LABEL_THUMBNAIL_SIZE), DP(RADIO_LABEL_THUMBNAIL_SIZE) );
    image.SetPadding( Padding( DP(20.0f), 0.0f, 0.0f, 0.0f ) );
    tableView.AddChild( image, Toolkit::TableView::CellPosition( 0, 1 ) );

    radioButtonsGroup1.Add( tableView );

    // First radio button
    {
      Toolkit::RadioButton radioButton = Dali::Toolkit::RadioButton::New( "Select enabled" );
      radioButton.SetProperty( Dali::Actor::Property::NAME, "radioSelectEnable" );
      radioButton.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
      radioButton.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
      radioButton.SetPosition( 0, 0 );
      radioButton.SetProperty( Toolkit::Button::Property::SELECTED, true );

      radioButton.StateChangedSignal().Connect( this, &ButtonsController::EnableSelectButton );

      tableView.AddChild( radioButton, Toolkit::TableView::CellPosition( 0, 0 ) );
    }

    // Second radio button
    {
      Toolkit::RadioButton radioButton = Dali::Toolkit::RadioButton::New( "Select disabled" );
      radioButton.SetProperty( Dali::Actor::Property::NAME, "radioSelectDisable" );
      radioButton.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
      radioButton.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
      radioButton.SetPosition( 0, DP(50) );

      radioButton.StateChangedSignal().Connect( this, &ButtonsController::EnableSelectButton );

      tableView.AddChild( radioButton, Toolkit::TableView::CellPosition( 1, 0 ) );
    }

    // CheckBoxes
    Toolkit::TableView checkBoxBackground = Toolkit::TableView::New( 3, 1 );
    checkBoxBackground.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    checkBoxBackground.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    checkBoxBackground.SetBackgroundColor( BACKGROUND_COLOUR );
    checkBoxBackground.SetCellPadding( Size( MARGIN_SIZE / 2.0f, MARGIN_SIZE / 2.0f ) );

    for( unsigned int i = 0; i < checkBoxBackground.GetRows(); ++i )
    {
      checkBoxBackground.SetFitHeight( i );
    }

    contentTable.Add( checkBoxBackground );

    {
      mCheckboxButton1 = Toolkit::CheckBoxButton::New();
      mCheckboxButton1.SetProperty( Dali::Actor::Property::NAME, "checkbox1" );
      mCheckboxButton1.SetProperty( Toolkit::Button::Property::LABEL, "CheckBox1 is unselected" );
      mCheckboxButton1.StateChangedSignal().Connect( this, &ButtonsController::OnCheckBoxesSelected );
      mCheckboxButton1.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );

      checkBoxBackground.Add( mCheckboxButton1 );
    }

    {
      mCheckboxButton2 = Toolkit::CheckBoxButton::New();
      mCheckboxButton2.SetProperty( Dali::Actor::Property::NAME, "checkbox2" );
      mCheckboxButton2.SetProperty( Toolkit::Button::Property::LABEL, "CheckBox2 is selected" );
      mCheckboxButton2.SetProperty( Toolkit::Button::Property::SELECTED, true );
      mCheckboxButton2.StateChangedSignal().Connect( this, &ButtonsController::OnCheckBoxesSelected );
      mCheckboxButton2.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );

      checkBoxBackground.Add( mCheckboxButton2 );
    }

    {
      mCheckboxButton3 = Toolkit::CheckBoxButton::New();
      mCheckboxButton3.SetProperty( Dali::Actor::Property::NAME, "checkbox3" );
      mCheckboxButton3.SetProperty( Toolkit::Button::Property::LABEL, "CheckBox3 is unselected" );
      mCheckboxButton3.StateChangedSignal().Connect( this, &ButtonsController::OnCheckBoxesSelected );
      mCheckboxButton3.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );

      checkBoxBackground.Add( mCheckboxButton3 );
    }

    // Create togglabe button
    Toolkit::TableView toggleBackground = Toolkit::TableView::New( 3, 1 );
    toggleBackground.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    toggleBackground.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    toggleBackground.SetBackgroundColor( BACKGROUND_COLOUR );
    toggleBackground.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );

    for( unsigned int i = 0; i < toggleBackground.GetRows(); ++i )
    {
      toggleBackground.SetFitHeight( i );
    }

    contentTable.Add( toggleBackground );

    mToggleButton = Toolkit::PushButton::New();
    mToggleButton.SetProperty( Toolkit::Button::Property::TOGGLABLE, true );
    mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, "Unselected" );
    mToggleButton.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mToggleButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    mToggleButton.StateChangedSignal().Connect( this, &ButtonsController::OnButtonSelected );

    toggleBackground.Add( mToggleButton );
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        // Exit application when click back or escape.
        mApplication.Quit();
      }
    }
  }

  bool OnButtonSelected( Toolkit::Button button )
  {
    Toolkit::PushButton pushButton = Toolkit::PushButton::DownCast( button );
    if( pushButton )
    {
      bool isSelected = button.GetProperty( Toolkit::Button::Property::SELECTED ).Get<bool>();
      if( isSelected )
      {
        pushButton.SetProperty( Toolkit::Button::Property::LABEL, "Selected" );
      }
      else
      {
        pushButton.SetProperty( Toolkit::Button::Property::LABEL, "Unselected" );
      }
    }

    return true;
  }

  bool EnableSelectButton( Toolkit::Button button )
  {
    bool isSelected = button.GetProperty( Toolkit::Button::Property::SELECTED ).Get<bool>();
    if( !isSelected )
    {
      return true;
    }

    if( button.GetProperty< std::string >( Dali::Actor::Property::NAME ) == "radioSelectEnable" )
    {
      mUpdateButton.SetProperty( Toolkit::Button::Property::DISABLED, false );

      mRadioButtonImage1.SetProperty( Toolkit::Button::Property::DISABLED, false );
      mRadioButtonImage2.SetProperty( Toolkit::Button::Property::DISABLED, false );
      mRadioButtonImage3.SetProperty( Toolkit::Button::Property::DISABLED, false );

      mCheckboxButton1.SetProperty( Toolkit::Button::Property::DISABLED, false );
      mCheckboxButton2.SetProperty( Toolkit::Button::Property::DISABLED, false );
      mCheckboxButton3.SetProperty( Toolkit::Button::Property::DISABLED, false );

      mToggleButton.SetProperty( Toolkit::Button::Property::DISABLED, false );
    }
    else if( button.GetProperty< std::string >( Dali::Actor::Property::NAME ) == "radioSelectDisable" )
    {
      mUpdateButton.SetProperty( Toolkit::Button::Property::DISABLED, true );

      mRadioButtonImage1.SetProperty( Toolkit::Button::Property::DISABLED, true );
      mRadioButtonImage2.SetProperty( Toolkit::Button::Property::DISABLED, true );
      mRadioButtonImage3.SetProperty( Toolkit::Button::Property::DISABLED, true );

      mCheckboxButton1.SetProperty( Toolkit::Button::Property::DISABLED, true );
      mCheckboxButton2.SetProperty( Toolkit::Button::Property::DISABLED, true );
      mCheckboxButton3.SetProperty( Toolkit::Button::Property::DISABLED, true );

      mToggleButton.SetProperty( Toolkit::Button::Property::DISABLED, true );
    }

    return true;
  }

  bool OnButtonClicked(Toolkit::Button button)
  {
    if( mRadioButtonImage1.GetProperty( Toolkit::Button::Property::SELECTED ).Get<bool>() )
    {
      mImage.SetImage( BIG_IMAGE_1 );
    }
    else if( mRadioButtonImage2.GetProperty( Toolkit::Button::Property::SELECTED ).Get<bool>() )
    {
      mImage.SetImage( BIG_IMAGE_2 );
    }
    else if( mRadioButtonImage3.GetProperty( Toolkit::Button::Property::SELECTED ).Get<bool>() )
    {
      mImage.SetImage( BIG_IMAGE_3 );
    }
    return true;
  }

  bool OnCheckBoxesSelected( Toolkit::Button button )
  {
    bool isSelected = button.GetProperty( Toolkit::Button::Property::SELECTED ).Get<bool>();
    if( button.GetProperty< std::string >( Dali::Actor::Property::NAME ) == "checkbox1" )
    {
      if( isSelected )
      {
        button.SetProperty( Toolkit::Button::Property::LABEL, "CheckBox1 is selected");
      }
      else
      {
        button.SetProperty( Toolkit::Button::Property::LABEL, "CheckBox1 is unselected");
      }
    }

    if( button.GetProperty< std::string >( Dali::Actor::Property::NAME ) == "checkbox2" )
    {
      if( isSelected )
      {
        button.SetProperty( Toolkit::Button::Property::LABEL, "CheckBox2 is selected");
      }
      else
      {
        button.SetProperty( Toolkit::Button::Property::LABEL, "CheckBox2 is unselected");
      }
    }

    if( button.GetProperty< std::string >( Dali::Actor::Property::NAME ) == "checkbox3" )
    {
      if( isSelected )
      {
        button.SetProperty( Toolkit::Button::Property::LABEL, "CheckBox3 is selected");
      }
      else
      {
        button.SetProperty( Toolkit::Button::Property::LABEL, "CheckBox3 is unselected");
      }
    }

    return true;
  }

private:

  Application&      mApplication;
  Toolkit::Control  mView;                              ///< The View instance.
  Toolkit::ToolBar  mToolBar;                           ///< The View's Toolbar.
  Layer             mContentLayer;                      ///< Content layer

  Toolkit::RadioButton mRadioButtonImage1;
  Toolkit::RadioButton mRadioButtonImage2;
  Toolkit::RadioButton mRadioButtonImage3;

  Toolkit::PushButton mUpdateButton;
  Toolkit::PushButton mToggleButton;

  Toolkit::CheckBoxButton mCheckboxButton1;
  Toolkit::CheckBoxButton mCheckboxButton2;
  Toolkit::CheckBoxButton mCheckboxButton3;

  Animation      mAnimation;

  Toolkit::ImageView mImage;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  ButtonsController test( application );
  application.MainLoop();
  return 0;
}
