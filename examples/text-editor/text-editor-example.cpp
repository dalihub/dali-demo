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

/**
 * @file simple-text-editor-example.cpp
 * @brief Very basic usage of TextEditor control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>
#include <sstream>

// INTERNAL INCLUDES
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char * const THEME_PATH( DEMO_STYLE_DIR "text-editor-example-theme.json" ); ///< The theme used for this example
const Vector4 BACKGROUND_COLOR( 0.04f, 0.345f, 0.392f, 1.0f );      ///< The background color.
const char*   TOOLBAR_IMAGE = DEMO_IMAGE_DIR "top-bar.png";         ///< The tool-bar image.
const float   TOOLBAR_BUTTON_PERCENTAGE = 0.1f;                     ///< The button's space width as a percentage of the toolbar's width.
const float   TOOLBAR_TITLE_PERCENTAGE = 0.7f;                      ///< The title's width as a percentage of the toolbar's width.
const float   TOOLBAR_HEIGHT_PERCENTAGE = 0.05f;                    ///< The toolbar's height as a percentage of the stage's height.
const float   TOOLBAR_PADDING = 4.f;                                ///< The padding in pixels.
const float   BUTTON_PERCENTAGE = 0.8f;                             ///< The button's height as a percentage of the space for the buttons in the toolbar.
const Vector3 TEXT_EDITOR_RELATIVE_SIZE( 1.f, 0.45f, 1.0f );        ///< The size of the text editor as a percentage of the stage's size.
const Vector4 TEXT_EDITOR_BACKGROUND_COLOR( 1.f, 1.f, 1.f, 0.15f ); ///< The background color of the text editor.

const Vector4 COLORS[] = { Color::RED,
                           Color::GREEN,
                           Color::BLUE,
                           Color::YELLOW,
                           Color::CYAN,
                           Color::MAGENTA,
                           Color::WHITE,
                           Color::BLACK };
const unsigned int NUMBER_OF_COLORS = sizeof( COLORS ) / sizeof( Vector4 );

} // Unnamed namespace

/**
 * @brief The main class of the demo.
 */
