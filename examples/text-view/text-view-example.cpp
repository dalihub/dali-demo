/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

namespace
{

const char* const BACKGROUND_IMAGE = DALI_IMAGE_DIR "background-gradient.jpg";
const char* const TOOLBAR_IMAGE = DALI_IMAGE_DIR "top-bar.png";

const char* const TOOLBAR_TITLE = "Text View";
const int TOOLBAR_HEIGHT = 80;

const int NUM_TABLE_ROWS = 12;
const int NUM_TABLE_COLUMNS = 12;

}  // namespace

/**
 * Helper structs
 */
struct TableCell
{
  unsigned int row;
  unsigned int column;
  unsigned int rowSpan;
  unsigned int columnSpan;
};

struct TableString
{
  std::string fontName;
  std::string fontStyle;
  float fontSize;
  Dali::TextStyle::Weight fontWeight;
  Vector4 fontColour;

  std::string text;

  Toolkit::Alignment::Type horizontalAlignment;
  Toolkit::Alignment::Type verticalAlignment;
  float padding;

  float orientation;

  TableCell cellPosition;
};

namespace
{
//                                      Font            Font style  Font point size  Font weight                  Text colour                           Text                          Horizontal alignment                  Vertical alignment                  Padding   Orientation   Table cell position and span
const TableString TABLE_STRINGS[] = { { "HelveticaNue", "Regular",  8.0f,            Dali::TextStyle::REGULAR,    Vector4( 0.5f, 1.0f, 0.0f, 1.0f ),    "Howdy",                      Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalTop,    10.0f,    0.0f,         { 0, 0, 2, 4 } },
                                      { "HelveticaNue", "Regular",  13.0f,           Dali::TextStyle::EXTRABOLD,  Vector4( 1.0f, 0.5f, 0.0f, 1.0f ),    "Hello!",                     Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalCenter, 10.0f,    0.0f,         { 0, 4, 1, 4 } },
                                      { "HelveticaNue", "Regular",  18.0f,           Dali::TextStyle::REGULAR,    Vector4( 1.0f, 0.75f, 0.25f, 1.0f ),  "שלום!",                                               Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalTop,    10.0f,    0.0f,         { 1, 4, 1, 4 } },
                                      { "HelveticaNue", "Regular",  8.0f,            Dali::TextStyle::REGULAR,    Vector4( 0.5f, 0.0f, 1.0f, 1.0f ),    "Hi there",                   Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalBottom, 10.0f,    0.0f,         { 0, 8, 2, 4 } },
                                      { "HelveticaNue", "Regular",  12.0f,           Dali::TextStyle::REGULAR,    Vector4( 0.5f, 1.0f, 1.0f, 1.0f ),    "Hola",                       Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalCenter, 10.0f,    90.0f,        { 2, 0, 4, 2 } },
                                      { "HelveticaNue", "Regular",  18.0f,           Dali::TextStyle::BOLD,       Vector4( 0.5f, 1.0f, 0.5f, 1.0f ),    "Bonjour",                    Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalTop,    10.0f,    0.0f,         { 2, 2, 2, 4 } },
                                      { "HelveticaNue", "Regular",  12.0f,           Dali::TextStyle::REGULAR,    Vector4( 1.0f, 1.0f, 0.5f, 1.0f ),    "Ciao",                       Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalCenter, 10.0f,    0.0f,         { 2, 7, 2, 3 } },
                                      { "HelveticaNue", "Regular",  26.0f,           Dali::TextStyle::EXTRABLACK, Vector4( 0.5f, 0.0f, 0.0f, 1.0f ),    "Hello",                      Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalCenter, 10.0f,    0.0f,         { 4, 2, 1, 6 } },
                                      { "HelveticaNue", "Regular",  8.0f,            Dali::TextStyle::DEMIBOLD,   Vector4( 0.0f, 0.5f, 0.0f, 1.0f ),    "Top of the morning to you",  Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalCenter, 10.0f,    90.0f,        { 4, 10, 8, 2 } },
                                      { "HelveticaNue", "Regular",  13.0f,           Dali::TextStyle::DEMIBOLD,   Vector4( 1.0f, 1.0f, 1.0f, 1.0f ),    "हैलो",                                                   Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalTop,    10.0f,    0.0f,         { 6, 1, 1, 3 } },
                                      { "HelveticaNue", "Regular",  8.0f,            Dali::TextStyle::DEMIBOLD,   Vector4( 1.0f, 1.0f, 0.0f, 1.0f ),    "สวัสดี",                       Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalTop,    10.0f,    90.0f,        { 6, 5, 2, 1 } },
                                      { "HelveticaNue", "Regular",  18.0f,           Dali::TextStyle::REGULAR,    Vector4( 0.0f, 1.0f, 1.0f, 1.0f ),    "你好",                        Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalBottom, 10.0f,    0.0f,         { 6, 6, 1, 3 } },
                                      { "HelveticaNue", "Regular",  34.0f,           Dali::TextStyle::REGULAR,    Vector4( 0.0f, 0.0f, 1.0f, 1.0f ),    "G'day",                      Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalCenter, 10.0f,    0.0f,         { 7, 0, 2, 10 } },
                                      { "HelveticaNue", "Regular",  16.0f,           Dali::TextStyle::EXTRABLACK, Vector4( 0.0f, 0.5f, 1.0f, 1.0f ),    "مرحبا",                      Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalTop,    10.0f,    0.0f,         { 9, 1, 2, 4 } },
                                      { "HelveticaNue", "Regular",  10.0f,           Dali::TextStyle::EXTRABLACK, Vector4( 1.0f, 0.0f, 0.0f, 1.0f ),    "こんにちは",                                         Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalCenter, 10.0f,    0.0f,         { 10, 0, 2, 6 } },
                                      { "HelveticaNue", "Regular",  14.0f,           Dali::TextStyle::REGULAR,    Vector4( 0.0f, 1.0f, 0.0f, 1.0f ),    "aloha",                      Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalTop,    10.0f,    0.0f,         { 10, 6, 2, 4 } }
                                    };

const int NUM_TABLE_STRINGS = sizeof( TABLE_STRINGS ) / sizeof( TABLE_STRINGS[0] );

} // namespace

