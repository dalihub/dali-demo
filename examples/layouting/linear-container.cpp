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

#include <iostream>
#include <string>
#include "linear-container.h"
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/hbox-layout.h>
#include <dali-toolkit/devel-api/layouting/vbox-layout.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* LTR_IMAGE( DEMO_IMAGE_DIR "icon-play.png" );
const char* LTR_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-play-selected.png" );

const char* RTL_IMAGE( DEMO_IMAGE_DIR "icon-reverse.png" );
const char* RTL_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-reverse-selected.png" );

const char* ROTATE_CLOCKWISE_IMAGE( DEMO_IMAGE_DIR "icon-reset.png" );
const char* ROTATE_CLOCKWISE_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-reset-selected.png" );


}  // namespace

namespace Demo
{

void LinearContainer::Create()
{
  // The Init signal is received once (only) during the Application lifetime
  auto stage = Stage::GetCurrent();

  mDirectionButton = PushButton::New();
  mDirectionButton.SetProperty( PushButton::Property::UNSELECTED_ICON, RTL_IMAGE );
  mDirectionButton.SetProperty( PushButton::Property::SELECTED_ICON, RTL_SELECTED_IMAGE );
  mDirectionButton.ClickedSignal().Connect( this, &LinearContainer::OnDirectionClicked );
  mDirectionButton.SetParentOrigin( Vector3(0.33f, 1.0f, 0.5f ) );
  mDirectionButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mDirectionButton.SetSize(75, 75);
  stage.Add( mDirectionButton );

  mRotateButton = PushButton::New();
  mRotateButton.SetProperty( PushButton::Property::UNSELECTED_ICON, ROTATE_CLOCKWISE_IMAGE );
  mRotateButton.SetProperty( PushButton::Property::SELECTED_ICON, ROTATE_CLOCKWISE_SELECTED_IMAGE );
  mRotateButton.ClickedSignal().Connect( this, &LinearContainer::OnRotateClicked );
  mRotateButton.SetParentOrigin( Vector3(0.66f, 1.0f, 0.5f ));
  mRotateButton.SetAnchorPoint( Vector3(0.5f, 1.0f, 0.5f));
  mRotateButton.SetSize(75, 75);
  stage.Add( mRotateButton );

  // Create a hbox layout
  mLinearContainer = Control::New();
  mIsHorizontal = false;

  auto layout = VboxLayout::New();
  layout.SetAnimateLayout(true);
  DevelControl::SetLayout( mLinearContainer, layout );

  mLinearContainer.SetParentOrigin( ParentOrigin::CENTER );
  mLinearContainer.SetAnchorPoint( AnchorPoint::CENTER );
  mLinearContainer.SetName( "linearContainer" );
  stage.Add( mLinearContainer );
  mLinearContainer.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  mLinearContainer.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  mLinearContainer.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT );

  for( unsigned int x = 0; x < NUMBER_OF_RESOURCES; ++x )
  {
    mImageViews[x] = Toolkit::ImageView::New();
    Property::Map imagePropertyMap;
    imagePropertyMap[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
    imagePropertyMap[ Toolkit::ImageVisual::Property::URL ] = IMAGE_PATH[ x ];
    imagePropertyMap[ ImageVisual::Property::DESIRED_WIDTH ] = 100.0f;
    imagePropertyMap[ ImageVisual::Property::DESIRED_HEIGHT ] = 100.0f;
    mImageViews[x].SetProperty(Toolkit::ImageView::Property::IMAGE , imagePropertyMap );
    mImageViews[x].SetName("ImageView");
    mImageViews[x].SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mImageViews[x].SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

    if( 1 == x )
    {
      mImageViews[x].SetProperty(Toolkit::Control::Property::PADDING, Extents(10.0f,10.0f,5.0f, 5.0f));
    }

    if( 0 == x )
    {
      mImageViews[x].SetProperty(Toolkit::Control::Property::MARGIN, Extents(10.0f,10.0f,5.0f, 5.0f));
    }

    mLinearContainer.Add( mImageViews[x] );
  }
}

void LinearContainer::Remove()
{
  if ( mLinearContainer )
  {
    UnparentAndReset( mDirectionButton );
    UnparentAndReset( mRotateButton );
    UnparentAndReset( mLinearContainer);
  }
}

bool LinearContainer::OnDirectionClicked( Button button )
{
  std::cout << "OnDirectionClicked" << std::endl;
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

bool LinearContainer::OnRotateClicked( Button button )
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