class TextEditorExample : public ConnectionTracker
{
public:
  TextEditorExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextEditorExample::Create );
  }

  ~TextEditorExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    // Respond to key events
    stage.KeyEventSignal().Connect(this, &TextEditorExample::OnKeyEvent);

    // Set a background color.
    stage.SetBackgroundColor( BACKGROUND_COLOR );

    // The stage size.
    const Vector2 stageSize = stage.GetSize();

    // Creates a default view with a default tool bar.
    // The view is added to the stage.

    // Set the toolbar style
    const float toolBarHeight = TOOLBAR_HEIGHT_PERCENTAGE * stageSize.height;
    const DemoHelper::ViewStyle viewStyle( TOOLBAR_BUTTON_PERCENTAGE,
                                           TOOLBAR_TITLE_PERCENTAGE,
                                           toolBarHeight,
                                           TOOLBAR_PADDING );

    Layer contents = DemoHelper::CreateView( mApplication,
                                             mView,
                                             mToolBar,
                                             "",
                                             TOOLBAR_IMAGE,
                                             "",
                                             viewStyle );

    // Create a label for the color selection button.
    // The button will be a child of this, so as to be placed next to it.
    TextLabel colorLabel = TextLabel::New( "Text Color: " );
    colorLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::WIDTH );
    colorLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
    colorLabel.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

    // Create a container for the color button, to layout the drop-down list below it.
    mColorContainer = Control::New();
    mColorContainer.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::WIDTH );
    mColorContainer.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::HEIGHT );
    mColorContainer.SetSizeModeFactor( Vector3( 0.0f, BUTTON_PERCENTAGE, 0.0f ) );

    // Place to right of parent.
    mColorContainer.SetParentOrigin( ParentOrigin::CENTER_RIGHT );
    mColorContainer.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
    colorLabel.Add( mColorContainer );

    // Add border to highlight harder-to-see colors.
    // We use a color rather than border visual as the container will always be behind the button.
    Property::Map colorMap;
    colorMap.Insert( Toolkit::Visual::Property::TYPE, Visual::COLOR);
    colorMap.Insert( ColorVisual::Property::MIX_COLOR, Color::BLACK );
    mColorContainer.SetProperty( Control::Property::BACKGROUND, colorMap );

    // Create a 'select color' button.
    mColorButtonOption = Toolkit::PushButton::New();
    mColorButtonOption.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mColorButtonOption.SetSizeModeFactor( Vector3( 0.9f, 0.9f, 0.0f ) ); // Smaller than container to show border.
    mColorButtonOption.SetParentOrigin( ParentOrigin::CENTER );
    mColorButtonOption.SetAnchorPoint( AnchorPoint::CENTER );

    SetButtonColor( mColorButtonOption, Color::BLACK );

    mColorButtonOption.ClickedSignal().Connect( this, &TextEditorExample::OnChangeColorButtonClicked );
    mColorContainer.Add( mColorButtonOption );

    //Add label to toolbar, which will also add the color button next to it.
    mToolBar.AddControl( colorLabel, viewStyle.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalLeft, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    // Create the text editor.
    mEditor = TextEditor::New();
    mEditor.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mEditor.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mEditor.SetPosition( 0.f, toolBarHeight, 0.f );
    mEditor.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mEditor.SetSizeModeFactor( TEXT_EDITOR_RELATIVE_SIZE );

    mEditor.SetBackgroundColor( TEXT_EDITOR_BACKGROUND_COLOR );

    const Size boundingBoxSize( stageSize * TEXT_EDITOR_RELATIVE_SIZE.GetVectorXY() );
    Rect<int> boundingBox( 0,
                           static_cast<int>( toolBarHeight ),
                           static_cast<int>( boundingBoxSize.width ),
                           static_cast<int>( boundingBoxSize.height - toolBarHeight ) );

    mEditor.SetProperty( TextEditor::Property::DECORATION_BOUNDING_BOX, boundingBox );
    mEditor.SetProperty( TextEditor::Property::TEXT_COLOR, Color::BLACK );
    mEditor.SetProperty( TextEditor::Property::TEXT,
                         "Lorem ipsum dolor sit amet, aeque definiebas ea mei, posse iracundia ne cum.\n"
                         "Usu ne nisl maiorum iudicabit, veniam epicurei oporteat eos an.\n"
                         "Ne nec nulla regione albucius, mea doctus delenit ad!\n"
                         "Et everti blandit adversarium mei, eam porro neglegentur suscipiantur an.\n"
                         "Quidam corpora at duo. An eos possim scripserit?\n\n"
                         "Aťqui dicant sěnťenťíae aň vel!\n"
                         "Vis viris médiocrem elaboraret ét, verear civibus moderatius ex duo!\n"
                         "Án veri laborě iňtěgré quó, mei aď poššit lobortis, mei prompťa čonsťitůťó eů.\n"
                         "Aliquip sanctůs delicáta quí ěá, et natum aliquam est?\n"
                         "Asšúm sapěret usu ůť.\n"
                         "Síť ut apeirián laboramúš percipitur, sůas hařum ín éos?\n" );

    mEditor.InputStyleChangedSignal().Connect( this, &TextEditorExample::OnTextInputStyleChanged );

    contents.Add( mEditor );
    StyleManager styleManager = StyleManager::Get();
    styleManager.ApplyTheme( THEME_PATH );
  }

  void CreateButtonContainer()
  {
    mButtonContainer = Toolkit::TableView::New( NUMBER_OF_COLORS, 1u );
    mButtonContainer.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mButtonContainer.SetSizeModeFactor( Vector3( 1.0f, NUMBER_OF_COLORS, 1.0f ) );

    // Place below color selection button.
    mButtonContainer.SetParentOrigin( ParentOrigin::BOTTOM_CENTER  );
    mButtonContainer.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mButtonContainer.SetPosition( 0.0f, 2.f * TOOLBAR_PADDING, 0.f );
    mColorContainer.Add( mButtonContainer );

    const Vector3 buttonPercentage( 1.f, 0.8f / static_cast<float>( NUMBER_OF_COLORS ), 1.f );
    for( unsigned int index = 0u; index < NUMBER_OF_COLORS; ++index )
    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      button.SetSizeModeFactor( buttonPercentage );

      std::ostringstream s;
      s << "color" << index;
      button.SetName( s.str() );

      SetButtonColor( button, COLORS[index] );

      button.ClickedSignal().Connect( this, &TextEditorExample::OnColorButtonClicked );

      mButtonContainer.Add( button );
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

  bool OnChangeColorButtonClicked( Button button )
  {
    if( !mButtonContainer )
    {
      CreateButtonContainer();
    }

    mButtonContainer.SetVisible( true );
    mButtonContainer.SetSensitive( true );
    return true;
  }

  bool OnColorButtonClicked( Button button )
  {
    const std::string& name = button.GetName();

    Vector4 color;
    if( "color" == name.substr( 0u, 5u ) )
    {
      const unsigned int index = strtoul( name.substr( 5u, 1u ).c_str(), NULL, 10u );
      color = COLORS[index];
      mEditor.SetProperty( TextEditor::Property::INPUT_COLOR, color );
    }

    SetButtonColor( mColorButtonOption, color  );

    mButtonContainer.SetVisible( false );
    mButtonContainer.SetSensitive( false );

    return true;
  }

  void OnTextInputStyleChanged( TextEditor editor, TextEditor::InputStyle::Mask mask )
  {
    if( TextEditor::InputStyle::NONE != static_cast<TextEditor::InputStyle::Mask>( mask & TextEditor::InputStyle::COLOR ) )
    {
      const Vector4 color = editor.GetProperty( TextEditor::Property::INPUT_COLOR ).Get<Vector4>();
      SetButtonColor( mColorButtonOption, color  );
    }

    editor.Reset();
  }

  void SetButtonColor( Button& button, const Vector4& color )
  {
    Property::Map colorVisualMap;
    colorVisualMap.Add( Toolkit::Visual::Property::TYPE, Visual::COLOR )
                  .Add( ColorVisual::Property::MIX_COLOR, color );

    button.SetProperty( DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, colorVisualMap );
    button.SetProperty( DevelButton::Property::SELECTED_BACKGROUND_VISUAL, colorVisualMap );
  }

private:

  Application& mApplication;

  Toolkit::Control    mView;
  Toolkit::ToolBar    mToolBar;
  Toolkit::TextEditor mEditor;
  Toolkit::Control    mColorContainer;
  Toolkit::PushButton mColorButtonOption;
  Toolkit::TableView  mButtonContainer;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  // DALI_DEMO_THEME_PATH not passed to Application so TextEditor example uses default Toolkit style sheet.
  Application application = Application::New( &argc, &argv );
  TextEditorExample test( application );
  application.MainLoop();
  return 0;
}
