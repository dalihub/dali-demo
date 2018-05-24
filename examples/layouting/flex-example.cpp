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

#include <string>
#include "flex-example.h"
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/fbox-layout.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

// Button file names
const char* LTR_IMAGE( DEMO_IMAGE_DIR "icon-play.png" );
const char* LTR_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-play-selected.png" );

const char* RTL_IMAGE( DEMO_IMAGE_DIR "icon-reverse.png" );
const char* RTL_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-reverse-selected.png" );

const char* ROTATE_CLOCKWISE_IMAGE( DEMO_IMAGE_DIR "icon-reset.png" );
const char* ROTATE_CLOCKWISE_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-reset-selected.png" );

const char* WRAP_IMAGE( DEMO_IMAGE_DIR "icon-replace.png" );
const char* WRAP_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-replace-selected.png" );

const char* JUSTIFY_IMAGE( DEMO_IMAGE_DIR "icon-item-view-layout-grid.png" );
const char* JUSTIFY_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-item-view-layout-grid-selected.png" );

// Child image filenames
const char* IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "application-icon-101.png",
  DEMO_IMAGE_DIR "application-icon-102.png",
  DEMO_IMAGE_DIR "application-icon-103.png",
  DEMO_IMAGE_DIR "application-icon-104.png",
  DEMO_IMAGE_DIR "application-icon-105.png",
  DEMO_IMAGE_DIR "application-icon-106.png",
  DEMO_IMAGE_DIR "application-icon-107.png",
  DEMO_IMAGE_DIR "application-icon-108.png",
  DEMO_IMAGE_DIR "application-icon-109.png",
  DEMO_IMAGE_DIR "application-icon-110.png"

};

const unsigned int NUMBER_OF_RESOURCES = sizeof(IMAGE_PATH) / sizeof(char*);

// Helper function to create ImageViews with given filename and size.
void CreateChildImageView( ImageView& imageView, const char* filename, Size size )
{
  imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imagePropertyMap[ Toolkit::ImageVisual::Property::URL ] = filename;
  imagePropertyMap[ ImageVisual::Property::DESIRED_WIDTH ] = size.width;
  imagePropertyMap[ ImageVisual::Property::DESIRED_HEIGHT ] = size.height;
  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE , imagePropertyMap );
  imageView.SetName("ImageView");
  imageView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  imageView.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
}

}  // namespace

namespace Demo
{

void FlexExample::Create()
{
  // The Init signal is received once (only) during the Application lifetime
  auto stage = Stage::GetCurrent();

  mDirectionButton = PushButton::New();
  mDirectionButton.SetProperty( PushButton::Property::UNSELECTED_ICON, RTL_IMAGE );
  mDirectionButton.SetProperty( PushButton::Property::SELECTED_ICON, RTL_SELECTED_IMAGE );
  mDirectionButton.ClickedSignal().Connect( this, &FlexExample::OnDirectionClicked );
  mDirectionButton.SetParentOrigin( Vector3(0.2f, 1.0f, 0.5f ) );
  mDirectionButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mDirectionButton.SetSize(75, 75);
  stage.Add( mDirectionButton );

  mWrapButton = Toolkit::PushButton::New();
  mWrapButton.SetProperty( PushButton::Property::UNSELECTED_ICON, WRAP_IMAGE );
  mWrapButton.SetProperty( PushButton::Property::SELECTED_ICON, WRAP_SELECTED_IMAGE );
  mWrapButton.ClickedSignal().Connect( this, &FlexExample::OnWrapClicked );
  mWrapButton.SetParentOrigin( Vector3(0.4f, 1.0f, 0.5f ));
  mWrapButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mWrapButton.SetSize(75, 75);
  stage.Add( mWrapButton );

  mJustifyButton = Toolkit::PushButton::New();
  mJustifyButton.SetProperty( PushButton::Property::UNSELECTED_ICON, JUSTIFY_IMAGE );
  mJustifyButton.SetProperty( PushButton::Property::SELECTED_ICON, JUSTIFY_SELECTED_IMAGE );
  mJustifyButton.ClickedSignal().Connect( this, &FlexExample::OnJustifyClicked );
  mJustifyButton.SetParentOrigin( Vector3(0.6f, 1.0f, 0.5f ));
  mJustifyButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mJustifyButton.SetSize(75, 75);
  stage.Add( mJustifyButton );

  mRotateButton = PushButton::New();
  mRotateButton.SetProperty( PushButton::Property::UNSELECTED_ICON, ROTATE_CLOCKWISE_IMAGE );
  mRotateButton.SetProperty( PushButton::Property::SELECTED_ICON, ROTATE_CLOCKWISE_SELECTED_IMAGE );
  mRotateButton.ClickedSignal().Connect( this, &FlexExample::OnRotateClicked );
  mRotateButton.SetParentOrigin( Vector3(0.8f, 1.0f, 0.5f ));
  mRotateButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mRotateButton.SetSize(75, 75);
  stage.Add( mRotateButton );

  // Create a linear layout
  mFlexContainer = Control::New();
  mDirection = true;

  auto layout = FboxLayout::New();
  layout.SetAnimateLayout( true );
  layout.SetFlexDirection( Toolkit::FboxLayout::FlexDirection::ROW );
  layout.SetFlexWrap( Toolkit::FboxLayout::WrapType::NO_WRAP );
  layout.SetFlexJustification( Toolkit::FboxLayout::JUSTIFY_FLEX_START );
  layout.SetFlexAlignment( Toolkit::FboxLayout::ALIGN_FLEX_START );

  DevelControl::SetLayout( mFlexContainer, layout );

  mFlexContainer.SetParentOrigin( ParentOrigin::CENTER );
  mFlexContainer.SetAnchorPoint( AnchorPoint::CENTER );
  mFlexContainer.SetName( "FlexExample" );
  mFlexContainer.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  mFlexContainer.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  mFlexContainer.SetAnchorPoint( AnchorPoint::CENTER );
  mFlexContainer.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT );

