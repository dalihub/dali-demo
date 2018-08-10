#ifndef DALI_DEMO_PADDING_EXAMPLE_H
#define DALI_DEMO_PADDING_EXAMPLE_H

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

#include <string>
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

#include "example.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace Demo
{

/**
 * @file padding-example.hcpp
 * @brief Example of a Linear Layout with padding applied, enables updating of padding values for
 * one of the children.
 */
class PaddingExample final: public ConnectionTracker, public Example
{
public:

  static const unsigned int NUMBER_OF_IMAGE_VIEWS = 3;

  // Constructor
  PaddingExample();

  // Create a Linear layout of ImagesViews, one with a Margin, One with padding.
  void Create() override;

  // Remove created Layout
  void Remove() override;

private:

  // Change Paddding callback
  bool ChangePaddingClicked( Toolkit::Button button );

private:

  Toolkit::Control           mHorizontalBox;
  Toolkit::ImageView         mImageViews[ NUMBER_OF_IMAGE_VIEWS ];
  bool                       mImageViewToggleStatus[ NUMBER_OF_IMAGE_VIEWS ];
  Toolkit::PushButton        mToggleButton;

};

} // namespace Demo

#endif // DALI_DEMO_PADDING_EXAMPLE_H
