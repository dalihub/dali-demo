#ifndef DALI_DEMO_EXPANDING_BUTTONS_CONTROL_H
#define DALI_DEMO_EXPANDING_BUTTONS_CONTROL_H

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
 */

#include <dali-toolkit/dali-toolkit.h>

namespace Demo
{

namespace Internal
{
class ExpandingButtons;
}

/**
 * Control which toggles between showing and hiding a set of buttons.
 */
class ExpandingButtons : public Dali::Toolkit::Control
{


public: // Construction / destruction

  /**
   * Create an uninitialized handle
   */
  ExpandingButtons();

  /**
   * Create an ExpandingButton control with the default pressed and released visual.
   */
  static ExpandingButtons New();

  /**
   * Destructor. This is non-virtual since derived Handle types must not
   * contain data or virtual methods
   */
  ~ExpandingButtons();

  /**
   * Copy Constructor
   */
  ExpandingButtons( const ExpandingButtons& expandingButtons );

  /**
   * Assignment Operator
   */
  ExpandingButtons& operator=( const ExpandingButtons& expandingButtons );

  /**
   * Downcast
   */
  static ExpandingButtons DownCast( BaseHandle handle );

public: // API

  /**
   * Add a control (button) to the Expanding button bar. Will be displayed in order of addition.
   */
  void RegisterButton( Control& control );

  /**
   * Expand the registered buttons out from the main button.
   */
  void Expand();

  /**
   * Collapse the expanded buttons back behind the main button.
   */
  void Collapse();

public: // Signals

  /**
   * ExpandingButtons signal type
   */
   typedef Dali::Signal< bool ( ExpandingButtons ) > ExpandingButtonsSignalType;

  /**
   * This signal is emitted when the button is going to collapse.
   */
  ExpandingButtonsSignalType& CollapsingSignal();

public: // Not for public use

  /**
   * Create a handle from an implementation
   */
  ExpandingButtons( Internal::ExpandingButtons& implementation );

  /**
   * Allow the creation of an ExpandingButtons handle from an internal CustomActor pointer
   */
  ExpandingButtons( Dali::Internal::CustomActor* internal );
};

} // namespace Demo

#endif // DALI_DEMO_EXPANDING_BUTTONS_CONTROL_H