  for( unsigned int x = 0; x < NUMBER_OF_RESOURCES; ++x )
  {
    Toolkit::ImageView imageView;
    CreateChildImageView( imageView, IMAGE_PATH[ x ], Size(100.0f, 100.0f) );
    imageView.SetProperty( Toolkit::Control::Property::PADDING, Extents(0.f, 0.f, 0.f, 0.f) );
    imageView.SetProperty( Toolkit::Control::Property::MARGIN, Extents(2.0f, 2.0f, 2.0f, 2.0f) );
    mFlexContainer.Add( imageView );
  }
  stage.Add( mFlexContainer );
}

// Remove controls added by this example from stage
void FlexExample::Remove()
{
  if ( mFlexContainer )
  {
    UnparentAndReset( mDirectionButton );
    UnparentAndReset( mWrapButton );
    UnparentAndReset( mJustifyButton );
    UnparentAndReset( mRotateButton );
    UnparentAndReset( mFlexContainer);
  }
}

// Mirror items in layout
bool FlexExample::OnDirectionClicked( Button button )
{
  mDirection = !mDirection;
  if( mDirection )
  {
    mDirectionButton.SetProperty( PushButton::Property::UNSELECTED_ICON, LTR_IMAGE );
    mDirectionButton.SetProperty( PushButton::Property::SELECTED_ICON, LTR_SELECTED_IMAGE );
    mFlexContainer.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT );
  }
  else
  {
    mDirectionButton.SetProperty( PushButton::Property::UNSELECTED_ICON, RTL_IMAGE );
    mDirectionButton.SetProperty( PushButton::Property::SELECTED_ICON, RTL_SELECTED_IMAGE );
    mFlexContainer.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT );
  }
  return true;
}

// Rotate layout by changing layout
bool FlexExample::OnRotateClicked( Button button )
{
  auto layout = FboxLayout::DownCast(DevelControl::GetLayout( mFlexContainer ));
  if( layout.GetFlexDirection() == Toolkit::FboxLayout::FlexDirection::COLUMN )
  {
    layout.SetFlexDirection( Toolkit::FboxLayout::FlexDirection::ROW );
  }
  else
  {
    layout.SetFlexDirection( Toolkit::FboxLayout::FlexDirection::COLUMN );
  }
  return true;
}

// Alternates the layout wrapping from wrapping to none
bool FlexExample::OnWrapClicked( Button button )
{
  auto layout = FboxLayout::DownCast(DevelControl::GetLayout( mFlexContainer ));
  if ( layout.GetFlexWrap() == Toolkit::FboxLayout::WrapType::NO_WRAP )
  {
    layout.SetFlexWrap( Toolkit::FboxLayout::WrapType::WRAP );
    layout.SetFlexAlignment( Toolkit::FboxLayout::ALIGN_CENTER );
    mFlexContainer.SetProperty( Toolkit::Control::Property::PADDING, Extents(0.0f, 0.0f, 45.0f, 75.0f) );
  }
  else
  {
    layout.SetFlexWrap( Toolkit::FboxLayout::WrapType::NO_WRAP );
    layout.SetFlexAlignment( Toolkit::FboxLayout::ALIGN_FLEX_START );
    mFlexContainer.SetProperty( Toolkit::Control::Property::PADDING, Extents(0.0f, 0.0f, 0.0f, 0.0f) );
  }
  return true;
}

// Alternates the layout to justify space between items or not
bool FlexExample::OnJustifyClicked( Button button )
{
   auto layout = FboxLayout::DownCast(DevelControl::GetLayout( mFlexContainer ));
   if ( layout.GetFlexJustification() == Toolkit::FboxLayout::JUSTIFY_FLEX_START )
   {
     layout.SetFlexJustification( Toolkit::FboxLayout::JUSTIFY_SPACE_BETWEEN);
   }
   else
   {
     layout.SetFlexJustification( Toolkit::FboxLayout::JUSTIFY_FLEX_START );
   }
   return true;
}

} // namespace Demo
