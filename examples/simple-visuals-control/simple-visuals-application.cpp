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
 */

// CLASS HEADER
#include "my-control.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <cstdio>
#include <sstream>

// INTERNAL INCLUDES
#include "simple-visuals-application.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

}

namespace Demo
{

const char* ICON_IMAGE( DEMO_IMAGE_DIR  "application-icon-13.png" );

SimpleVisualsApplication::SimpleVisualsApplication( Application& application )
: mApplication( application ),
  mMyControl()
{
  application.InitSignal().Connect( this, &SimpleVisualsApplication::Create );
}

Dali::Actor SimpleVisualsApplication::OnKeyboardPreFocusChange( Dali::Actor current, Dali::Actor proposed, Dali::Toolkit::Control::KeyboardFocus::Direction direction )
{
  Actor nextFocusActor = proposed;

  if( !current && !proposed  )
  {
    // Set the initial focus to the first tile in the current page should be focused.
    nextFocusActor = mMyControl;
  }
  else
  {
    if ( current == mMyControl )
    {
      nextFocusActor = mMyControl2;
    }
    else
    {
      nextFocusActor = mMyControl;
    }
  }

  return nextFocusActor;
}


void SimpleVisualsApplication::OnKeyEvent( const KeyEvent& keyEvent )
{
  static int keyPressed = 0;

  if( keyEvent.state == KeyEvent::Down)
  {
    if( keyPressed == 0 ) // Is this the first down event?
    {
      printf("Key pressed: %s %d\n", keyEvent.keyPressedName.c_str(), keyEvent.keyCode );

      if( IsKey( keyEvent, DALI_KEY_ESCAPE) || IsKey( keyEvent, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
      else if( keyEvent.keyPressedName.compare("Return") == 0 )
      {
      }
    }
    keyPressed = 1;
  }
  else if( keyEvent.state == KeyEvent::Up )
  {
    keyPressed = 0;
  }
}

void SimpleVisualsApplication::Create( Application& application )
{
  Stage stage = Stage::GetCurrent();
  stage.SetBackgroundColor( Vector4( 0.1f, 0.1f, 0.1f, 1.0f ) );

  // Connect to key events so can quit application
  stage.KeyEventSignal().Connect(this, &SimpleVisualsApplication::OnKeyEvent);

  // Hide the indicator bar
  application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

  // Create a table view to parent the 2 MyControls
  TableView contentLayout = TableView::New( 2, 2 );
  contentLayout.SetName("ContentLayout");
  contentLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  contentLayout.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::HEIGHT );
  contentLayout.SetSizeModeFactor( Vector3( 1.0f, .5f, 1.0f ) );
  contentLayout.SetAnchorPoint( AnchorPoint::CENTER );
  contentLayout.SetParentOrigin( ParentOrigin::CENTER );
  contentLayout.SetCellPadding( Vector2( 50.0f, 15.0f ) );
  contentLayout.SetBackgroundColor( Vector4(0.949, 0.949, 0.949, 1.0) );

  // Listen to focus change so can see Visual change from NORMAL to FOCUSED state
  KeyboardFocusManager::Get().PreFocusChangeSignal().Connect( this, &SimpleVisualsApplication::OnKeyboardPreFocusChange );

  stage.Add( contentLayout );

  // Create 2 MyControls and add to table view.
  mMyControl = MyControl::New();
  mMyControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  mMyControl.SetParentOrigin(ParentOrigin::TOP_LEFT);

  mMyControl2 = MyControl::New();
  mMyControl2.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  mMyControl2.SetParentOrigin(ParentOrigin::CENTER);

  contentLayout.AddChild( mMyControl2, TableView::CellPosition(0, 0) );
  contentLayout.AddChild( mMyControl, TableView::CellPosition(0, 1) );
}

} // namespace Demo
