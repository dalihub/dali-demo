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
#include "layout-utilities.h"
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/absolute-layout.h>
#include <dali-toolkit/devel-api/layouting/grid.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* const TITLE = "Grid Example";

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

enum CurrentExample
{
  GRID_EXACT_WIDTH = 0,
  ITEMS_WITH_MARGINS,
  GRID_MATCH_PARENT,
  GRID_WRAP_CONTENT,
  ADD_ITEMS,
  CHANGE_TO_3_COLUMNS
};

}

namespace Demo
{

GridExample::GridExample()
: Example( TITLE )
{
}

void GridExample::Create()
{
  // The Init signal is received once (only) during the Application lifetime
  mToggleStatus = 0;
  auto stage = Stage::GetCurrent();

  // This layout will be the size of the stage but allow the Grid layout to be any size.
  mRootLayoutControl = LayoutUtilities::CreateRootContainer();
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
  mGridContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::WRAP_CONTENT );
  mGridContainer.SetProperty( LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );
  mGridContainer.SetProperty( Toolkit::Control::Property::PADDING, Extents( 20,20,20,20 ) );
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
  mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, "Set Width 300" );
  mToggleButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  mToggleButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mToggleButton.ClickedSignal().Connect( this, &Demo::GridExample::ToggleButtonClicked );
  mToggleButton.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  mToggleButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );

  stage.Add( mToggleButton );
}

void GridExample::Remove()
{
  mImageViews.clear();
  UnparentAndReset( mGridContainer );
  UnparentAndReset( mRootLayoutControl );
  UnparentAndReset( mToggleButton );
}

void GridExample::ChangeTo3Columns()
{
  Grid gridLayout = Grid::DownCast( DevelControl::GetLayout(mGridContainer) );
  if ( gridLayout )
  {
    gridLayout.SetNumberOfColumns( 3 );
  }
}

void GridExample::AddItemsInteractively()
{
  if( mImageViews.size() < MAX_NUMBER_OF_IMAGE_VIEWS )
  {
    ImageView imageView;
    CreateChildImageView( imageView, DEMO_IMAGE_DIR "gallery-small-23.jpg" , Size(100.0f, 100.0f) );
    mImageViews.push_back( imageView );
    mGridContainer.Add( imageView);

    // Add item button shows how many items left to add.
    unsigned int numberOfAdditonalImageViews = MAX_NUMBER_OF_IMAGE_VIEWS-INITIAL_NUMBER_OF_IMAGE_VIEWS;
    unsigned int remainingImageViews = numberOfAdditonalImageViews - ( ( mImageViews.size() - INITIAL_NUMBER_OF_IMAGE_VIEWS) );
    std::string buttonLabel( "Add item["+ std::to_string( numberOfAdditonalImageViews-remainingImageViews ) +"/"+
                                          std::to_string( numberOfAdditonalImageViews)+"]" );
    mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, buttonLabel );
  }
}

void GridExample::AddMarginToItems()
{
  for( unsigned int x = 0; x < INITIAL_NUMBER_OF_IMAGE_VIEWS; x++ )
  {
    mImageViews[x].SetProperty(Toolkit::Control::Property::MARGIN, Extents( 20,20,20,10));
  }
}

void GridExample::RemoveMarginsFromItems()
{
  for( unsigned int x = 0; x < INITIAL_NUMBER_OF_IMAGE_VIEWS; x++ )
  {
    mImageViews[x].SetProperty(Toolkit::Control::Property::MARGIN, Extents());
  }
}

void GridExample::MatchParentOnWidth()
{
  mGridContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION,ChildLayoutData::MATCH_PARENT );
}

void GridExample::WrapContentOnWidth()
{
  mGridContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION,  ChildLayoutData::WRAP_CONTENT );
}

void GridExample::SetExactWidth()
{
  mGridContainer.SetProperty( LayoutItem::ChildProperty::WIDTH_SPECIFICATION,  300 );
}

bool GridExample::ToggleButtonClicked( Toolkit::Button button )
{
  switch( mToggleStatus )
  {
    case GRID_EXACT_WIDTH :
    {
      SetExactWidth();
      mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, "Set Child Margin" );
      mToggleStatus = ITEMS_WITH_MARGINS;
      break;
    }
    case ITEMS_WITH_MARGINS :
    {
      AddMarginToItems();
      mToggleStatus = GRID_MATCH_PARENT;
      mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, "Set width MATCH_PARENT" );
      break;
    }
    case GRID_MATCH_PARENT :
    {
      RemoveMarginsFromItems();
      MatchParentOnWidth();
      mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, "Set width WRAP_CONTENT" );
      mToggleStatus = GRID_WRAP_CONTENT;
      break;
    }
    case GRID_WRAP_CONTENT :
    {
      WrapContentOnWidth();
      mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, "Add item" );
      mToggleStatus = ADD_ITEMS;
      break;
    }
    case ADD_ITEMS :
    {
      if( mGridContainer.GetChildCount() < MAX_NUMBER_OF_IMAGE_VIEWS )
      {
        AddItemsInteractively();
      }

      if( mGridContainer.GetChildCount() == MAX_NUMBER_OF_IMAGE_VIEWS )
      {
        // Remove button when no more items to add
        mToggleStatus= CHANGE_TO_3_COLUMNS;
        mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, "Change Columns" );
      }
      break;
    }
    case CHANGE_TO_3_COLUMNS :
    {
      ChangeTo3Columns();
      mToggleStatus = GRID_EXACT_WIDTH;
      UnparentAndReset( mToggleButton );
      break;
    }
    default :
    {
      mToggleStatus = GRID_EXACT_WIDTH;
    }
  }
  return true;
}

} // namespace Demo
