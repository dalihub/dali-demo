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

#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/actors/actor-devel.h>

#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const int TABLE_VIEW_ROWS = 5;
const int TABLE_VIEW_COLUMNS = 3;
const Vector3 TABLE_VIEW_SIZE_MODE_FACTOR( 0.6f, 0.6f, 1.0f );

const Quaternion ANIMATION_ROTATION( Degree( 360.0f), Vector3::ZAXIS );
const AlphaFunction::BuiltinFunction ROTATION_ANIMATION_ALPHA_FUNCTION( AlphaFunction::EASE_IN_OUT );
const TimePeriod ROTATION_ANIMATION_TIME_PERIOD( 0.0f, 0.5f );

const Vector3 ANIMATION_SCALE( 2.0f, 2.0f, 1.0f );
const AlphaFunction::BuiltinFunction SCALE_ANIMATION_ALPHA_FUNCTION( AlphaFunction::SIN );
const TimePeriod SCALE_ANIMATION_TIME_PERIOD( 0.15f, 0.85f );
} // unnamed namespace

/**
 * @brief Demonstrates how to animate a control in a layout-container using the anchor point as the pivot.
 *
 * Positions of the controls in the layout-container will be set using the top-left of the control.
 */
class PivotController : public ConnectionTracker
{
public:

  /**
   * @brief Constructor.
   * @param[in] application Reference to the application instance.
   */
  PivotController( Application& application )
  : mApplication( application )
  {
    mApplication.InitSignal().Connect( this, &PivotController::Create );
  }

private:

  /**
   * @brief The Init signal is received once (only) during the Application lifetime.
   */
  void Create( Application& /* application */ )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );
    stage.KeyEventSignal().Connect( this, &PivotController::OnKeyEvent );

    // Create a table view.
    TableView tableView = TableView::New( TABLE_VIEW_ROWS, TABLE_VIEW_COLUMNS );
    tableView.SetAnchorPoint( AnchorPoint::CENTER );
    tableView.SetParentOrigin( ParentOrigin::CENTER );
    tableView.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    tableView.SetSizeModeFactor( TABLE_VIEW_SIZE_MODE_FACTOR );
    stage.Add( tableView );

    // Create a tap detector - we are going to rotate an actor round our anchor-point (pivot) when one of our controls is tapped.
    mTapDetector = TapGestureDetector::New();
    mTapDetector.DetectedSignal().Connect( this, &PivotController::OnTap );

    Vector3 anchorPoint( AnchorPoint::CENTER ); // This will be changed depending on the row and column.

    // Add controls to the table-view - each control will have a random color.
    for( int row = 0; row < TABLE_VIEW_ROWS; ++row )
    {
      anchorPoint.y = ( row % TABLE_VIEW_ROWS ) * ( 1.0f / ( TABLE_VIEW_ROWS - 1 ) ); // Calculate this row's y anchor-point.

      for( int column = 0; column < TABLE_VIEW_COLUMNS; ++column )
      {
        anchorPoint.x = ( column % TABLE_VIEW_COLUMNS ) * ( 1.0f / ( TABLE_VIEW_COLUMNS - 1 ) ); // Calculate this column's x anchor-point.

        // Create a control with a randomly chosen background color.
        Control control = Control::New();
        control.SetBackgroundColor( Vector4( Random::Range( 0.0f, 1.0f ), Random::Range( 0.0f, 1.0f ), Random::Range( 0.0f, 1.0f ), 1.0f ) );
        control.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        control.SetProperty( DevelActor::Property::POSITION_USES_ANCHOR_POINT, false ); // Ensures the position always uses top-left for its calculations.
        control.SetAnchorPoint( anchorPoint ); // This anchor-point is used for the rotation and the scale.

        // Add to the table-view
        tableView.AddChild( control, TableView::CellPosition( row, column ) );

        // Attach each control to the tap-detector so we can start the animation around our anchor point.
        mTapDetector.Attach( control );
      }
    }
  }

  /**
   * @brief Called when a control is tapped.
   *
   * Here we will start an animation around our pivot.
   */
  void OnTap( Actor actor, const TapGesture& /* tap */ )
  {
    // Raise the actor to the top.
    actor.RaiseToTop();

    // Create the animation to rotate and scale our actor.
    Animation animation = Animation::New( 1.0f );
    animation.AnimateBy( Property( actor, Actor::Property::ORIENTATION ), ANIMATION_ROTATION, ROTATION_ANIMATION_ALPHA_FUNCTION, ROTATION_ANIMATION_TIME_PERIOD );
    animation.AnimateTo( Property( actor, Actor::Property::SCALE ), ANIMATION_SCALE, SCALE_ANIMATION_ALPHA_FUNCTION, SCALE_ANIMATION_TIME_PERIOD );

    // Play the animation.
    animation.Play();
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
      if ( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  Application&  mApplication; ///< Reference to the Application instance.
  TapGestureDetector mTapDetector; ///< Used for animating the tapped control.
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  PivotController test( application );
  application.MainLoop();
  return 0;
}
