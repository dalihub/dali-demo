/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <vector>
#include <dali/public-api/adaptor-framework/application.h>
#include <dali/public-api/adaptor-framework/key.h>
#include <dali/public-api/events/key-event.h>
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>

// INTERNAL INCLUDES
#include "contact-card-layouter.h"
#include "contact-data.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const Vector4 WINDOW_COLOR( 211.0f / 255.0f, 211.0f / 255.0f, 211.0f / 255.0f, 1.0f ); ///< The color of the window
const char * const THEME_PATH( DEMO_STYLE_DIR "contact-cards-example-theme.json" ); ///< The theme used for this example
} // unnamed namespace

/**
 * @brief Creates several contact cards that animate between a folded and unfolded state.
 *
 * This demonstrates how different animations can start and stop at different times within the same Animation function.
 * Additionally, this also shows how to morph between two different geometries.
 *
 * ContactCardLayouter: This class is used to lay out the different contact cards on the screen.
 *                      This takes window size into account but does not support relayouting.
 * ContactCard: This class represents each contact card on the screen.
 *              Two animations are set up in this class which animate several properties with multiple start and stop times.
 *              An overview of the two animations can be found in contact-card.cpp.
 * ContactCardLayoutInfo: This is a structure to store common layout information and is created by the ContactCardLayouter and used by each ContactCard.
 * ContactData: This namespace contains a table which has the contact information we use to populate the contact cards.
 * ClippedImage: This namespace provides a helper function which creates an ImageView which is added to a control that has clipping.
 *               This clipping comes in the form of a Circle or Quad.
 *               The Vertex shader mixes in the Circle and Quad geometry depending on the value of a uniform float.
 *               Animating this float between CIRCLE_GEOMETRY and QUAD_GEOMETRY is what enables the morphing between the two geometries.
 * MaskedImage: This namespace provides a helper function which creates an ImageView with a mask that matches the Circle geometry provided by ClippedImage.
 *              Using a mask yields much better quality than when using an image with a circle geometry, so this is ONLY used when the contact card is folded.
 */
class ContactCardController : public ConnectionTracker // Inherit from ConnectionTracker so that our signals can be automatically disconnected upon our destruction.
{
public:

  /**
   * @brief Constructor.
   * @param[in]  application A reference to the Application class.
   */
  ContactCardController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ContactCardController::Create );
  }

private:

  /**
   * @brief Called to initialise the application content
   * @param[in] application A reference to the Application class.
   */
  void Create( Application& application )
  {
    // Set the window background color and connect to the window's key signal to allow Back and Escape to exit.
    Window window = application.GetWindow();
    window.SetBackgroundColor( WINDOW_COLOR );
    window.KeyEventSignal().Connect( this, &ContactCardController::OnKeyEvent );

    // Add all the contacts to the layouter
    for( size_t i = 0; i < ContactData::TABLE_SIZE; ++i )
    {
      mContactCardLayouter.AddContact( window, ContactData::TABLE[ i ].name, ContactData::TABLE[ i ].address, ContactData::TABLE[ i ].imagePath );
    }
  }

  /**
   * @brief Called when any key event is received
   *
   * Will use this to quit the application if Back or the Escape key is received
   * @param[in] event The key event information
   */
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      KeyInputFocusManager keyInputFocusManager = KeyInputFocusManager::Get();
      if( ! keyInputFocusManager.GetCurrentFocusControl() ) // Don't quit if a control has focus
      {
        if ( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
        {
          mApplication.Quit();
        }
      }
    }
  }

  Application& mApplication; ///< Reference to the application class.
  ContactCardLayouter mContactCardLayouter; ///< The contact card layouter.
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, THEME_PATH );
  ContactCardController contactCardController( application );
  application.MainLoop();
  return 0;
}
