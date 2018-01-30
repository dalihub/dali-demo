#ifndef DALI_DEMO_INTERNAL_EXPANDING_BUTTONS_IMPL_H
#define DALI_DEMO_INTERNAL_EXPANDING_BUTTONS_IMPL_H

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

#include "expanding-buttons.h"
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/public-api/object/weak-handle.h>

namespace Demo
{

namespace Internal // To use TypeRegistry, handle and body classes need the same name
{

class ExpandingButtons : public Dali::Toolkit::Internal::Control
{
public:

  /**
   * Instantiate a new ExpandingButtons object
   */
  static Demo::ExpandingButtons New();

  /**
   * Constructor
   */
  ExpandingButtons();

  /**
   * Destructor
   */
  ~ExpandingButtons();

public: // API

  void RegisterButton( Dali::Toolkit::Control& control );

  void Expand();

  void Collapse();

public: // Signals

  Demo::ExpandingButtons::ExpandingButtonsSignalType& CollapsingSignal();

private: // From Control

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize() override;

  /**
   * @copydoc Toolkit::Control::OnRelayout()
   */
  virtual void OnRelayout( const Dali::Vector2& targetSize, Dali::RelayoutContainer& container ) override;

private:

  /**
   * Callback when expand or collapse animation ends.
   */
  void OnExpandAnimationFinished( Dali::Animation& animation );

  /**
   * Called when the main button clicked.
   */
  bool OnExpandButtonClicked( Dali::Toolkit::Button button );


private:
  //undefined
  ExpandingButtons( const ExpandingButtons& );
  ExpandingButtons& operator=( const ExpandingButtons& );

private:

  std::vector< Dali::WeakHandle< Dali::Toolkit::Control> > mExpandingControls;

  Dali::Animation mExpandCollapseAnimation;

  bool mStyleButtonsHidden;

  Dali::Size mButtonSize; // Size of the buttons, used in animation calculations.

  Dali::Toolkit::PushButton mExpandButton; // Main button that is clicked to expand/collapse.

  Demo::ExpandingButtons::ExpandingButtonsSignalType mCollapsedSignal;
};

} // Internal

inline Internal::ExpandingButtons& GetImpl( Demo::ExpandingButtons& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  Dali::RefObject& object = handle.GetImplementation();
  return static_cast<Internal::ExpandingButtons&>(object);
}

inline const Internal::ExpandingButtons& GetImpl( const Demo::ExpandingButtons& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  const Dali::RefObject& object = handle.GetImplementation();
  return static_cast<const Internal::ExpandingButtons&>(object);
}

} // Demo

#endif //  DALI_DEMO_INTERNAL_EXPANDING_BUTTONS_IMPL_H
