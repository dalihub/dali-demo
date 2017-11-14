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

/**
 * @file transition-application.cpp
 * @brief Application class for showing stylable transitions
 */

// Class include
#include "transition-application.h"

// External includes
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include "shadow-button.h"
#include <cstdio>
#include <sstream>

// Internal includes

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

void SetLabelText( Button button, const char* label )
{
  button.SetProperty( Toolkit::Button::Property::LABEL, label );
}

}

namespace Demo
{

const char* TransitionApplication::DEMO_THEME_ONE_PATH( DEMO_STYLE_DIR "style-example-theme-one.json" );
const char* TransitionApplication::DEMO_THEME_TWO_PATH( DEMO_STYLE_DIR "style-example-theme-two.json" );


TransitionApplication::TransitionApplication( Application& application )
: mApplication( application ),
  mTitle(),
  mShadowButton(),
  mActionButtons(),
  mVisualIndex( Property::INVALID_INDEX ),
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
  contentLayout.SetCellPadding( Vector2( 0.0f, 5.0f ) );
  contentLayout.SetBackgroundColor( Vector4(0.949, 0.949, 0.949, 1.0) );
  // Assign all rows the size negotiation property of fitting to children

  stage.Add( contentLayout );

  mTitle = TextLabel::New( "Custom Control Transition Example" );
  mTitle.SetName( "Title" );
  mTitle.SetStyleName("Title");
  mTitle.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  mTitle.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  mTitle.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  contentLayout.Add( mTitle );
  contentLayout.SetFitHeight(0); // Fill width

  // Provide some padding around the center cell
  TableView buttonLayout = TableView::New( 3, 3 );
  buttonLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  buttonLayout.SetFixedHeight(1, 100 );
  buttonLayout.SetFixedWidth(1, 350 );
  contentLayout.Add( buttonLayout );

  mShadowButton = ShadowButton::New();
  mShadowButton.SetName("ShadowButton");
  mShadowButton.SetActiveState( false );
  mShadowButton.SetAnchorPoint( AnchorPoint::CENTER );
  mShadowButton.SetParentOrigin( ParentOrigin::CENTER );
  mShadowButton.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  mShadowButton.SetProperty( DevelControl::Property::STATE, DevelControl::DISABLED );
  mShadowButton.SetProperty( DevelControl::Property::SUB_STATE, "UNCHECKED" );

  buttonLayout.AddChild( mShadowButton, TableView::CellPosition(1, 1) );

  TableView actionButtonLayout = TableView::New( 1, NUMBER_OF_ACTION_BUTTONS+1 );
  actionButtonLayout.SetName("ThemeButtonsLayout");
  actionButtonLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  actionButtonLayout.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );
  actionButtonLayout.SetFitHeight( 0 );

  TextLabel label = TextLabel::New( "Action: ");
  label.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  label.SetStyleName("ActionLabel");
  actionButtonLayout.AddChild( label, TableView::CellPosition( 0, 0 ) );
  actionButtonLayout.SetCellAlignment( TableView::CellPosition( 0, 0 ), HorizontalAlignment::LEFT, VerticalAlignment::CENTER );

  for( int i=0; i<NUMBER_OF_ACTION_BUTTONS; ++i )
  {
    mActionButtons[i] = PushButton::New();
    mActionButtons[i].SetName("ActionButton");
    mActionButtons[i].SetStyleName("ActionButton");
    mActionButtons[i].SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mActionButtons[i].SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    mActionIndex = mActionButtons[i].RegisterProperty( "actionId", i, Property::READ_WRITE );
    mActionButtons[i].ClickedSignal().Connect( this, &TransitionApplication::OnActionButtonClicked );
    actionButtonLayout.AddChild( mActionButtons[i], TableView::CellPosition( 0, 1+i ) );
  }
  SetLabelText( mActionButtons[0], "Enable" );
  SetLabelText( mActionButtons[1], "Check" );
  mActionButtons[1].SetProperty( Button::Property::DISABLED, true );

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
      bool activeState = mShadowButton.GetActiveState();
      mShadowButton.SetActiveState( ! activeState );
      if( activeState )
      {
        SetLabelText( button, "Enable" );
        mShadowButton.SetProperty( DevelControl::Property::STATE, DevelControl::DISABLED );
      }
      else
      {
        SetLabelText( button, "Disable" );
        mShadowButton.SetProperty( DevelControl::Property::STATE, DevelControl::NORMAL );
      }
      mActionButtons[1].SetProperty( Button::Property::DISABLED, activeState );
      break;
    }
    case 1:
    {
      bool checkState = mShadowButton.GetCheckState();
      mShadowButton.SetCheckState( ! checkState );
      if( checkState )
      {
        SetLabelText( button, "Check" );
        mShadowButton.SetProperty( DevelControl::Property::SUB_STATE, "UNCHECKED" );
      }
      else
      {
        SetLabelText( button, "Uncheck" );
        mShadowButton.SetProperty( DevelControl::Property::SUB_STATE, "CHECKED" );
      }
      break;
    }
    case 2:
    {
      break;
    }
    case 3:
    {
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
