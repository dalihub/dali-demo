#ifndef DALI_DEMO_ABSOLUTE_EXAMPLE_H
#define DALI_DEMO_ABSOLUTE_EXAMPLE_H

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
 *
 */

#include <string>
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include "example.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace Demo
{

/**
 * @file absolute-example.hcpp
 * @brief Example of a Linear Layout with padding applied, enables updating of padding values for
 * one of the children.
 */
class AbsoluteExample: public ConnectionTracker, public Example
{
public:
  static const unsigned int NUMBER_OF_IMAGE_VIEWS = 4;

  AbsoluteExample();

  // Creates a Absolute Layout Example and displays it.
  virtual void Create() override;

  // Remove and destroy this layout
  virtual void Remove() override;

private:


  // Callback when change size button is pressed
  bool ChangeSizeClicked( Toolkit::Button button );

private:

  Toolkit::Control           mRootLayoutControl;
  Toolkit::Control           mAbsoluteLayoutContainer;
  Toolkit::ImageView         mImageViews[ NUMBER_OF_IMAGE_VIEWS ];
  bool                       mLayoutSizeToggleStatus;
  Toolkit::PushButton        mToggleButton;
};

} // namespace Demo

#endif // DALI_DEMO_ABSOLUTE_EXAMPLE_H
