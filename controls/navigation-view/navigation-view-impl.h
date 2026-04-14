#ifndef DALI_DEMO_CONTROLS_INTERNAL_NAVIGATION_CONTROL_H
#define DALI_DEMO_CONTROLS_INTERNAL_NAVIGATION_CONTROL_H

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
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/devel-api/common/vector-wrapper.h>
#include <dali/public-api/actors/layer.h>

#include <list>

// INTERNAL INCLUDES
#include <controls/navigation-view/navigation-view.h>

namespace Dali::Demo
{
class NavigationView;

namespace Internal
{
class NavigationBar;

/**
 * @brief
 *
 * NavigationView implements a controller than manages the navigation of hierarchical contents.
 */

class NavigationView : public Toolkit::ControlImpl
{
public:
  /**
   * Create an initialized NavigationView.
   * @return A handle to a newly allocated Dali resource
   */
  static Demo::NavigationView New();

  /**
   * @copydoc Dali::Demo::NavigationView::Push()
   */
  void Push(Actor& actor);

  /**
   * @copydoc Dali::Demo::NavigationView::Pop()
   */
  Actor Pop();

private: // override functions from Control
  /**
   * @copydoc Toolkit::ControlImpl::OnInitialize
   */
  virtual void OnInitialize() override;

  /**
   * @copydoc Toolkit::ControlImpl::OnSceneConnection( int depth  )
   */
  void OnSceneConnection(int depth) override;

protected:
  /**
   * Constructor.
   * It initializes the NavigationView members
   */
  NavigationView();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~NavigationView();

private:
  // Undefined
  NavigationView(const NavigationView&);

  // Undefined
  NavigationView& operator=(const NavigationView& rhs);

private:
  std::vector<Actor> mContentStack;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Demo::Internal::NavigationView& GetImpl(Demo::NavigationView& navigationView)
{
  DALI_ASSERT_ALWAYS(navigationView);

  Dali::RefObject& handle = navigationView.GetImplementation();

  return static_cast<Demo::Internal::NavigationView&>(handle);
}

inline const Demo::Internal::NavigationView& GetImpl(const Demo::NavigationView& navigationView)
{
  DALI_ASSERT_ALWAYS(navigationView);

  const Dali::RefObject& handle = navigationView.GetImplementation();

  return static_cast<const Demo::Internal::NavigationView&>(handle);
}

} // namespace Dali::Demo

#endif // DALI_DEMO_CONTROLS_INTERNAL_NAVIGATION_CONTROL_H
