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

// EXTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/actors/actor-devel.h>

// INTERNAL INCLUDES
#include "clipping-item-factory.h"
#include "item-view-orientation-constraint.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * const APPLICATION_TITLE( "Clipping Controls" );
const Vector3 APPLICATION_TITLE_PARENT_ORIGIN( 0.5f, 0.03f, 0.5f ); // Set the parent origin to a small percentage below the top (so the demo will scale for different resolutions).

const Vector3 ITEM_VIEW_LAYOUT_SIZE_SCALE( 0.75f, 0.5f, 0.75f );
const float ITEM_VIEW_BORDER_SIZE = 2.0f;
const float ITEM_VIEW_MAXIMUM_ROTATION_IN_DEGREES = 20.0f;
const float ITEM_VIEW_LAYOUT_POSITION_CHANGE_MULTIPLIER = 3.0f;
const float ITEM_VIEW_ROTATION_ANIMATION_TIME = 0.2f;

const char * const BUTTON_LABEL( "Toggle Clipping Mode" );
} // unnamed namespace

/**
 * @brief Demonstrates the control clipping of a UI Control.
 *
 * When an Actor is set to clip its children, the renderers have to be added manually in order to specify what its children
 * need to clip to. UI Controls automate the creation of the renderers/visuals when they are set to clip their children.
 *
 * This example displays an item-view whose clipping mode is toggled without the need for adding any renderers to it.
 *
 * Additionally, a constraint is used to modify the item-view's orientation.
 */
class ClippingExample : public ConnectionTracker
{
public:

  /**
   * @brief Constructor.
   * @param[in] application A reference to the Application class.
   */
  ClippingExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ClippingExample::Create );
  }

