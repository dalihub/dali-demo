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
#include <dali-toolkit/devel-api/layouting/absolute-layout.h>
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
  imagePropertyMap[ ImageVisual::Property::DESIRED_WIDTH ] = size.width;
  imagePropertyMap[ ImageVisual::Property::DESIRED_HEIGHT ] = size.height;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE , imagePropertyMap );
  imageView.SetName("ImageView");
  imageView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  imageView.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
}

}

namespace Demo
{

void GridExample::Create()
{
  // The Init signal is received once (only) during the Application lifetime

  // Create a root layout, ideally Dali would have a default layout in the root layer.
  // Without this root layer the mAbsoluteLayout (or any other layout) will not
  // honour WIDTH_SPECIFICATION or HEIGHT_SPECIFICATION settings.
  // It uses the default stage size and ideally should have a layout added to it.
  mToggleStatus = 0;
  auto stage = Stage::GetCurrent();
  mRootLayoutControl = Control::New();
  mRootLayoutControl.SetName( "AbsoluteLayout");
  auto rootLayout = AbsoluteLayout::New();
  DevelControl::SetLayout( mRootLayoutControl, rootLayout );
  mRootLayoutControl.SetAnchorPoint( AnchorPoint::CENTER );
  mRootLayoutControl.SetParentOrigin( ParentOrigin::CENTER );
  stage.Add( mRootLayoutControl );

  // Create a table view to show a pair of buttons above each image.
  mGridContainer = Control::New();

  // Create LinearLayout for this control.
  auto gridLayout = Grid::New();
  gridLayout.SetAnimateLayout(true);
  gridLayout.SetNumberOfColumns( 2 );
  DevelControl::SetLayout( mGridContainer, gridLayout );
  mGridContainer.SetName("GridContainer");
  mGridContainer.SetBackgroundColor( Color::WHITE );
  mGridContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, 480 );
  mGridContainer.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  700 );
  mGridContainer.SetAnchorPoint( AnchorPoint::CENTER );
  mGridContainer.SetParentOrigin( ParentOrigin::CENTER );

  mRootLayoutControl.Add( mGridContainer );

  for( unsigned int x = 0; x < INITIAL_NUMBER_OF_IMAGE_VIEWS; x++ )
  {
    ImageView imageView;
    CreateChildImageView( imageView, DEMO_IMAGE_DIR "gallery-small-23.jpg" , Size(100.0f, 100.0f) );
    mImageViews.push_back( imageView );
    mGridContainer.Add( mImageViews[x] );
  }

    // Button toggles the size of the layout
  mToggleButton = PushButton::New();
  mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, "Set LayoutSize 600" );
  mToggleButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  mToggleButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mToggleButton.ClickedSignal().Connect( this, &Demo::GridExample::ToggleButtonClicked );
  mToggleButton.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  mToggleButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );

  stage.Add( mToggleButton );
}

void GridExample::Remove()
{
  UnparentAndReset( mGridContainer );
  UnparentAndReset( mRootLayoutControl );
  UnparentAndReset( mToggleButton );
}

bool GridExample::ToggleButtonClicked( Toolkit::Button button )
{
  switch( mToggleStatus )
  {
    case 0 :
    {
      mGridContainer.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  600 );
      mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, "Set MATCH_PARENT" );
      mToggleStatus++;
      break;
    }
    case 1 :
    {
      mGridContainer.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::MATCH_PARENT );
      mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, "Add item" );
      mToggleStatus++;
      break;
    }
    case 2 :
    {
      if( mImageViews.size() < MAX_NUMBER_OF_IMAGE_VIEWS )
      {
        ImageView imageView;
        CreateChildImageView( imageView, DEMO_IMAGE_DIR "gallery-small-23.jpg" , Size(100.0f, 100.0f) );
        mImageViews.push_back( imageView );
        mGridContainer.Add( imageView);
        unsigned int numberOfAdditonalImageViews = MAX_NUMBER_OF_IMAGE_VIEWS-INITIAL_NUMBER_OF_IMAGE_VIEWS;
        unsigned int remainingImageViews = numberOfAdditonalImageViews - ( MAX_NUMBER_OF_IMAGE_VIEWS - mImageViews.size() );
        std::string buttonLabel( "Add item["+ std::to_string( remainingImageViews ) +"/"+ std::to_string(numberOfAdditonalImageViews)+"]" );
        mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, buttonLabel );
        if(  0 == remainingImageViews )
        {
          mToggleStatus++;
          mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, "Set LayoutSize 600" );
        }
      }
      else
      {
        mToggleStatus++;
        mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, "Set LayoutSize 600" );
      }
      break;
    }
    default:
    {
      mToggleStatus = 0;
      break;
    }

  }

  return true;
}

} // namespace Demo
