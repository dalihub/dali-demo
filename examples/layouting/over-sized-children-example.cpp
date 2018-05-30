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
#include "over-sized-children-example.h"
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/hbox-layout.h>
#include <dali-toolkit/devel-api/layouting/vbox-layout.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

// Helper function to create ImageViews with given filename and size.
void CreateChildImageView( ImageView& imageView, const char* filename, Size size )
{
  imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imagePropertyMap[ Toolkit::ImageVisual::Property::URL ] = filename;

  if ( size.width > 0)
  {
    imagePropertyMap[ ImageVisual::Property::DESIRED_WIDTH ] = size.width;
  }

  if( size.height > 0 )
  {
    imagePropertyMap[ ImageVisual::Property::DESIRED_HEIGHT ] = size.height;
  }

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE , imagePropertyMap );
  imageView.SetName("ImageView");
  imageView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
}

} // namespace

void CreateParent( Control child )
{
  Control parent = Control::New();
  auto parentLayout = HboxLayout::New();
  DevelControl::SetLayout( parent, parentLayout );
  parent.SetName( "HboxParent");
  parent.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 200 ); // Ignores this value and uses that of its parent
  parent.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, 300 ); // Ignores this value and uses that of its parent
  parent.SetAnchorPoint( AnchorPoint::CENTER );
  parent.SetParentOrigin( ParentOrigin::CENTER );
  parent.SetBackgroundColor( Color::BLUE );
  Stage::GetCurrent().Add( parent );
  parent.Add(child);
}

namespace Demo
{

void OverSizedChildrenExample::Create()
{
  // Create an ImageView with a DESIRED_WIDTH and DESIRED_HEIGHT
  ImageView imageView = ImageView::New();
  CreateChildImageView( imageView, DEMO_IMAGE_DIR "gallery-small-23.jpg", Size( 0.0f, 0.f ) ) ;
  // Create a HorizontalBox Container
  mContainer = Control::New();
  auto hboxLayout = HboxLayout::New();
  DevelControl::SetLayout( mContainer, hboxLayout );
  mContainer.SetName( "Hbox");
  mContainer.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT ); // Ignores explict values
  mContainer.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT ); // Ignores explict values
  mContainer.SetAnchorPoint( AnchorPoint::CENTER );
  mContainer.SetParentOrigin( ParentOrigin::CENTER );
  mContainer.SetBackgroundColor( Color::WHITE );

  const auto NUMBER_OF_CHILDREN = 1;
  for( auto i =0; i < NUMBER_OF_CHILDREN; i++ )
  {
    // Create an ImageView with a DESIRED_WIDTH and DESIRED_HEIGHT
    ImageView imageView = ImageView::New();
    CreateChildImageView( imageView, DEMO_IMAGE_DIR "gallery-small-23.jpg", Size( 0.0f, 0.f ) ) ;
    imageView.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION,  ChildLayoutData::MATCH_PARENT ); // Ignore this spec
    imageView.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT ); // Ignore this spec
    imageView.SetAnchorPoint( AnchorPoint::CENTER );
    imageView.SetParentOrigin( ParentOrigin::CENTER );
    mContainer.Add( imageView );
  }

  // Create Parent for container
  CreateParent( mContainer );
}

void OverSizedChildrenExample::Remove()
{
  UnparentAndReset( mContainer );
}

} //namespace Demo