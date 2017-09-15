/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 * @brief Basic usage of TextLabel control
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/object/handle-devel.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>
#include <dali-toolkit/dali-toolkit.h>
#include <iostream>

// INTERNAL INCLUDES
#include "shared/multi-language-strings.h"
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace MultiLanguageStrings;

namespace
{
const char* const BACKGROUND_IMAGE = DEMO_IMAGE_DIR "grab-handle.png";
const char* const STYLES_IMAGE = DEMO_IMAGE_DIR "FontStyleButton_Main.png";
const char* const TICK_IMAGE_IMAGE = DEMO_IMAGE_DIR "FontStyleButton_OK_02.png";
const char* const STYLE_SELECTED_IMAGE = DEMO_IMAGE_DIR "FontStyleButton_OK_03.png";

const char* BUTTON_IMAGES[] =
{
  DEMO_IMAGE_DIR "FontStyleButton_Colour.png",
  DEMO_IMAGE_DIR "FontStyleButton_Outline.png",
  DEMO_IMAGE_DIR "FontStyleButton_Shadow.png"
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
  NUMBER_OF_STYLES
};

const Vector4 AVAILABLE_COLORS[] =
{
  Color::GREEN,
  Color::BLUE,
  Color::RED,
  Color::CYAN
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
    mShadowActive( false ),
    mOutlineActive( false ),
    mSelectedColor(AVAILABLE_COLORS[0]),
    mContainer(),
    mGrabCorner(),
    mBorder(),
    mPanGestureDetector(),
    mLayoutSize(),
    mLanguageId( 0u ),
    mAlignment( 0u ),
    mHueAngleIndex( Property::INVALID_INDEX ),
    mOverrideMixColorIndex( Property::INVALID_INDEX )

  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextLabelExample::Create );
  }

  ~TextLabelExample()
  {
    // Nothing to do here.
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
    mLayoutSize = Vector2(mStageSize.width*0.6f, mStageSize.width*0.6f);
    mContainer.SetSize( mLayoutSize );
    mContainer.SetDrawMode( DrawMode::OVERLAY_2D );
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
    mLabel.SetProperty( DevelTextLabel::Property::TEXT_COLOR_ANIMATABLE, Color::GREEN );
    mLabel.SetBackgroundColor( Color::WHITE );
    mContainer.Add( mLabel );

    // Create style activate button
    mStyleMenuButton = PushButton::New();
    mStyleMenuButton.SetPosition( mButtonSize.width, mStageSize.height * STYLE_BUTTON_POSTION_RELATIVE_TO_STAGE );
    mStyleMenuButton.SetSize( mButtonSize );
    mStyleMenuButton.SetProperty( Button::Property::TOGGLABLE, true );
    mStyleMenuButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, STYLES_IMAGE );
    mStyleMenuButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, TICK_IMAGE_IMAGE );

    mStyleMenuButton.ClickedSignal().Connect( this, &TextLabelExample::OnStyleButtonClicked );
    stage.Add( mStyleMenuButton );

    // Add a border for the container so you can see the container is being resized while grabbing the handle.
    mBorder = Control::New();
    mBorder.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mBorder.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mBorder.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );

    Dali::Property::Map border;
    border.Insert( Visual::Property::TYPE,  Visual::BORDER );
    border.Insert( BorderVisual::Property::COLOR,  Color::WHITE );
    border.Insert( BorderVisual::Property::SIZE,  2.f );
    mBorder.SetProperty( Control::Property::BACKGROUND, border );
    mContainer.Add( mBorder );
    mBorder.SetVisible(false);

    DevelActor::RaiseToTop(mGrabCorner);

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

    // Animate the text color 3 times from source color to Yellow
    Animation animation = Animation::New( 2.f );
    animation.AnimateTo( Property( mLabel, DevelTextLabel::Property::TEXT_COLOR_ANIMATABLE ), Color::YELLOW, AlphaFunction::SIN );
    animation.SetLoopCount( 3 );
    animation.Play();

    Property::Value labelText = mLabel.GetProperty( TextLabel::Property::TEXT );
    std::cout << "Displaying text: \"" << labelText.Get< std::string >() << "\"" << std::endl;
  }

  // Depending on button pressed, apply the style to the text label
  bool OnStyleSelected( Toolkit::Button button )
  {
    if( button == mStyleButtons[ StyleType::TEXT_COLOR ] )
    {
      Animation animation = Animation::New( 2.f );
      animation.AnimateTo( Property( mLabel, DevelTextLabel::Property::TEXT_COLOR_ANIMATABLE ), mSelectedColor, AlphaFunction::LINEAR );
      animation.Play();
    }
    else if( button == mStyleButtons[ StyleType::OUTLINE ] )
    {
      Property::Map outlineMap;
      float outlineWidth = OUTLINE_WIDTH;

      if( mOutlineActive )
      {
        outlineWidth = ( mOutlineColor == mSelectedColor ) ? 0.0f : OUTLINE_WIDTH ;  // toggles outline on/off
      }
      mOutlineActive = ( outlineWidth > 0.0f ) ? true : false;

      mOutlineColor = mSelectedColor;
      outlineMap["color"] = mOutlineColor;
      outlineMap["width"] = outlineWidth;
      mLabel.SetProperty( TextLabel::Property::OUTLINE, outlineMap );
    }
    else if( button == mStyleButtons[ StyleType::SHADOW ] )
    {
      Vector2 shadowOffset( SHADOW_OFFSET ); // Will be set to zeros if color already set
      Property::Value value = mLabel.GetProperty( TextLabel::Property::SHADOW_COLOR  );
      Vector4 currentShadowColor;
      value.Get( currentShadowColor );

      if ( mShadowActive )
      {
        // toggle shadow off ( zero offset ) if color is already set
        shadowOffset = ( currentShadowColor == mSelectedColor ) ? Vector2::ZERO : Vector2( SHADOW_OFFSET );
      }

      mShadowActive = ( shadowOffset == Vector2::ZERO ) ? false : true;

      mLabel.SetProperty( TextLabel::Property::SHADOW_OFFSET, shadowOffset );
      mLabel.SetProperty( TextLabel::Property::SHADOW_COLOR, mSelectedColor );
    }
    return true;
  }

  bool OnColorSelected( Toolkit::Button button )
  {
    for( unsigned int index = 0; index < NUMBER_OF_COLORS; index++)
    {
      if ( mColorButtons[index] == button )
      {
        mSelectedColor = AVAILABLE_COLORS[ index ];
        return true;
      }
    }
    return true;
  }

  void ShowColorButtons()
  {
    for( unsigned int index = 0; index < NUMBER_OF_COLORS; index++)
    {
      mColorButtons[index] = RadioButton::New();
      mColorButtons[index].SetPosition( mButtonSize.width, mStageSize.height * STYLE_BUTTON_POSTION_RELATIVE_TO_STAGE - ( mButtonSize.width * (index+1) ) );
      mColorButtons[index].SetSize( mButtonSize );
      mColorButtons[index].ClickedSignal().Connect( this, &TextLabelExample::OnColorSelected );
      mColorButtons[index].SetProperty( Button::Property::TOGGLABLE, true );
      Property::Map propertyMap;
      propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
      propertyMap.Insert(ColorVisual::Property::MIX_COLOR, AVAILABLE_COLORS[ index ]);
      mColorButtons[index].SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, propertyMap );
      mColorButtons[index].SetProperty( Toolkit::DevelButton::Property::UNSELECTED_VISUAL, propertyMap );

      propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
      propertyMap.Insert(ColorVisual::Property::MIX_COLOR, AVAILABLE_COLORS[ index ]);
      mColorButtons[index].SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, propertyMap );

      mColorButtons[index].SetProperty( Toolkit::DevelButton::Property::SELECTED_VISUAL,
                          Property::Map().Add( Visual::Property::TYPE, Visual::BORDER )
                                         .Add( BorderVisual::Property::COLOR, Color::WHITE )
                                         .Add( BorderVisual::Property::SIZE, 2.0f )
                                         .Add( BorderVisual::Property::ANTI_ALIASING, true ) );

      Stage::GetCurrent().Add( mColorButtons[index] );
    }
  }


  void HideColorButtons()
  {
    for( unsigned int index = 0; index < NUMBER_OF_COLORS; index++)
    {
       UnparentAndReset( mColorButtons[index] );
    }
  }

  void HideStyleButtons()
  {
    for( unsigned int index = 0; index < NUMBER_OF_STYLES; index++)
    {
       UnparentAndReset( mStyleButtons[index] );
    }
  }

  bool OnStyleButtonClicked( Toolkit::Button button )
  {
    if ( button.GetProperty( Toolkit::Button::Property::SELECTED ).Get<bool>() )
    {
      for ( unsigned int index = 0; index < NUMBER_OF_STYLES; index++ )
      {
        mStyleButtons[index] = PushButton::New();
        mStyleButtons[index].SetPosition( mButtonSize.width + ( mButtonSize.width * (index+1) ), mStageSize.height * STYLE_BUTTON_POSTION_RELATIVE_TO_STAGE );
        mStyleButtons[index].SetSize( mButtonSize );
        mStyleButtons[index].SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, BUTTON_IMAGES[ index ] );
        mStyleButtons[index].SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, STYLE_SELECTED_IMAGE );
        mStyleButtons[index].ClickedSignal().Connect( this, &TextLabelExample::OnStyleSelected );
        Stage::GetCurrent().Add( mStyleButtons[index] );
      }
      ShowColorButtons();
    }
    else
    {
      // hide menu and colors
      HideColorButtons();
      HideStyleButtons();
    }
    return true;
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
            animation.AnimateTo( Property( mLabel, DevelTextLabel::Property::TEXT_COLOR_ANIMATABLE ), Color::RED, AlphaFunction::SIN );
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

  PushButton mStyleMenuButton;
  PushButton mStyleButtons[ NUMBER_OF_STYLES ];
  bool mShadowActive;
  bool mOutlineActive;
  Vector4 mSelectedColor;
  Vector4 mOutlineColor; // Store outline as Vector4 whilst TextLabel Outline Property returns a string when using GetProperty
  Button mColorButtons[ NUMBER_OF_COLORS ];

  Control mContainer;
  Control mGrabCorner;
  Control mBorder;

  PanGestureDetector mPanGestureDetector;

  Vector2 mLayoutSize;

  Size mStageSize;
  Size mButtonSize;

  unsigned int mLanguageId;
  unsigned int mAlignment;
  Property::Index mHueAngleIndex;
  Property::Index mOverrideMixColorIndex;
};

void RunTest( Application& application )
{
  TextLabelExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
