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
#include "beat-control.h"
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
const char* DALI_LOGO_PATH( DEMO_IMAGE_DIR "Logo-for-demo.png" );
const char* DALI_ROBOT_MODEL_PATH( DEMO_MODEL_DIR "ToyRobot-Metal.obj" );
const char* DALI_ROBOT_MATERIAL_PATH( DEMO_MODEL_DIR "ToyRobot-Metal.mtl" );

TransitionApplication::TransitionApplication( Application& application )
: mApplication( application ),
  mTitle(),
  mBeatControl(),
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
  TableView contentLayout = TableView::New( 4, 1 );
  contentLayout.SetName("ContentLayout");
  contentLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  contentLayout.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  contentLayout.SetParentOrigin( ParentOrigin::TOP_LEFT );
  contentLayout.SetCellPadding( Vector2( 0.0f, 5.0f ) );

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

  mBeatControl = BeatControl::New();
  mBeatControl.SetName("BeatControl");
  mBeatControl.SetProperty( BeatControl::Property::BEAT_VISUAL, Property::Map()
                            .Add( Visual::Property::TRANSFORM, Property::Map()
                                  .Add( Visual::Transform::Property::SIZE, Vector2(0.5f, 0.5f) ) ) );

  mBeatControl.SetAnchorPoint( AnchorPoint::CENTER );
  mBeatControl.SetParentOrigin( ParentOrigin::CENTER );
  mBeatControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  contentLayout.Add( mBeatControl );
  // beat control should fill the tableview cell, so no change to default parameters

  TableView visualTypeLayout = TableView::New( 1, NUMBER_OF_VISUAL_BUTTONS );
  visualTypeLayout.SetName("VisualTypeLayout");
  visualTypeLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  visualTypeLayout.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );
  visualTypeLayout.SetFitHeight( 0 );

  contentLayout.Add( visualTypeLayout );
  contentLayout.SetFitHeight(2);

  for( int i=0; i<NUMBER_OF_VISUAL_BUTTONS; ++i )
  {
    Property::Map map;
    CreateVisualMap( i, map );
    map.Add( Visual::Property::TRANSFORM, Property::Map()
             .Add( Visual::Transform::Property::SIZE, Vector2(0.8f, 0.8f) )  );
    mVisualButtons[i] = BeatControl::New();
    mVisualButtons[i].SetProperty( BeatControl::Property::BEAT_VISUAL, map );
    mVisualButtons[i].SetName("VisualButton");
    mVisualButtons[i].SetStyleName("VisualButton");
    mVisualButtons[i].SetSize(0, 50);
    mVisualButtons[i].SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mVisualButtons[i].SetResizePolicy( ResizePolicy::FIXED, Dimension::HEIGHT );
    mVisualIndex = mVisualButtons[i].RegisterProperty( "visualId", i, Property::READ_WRITE );
    mVisualButtons[i].TouchSignal().Connect( this, &TransitionApplication::OnVisualButtonClicked );
    visualTypeLayout.AddChild( mVisualButtons[i], TableView::CellPosition( 0, i ) );
  }

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

  SetLabelText( mActionButtons[0], "Bounce" );
  SetLabelText( mActionButtons[1], "X" );
  SetLabelText( mActionButtons[2], "Y" );
  SetLabelText( mActionButtons[3], "Fade" );

  contentLayout.Add( actionButtonLayout );
  contentLayout.SetFitHeight(3);
}

