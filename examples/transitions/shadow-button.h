#ifndef DALI_DEMO_SHADOW_BUTTON_H
#define DALI_DEMO_SHADOW_BUTTON_H

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

// All type registered controls need to have the same name for the body
// and the handle
namespace Internal
{
class ShadowButton;
}

/**
 * Button that allows the RGB channels of an image to be altered.
 */
class ShadowButton : public Dali::Toolkit::Control
{
public:
  /**
   * The start and end property ranges for this Control
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
      ACTIVE_TRANSITION = PROPERTY_START_INDEX,
      INACTIVE_TRANSITION,
      CHECK_TRANSITION,
      UNCHECK_TRANSITION,
      BACKGROUND_VISUAL,
      CHECKBOX_BG_VISUAL,
      CHECKBOX_FG_VISUAL,
      LABEL_VISUAL,
      ACTIVE_STATE,
      CHECK_STATE,
    };
  };

public: // Construction / destruction

  /**
   * Create an uninitialized handle
   */
  ShadowButton();

  /**
   * Create a new image channel button without an image. Use
   * SetImage to give this button an image
   */
  static ShadowButton New();

  /**
   * Create a new image channel button from a given URL
   */
  static ShadowButton New( const std::string& url );

  /**
   * Destructor. This is non-virtual since derived Handle types must not
   * contain data or virtual methods
   */
  ~ShadowButton();

  /**
   * Copy Constructor
   */
  ShadowButton( const ShadowButton& shadowButton );

  /**
   * Assignment Operator
   */
  ShadowButton& operator=( const ShadowButton& shadowButton );

  /**
   * Downcast
   */
  static ShadowButton DownCast( BaseHandle handle );

public: // API

  /**
   * @brief Set the button to be active or inactive.
   *
   * The button will perform a transition if there is a state change.
   * @param[in] active The active state
   */
  void SetActiveState( bool active );

  /**
   * @brief Get the button's active state
   *
   * @return The active state
   */
  bool GetActiveState();

  /**
   * Set the check state
   * @param[in] checkState The state of the checkbox
   */
  void SetCheckState( bool checkState );

  /**
   * Get the check state
   * @return the check state
   */
  bool GetCheckState();

public: // Not for public use
  /**
   * Create a handle from an implementation
   */
  ShadowButton( Internal::ShadowButton& implementation );

  /**
   * Allow the creation of an ShadowButton handle from an internal CustomActor pointer
   */
  ShadowButton( Dali::Internal::CustomActor* internal );
};

} // namespace Demo

#endif // DALI_DEMO_SHADOW_BUTTON_H
