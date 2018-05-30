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
#include "grid-example.h"
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/grid.h>

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


namespace Demo
{

void GridExample::Create()
{
  // Create a Grid Container to fit a number of children.
  mContainer = Control::New();
  auto gridLayout = Grid::New();
  gridLayout.SetColumnCount( 3 );
  DevelControl::SetLayout( mContainer, gridLayout );
  mContainer.SetName( "Grid");
  mContainer.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  mContainer.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  mContainer.SetAnchorPoint( AnchorPoint::CENTER );
  mContainer.SetParentOrigin( ParentOrigin::CENTER );
  mContainer.SetBackgroundColor( Color::WHITE );

  const auto NUMBER_OF_CHILDREN = 3;
  for ( auto i = 0; i < NUMBER_OF_CHILDREN; i++ )
  {
    ImageView imageView = ImageView::New();
    CreateChildImageView( imageView, DEMO_IMAGE_DIR "gallery-small-23.jpg", Size( 0.0f, 0.f ) ) ;
    mContainer.Add( imageView );
  }

  Stage::GetCurrent().Add( mContainer );
}

void GridExample::Remove()
{
  UnparentAndReset( mContainer );
}

} //namespace Demo