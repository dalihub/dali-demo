#ifndef DALI_DEMO_LINEAR_CONTAINER_H
#define DALI_DEMO_LINEAR_CONTAINER_H

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
#include "linear-container.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  const char* IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "application-icon-101.png",
  DEMO_IMAGE_DIR "application-icon-102.png",
  DEMO_IMAGE_DIR "application-icon-103.png",
  DEMO_IMAGE_DIR "application-icon-104.png"
};

}

namespace Demo
{

class LinearContainer: public ConnectionTracker
{
static const unsigned int NUMBER_OF_RESOURCES = sizeof(IMAGE_PATH) / sizeof(char*);

public:
 
  // Creates a Linear Layout Example and displays it.
  void Create();

  // Remove and destroy this layout
  void Remove();

private:
  
  // Changes the direction of the items.
  bool OnDirectionClicked( Button button );

  // Alternates the linear layout from horizontal to vertical
  bool OnRotateClicked( Button button );

private:
  Toolkit::ImageView mImageViews[ NUMBER_OF_RESOURCES ];
  PushButton mDirectionButton;
  PushButton mRotateButton;
  Control mLinearContainer;
  bool mDirection;
  bool mIsHorizontal = true;
}; // class LinearContainer

} // namespace Demo

#endif //DALI_DEMO_LINEAR_CONTAINER_H