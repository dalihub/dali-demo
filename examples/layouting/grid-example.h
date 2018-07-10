#ifndef DALI_DEMO_GRID_EXAMPLE_H
#define DALI_DEMO_GRID_EXAMPLE_H

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
 * @file grid-example.hcpp
 * @brief Example of a Grid Layout
 */
class GridExample final: public ConnectionTracker, public Example
{
public:

  static const unsigned int MAX_NUMBER_OF_IMAGE_VIEWS = 9;
  static const unsigned int INITIAL_NUMBER_OF_IMAGE_VIEWS = 5;

  // Create a Grid layout of ImagesViews
  void Create() override;

  // Remove created Layout
  void Remove() override;

private:

  // Callback for button pressed
  bool ToggleButtonClicked( Toolkit::Button button );

private:

  Toolkit::Control                mRootLayoutControl;
  Toolkit::Control                mGridContainer;
  std::vector<Toolkit::ImageView> mImageViews;
  Toolkit::PushButton             mToggleButton;
  unsigned int                    mToggleStatus;
};

} // namespace Demo

#endif // DALI_DEMO_GRID_EXAMPLE_H