void TransitionApplication::CreateVisualMap( int index, Property::Map& map )
{
  switch(index)
  {
    case 0:
    {
      map[ Toolkit::Visual::Property::TYPE ] = Visual::COLOR;
      map[ ColorVisual::Property::MIX_COLOR ] = Color::YELLOW;
      break;
    }
    case 1:
    {
      map[ Toolkit::Visual::Property::TYPE ] = Visual::BORDER;
      map[ BorderVisual::Property::COLOR ] = Color::GREEN;
      map[ BorderVisual::Property::SIZE ] = 5;
      break;
    }
    case 2:
    {
      map[ Toolkit::Visual::Property::TYPE ] = Visual::GRADIENT;

      Property::Array stopOffsets;
      stopOffsets.PushBack( 0.0f );
      stopOffsets.PushBack( 0.3f );
      stopOffsets.PushBack( 0.6f );
      stopOffsets.PushBack( 0.8f );
      stopOffsets.PushBack( 1.0f );
      map[ GradientVisual::Property::STOP_OFFSET ] = stopOffsets;

      Property::Array stopColors;
      stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 255.f )/255.f );
      stopColors.PushBack( Vector4( 196.f, 198.f, 71.f, 122.f )/255.f );
      stopColors.PushBack( Vector4( 214.f, 37.f, 139.f, 191.f )/255.f );
      stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 150.f )/255.f );
      stopColors.PushBack( Color::YELLOW );
      map[ GradientVisual::Property::STOP_COLOR ] = stopColors;
      map[ GradientVisual::Property::START_POSITION ] = Vector2(-0.5f, -0.5f );
      map[ GradientVisual::Property::END_POSITION ] = Vector2( 0.5f,  0.5f );
      break;
    }
    case 3:
    {
      map[ Toolkit::Visual::Property::TYPE ] = Visual::IMAGE;
      map[ ImageVisual::Property::URL ] = DALI_LOGO_PATH;
      break;
    }
    case 4:
    {
      map[ Toolkit::Visual::Property::TYPE ] = Visual::IMAGE;
      map[ ImageVisual::Property::URL ] = DEMO_IMAGE_DIR "preMultAlpha.png";
      map[ Visual::Property::PREMULTIPLIED_ALPHA ] = true;
      break;
    }

    case 5:
    {
      map[ Toolkit::Visual::Property::TYPE ] = Visual::MESH;
      map[ MeshVisual::Property::OBJECT_URL ] = DALI_ROBOT_MODEL_PATH;
      map[ MeshVisual::Property::MATERIAL_URL ] = DALI_ROBOT_MATERIAL_PATH;
      map[ MeshVisual::Property::TEXTURES_PATH ] = DEMO_IMAGE_DIR;
      map[ MeshVisual::Property::SHADING_MODE ] = MeshVisual::ShadingMode::TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING;
      break;
    }

    case 6:
    {
      map[ Toolkit::Visual::Property::TYPE ] = Visual::PRIMITIVE;

      map[ PrimitiveVisual::Property::SHAPE ] = PrimitiveVisual::Shape::BEVELLED_CUBE;
      map[ PrimitiveVisual::Property::BEVEL_PERCENTAGE ] = 0.3f;
      map[ PrimitiveVisual::Property::BEVEL_SMOOTHNESS ] = 0.0f;
      map[ PrimitiveVisual::Property::SCALE_DIMENSIONS ] = Vector3(1.0f,1.0f,0.3f);
      map[ PrimitiveVisual::Property::MIX_COLOR ]        = Vector4(0.7f, 0.5f, 0.05f, 1.0f);

      break;
    }

    case 7:
    {
      // NPatch
      map[ Toolkit::Visual::Property::TYPE ] = Visual::IMAGE;
      map[ ImageVisual::Property::URL ] = DEMO_IMAGE_DIR "button-up-16.9.png";
      break;
    }
    case 8:
    {
      // SVG
      map[ Toolkit::Visual::Property::TYPE ] = Visual::IMAGE;
      map[ ImageVisual::Property::URL ] = DEMO_IMAGE_DIR "Kid1.svg";
      break;
    }

    case 9:
    {
      map[ Toolkit::Visual::Property::TYPE ] = Visual::TEXT;
      map[ TextVisual::Property::TEXT ] = "Text";
      map[ TextVisual::Property::TEXT_COLOR ] = Color::CYAN;
      map[ TextVisual::Property::POINT_SIZE ] = 10;
      break;
    }

    default:
    {
      map[ Toolkit::Visual::Property::TYPE ] = Visual::COLOR;
      map[ ColorVisual::Property::MIX_COLOR ] = Color::MAGENTA;
      break;
    }
  }
}

bool TransitionApplication::OnVisualButtonClicked( Actor actor, const TouchData& touchData )
{
  if( touchData.GetState(0) == PointState::FINISHED )
  {
    int visual = actor.GetProperty<int>( mVisualIndex );
    Property::Map map;
    CreateVisualMap( visual, map );
    map.Add( Visual::Property::TRANSFORM, Property::Map()
             .Add( Visual::Transform::Property::SIZE, Vector2( 0.5f, 0.5f ) ) );
    mBeatControl.SetProperty( BeatControl::Property::BEAT_VISUAL, map );
  }
  return true;
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
    case 3:
    {
      mBeatControl.StartFadeAnimation();
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
