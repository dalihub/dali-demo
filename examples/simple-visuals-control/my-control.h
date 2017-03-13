#ifndef DALI_DEMO_MY_CONTROL_H
#define DALI_DEMO_MY_CONTROL_H

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

#include <dali-toolkit/dali-toolkit.h>
#include <string>

namespace Demo
{

namespace Internal
{
class MyControl;
}

/**
 * @brief MyControl is an example control,
 *
 * @details It's purpose is to show how to create a simple control to work with the style sheet and state changes.
 * States changes includes Normal, Focused and Disabled, this example uses the style sheet to set visuals for Normal and Focused states.
 * When the Focus manager changes the control to be focused the visual displayed is changed and vice versa.
 *
 * The visual has the property name ICON_VISUAL with the style sheet string equivalent of "iconVisual"
 *
 */

class MyControl : public Dali::Toolkit::Control
{
public:

  /**
   * The start and end property ranges for this Control
   * My control can use properties from Toolkit::Control as it is derived from it. As control is derived from Actor, MyControl can also use Dali::Actor Properties.
   *
   * To ensure that the Property indexes from MyControl do not shadow any from Control we start it's index from the end of Toolkit::Control's indexes.
   *
   * Toolkit::Control would have done the same with Actor.
   *
   * The end index for this control is set to the start index + 1000 hence MyControl can have 1000 property indexes.
   *
   * PROPERTY_END_INDEX for MyControl is public, if another control is derived from it then if can specify MyControl::PROPERTY_END_INDEX+1 to start it's
   * indexing after MyControls last index.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Dali::Toolkit::Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000,
    ANIMATABLE_PROPERTY_START_INDEX = Dali::ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,
    ANIMATABLE_PROPERTY_END_INDEX = ANIMATABLE_PROPERTY_START_INDEX+1000
  };

  struct Property
  {
    enum
    {
      /**
       * @brief name "iconVisual", type string if it is a url, map otherwise
       * @details Sets the icon visual to be displayed by the control
       */
      ICON_VISUAL = PROPERTY_START_INDEX
    };
  };

public: // Construction / destruction

  /**
   * @brief Create an uninitialized handle
   */
  MyControl();

  /**
   * @brief Create a new MyControl
   */
  static MyControl New();

  /**
   * @brief Destructor. This is non-virtual since derived Handle types must not contain data or virtual methods
   */
  ~MyControl();

  /**
   * @brief Copy Constructor
   *
   * @param[in] shadowButton the handle of the control to copy
   */
  MyControl( const MyControl& shadowButton );

  /**
   * @brief Assignment Operator
   *
   * @param[in] shadowButton the source of the assignment
   */
  MyControl& operator=( const MyControl& shadowButton );

  /**
   * @brief Downcast
   *
   * @param[in] shadowButton the handle of control to downcast to MyControl
   */
  static MyControl DownCast( BaseHandle handle );


public: //  // Not intended for application developers

  /// @cond internal
  /**
   * @brief Create a handle from an implementation
   */
  MyControl( Internal::MyControl& implementation );

  /**
   * @brief Allow the creation of an ShadowButton handle from an internal CustomActor pointer
   */
  MyControl( Dali::Internal::CustomActor* internal );
  /// @endcond
};

} // namespace Demo

#endif // DALI_DEMO_MY_CONTROL_H
