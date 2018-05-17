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
#include "linear-example.h"
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/hbox-layout.h>
#include <dali-toolkit/devel-api/layouting/vbox-layout.h>

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

// Child image filenames
const char* IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "application-icon-101.png",
  DEMO_IMAGE_DIR "application-icon-102.png",
  DEMO_IMAGE_DIR "application-icon-103.png",
  DEMO_IMAGE_DIR "application-icon-104.png"
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
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE , imagePropertyMap );
  imageView.SetName("ImageView");
  imageView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  imageView.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
}

}  // namespace

namespace Demo
{

void LinearExample::Create()
{
  // The Init signal is received once (only) during the Application lifetime
  auto stage = Stage::GetCurrent();

  mDirectionButton = PushButton::New();
  mDirectionButton.SetProperty( PushButton::Property::UNSELECTED_ICON, RTL_IMAGE );
  mDirectionButton.SetProperty( PushButton::Property::SELECTED_ICON, RTL_SELECTED_IMAGE );
  mDirectionButton.ClickedSignal().Connect( this, &LinearExample::OnDirectionClicked );
  mDirectionButton.SetParentOrigin( Vector3(0.33f, 1.0f, 0.5f ) );
  mDirectionButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mDirectionButton.SetSize(75, 75);
  stage.Add( mDirectionButton );

  mRotateButton = PushButton::New();
  mRotateButton.SetProperty( PushButton::Property::UNSELECTED_ICON, ROTATE_CLOCKWISE_IMAGE );
  mRotateButton.SetProperty( PushButton::Property::SELECTED_ICON, ROTATE_CLOCKWISE_SELECTED_IMAGE );
  mRotateButton.ClickedSignal().Connect( this, &LinearExample::OnRotateClicked );
  mRotateButton.SetParentOrigin( Vector3(0.66f, 1.0f, 0.5f ));
  mRotateButton.SetAnchorPoint( Vector3(0.5f, 1.0f, 0.5f));
  mRotateButton.SetSize(75, 75);
  stage.Add( mRotateButton );

  // Create a linear layout
  mLinearContainer = Control::New();
  mIsHorizontal = false;

  auto layout = VboxLayout::New();
  layout.SetAnimateLayout(true);
  DevelControl::SetLayout( mLinearContainer, layout );

  mLinearContainer.SetParentOrigin( ParentOrigin::CENTER );
  mLinearContainer.SetAnchorPoint( AnchorPoint::CENTER );
  mLinearContainer.SetName( "LinearExample" );
  stage.Add( mLinearContainer );
  mLinearContainer.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  mLinearContainer.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  mLinearContainer.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT );

  for( unsigned int x = 0; x < NUMBER_OF_RESOURCES; ++x )
  {
    Toolkit::ImageView imageView;
    CreateChildImageView( imageView, IMAGE_PATH[ x ], Size(100.0f, 100.0f) );
    mLinearContainer.Add( imageView );
  }
}

// Remove controls added by this example from stage
void LinearExample::Remove()
{
  if ( mLinearContainer )
  {
    UnparentAndReset( mDirectionButton );
    UnparentAndReset( mRotateButton );
    UnparentAndReset( mLinearContainer);
  }
}

// Mirror items in layout
bool LinearExample::OnDirectionClicked( Button button )
{
  if( mDirection )
  {
    mDirectionButton.SetProperty( PushButton::Property::UNSELECTED_ICON, LTR_IMAGE );
    mDirectionButton.SetProperty( PushButton::Property::SELECTED_ICON, LTR_SELECTED_IMAGE );
    mLinearContainer.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT );
  }
  else
  {
    mDirectionButton.SetProperty( PushButton::Property::UNSELECTED_ICON, RTL_IMAGE );
    mDirectionButton.SetProperty( PushButton::Property::SELECTED_ICON, RTL_SELECTED_IMAGE );
    mLinearContainer.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT );
  }
  mDirection = !mDirection;
  return true;
}

// Rotate layout by changing layout
bool LinearExample::OnRotateClicked( Button button )
{
  mIsHorizontal = !mIsHorizontal;
  if( mIsHorizontal )
  {
    auto hboxLayout = HboxLayout::New();
    hboxLayout.SetAnimateLayout(true);
    DevelControl::SetLayout( mLinearContainer, hboxLayout );
  }
  else
  {
    auto vboxLayout = VboxLayout::New();
    vboxLayout.SetAnimateLayout(true);
    DevelControl::SetLayout( mLinearContainer, vboxLayout );
  }
  return true;
}

} // namespace Demo