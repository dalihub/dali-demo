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
#include "padding-example.h"
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/hbox-layout.h>

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

void PaddingExample::Create()
{
  // The Init signal is received once (only) during the Application lifetime

  // Creates a default view with a default tool bar.
  // The view is added to the stage.
  auto stage = Stage::GetCurrent();
  // Create a table view to show a pair of buttons above each image.
  mHorizontalBox = Control::New();

  // Create HBoxLayout for this control.
  auto hboxLayout = HboxLayout::New();
  DevelControl::SetLayout( mHorizontalBox, hboxLayout );
  mHorizontalBox.SetName("HBox");

  mHorizontalBox.SetAnchorPoint( AnchorPoint::CENTER );
  mHorizontalBox.SetParentOrigin( ParentOrigin::CENTER );

  stage.Add( mHorizontalBox );

  for( unsigned int x = 0; x < NUMBER_OF_IMAGE_VIEWS; x++ )
  {
    mToggleButton = Toolkit::PushButton::New();
    mToggleButton.SetProperty( Toolkit::Button::Property::LABEL, "Toggle Padding on #2" );
    mToggleButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mToggleButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mToggleButton.ClickedSignal().Connect( this, &Demo::PaddingExample::ChangePaddingClicked );
    mToggleButton.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mToggleButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );

    stage.Add( mToggleButton );

    CreateChildImageView( mImageViews[x], DEMO_IMAGE_DIR "gallery-small-23.jpg" , Size(100.0f, 100.0f) );

    // Set Padding for second ImageView
    if( 1 == x )
    {
      mImageViews[x].SetProperty(Toolkit::Control::Property::PADDING, Extents(10.0f,10.0f,5.0f, 5.0f));
    }

    // Set margin for first ImageView
    if( 0 == x )
    {
      mImageViews[x].SetProperty(Toolkit::Control::Property::MARGIN, Extents(10.0f,10.0f,0.0f, 0.0f));
    }

    mHorizontalBox.Add( mImageViews[x] );

    mImageViewToggleStatus[ x ] = true;
  }
}

void PaddingExample::Remove()
{
  UnparentAndReset( mToggleButton );
  UnparentAndReset( mHorizontalBox );
}

bool PaddingExample::ChangePaddingClicked( Toolkit::Button button )
{
  if ( true == mImageViewToggleStatus[ 1 ] )
  {
    mImageViews[1].SetProperty(Toolkit::Control::Property::PADDING, Extents( .0f, .0f, .0f, .0f));
    mImageViews[1].SetProperty(Actor::Property::COLOR_ALPHA, 0.5f);
    mImageViewToggleStatus[ 1 ] = false;
  }
  else
  {
    mImageViews[1].SetProperty(Toolkit::Control::Property::PADDING, Extents( 10.0f, 10.0f, 5.0f, 5.0f));
    mImageViews[1].SetProperty(Actor::Property::COLOR_ALPHA, 1.0f);
    mImageViewToggleStatus[ 1 ] = true;
  }

  return true;
}

} // namespace Demo