private:

  /**
   * @brief Called to initialise the application content.
   * @param[in] application A reference to the Application class.
   */
  void Create( Application& application )
  {
    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    // Connect to the stage's key signal to allow Back and Escape to exit.
    Stage stage = Dali::Stage::GetCurrent();
    stage.KeyEventSignal().Connect( this, &ClippingExample::OnKeyEvent );

    // Create a TextLabel for the application title.
    Toolkit::TextLabel label = Toolkit::TextLabel::New( APPLICATION_TITLE );
    label.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    label.SetParentOrigin( APPLICATION_TITLE_PARENT_ORIGIN );
    label.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    label.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    label.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
    stage.Add( label );

    // Create an item-view which clips its children.
    mItemView = ItemView::New( mClippingItemFactory );
    mItemView.SetParentOrigin( ParentOrigin::CENTER );
    mItemView.SetAnchorPoint( AnchorPoint::CENTER );
    mItemView.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN ); // Enable clipping. No need to create any renderers.
    stage.Add( mItemView );

    // Create a Spiral Layout and add it to the Item View.
    mItemView.AddLayout( * DefaultItemLayout::New( DefaultItemLayout::SPIRAL ) );
    stage.GetRootLayer().SetBehavior( Layer::LAYER_3D ); // The item-view spiral layout requires Layer 3D behaviour.

    // Calculate the size we would like our item-view layout to be, and then activate the layout.
    const Vector2 stageSize = stage.GetSize();
    const Vector3 itemViewLayoutSize( ITEM_VIEW_LAYOUT_SIZE_SCALE.x * stageSize.x, ITEM_VIEW_LAYOUT_SIZE_SCALE.y * stageSize.y, ITEM_VIEW_LAYOUT_SIZE_SCALE.z * stageSize.x );
    mItemView.ActivateLayout( 0, itemViewLayoutSize, 0.0f );

    // Connect to the scroll started and completed signals to apply orientation constraints & animations.
    mItemView.ScrollStartedSignal().Connect( this, &ClippingExample::ScrollStarted );
    mItemView.ScrollCompletedSignal().Connect( this, &ClippingExample::ScrollCompleted );

    // Create a constraint for the item-view which we apply when we start scrolling and remove when we stop.
    mItemViewOrientationConstraint = Constraint::New< Quaternion >( mItemView, Actor::Property::ORIENTATION, ItemViewOrientationConstraint( ITEM_VIEW_MAXIMUM_ROTATION_IN_DEGREES, ITEM_VIEW_LAYOUT_POSITION_CHANGE_MULTIPLIER ) );
    mItemViewOrientationConstraint.AddSource( LocalSource( ItemView::Property::LAYOUT_POSITION ) );

    // Create a border around item-view (as item-view is clipping its children, we should NOT add this as a child of item-view).
    Control border = Control::New();
    border.SetParentOrigin( ParentOrigin::CENTER );
    border.SetAnchorPoint( AnchorPoint::CENTER );
    border.SetProperty( Control::Property::BACKGROUND,
                        Property::Map().Add( Toolkit::Visual::Property::TYPE, Visual::BORDER )
                                       .Add( BorderVisual::Property::COLOR, Color::WHITE )
                                       .Add( BorderVisual::Property::SIZE, 2.0f )
                                       .Add( BorderVisual::Property::ANTI_ALIASING, true ) );
    border.SetSize( Vector3( itemViewLayoutSize.x + ITEM_VIEW_BORDER_SIZE * 2.0f, itemViewLayoutSize.y + ITEM_VIEW_BORDER_SIZE * 2.0f, itemViewLayoutSize.z + ITEM_VIEW_BORDER_SIZE * 2.0f ) );
    stage.Add( border );

    // Constrain the border's orientation to the orientation of item-view.
    Constraint constraint = Constraint::New< Quaternion >( border, Actor::Property::ORIENTATION, EqualToConstraint() );
    constraint.AddSource( Source( mItemView, Actor::Property::ORIENTATION ) );
    constraint.Apply();

    // Create a button to toggle the clipping mode
    PushButton button = Toolkit::PushButton::New();
    button.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    button.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    button.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    button.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    button.SetProperty( Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D );
    button.SetProperty( Button::Property::LABEL, BUTTON_LABEL );
    button.ClickedSignal().Connect( this, &ClippingExample::OnButtonClicked );
    stage.Add( button );
  }

  /**
   * @brief Called when the item-view starts to scroll.
   *
   * Here we want to apply the item-view constraint.
   */
  void ScrollStarted( const Vector2& /* currentScrollPosition */ )
  {
    mItemViewOrientationConstraint.Apply();
  }

  /**
   * @brief Called when the item-view scrolling completes.
   *
   * Here we remove the item-view orientation constraint and perform an animation to return the item-view back to base-rotation.
   */
  void ScrollCompleted( const Vector2& /* currentScrollPosition */ )
  {
    Animation animation = Animation::New( ITEM_VIEW_ROTATION_ANIMATION_TIME );
    animation.AnimateTo( Property( mItemView, Actor::Property::ORIENTATION ), Quaternion( Degree( 0.0f ), Vector3::XAXIS ), AlphaFunction::EASE_IN_SINE );
    animation.Play();

    mItemViewOrientationConstraint.Remove();
  }

  /**
   * @brief Called when any key event is received
   *
   * Will use this to quit the application if Back or the Escape key is received
   * @param[in] event The key event information
   */
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

  /**
   * @brief Called when the button is clicked.
   *
   * Will use this to toggle between the clipping modes.
   * @param[in] button The button that has been clicked.
   */
  bool OnButtonClicked( Toolkit::Button button )
  {
    if( mItemView )
    {
      ClippingMode::Type currentMode = static_cast< ClippingMode::Type >( mItemView.GetProperty( Actor::Property::CLIPPING_MODE ).Get< int >() );
      mItemView.SetProperty( Actor::Property::CLIPPING_MODE, ( currentMode == ClippingMode::CLIP_CHILDREN ) ? ClippingMode::DISABLED : ClippingMode::CLIP_CHILDREN );
    }
    return true;
  }

  // Data

  Application& mApplication; ///< Reference to the application class.
  ItemView mItemView; ///< The item view which whose children we would like to clip.
  ClippingItemFactory mClippingItemFactory; ///< The ItemFactory used to create our items.
  Constraint mItemViewOrientationConstraint; ///< The constraint used to control the orientation of item-view.
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application app = Application::New(&argc, &argv, DEMO_THEME_PATH);
  ClippingExample test(app);
  app.MainLoop();
  return 0;
}
