#ifndef DALI_DEMO_ANIMATION_EXAMPLE_H
#define DALI_DEMO_ANIMATION_EXAMPLE_H

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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/layouting/linear-layout.h>
#include <dali-toolkit/devel-api/layouting/grid.h>

#include "example.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace Demo
{

/**
 * @file animation-example.hcpp
 * @brief Example of a layout complex animation.
 */
class AnimationExample final: public ConnectionTracker, public Example
{
public:
  AnimationExample();

  // Creates a Animation Layout Example and displays it.
  virtual void Create() override;

  // Remove and destroy this layout
  virtual void Remove() override;

private:

  bool OnRemoveClicked( Button button );

  bool OnSelectGridClicked( Button button );

  bool OnAddClicked( Button button );

  bool OnChangeClicked( Button button );

private:
  PushButton mRemoveButton;
  PushButton mAddButton;
  PushButton mSelectGridButton;
  PushButton mShakeButton;
  Control mAnimationContainer;
  Grid mGridLayout;
  LinearLayout mHorizontalLayout;
  bool mGridSet;
}; // class AnimationContainer

} // namespace Demo

#endif //DALI_DEMO_ANIMATION_CONTAINER_H
