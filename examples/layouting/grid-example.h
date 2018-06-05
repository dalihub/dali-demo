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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace Demo
{

/**
 * @file grid-example.pp
 * @brief Example of a Grid Layout 
 */
class GridExample: public ConnectionTracker
{

public:

  // Creates a the example and displays it.
  void Create();

  // Remove and destroy this layout
  void Remove();

private:

  Control mContainer;

}; // class GridExample

} // namespace Demo

#endif //DALI_DEMO_GRID_EXAMPLE_H