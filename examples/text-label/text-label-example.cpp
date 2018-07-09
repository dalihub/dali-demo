/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
 * @file text-label-example.cpp
 * @brief Usage of TextLabel control with style application.
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/object/handle-devel.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali-toolkit/dali-toolkit.h>
#include <iostream>

// INTERNAL INCLUDES
#include "shared/multi-language-strings.h"
#include "shared/view.h"
#include "expanding-buttons.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace MultiLanguageStrings;

namespace
{
const char* const BACKGROUND_IMAGE = DEMO_IMAGE_DIR "grab-handle.png";
const char* const STYLE_SELECTED_IMAGE = DEMO_IMAGE_DIR "FontStyleButton_OK_03.png";

const char* BUTTON_IMAGES[] =
{
  DEMO_IMAGE_DIR "FontStyleButton_Colour.png",
  DEMO_IMAGE_DIR "FontStyleButton_Outline.png",
  DEMO_IMAGE_DIR "FontStyleButton_Shadow.png",
  DEMO_IMAGE_DIR "FontStyleButton_Background.png"
};

const unsigned int KEY_ZERO = 10;
const unsigned int KEY_ONE = 11;
const unsigned int KEY_A = 38;
const unsigned int KEY_F = 41;
const unsigned int KEY_H = 43;
const unsigned int KEY_U = 30;
const unsigned int KEY_V = 55;
const unsigned int KEY_M = 58;
const unsigned int KEY_L = 46;
const unsigned int KEY_S = 39;
const unsigned int KEY_PLUS = 21;
const unsigned int KEY_MINUS = 20;

const char* H_ALIGNMENT_STRING_TABLE[] =
{
  "BEGIN",
  "CENTER",
  "END"
};

const unsigned int H_ALIGNMENT_STRING_COUNT = sizeof( H_ALIGNMENT_STRING_TABLE ) / sizeof( H_ALIGNMENT_STRING_TABLE[0u] );

const char* V_ALIGNMENT_STRING_TABLE[] =
{
  "TOP",
  "CENTER",
  "BOTTOM"
};

const unsigned int V_ALIGNMENT_STRING_COUNT = sizeof( V_ALIGNMENT_STRING_TABLE ) / sizeof( V_ALIGNMENT_STRING_TABLE[0u] );

enum StyleType
{
  TEXT_COLOR = 0,
  OUTLINE,
  SHADOW,
  BACKGROUND,
  NUMBER_OF_STYLES
};

const Vector4 AVAILABLE_COLORS[] =
{
  Color::GREEN,
  Color::BLUE,
  Color::RED,
  Color::CYAN,
  Color::WHITE // Used as clear
};

const unsigned int NUMBER_OF_COLORS = sizeof( AVAILABLE_COLORS ) / sizeof( AVAILABLE_COLORS[0u] );

int ConvertToEven(int value)
{
  return (value % 2 == 0) ? value : (value + 1);
}

struct HSVColorConstraint
{
  HSVColorConstraint(float hue, float saturation, float value)
  : hue(hue),
    saturation(saturation),
    value(value)
  {
  }

  void operator()(Vector3& current, const PropertyInputContainer& inputs )
  {
    current = hsv2rgb(Vector3(inputs[0]->GetFloat(), saturation, value));
  }

  Vector3 hsv2rgb(Vector3 colorHSV)
  {
    float r=colorHSV.z*(1+colorHSV.y*(cos(colorHSV.x)-1));
    float g=colorHSV.z*(1+colorHSV.y*(cos(colorHSV.x-2.09439)-1));
    float b=colorHSV.z*(1+colorHSV.y*(cos(colorHSV.x+2.09439)-1));
    return Vector3(r, g, b);
  }
  float hue;
  float saturation;
  float value;
};

const float STYLE_BUTTON_POSTION_RELATIVE_TO_STAGE = 0.9f;
const float BUTTON_SIZE_RATIO_TO_STAGE = 0.1f;
const float OUTLINE_WIDTH = 2.0f;
const Vector2 SHADOW_OFFSET = Vector2( 2.0f, 2.0f );
const int GAP_BETWEEN_BUTTONS = 3;


} // anonymous namespace

/**
 * @brief The main class of the demo.
 */
class TextLabelExample : public ConnectionTracker
{
public:

