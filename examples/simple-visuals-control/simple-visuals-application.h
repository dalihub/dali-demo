#ifndef DALI_DEMO_SIMPLE_VISUALS_APPLICATION_H
#define DALI_DEMO_SIMPLE_VISUALS_APPLICATION_H

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
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <cstdio>
#include <sstream>

// INTERNAL INCLUDES
#include "my-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace Demo
{

/**
 * @brief An application that uses the my-control to display 2 icons, if focus is allowed ( by using a keyboard or remote ) then the icons will change
 * depending on which one is focused.
 *
 * Inherits from connection tracker to manage connection and disconnection of signals,  In this case PreFocusChangeSignal
 */
class SimpleVisualsApplication : public ConnectionTracker
{

public:

  /**
   * @brief Constructor.
   *
   * @param[in]  application A reference to the Application class.
   */
  SimpleVisualsApplication( Application& application );


private:
  /**
   *  @brief Listen to Focus change signal
   *  @param[in] current  Current focused Actor
   *  @param[in] proposed New actor that is requesting to be focused
   *  @param[in] direction The direction of the focus event from current actor
   */
  Dali::Actor OnKeyboardPreFocusChange( Dali::Actor current, Dali::Actor proposed, Dali::Toolkit::Control::KeyboardFocus::Direction direction );

  /**
   * @brief Derived from control, enables capture of key presses
   *
   * @param[in] event In incoming key event
   */
  void OnKeyEvent( const KeyEvent& event );

  /**
   * @brief Called to initialise the application content
   *
   * @param[in] application A reference to the Application class.
   */
  void Create( Application& application );


private:

  Application& mApplication;  // Handle to the application that is created and passed in.

  MyControl mMyControl;  //  Handle to first instance of MyControl
  MyControl mMyControl2; //  Handle to second instance of MyControl

};

} // Namespace Demo


#endif // DALI_DEMO_SIMPLE_VISUALS_APPLICATION_H
