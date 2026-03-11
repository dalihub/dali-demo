#ifndef DALI_DEMO_CONTROLS_INTERNAL_TOOL_BAR_H
#define DALI_DEMO_CONTROLS_INTERNAL_TOOL_BAR_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/devel-api/common/map-wrapper.h>

// INTERNAL INCLUDES
#include "tool-bar.h"

namespace Dali::Demo
{
class ToolBar;

namespace Internal
{
/**
 * ToolBar is a control to create a tool bar.
 * @see Dali::Demo::ToolBar for more details.
 */
class ToolBar : public Toolkit::Internal::Control
{
public:
  /**
   * Create an initialized ToolBar.
   * @return A handle to a newly allocated Dali resource.
   */
  static Demo::ToolBar New();

  /**
   * @copydoc Dali::Demo::ToolBar::AddControl()
   */
  void AddControl(Dali::Actor control, float relativeSize, Demo::Alignment::Type alignment, const Demo::Alignment::Padding& padding);

  /**
   * @copydoc Dali::Demo::ToolBar::RemoveControl()
   */
  void RemoveControl(Dali::Actor control);

private: // From Control
  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  void OnInitialize() override;

  /**
   * Adds a control using some default values (the control uses 10% of the tool bar space and is placed on the left group).
   * @param child The control to be added.
   *
   * @see Control::OnChildAdd()
   */
  void OnChildAdd(Actor& child) override;

private:
  /**
   */
  class Lock
  {
  public:
    /**
     * Constructor, sets the lock boolean
     */
    Lock(bool& lock)
    : mLock(lock)
    {
      mLock = true;
    }

    /**
     * Destructor, releases lock boolean
     */
    ~Lock()
    {
      mLock = false;
    }

  private:
    bool& mLock;
  };

  /**
   * Constructor.
   * It initializes ToolBar members.
   */
  ToolBar();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ToolBar();

private:
  Toolkit::TableView mLayout;                   ///< TableView used to place controls.
  unsigned int       mLeftOffset;               ///< Offset index where the next control is going to be added in the left group.
  unsigned int       mCenterBase;               ///< Base index where the first control of the center group is placed.
  unsigned int       mCenterOffset;             ///< Offset index where the next control is going to be added in the center group.
  unsigned int       mRightBase;                ///< Base index where the first control of the right group is placed.
  unsigned int       mRightOffset;              ///< Offset index where the next control is going to be added in the right group.
  float              mLeftRelativeSpace;        ///< Relative space between left and center groups of controls.
  float              mRightRelativeSpace;       ///< Relative space between center and right groups of controls.
  float              mAccumulatedRelativeSpace; ///< Stores the total percentage space used by controls.
  bool               mInitializing;             ///< Allows the use of Actor's API to add controls.

  std::map<Actor /*control*/, Demo::Alignment> mControls; ///< Stores a relationship between controls and their alignments used to place them inside the table view.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Demo::Internal::ToolBar& GetImpl(Demo::ToolBar& toolBar)
{
  DALI_ASSERT_ALWAYS(toolBar);

  Dali::RefObject& handle = toolBar.GetImplementation();

  return static_cast<Demo::Internal::ToolBar&>(handle);
}

inline const Demo::Internal::ToolBar& GetImpl(const Demo::ToolBar& toolBar)
{
  DALI_ASSERT_ALWAYS(toolBar);

  const Dali::RefObject& handle = toolBar.GetImplementation();

  return static_cast<const Demo::Internal::ToolBar&>(handle);
}

} // namespace Dali::Demo

#endif // DALI_DEMO_CONTROLS_INTERNAL_TOOL_BAR_H