/**
 * This example shows the usage of TextView.
 */
class TextViewController: public ConnectionTracker
{
public:

  TextViewController( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextViewController::Create );
  }

  ~TextViewController()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    Stage stage = Stage::GetCurrent();

    // Respond to key events
    stage.KeyEventSignal().Connect(this, &TextViewController::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            TOOLBAR_TITLE );

    // Create a table view the height of the stage
    Toolkit::TableView textContainer = Toolkit::TableView::New( NUM_TABLE_ROWS, NUM_TABLE_COLUMNS );
    textContainer.SetParentOrigin( ParentOrigin::TOP_LEFT );
    textContainer.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    textContainer.SetPosition( 0, TOOLBAR_HEIGHT );
    textContainer.SetSize( stage.GetSize().width, stage.GetSize().height - TOOLBAR_HEIGHT );

    mContentLayer.Add( textContainer );

    // Add data
    for( int i = 0; i < NUM_TABLE_STRINGS; ++i )
    {
      const TableString& tableString = TABLE_STRINGS[ i ];

      Dali::TextStyle textStyle;
      textStyle.SetFontName( tableString.fontName );
      textStyle.SetFontStyle( tableString.fontStyle );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( tableString.fontSize ) ) );
      textStyle.SetWeight( tableString.fontWeight );
      textStyle.SetTextColor( tableString.fontColour );

      Toolkit::TextView textView = Toolkit::TextView::New( tableString.text );
      textView.SetStyleToCurrentText( textStyle );
      textView.SetRotation( Dali::Degree( tableString.orientation ), Vector3( 0.0f, 0.0f, 1.0f ) );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( tableString.horizontalAlignment, tableString.verticalAlignment );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( tableString.padding, tableString.padding, tableString.padding, tableString.padding ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( tableString.cellPosition.row, tableString.cellPosition.column, tableString.cellPosition.rowSpan, tableString.cellPosition.columnSpan ) );
    }
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

private:

  Application&      mApplication;
  Toolkit::View     mView;                   ///< The View instance.
  Toolkit::ToolBar  mToolBar;                ///< The View's Toolbar.
  Layer             mContentLayer;           ///< Content layer

};

void RunTest( Application& application )
{
  TextViewController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
