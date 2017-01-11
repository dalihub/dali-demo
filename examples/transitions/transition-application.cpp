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
 */

/**
 * @file transition-application.cpp
 * @brief Application class for showing stylable transitions
 */

// Class include
#include "transition-application.h"

// External includes
#include <dali-toolkit/dali-toolkit.h>
#include "beat-control.h"
#include <cstdio>
#include <sstream>

// Internal includes

using namespace Dali;
using namespace Dali::Toolkit;

namespace Demo
{

const char* TransitionApplication::DEMO_THEME_ONE_PATH( DEMO_STYLE_DIR "style-example-theme-one.json" );


TransitionApplication::TransitionApplication( Application& application )
: mApplication( application ),
  mTitle(),
  mBeatControl(),
  mActionButtons(),
  mActionIndex( Property::INVALID_INDEX )
{
  application.InitSignal().Connect( this, &TransitionApplication::Create );
}

TransitionApplication::~TransitionApplication()
{
}

void TransitionApplication::Create( Application& application )
{
  Stage stage = Stage::GetCurrent();
  stage.KeyEventSignal().Connect(this, &TransitionApplication::OnKeyEvent);
  stage.SetBackgroundColor( Vector4( 0.1f, 0.1f, 0.1f, 1.0f ) );

  // Hide the indicator bar
  application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

  // Content panes:
  TableView contentLayout = TableView::New( 3, 1 );
  contentLayout.SetName("ContentLayout");
  contentLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  contentLayout.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  contentLayout.SetParentOrigin( ParentOrigin::TOP_LEFT );
  contentLayout.SetCellPadding( Size( 10, 10 ) );

  // Assign all rows the size negotiation property of fitting to children

  stage.Add( contentLayout );

  mTitle = TextLabel::New( "Custom Control Transition Example" );
  mTitle.SetName( "Title" );
  mTitle.SetStyleName("Title");
  mTitle.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  mTitle.SetParentOrigin( ParentOrigin::TOP_CENTER );
  mTitle.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  mTitle.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  mTitle.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  contentLayout.Add( mTitle );
  contentLayout.SetFitHeight(0);

  mBeatControl = BeatControl::New();
  mBeatControl.SetName("BeatControl");
  mBeatControl.SetAnchorPoint( AnchorPoint::CENTER );
  mBeatControl.SetParentOrigin( ParentOrigin::CENTER );
  mBeatControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  contentLayout.Add( mBeatControl );
  // beat control should fill the tableview cell, so no change to default parameters

  TableView actionButtonLayout = TableView::New( 1, 4 );
  actionButtonLayout.SetName("ThemeButtonsLayout");
  actionButtonLayout.SetCellPadding( Vector2( 6.0f, 0.0f ) );

  actionButtonLayout.SetAnchorPoint( AnchorPoint::CENTER );
  actionButtonLayout.SetParentOrigin( ParentOrigin::CENTER );
  actionButtonLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  actionButtonLayout.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );
  actionButtonLayout.SetCellPadding( Size( 10, 10 ) );
  actionButtonLayout.SetFitHeight( 0 );

  TextLabel label = TextLabel::New( "Action: ");
  label.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  label.SetStyleName("ActionLabel");
  label.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  label.SetParentOrigin( ParentOrigin::TOP_CENTER );
  actionButtonLayout.AddChild( label, TableView::CellPosition( 0, 0 ) );
  actionButtonLayout.SetCellAlignment( TableView::CellPosition( 0, 0 ), HorizontalAlignment::LEFT, VerticalAlignment::CENTER );

  for( int i=0; i<3; ++i )
  {
    mActionButtons[i] = PushButton::New();
    mActionButtons[i].SetName("ActionButton");
    mActionButtons[i].SetStyleName("ActionButton");
    mActionButtons[i].SetParentOrigin( ParentOrigin::CENTER );
    mActionButtons[i].SetAnchorPoint( ParentOrigin::CENTER );
    mActionButtons[i].SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mActionButtons[i].SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    mActionIndex = mActionButtons[i].RegisterProperty( "actionId", i, Property::READ_WRITE );
    mActionButtons[i].ClickedSignal().Connect( this, &TransitionApplication::OnActionButtonClicked );
    actionButtonLayout.AddChild( mActionButtons[i], TableView::CellPosition( 0, 1+i ) );
  }
  mActionButtons[0].SetProperty( Toolkit::Button::Property::LABEL, "Bounce" );
  mActionButtons[1].SetProperty( Toolkit::Button::Property::LABEL, "X" );
  mActionButtons[2].SetProperty( Toolkit::Button::Property::LABEL, "Y" );

  contentLayout.Add( actionButtonLayout );
  contentLayout.SetFitHeight(2);
}


bool TransitionApplication::OnActionButtonClicked( Button button )
{
  int action = button.GetProperty<int>( mActionIndex );
  switch( action )
  {
    case 0:
    {
      mBeatControl.StartBounceAnimation();
      break;
    }
    case 1:
    {
      mBeatControl.StartXAnimation();
      break;
    }
    case 2:
    {
      mBeatControl.StartYAnimation();
      break;
    }
  }

  return true;
}

void TransitionApplication::OnKeyEvent( const KeyEvent& keyEvent )
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

} // namespace Demo
