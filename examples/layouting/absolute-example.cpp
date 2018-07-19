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

#include <string>
#include "absolute-example.h"
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/absolute-layout.h>

using namespace Dali;
using namespace Dali::Toolkit;


namespace
{

struct ImageDetails
{
  const char * name;
  Vector2 position;
  Size size;
};

ImageDetails IMAGES[] =
{
  { DEMO_IMAGE_DIR "gallery-small-23.jpg", Vector2( 0.0f,   0.0f ), Size( 100.0f, 100.0f ) },
  { DEMO_IMAGE_DIR "gallery-small-23.jpg", Vector2( 100.0f,   0.0f ), Size( 100.0f, 100.0f ) },
  { DEMO_IMAGE_DIR "gallery-small-23.jpg", Vector2( 0.0f, 100.0f ), Size( 100.0f, 100.0f ) },
  { DEMO_IMAGE_DIR "gallery-small-23.jpg", Vector2( 200.0f, 200.0f ), Size( 100.0f, 100.0f ) },
};
unsigned int IMAGE_COUNT=sizeof(IMAGES)/sizeof(IMAGES[0]);

// Helper function to create ImageViews with given filename and size.
void CreateChildImageView( ImageView& imageView, unsigned imageIndex )
{
  imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap[ Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imagePropertyMap[ ImageVisual::Property::URL ] = IMAGES[imageIndex].name;
  imagePropertyMap[ ImageVisual::Property::DESIRED_WIDTH ] = IMAGES[imageIndex].size.width ;
  imagePropertyMap[ ImageVisual::Property::DESIRED_HEIGHT ] = IMAGES[imageIndex].size.height;
  imageView.SetProperty( ImageView::Property::IMAGE , imagePropertyMap );
  imageView.SetName("ImageView");
  imageView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  imageView.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
  imageView.SetProperty( Dali::Actor::Property::POSITION, Vector3( IMAGES[imageIndex].position ) );
}

} // namespace

namespace Demo
{

AbsoluteExample::AbsoluteExample()
: mRootLayoutControl(),
  mAbsoluteLayoutContainer(),
  mLayoutSizeToggleStatus( true ),
  mToggleButton()
{

}

void AbsoluteExample::Create()
{
  // Create a root layout, ideally Dali would have a default layout in the root layer.
  // Without this root layer the mAbsoluteLayout (or any other layout) will not
  // honour WIDTH_SPECIFICATION or HEIGHT_SPECIFICATION settings.
  // It uses the default stage size and ideally should have a layout added to it.
  auto stage = Stage::GetCurrent();
  mRootLayoutControl = Control::New();
  auto rootLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( mRootLayoutControl, rootLayout );
  mRootLayoutControl.SetAnchorPoint( AnchorPoint::CENTER );
  mRootLayoutControl.SetParentOrigin( ParentOrigin::CENTER );
  stage.Add( mRootLayoutControl );

  // Create an Absolute Layout to show ImageViews at explictly provided positions.
  mAbsoluteLayoutContainer = Control::New();
  mAbsoluteLayoutContainer.SetBackgroundColor( Color::WHITE );
  auto absoluteLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( mAbsoluteLayoutContainer, absoluteLayout );
  mAbsoluteLayoutContainer.SetName("AbsoluteLayout");

  mAbsoluteLayoutContainer.SetAnchorPoint( AnchorPoint::CENTER );
  mAbsoluteLayoutContainer.SetParentOrigin( ParentOrigin::CENTER );

  // Initially absolute layout to use these specifications, toggle button will alter them.
  mAbsoluteLayoutContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  mAbsoluteLayoutContainer.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );

  mRootLayoutControl.Add( mAbsoluteLayoutContainer );

  for( unsigned int x = 0; x < NUMBER_OF_IMAGE_VIEWS; x++ )
  {
    CreateChildImageView( mImageViews[x], x%IMAGE_COUNT );
    mAbsoluteLayoutContainer.Add( mImageViews[x] );
  }

  // Button toggles the size of the layout
  mToggleButton = PushButton::New();
  mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, "Change layout size" );
  mToggleButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  mToggleButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mToggleButton.ClickedSignal().Connect( this, &Demo::AbsoluteExample::ChangeSizeClicked );
  mToggleButton.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  mToggleButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );

  stage.Add( mToggleButton );

}

void AbsoluteExample::Remove()
{
  UnparentAndReset( mAbsoluteLayoutContainer );
  UnparentAndReset( mToggleButton );
  UnparentAndReset( mRootLayoutControl );
}

bool AbsoluteExample::ChangeSizeClicked( Toolkit::Button button )
{
  if ( true == mLayoutSizeToggleStatus )
  {
    mAbsoluteLayoutContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 480 );
    mAbsoluteLayoutContainer.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  700 );
    mLayoutSizeToggleStatus = false;
  }
  else
  {
    mAbsoluteLayoutContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
    mAbsoluteLayoutContainer.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );
    mLayoutSizeToggleStatus = true;
  }

  return true;
}

} // namespace Demo