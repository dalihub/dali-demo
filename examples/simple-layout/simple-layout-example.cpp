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
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>

#include "custom-layout.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

/// Child image filenames
const char* IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "application-icon-101.png",
  DEMO_IMAGE_DIR "application-icon-102.png",
  DEMO_IMAGE_DIR "application-icon-103.png",
  DEMO_IMAGE_DIR "application-icon-104.png",
};
const unsigned int NUMBER_OF_IMAGES = sizeof( IMAGE_PATH ) / sizeof( char* );

/**
 * @brief Helper function to create ImageViews with given filename and size.
 * @param[in]  filename  The filename of the image to use
 * @param[in]  size      The size that the image should be loaded at
 * @return The created ImageView
 */
ImageView CreateChildImageView( const char* filename, Size size )
{
  ImageView imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imagePropertyMap[ Toolkit::ImageVisual::Property::URL ] = filename;
  imagePropertyMap[ ImageVisual::Property::DESIRED_WIDTH ] = size.width;
  imagePropertyMap[ ImageVisual::Property::DESIRED_HEIGHT ] = size.height;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE , imagePropertyMap );
  imageView.SetName("ImageView");
  imageView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  imageView.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
  return imageView;
}

} // unnamed namespace

/**
 * @brief Demonstrates how to create a very simple layout and apply that to any Control.
 */
class SimpleLayoutExample : public ConnectionTracker
{
public:

  /**
   * @brief Constructor.
   * @param[in]  application  A reference to the Application class.
   */
  SimpleLayoutExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &SimpleLayoutExample::Create );
  }

private:

  /**
   * @brief Called to initialise the application content
   * @param[in] application A reference to the Application class.
   */
  void Create( Application& application )
  {
    // Get a handle to the stage, change the background color and connect to the Touch & Key signals
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );
    stage.GetRootLayer().TouchSignal().Connect( this, &SimpleLayoutExample::OnTouch );
    stage.KeyEventSignal().Connect( this, &SimpleLayoutExample::OnKeyEvent );
    stage.KeepRendering(0.5f); // TODO: Should remove after bugfix, but currently required to ensure renders are done after resources are loaded


    // Create a new control
    Control control = Control::New();
    control.SetParentOrigin( ParentOrigin::CENTER );
    control.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( control);

    // Set our Custom Layout on the control
    auto layout = Demo::CustomLayout::New();
    DevelControl::SetLayout( control, layout );

    // Add child image-views to the created control
    for( auto i = 0u; i < NUMBER_OF_IMAGES; ++i )
    {
      control.Add( CreateChildImageView( IMAGE_PATH[ i ], Size( 100.0f, 100.0f ) ) );
    }
  }

  /**
   * @brief Called when the stage is touched.
   *
   * We will use this to quit the application.
   */
  bool OnTouch( Actor /* actor */, const TouchData& /* touch */ )
  {
    mApplication.Quit();
    return true;
  }

  /**
   * @brief Called when any key event is received.
   *
   * Will use this to quit the application if Back or the Escape key is received
   * @param[in] event The key event information
   */
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if ( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication; ///< A reference to the application object.
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  SimpleLayoutExample test( application );
  application.MainLoop();
  return 0;
}