  TextLabelExample( Application& application )
  : mApplication( application ),
    mLabel(),
    mSelectedColor(AVAILABLE_COLORS[0]),
    mStyleActivatedForColor( NUMBER_OF_STYLES ),
    mContainer(),
    mGrabCorner(),
    mBorder(),
    mPanGestureDetector(),
    mLayoutSize(),
    mLanguageId( 0u ),
    mAlignment( 0u ),
    mHueAngleIndex( Property::INVALID_INDEX ),
    mOverrideMixColorIndex( Property::INVALID_INDEX ),
    mColorButtonsHidden( true ),
    mCollapseColorsAndStyles( false )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextLabelExample::Create );

    // Set Style flags to inactive
    for ( unsigned int i = TEXT_COLOR; i < NUMBER_OF_STYLES; i++ )
    {
      mStyleActiveState[ i ] = false;
      mCurrentStyleColor[i] = AVAILABLE_COLORS[ NUMBER_OF_COLORS - 1 ];
    }
  }

  ~TextLabelExample()
  {
    // Nothing to do here.
  }

  // Clicking the expanding button shows the registered style buttons.
  void SetUpExpandingStyleButtons( Vector2 position )
  {
    mExpandingButtons = Demo::ExpandingButtons::New();
    mExpandingButtons.SetPosition( mButtonSize.width, mStageSize.height * STYLE_BUTTON_POSTION_RELATIVE_TO_STAGE );
    mExpandingButtons.CollapsingSignal().Connect( this, &TextLabelExample::OnExpandingButtonCollapsing );
    mExpandingButtons.SetSize( mButtonSize );
    // Creates the buttons to be expanded
    CreateStyleButtons();

    // Register the created buttons with the ExpandingButtons.
    for ( unsigned int index = 0; index < NUMBER_OF_STYLES; index++ )
    {
      mExpandingButtons.RegisterButton( mStyleButtons[index] );
    }
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    stage.KeyEventSignal().Connect(this, &TextLabelExample::OnKeyEvent);
    mStageSize = stage.GetSize();
    mButtonSize = Size( mStageSize.height * 0.1, mStageSize.height * 0.1 ); // Button size 1/10 of stage height

    mContainer = Control::New();
    mContainer.SetName( "Container" );
    mContainer.SetParentOrigin( ParentOrigin::CENTER );
    mLayoutSize = Vector2(mStageSize.width*0.6f, mStageSize.height*0.6f);
    mContainer.SetSize( mLayoutSize );
    stage.Add( mContainer );

    // Resize the center layout when the corner is grabbed
    mGrabCorner = ImageView::New( BACKGROUND_IMAGE );
    mGrabCorner.SetName( "GrabCorner" );
    mGrabCorner.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mGrabCorner.SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );
    mGrabCorner.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    mContainer.Add( mGrabCorner );

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( mGrabCorner );
    mPanGestureDetector.DetectedSignal().Connect( this, &TextLabelExample::OnPan );

    mLabel = TextLabel::New( "\xF0\x9F\x98\x89 A Quick Brown Fox Jumps Over The Lazy Dog" );

    mLabel.SetName( "TextLabel" );
    mLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mLabel.SetSize(mLayoutSize);
    mLabel.SetProperty( TextLabel::Property::MULTI_LINE, true );
    mLabel.SetProperty( TextLabel::Property::TEXT_COLOR, Color::GREEN );
    mLabel.SetBackgroundColor( Color::WHITE );
    mContainer.Add( mLabel );

    // Clicking ExpandingButton shows the Registered Style buttons, clicking again hides them.
    Vector2 expandingButtonPosition( mButtonSize.width, mStageSize.height * STYLE_BUTTON_POSTION_RELATIVE_TO_STAGE );
    SetUpExpandingStyleButtons( expandingButtonPosition );
    stage.Add( mExpandingButtons );

    // Add a border for the container so you can see the container is being resized while grabbing the handle.
    mBorder = Control::New();
    mBorder.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mBorder.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mBorder.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );

    Dali::Property::Map border;
    border.Insert( Toolkit::Visual::Property::TYPE,  Visual::BORDER );
    border.Insert( BorderVisual::Property::COLOR,  Color::WHITE );
    border.Insert( BorderVisual::Property::SIZE,  3.f );
    mBorder.SetProperty( Control::Property::BACKGROUND, border );
    mContainer.Add( mBorder );
    mBorder.SetVisible(false);
    mGrabCorner.RaiseToTop();

    mHueAngleIndex = mLabel.RegisterProperty( "hue", 0.0f );
    Renderer bgRenderer = mLabel.GetRendererAt(0);
    mOverrideMixColorIndex = DevelHandle::GetPropertyIndex( bgRenderer, ColorVisual::Property::MIX_COLOR );

    Constraint constraint = Constraint::New<Vector3>( bgRenderer, mOverrideMixColorIndex, HSVColorConstraint(0.0f, 0.5f, 0.8f));
    constraint.AddSource( Source( mLabel, mHueAngleIndex ) );
    constraint.SetRemoveAction( Constraint::Discard );
    constraint.Apply();

    Animation anim = Animation::New(50.0f);
    anim.AnimateTo(Property(mLabel, mHueAngleIndex), 6.28318f);
    anim.SetLooping(true);
    anim.Play();

    mContainer.RaiseToTop();
    mGrabCorner.RaiseToTop();

    Property::Value labelText = mLabel.GetProperty( TextLabel::Property::TEXT );
    std::cout << "Displaying text: \"" << labelText.Get< std::string >() << "\"" << std::endl;
  }

  // If the styling buttons should colapse (hide) then the color buttons should also hide.
  bool OnExpandingButtonCollapsing( Demo::ExpandingButtons button )
  {
    mCollapseColorsAndStyles = true;
    HideColorButtons();
    return true;
  }

  // Get the style type from the given button
  StyleType GetStyleTypeFromButton( Toolkit::Button button )
  {
    StyleType style = StyleType::TEXT_COLOR;

    if( button == mStyleButtons[ StyleType::OUTLINE ] )
    {
      style = StyleType::OUTLINE;
    }
    else if( button == mStyleButtons[ StyleType::SHADOW ] )
    {
      style = StyleType::SHADOW;
    }
    else if( button == mStyleButtons[ StyleType::BACKGROUND ] )
    {
      style = StyleType::BACKGROUND;
    }
    return style;
  }

  // Style selected, show color buttons
  bool OnStyleButtonClicked( Toolkit::Button button )
  {
    StyleType selectedStyle = GetStyleTypeFromButton( button );
    if ( mStyleActivatedForColor == selectedStyle )
    {
      HideColorButtons();
    }
    else
    {
      ResetColorButtons( mColorButtons, NUMBER_OF_COLORS );
      ShowColorButtons( selectedStyle);
    }
    return true;
  }

  // Set style to selected color
  bool OnColorSelected( Toolkit::Button button )
  {
    for( unsigned int index = 0; index < NUMBER_OF_COLORS; index++)
    {
      if ( mColorButtons[index] == button )
      {
        mSelectedColor = AVAILABLE_COLORS[ index ];
      }
    }

    switch ( mStyleActivatedForColor )
    {
      case TEXT_COLOR :
      {
        Animation animation = Animation::New( 1.f );
        animation.AnimateTo( Property( mLabel, TextLabel::Property::TEXT_COLOR ), mSelectedColor, AlphaFunction::LINEAR );
        mCurrentStyleColor[ TEXT_COLOR ] = mSelectedColor;
        animation.Play();
        break;
      }
      case OUTLINE :
      {
        Property::Map outlineMap;
        float outlineWidth = OUTLINE_WIDTH;

        if( mStyleActiveState[ OUTLINE ] )
        {
          outlineWidth = ( Color::WHITE == mSelectedColor ) ? 0.0f : OUTLINE_WIDTH ;  // toggles outline on/off
        }
        mStyleActiveState[ OUTLINE ] = ( outlineWidth > 0.0f ) ? true : false;

        outlineMap["color"] = mSelectedColor;
        outlineMap["width"] = outlineWidth;
        mCurrentStyleColor[ OUTLINE ] = mSelectedColor;
        mLabel.SetProperty( TextLabel::Property::OUTLINE, outlineMap );
        break;
      }
      case SHADOW :
      {
        Vector2 shadowOffset( SHADOW_OFFSET ); // Will be set to zeros if color already set
        Property::Value value = mLabel.GetProperty( TextLabel::Property::SHADOW_COLOR  );
        Vector4 currentShadowColor;
        value.Get( currentShadowColor );

        if ( mStyleActiveState[ SHADOW ] )
        {
          // toggle shadow off ( zero offset ) if color is already set
          shadowOffset = (  Color::WHITE == mSelectedColor ) ? Vector2::ZERO : Vector2( SHADOW_OFFSET );
        }

        mStyleActiveState[ SHADOW ] = ( shadowOffset == Vector2::ZERO ) ? false : true;
        mCurrentStyleColor[ SHADOW ] = mSelectedColor;

        Property::Map shadowMap;
        shadowMap.Insert( "offset", shadowOffset );
        shadowMap.Insert( "color", mSelectedColor );
        mLabel.SetProperty( TextLabel::Property::SHADOW, shadowMap );

        break;
      }
      case BACKGROUND :
      {
        Property::Map backgroundMap;
        auto backgroundEnabled(true);

        if( mStyleActiveState[ BACKGROUND ] )
        {
          backgroundEnabled = ( Color::WHITE != mSelectedColor );  // toggles background on/off
        }
        mStyleActiveState[ BACKGROUND ] = backgroundEnabled;

        backgroundMap["color"] = mSelectedColor;
        backgroundMap["enable"] = backgroundEnabled;
        mCurrentStyleColor[ BACKGROUND ] = mSelectedColor;
        mLabel.SetProperty( DevelTextLabel::Property::BACKGROUND, backgroundMap );

        break;
      }
      default :
        break;
    }

    return true;
  }

  // Set the inital color button that should be be selected.
  // If the style already has a color set then that should be used
  void SetInitialSelectedColorButton( StyleType styleButtonIndex )
  {
    Vector4 selectedColor = mCurrentStyleColor[ styleButtonIndex ];

    for ( unsigned int i = 0; i < NUMBER_OF_COLORS; i++ )
    {
      if ( AVAILABLE_COLORS[i] == selectedColor )
      {
        if ( mColorButtons[i] )
        {
          mColorButtons[ i ].SetProperty( Toolkit::DevelButton::Property::SELECTED, true );
        }
        break;
      }
    }
  }

  // Create a bar of color buttons that the user can select.
  void ShowColorButtons( StyleType styleButtonIndex )
  {
    mCollapseColorsAndStyles = false; // Request to show colors so reset flag
    mStyleActivatedForColor = styleButtonIndex;

    for( unsigned int index = 0; index < NUMBER_OF_COLORS; index++)
    {
      if( !mColorButtonsAnimation )
      {
        mColorButtonsAnimation = Animation::New( 0.15f );
        mColorButtonsAnimation.FinishedSignal().Connect( this, &TextLabelExample::OnColorButtonAnimationFinished );
      }

      // Create a color button
      if ( ! mColorButtons[index] )
      {
        mColorButtons[index] = RadioButton::New();
        mColorButtons[index].SetSize( mButtonSize );
        mColorButtons[index].ClickedSignal().Connect( this, &TextLabelExample::OnColorSelected );
        mColorButtons[index].SetProperty( Button::Property::TOGGLABLE, true );
        Property::Map propertyMap;
        propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
        propertyMap.Insert(ColorVisual::Property::MIX_COLOR, AVAILABLE_COLORS[ index ]);
        mColorButtons[index].SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, propertyMap );
        mColorButtons[index].SetProperty( Toolkit::DevelButton::Property::UNSELECTED_VISUAL, propertyMap );
        mColorButtons[index].SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER );
        mColorButtons[index].SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER );


        propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
        propertyMap.Insert(ColorVisual::Property::MIX_COLOR, AVAILABLE_COLORS[ index ]);
        mColorButtons[index].SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, propertyMap );

        mColorButtons[index].SetProperty( Toolkit::DevelButton::Property::SELECTED_VISUAL,
                            Property::Map().Add( Visual::Property::TYPE, Visual::BORDER )
                                           .Add( BorderVisual::Property::COLOR, Color::WHITE )
                                           .Add( BorderVisual::Property::SIZE, 4.0f )
                                           .Add( BorderVisual::Property::ANTI_ALIASING, true ) );

        // Use a white button with 50% transparency as a clear color button
        if ( Color::WHITE == AVAILABLE_COLORS[ index ] && styleButtonIndex != StyleType::TEXT_COLOR )
        {
          mColorButtons[index].SetOpacity(0.5f);

          mColorButtons[index].SetProperty( Toolkit::Button::Property::LABEL,
                                            Property::Map().Add( Toolkit::Visual::Property::TYPE, Toolkit::Visual::TEXT )
                                                           .Add( Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::CENTER )
                                                           .Add( Toolkit::TextVisual::Property::TEXT, "off") );

        }
      }

      SetInitialSelectedColorButton( mStyleActivatedForColor );

      mColorButtons[index].Unparent();

      mStyleButtons[styleButtonIndex].Add( mColorButtons[index] );
      mColorButtons[index].Lower();

      // Position button using nice animation
      mColorButtons[index].SetY( -GAP_BETWEEN_BUTTONS );
      float desiredPosition = -( mButtonSize.height + GAP_BETWEEN_BUTTONS ) * (index);
      AlphaFunction focusedAlphaFunction = AlphaFunction( Vector2 ( 0.32f, 0.08f ), Vector2( 0.38f, 1.72f ) );
      mColorButtonsAnimation.AnimateBy( Property( mColorButtons[index], Actor::Property::POSITION_Y ), desiredPosition, focusedAlphaFunction );
    }

    mColorButtonsHidden = false;
    mColorButtonsAnimation.Play();
  }

  // Remove the color buttons when not being shown.
  void ResetColorButtons( Button buttons[], unsigned int numberOfButtons )
  {
    for( unsigned int index = 0; index < numberOfButtons; index++)
    {
      UnparentAndReset( buttons[index] );
    }
  }

  void OnColorButtonAnimationFinished( Animation& animation )
  {
    animation.Clear();
    if ( mColorButtonsHidden )
    {
      ResetColorButtons( mColorButtons, NUMBER_OF_COLORS );
      animation.Reset(); // Handle reset
      if ( mCollapseColorsAndStyles )
      {
        mExpandingButtons.Collapse();
      }
    }
  }

  // Create the style buttons that will expand from the expanding button.
  void CreateStyleButtons()
  {
    for ( unsigned int index = 0; index < NUMBER_OF_STYLES; index++ )
    {
      if ( ! mStyleButtons[index] )
      {
        mStyleButtons[index] = PushButton::New();
        mStyleButtons[index].SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, BUTTON_IMAGES[ index ] );
        mStyleButtons[index].SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, STYLE_SELECTED_IMAGE );
        mStyleButtons[index].SetProperty( Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
        mStyleButtons[index].SetSize( mButtonSize );
        mStyleButtons[index].ClickedSignal().Connect( this, &TextLabelExample::OnStyleButtonClicked );
      }
    }
  }

  // Animate away the color bar.
  void HideColorButtons()
  {
    if ( ! mColorButtonsHidden )
    {
      for( unsigned int index = 0; index < NUMBER_OF_COLORS; index++)
      {
        mColorButtonsAnimation.AnimateTo( Property( mColorButtons[index], Actor::Property::POSITION_Y ), 0.0f );
      }
      mColorButtonsHidden = true;
      mColorButtonsAnimation.Play();
    }
    mStyleActivatedForColor = NUMBER_OF_STYLES;
  }

  //  Request the expanding button to collapse.
  void HideStyleAndColorButtons()
  {
    mCollapseColorsAndStyles = true;
    if ( mColorButtonsHidden )
    {
      mExpandingButtons.Collapse();
    }
    else
    {
      HideColorButtons();
    }
  }

  // Resize the text-label with pan gesture
  void OnPan( Actor actor, const PanGesture& gesture )
  {
    // Reset mLayoutSize when the pan starts
    if( gesture.state == Gesture::Started )
    {
      if( mLayoutSize.x < 2.0f )
      {
        mLayoutSize.x = 2.0f;
      }

      if( mLayoutSize.y < 2.0f )
      {
        mLayoutSize.y = 2.0f;
      }

      // Only show the border during the panning
      mBorder.SetVisible(true);

      HideStyleAndColorButtons();
    }

    mLayoutSize.x += gesture.displacement.x * 2.0f;
    mLayoutSize.y += gesture.displacement.y * 2.0f;

    if( mLayoutSize.x >= 2.0f ||
        mLayoutSize.y >= 2.0f )
    {
      mLayoutSize.x = std::min ( mLayoutSize.x, mStageSize.width );
      mLayoutSize.y = std::min ( mLayoutSize.y, mStageSize.height*.9f );

      // Avoid pixel mis-alignment issue
      Vector2 clampedSize = Vector2( std::max( ConvertToEven( static_cast<int>( mLayoutSize.x )), 2 ),
                                     std::max( ConvertToEven( static_cast<int>( mLayoutSize.y )), 2 ) );

      mContainer.SetSize( clampedSize );
    }

    if( gesture.state == Gesture::Cancelled || gesture.state == Gesture::Finished )
    {
      // Resize the text label to match the container size when panning is finished
      mLabel.SetSize(mLayoutSize);
      mBorder.SetVisible(false);
    }
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
      else if( event.IsCtrlModifier() )
      {
        switch( event.keyCode )
        {
          // Select rendering back-end
          case KEY_ZERO: // fall through
          case KEY_ONE:
          {
            mLabel.SetProperty( TextLabel::Property::RENDERING_BACKEND, event.keyCode - 10 );
            break;
          }
          case KEY_A: // Animate text colour
          {
            Animation animation = Animation::New( 2.f );
            animation.AnimateTo( Property( mLabel, TextLabel::Property::TEXT_COLOR ), Color::RED, AlphaFunction::SIN );
            animation.SetLoopCount( 3 );
            animation.Play();
            break;
          }
          case KEY_F: // Fill vertically
          {
            if( ResizePolicy::DIMENSION_DEPENDENCY == mLabel.GetResizePolicy(Dimension::HEIGHT) )
            {
              mLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
            }
            else
            {
              mLabel.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
            }
            break;
          }
          case KEY_H: // Horizontal alignment
          {
            if( ++mAlignment >= H_ALIGNMENT_STRING_COUNT )
            {
              mAlignment = 0u;
            }

            mLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, H_ALIGNMENT_STRING_TABLE[ mAlignment ] );
            break;
          }
          case KEY_V: // Vertical alignment
          {
            if( ++mAlignment >= V_ALIGNMENT_STRING_COUNT )
            {
              mAlignment = 0u;
            }

            mLabel.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, V_ALIGNMENT_STRING_TABLE[ mAlignment ] );
            break;
          }
          case KEY_M: // Multi-line
          {
            bool multiLine = mLabel.GetProperty<bool>( TextLabel::Property::MULTI_LINE );
            mLabel.SetProperty( TextLabel::Property::MULTI_LINE, !multiLine );
            break;
          }
          case KEY_L: // Language
          {
            const Language& language = LANGUAGES[ mLanguageId ];

            mLabel.SetProperty( TextLabel::Property::TEXT, language.text );

            if( ++mLanguageId >= NUMBER_OF_LANGUAGES )
            {
              mLanguageId = 0u;
            }
            break;
          }
          case KEY_S: // Shadow color
          {
            if( Color::BLACK == mLabel.GetProperty<Vector4>( TextLabel::Property::SHADOW_COLOR ) )
            {
              mLabel.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::RED );
            }
            else
            {
              mLabel.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::BLACK );
            }
            break;
          }
          case KEY_U: // Markup
          {
            mLabel.SetProperty( TextLabel::Property::ENABLE_MARKUP, true );
            mLabel.SetProperty( TextLabel::Property::TEXT, "<font family='DejaVuSerif' size='18'>H<color value='blue'>ello</color> <font weight='bold'>world</font> demo</font>" );
            break;
          }
          case KEY_PLUS: // Increase shadow offset
          {
            mLabel.SetProperty( TextLabel::Property::SHADOW_OFFSET, mLabel.GetProperty<Vector2>( TextLabel::Property::SHADOW_OFFSET ) + Vector2( 1.0f, 1.0f ) );
            break;
          }
          case KEY_MINUS: // Decrease shadow offset
          {
            mLabel.SetProperty( TextLabel::Property::SHADOW_OFFSET, mLabel.GetProperty<Vector2>( TextLabel::Property::SHADOW_OFFSET ) - Vector2( 1.0f, 1.0f ) );
            break;
          }

        }
      }
    }
  }

private:

  Application& mApplication;

  TextLabel mLabel;

  Demo::ExpandingButtons mExpandingButtons;
  PushButton mStyleButtons[ NUMBER_OF_STYLES ];
  bool mStyleActiveState[ NUMBER_OF_STYLES ];

  Vector4 mCurrentStyleColor[NUMBER_OF_STYLES ];

  Vector4 mSelectedColor;

  Button mColorButtons[ NUMBER_OF_COLORS ];

  StyleType mStyleActivatedForColor; // The style that the color bar is connected to

  Control mContainer;
  Control mGrabCorner;
  Control mBorder;

  PanGestureDetector mPanGestureDetector;

  Vector2 mLayoutSize;

  Animation mColorButtonsAnimation;

  Size mStageSize;
  Size mButtonSize;

  unsigned int mLanguageId;
  unsigned int mAlignment;
  Property::Index mHueAngleIndex;
  Property::Index mOverrideMixColorIndex;

  bool mColorButtonsHidden;
  bool mCollapseColorsAndStyles;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  TextLabelExample test( application );
  application.MainLoop();
  return 0;
}
