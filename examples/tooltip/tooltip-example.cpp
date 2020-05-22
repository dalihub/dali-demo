/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/tooltip/tooltip-properties.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const Vector4 STAGE_COLOR( 211.0f / 255.0f, 211.0f / 255.0f, 211.0f / 255.0f, 1.0f ); ///< The color of the stage
const char * const THEME_PATH( DEMO_STYLE_DIR "tooltip-example-theme.json" ); ///< The theme used for this example
const float POSITION_INCREMENTER( 0.2f ); ///< The position difference between the controls along the Y-Axis.
} // unnamed namespace

/**
 * @brief Creates a controller which demonstrates the tooltip functionality in control.
 *
 * The Control base class supports tooltip functionality. However, the Toolkit Tooltip style is only set on Buttons by default.
 * This example portrays the different ways in which a tooltip can be displayed and customised.
 */
class TooltipController : public ConnectionTracker
{
public:

  TooltipController( Application& application )
  : mApplication( application ),
    previousPosition( 0.0f )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TooltipController::Create );
  }

private:

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    // Set the stage background color and connect to the stage's key signal to allow Back and Escape to exit.
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( STAGE_COLOR );
    stage.KeyEventSignal().Connect( this, &TooltipController::OnKeyEvent );
    const Vector2 stageSize = stage.GetSize();

    // Add a text label at the top for information purposes
    Control label = TextLabel::New( "Hover over buttons to see tooltip" );
    label.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER );
    label.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER );
    label.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "Center" );
    stage.Add( label );

    // Simple tooltip from stylesheet
    Control simple = PushButton::New();
    simple.SetStyleName( "TooltipTextOnly" );
    SetLabel( simple, "Simple" );
    Layout( simple, stageSize );
    stage.Add( simple );

    // Tooltip with icon and text, from stylesheet
    Control iconWithText = PushButton::New();
    iconWithText.SetStyleName( "TooltipArray" );
    SetLabel( iconWithText, "Icon with Text" );
    Layout( iconWithText, stageSize );
    stage.Add( iconWithText );

    // Tooltip with custom style, from stylesheet
    Control customFromStylesheet = PushButton::New();
    customFromStylesheet.SetStyleName( "TooltipCustom" );
    SetLabel( customFromStylesheet, "Custom From Stylesheet" );
    Layout( customFromStylesheet, stageSize );
    stage.Add( customFromStylesheet );

    // Tooltip with custom style, from code
    Control customFromCode = PushButton::New();
    SetLabel( customFromCode, "Custom From Code" );
    Layout( customFromCode, stageSize );
    customFromCode.SetProperty( DevelControl::Property::TOOLTIP,
                                { { Tooltip::Property::CONTENT,
                                    Property::Array{ { { { Toolkit::Visual::Property::TYPE, Visual::IMAGE },
                                                         { ImageVisual::Property::URL, DEMO_IMAGE_DIR "Logo-for-demo.png" } } },
                                                     { { { Toolkit::Visual::Property::TYPE, Visual::TEXT },
                                                         { TextVisual::Property::TEXT_COLOR, Color::WHITE },
                                                         { TextVisual::Property::TEXT, "Custom coded style\nat hover point" },
                                                         { TextVisual::Property::MULTI_LINE, true },
                                                         { TextVisual::Property::HORIZONTAL_ALIGNMENT, "CENTER" },
                                                         { TextVisual::Property::POINT_SIZE, 16 } } } } },
                                  { Tooltip::Property::LAYOUT, Vector2( 2, 1 ) },
                                  { Tooltip::Property::POSITION, Tooltip::Position::HOVER_POINT },
                                  { Tooltip::Property::BACKGROUND,
                                    { { Tooltip::Background::Property::VISUAL, DEMO_IMAGE_DIR "tooltip.9.png" },
                                      { Tooltip::Background::Property::BORDER, Rect< int >( 1, 5, 5, 1 ) } } }
                                } );
    stage.Add( customFromCode );
  }

  /**
   * @brief Called when any key event is received
   *
   * Will use this to quit the application if Back or the Escape key is received
   * @param[in]  event  The key event information
   */
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if ( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

  /**
   * @brief Sets the label on the control.
   * @param[in]  label  The label to set.
   */
  void SetLabel( Control control, std::string label )
  {
    if( control )
    {
      control.SetProperty( Button::Property::LABEL,
                           Property::Map().Add( Toolkit::Visual::Property::TYPE, Visual::TEXT )
                                          .Add( TextVisual::Property::TEXT, label ) );
    }
  }

  /**
   * @brief Lays out the control in the appropriate location.
   * @param[in]  control    The control to layout.
   * @param[in]  stageSize  The size of the stage, passing it in so we don't have to retrieve it every time.
   */
  void Layout( Control control, const Vector2& stageSize )
  {
    if( control )
    {
      previousPosition += POSITION_INCREMENTER;
      control.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      control.SetSizeModeFactor( Vector3( 0.75, 0.1, 1.0 ) );
      control.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
      control.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER );
      control.SetY( stageSize.height * previousPosition );
    }
  }

private:
  Application&  mApplication;
  float previousPosition;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, THEME_PATH );

  TooltipController test( application );

  application.MainLoop();

  return 0;
}
