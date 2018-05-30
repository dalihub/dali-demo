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

#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/hbox-layout.h>
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>

//#include "custom-layout.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

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

} // unnamed namespace

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &HelloWorldController::OnTouch );

    Control control = Control::New();

    auto layout = HboxLayout::New();
    DevelControl::SetLayout( control, layout );

    control.SetParentOrigin( ParentOrigin::CENTER );
    control.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( control);
    //control.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
    //control.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
    //mLinearContainer.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT );

    for( unsigned int x = 0; x < NUMBER_OF_RESOURCES; ++x )
    {
       Toolkit::ImageView imageView;
       CreateChildImageView( imageView, IMAGE_PATH[ x ], Size(100.0f, 100.0f) );
       control.Add( imageView );
    }
  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  HelloWorldController test( application );
  application.MainLoop();
  return 0;
}
