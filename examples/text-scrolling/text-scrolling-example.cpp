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
 * @file text-scrolling-example.cpp
 * @brief Shows text labels with scrolling text and allows a text label and text field control to be scrolled vertically
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const Vector2 DESKTOP_SIZE( Vector2( 1440.f, 1600.f ) );
const Vector2 BOX_SIZE( Vector2(330.0f, 80.0f ) );
const Vector2 SCROLLING_BOX_SIZE( Vector2(330.0f, 40.0f ) );
const float MAX_OFFSCREEN_RENDERING_SIZE = 2048.f;
const float SCREEN_BORDER = 5.0f; // Border around screen that Popups and handles will not exceed

const char * ALIGNMENT_TABLE[] =
{
  "BEGIN",
  "CENTER",
  "END"
};
const unsigned int ALIGNMENT_TABLE_COUNT = sizeof( ALIGNMENT_TABLE ) / sizeof( ALIGNMENT_TABLE[ 0 ] );

enum Labels
{
  SMALL = 1u,
  RTL = 1u << 1,
  LARGE = 1u << 2,
  RTL_LONG = 1u << 4,
  NONE = 1u << 6,
};
}
/**
 * @brief The main class of the demo.
 */
class TextScrollingExample : public ConnectionTracker
{
public:

  TextScrollingExample( Application& application )
  : mApplication( application ),
    mTargetActorPosition(),
    mTargetActorSize(),
    mToggleColor( false )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextScrollingExample::Create );
  }

  ~TextScrollingExample()
  {
    // Nothing to do here.
  }


  void CreateBox( const std::string& name, Actor& box, Actor parent, const Vector2& size )
  {
    box.SetName(name);
    box.SetAnchorPoint( AnchorPoint::CENTER );
    box.SetParentOrigin( ParentOrigin::CENTER );
    box.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );
    box.SetResizePolicy( ResizePolicy::FIXED, Dimension::WIDTH );
    box.SetSize( size.width, 0.f );
    parent.Add( box );

    Dali::Property::Map border;
    border.Insert( Toolkit::Visual::Property::TYPE,  Visual::BORDER );
    border.Insert( BorderVisual::Property::COLOR,  Color::BLUE );
    border.Insert( BorderVisual::Property::SIZE,  1.f );
    box.SetProperty( Control::Property::BACKGROUND, border );
  }

  void CreateLabel( Actor& label, const std::string text, Actor parent, bool scrollOnStart, PushButton button )
  {
    label = TextLabel::New( text );
    label.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    label.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
    label.SetPadding( Padding( 1.0f, 1.0f, 1.0f, 1.0f ) );
    label.SetAnchorPoint( AnchorPoint::CENTER );
    label.SetParentOrigin( ParentOrigin::CENTER );
    parent.Add( label );

    if ( scrollOnStart )
    {
      label.SetProperty(TextLabel::Property::ENABLE_AUTO_SCROLL, true);
    }

    button.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    button.SetSize(BOX_SIZE.height,BOX_SIZE.height);
    button.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    button.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    parent.Add(button);
  }


  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    mStageSize = stage.GetSize();

    stage.KeyEventSignal().Connect(this, &TextScrollingExample::OnKeyEvent);

    // Create Root actor
    Actor rootActor = Actor::New();
    rootActor.SetName("rootActor");
    rootActor.SetResizePolicy( ResizePolicy::FIXED,  Dimension::ALL_DIMENSIONS );
    rootActor.SetSize( mStageSize );
    rootActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );

    stage.Add( rootActor );

    mAnimation = Animation::New( 1.0f );

    const Size mTargetActorSize( mStageSize.width, DESKTOP_SIZE.height );

    // Create Desktop
    Control desktop = Control::New();
    desktop.SetBackgroundColor( Color::WHITE );
    desktop.SetName("desktopActor");
    desktop.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    desktop.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    desktop.SetSize( mTargetActorSize );

    rootActor.Add( desktop ); // Add desktop (content) to offscreen actor

    // Create Boxes
    Control boxA = Control::New();
    Control boxB = Control::New();
    Control boxC = Control::New();
    Control boxD = Control::New();
    Control boxE = Control::New();

    CreateBox( "boxA", boxA, desktop, BOX_SIZE );
    boxA.SetPosition( 0.0f, -500.0f, 1.0f );

    // Create TextField
    TextField field = TextField::New();
    field.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    field.SetPadding( Padding( 1.0f, 1.0f, 1.0f, 1.0f ) );
    field.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Enter Folder Name" );
    field.SetProperty( TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>( SCREEN_BORDER, SCREEN_BORDER, mStageSize.width - SCREEN_BORDER*2, mStageSize.height - SCREEN_BORDER*2 ) );
    boxA.Add( field );
    boxA.SetSize(BOX_SIZE);

    CreateBox( "boxB", boxB, desktop, SCROLLING_BOX_SIZE );
    boxB.SetPosition( 0.0f, -400.0f, 1.0f );
    Toolkit::PushButton scrollLargeButton = Toolkit::PushButton::New();
    scrollLargeButton.ClickedSignal().Connect( this, &TextScrollingExample::OnButtonClickedLarge );
    CreateLabel( mLargeLabel, "A Quick Brown Fox Jumps Over The Lazy Dog", boxB, false ,scrollLargeButton );


    CreateBox( "boxC", boxC, desktop, SCROLLING_BOX_SIZE );
    boxC.SetPosition( 0.0f, -300.0f, 1.0f );
    Toolkit::PushButton scrollSmallButton = Toolkit::PushButton::New();
    scrollSmallButton.ClickedSignal().Connect( this, &TextScrollingExample::OnButtonClickedSmall );
    CreateLabel( mSmallLabel, "Hello Text", boxC , true, scrollSmallButton );
    mSmallLabel.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLACK );
    mSmallLabel.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 1.0f, 1.0f ) );
    mSmallLabel.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::CYAN );

    CreateBox( "boxD", boxD, desktop, SCROLLING_BOX_SIZE );
    boxD.SetPosition( 0.0f, -200.0f, 1.0f );
    Toolkit::PushButton scrollRtlButton = Toolkit::PushButton::New();
    scrollRtlButton.ClickedSignal().Connect( this, &TextScrollingExample::OnButtonClickedRtl );
    CreateLabel( mRtlLabel, "مرحبا بالعالم", boxD , true, scrollRtlButton );
    mRtlLabel.SetProperty(TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE );
    mRtlLabel.SetProperty(TextLabel::Property::AUTO_SCROLL_LOOP_DELAY, 0.3f );

    CreateBox( "boxE", boxE, desktop, SCROLLING_BOX_SIZE );
    boxE.SetPosition( 0.0f, -100.0f, 1.0f );
    Toolkit::PushButton scrollRtlLongButton = Toolkit::PushButton::New();
    scrollRtlLongButton.ClickedSignal().Connect( this, &TextScrollingExample::OnButtonClickedRtlLong );
    CreateLabel( mRtlLongLabel, " مرحبا بالعالم مرحبا بالعالم مرحبا بالعالم مرحبا بالعالم مرحبا بالعالم مرحبا بالعالم مرحبا بالعالم مرحبا بالعالم مرحبا بالعالم", boxE , false, scrollRtlLongButton );
    mRtlLongLabel.SetProperty(TextLabel::Property::AUTO_SCROLL_SPEED, 500);
    mRtlLongLabel.SetProperty(TextLabel::Property::AUTO_SCROLL_GAP, 500);
    mRtlLongLabel.SetProperty(TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 3);
    mRtlLongLabel.SetProperty(TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::FINISH_LOOP );

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect(this, &TextScrollingExample::OnPanGesture );
    mPanGestureDetector.Attach( desktop );

    Toolkit::PushButton colorButton = Toolkit::PushButton::New();
    colorButton.SetProperty( Button::Property::TOGGLABLE, true );
    colorButton.SetProperty( DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, Property::Map().Add ( Toolkit::Visual::Property::TYPE, Visual::COLOR ).Add( ColorVisual::Property::MIX_COLOR, Color::RED ) );
    colorButton.SetProperty( DevelButton::Property::SELECTED_BACKGROUND_VISUAL, Property::Map().Add ( Toolkit::Visual::Property::TYPE, Visual::COLOR ).Add( ColorVisual::Property::MIX_COLOR, Color::BLACK ) );
    colorButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    colorButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    colorButton.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    colorButton.SetSize(BOX_SIZE.height,BOX_SIZE.height);
    colorButton.ClickedSignal().Connect( this, &TextScrollingExample::OnColorButtonClicked );
    rootActor.Add( colorButton );

    for( unsigned int i = 0; i < ALIGNMENT_TABLE_COUNT; ++i )
    {
      Toolkit::RadioButton alignButton = Toolkit::RadioButton::New( ALIGNMENT_TABLE[ i ] );
      alignButton.ClickedSignal().Connect( this, &TextScrollingExample::OnAlignButtonClicked );
      alignButton.SetName( ALIGNMENT_TABLE[ i ] );

      // Place first button to left aligned, second center aligned and third right aligned
      alignButton.SetAnchorPoint( Vector3( i * 0.5f, 0.0f, 0.5f ) );
      alignButton.SetParentOrigin( Vector3( i * 0.5f, 0.0f, 0.5f ) );

      rootActor.Add( alignButton );

      if( i == 0 )
      {
        // Set the first button as selected
        alignButton.SetProperty( Button::Property::SELECTED, true );
      }
    }
  }

  void EnableScrolling( Labels labels )
    {
      Actor label;
      switch( labels )
      {
        case LARGE:
        {
          label = mLargeLabel;
          break;
        }
        case RTL:
        {
          label = mRtlLabel;
          break;
        }
        case SMALL:
        {
          label = mSmallLabel;
          break;
        }
        case RTL_LONG:
        {
          label = mRtlLongLabel;
          break;
        }
        case NONE:
        {
          return;
        }
      }

      if ( labels != NONE )
      {
        Property::Value value = label.GetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL);
        if (value.Get< bool >())
        {
          label.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false  );
        }
        else
        {
          label.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true  );
        }
      }
    }

  bool OnButtonClickedSmall( Toolkit::Button button )
  {
    EnableScrolling( SMALL );
    return true;
  }

  bool OnButtonClickedLarge( Toolkit::Button button )
  {
    EnableScrolling( LARGE );
    return true;
  }

  bool OnButtonClickedRtl( Toolkit::Button button )
  {
    EnableScrolling( RTL );
    return true;
  }

  bool OnButtonClickedRtlLong( Toolkit::Button button )
  {
    EnableScrolling( RTL_LONG );
    return true;
  }

  bool OnColorButtonClicked( Toolkit::Button button )
  {
    Vector4 color = Color::RED;

    if ( mToggleColor )
    {
      color = Color::BLACK;
      mToggleColor = false;
    }
    else
    {
      mToggleColor = true;
    }

    mSmallLabel.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::BLACK );
    mSmallLabel.SetProperty( TextLabel::Property::TEXT_COLOR, color );
    mRtlLabel.SetProperty( TextLabel::Property::TEXT_COLOR, color );
    mLargeLabel.SetProperty( TextLabel::Property::TEXT_COLOR, color );
    mRtlLongLabel.SetProperty( TextLabel::Property::TEXT_COLOR, color );

    return true;
  }

  bool OnAlignButtonClicked( Toolkit::Button button )
  {
    for( unsigned int index = 0; index < ALIGNMENT_TABLE_COUNT; ++index )
    {
      const std::string& buttonName = button.GetName();
      if( buttonName == ALIGNMENT_TABLE[ index ] )
      {
        mSmallLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, ALIGNMENT_TABLE[ index ] );
        mRtlLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, ALIGNMENT_TABLE[ index ] );
        mLargeLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, ALIGNMENT_TABLE[ index ] );
        mRtlLongLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, ALIGNMENT_TABLE[ index ] );
        break;
      }
    }

    return true;
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
      else
      {
        if ( event.keyPressedName == "2" )
        {
          mAnimation.AnimateTo( Property( mSmallLabel, Actor::Property::SCALE ), Vector3(1.2f, 1.2f, 0.0f), AlphaFunction::BOUNCE, TimePeriod( 1.0f, 1.0f ) );
          mAnimation.AnimateTo( Property( mLargeLabel, Actor::Property::SCALE ), Vector3(1.2f, 1.2f, 0.0f), AlphaFunction::BOUNCE, TimePeriod( 1.0f, 1.0f ) );
          mAnimation.AnimateTo( Property( mRtlLabel, Actor::Property::SCALE ), Vector3(1.2f, 1.2f, 0.0f), AlphaFunction::BOUNCE, TimePeriod( 1.0f, 1.0f ) );
          mAnimation.AnimateTo( Property( mRtlLongLabel, Actor::Property::SCALE ), Vector3(1.2f, 1.2f, 0.0f), AlphaFunction::BOUNCE, TimePeriod( 1.0f, 1.0f ) );

          mAnimation.Play();
        }
      }
    }
  }

  void OnPanGesture( Actor actor, const PanGesture& gesture )
  {
    if( gesture.state == Gesture::Continuing )
    {
      Vector2 position = Vector2( gesture.displacement );
      mTargetActorPosition.y = mTargetActorPosition.y + position.y;
      mTargetActorPosition.y = std::min( mTargetActorPosition.y, -mTargetActorSize.height );
      mTargetActorPosition.y = std::max( mTargetActorPosition.y, ( mTargetActorSize.height - mStageSize.height*0.25f ) );
      actor.SetPosition( 0.0f, mTargetActorPosition.y );
    }
  }

private:

  Application& mApplication;
  PanGestureDetector mPanGestureDetector;

  Vector2 mTargetActorPosition;
  Vector2 mTargetActorSize;
  Vector2 mStageSize;

  TextLabel mLargeLabel;
  TextLabel mSmallLabel;
  TextLabel mRtlLabel;
  TextLabel mRtlLongLabel;

  Animation mAnimation;

  bool mToggleColor;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  TextScrollingExample test( application );
  application.MainLoop();
  return 0;
}
