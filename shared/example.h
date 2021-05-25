#ifndef DALI_DEMO_EXAMPLE_H
#define DALI_DEMO_EXAMPLE_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <string>
#include <vector>

/**
 * Example information
 *
 * Represents a single Example.
 */
struct Example
{
  // Constructors

  /**
   * @param[in] uniqueName unique name of example
   * @param[in] captionTitle The caption for the example to appear on a tile button.
   */
  Example(std::string uniqueName, std::string captionTitle)
  : name(uniqueName),
    title(captionTitle)
  {
  }

  ~Example() = default;

  // Data

  std::string name;  ///< unique name of example
  std::string title; ///< title (caption) of example to appear on tile button.
};

using ExampleList          = std::vector<Example>;
using ExampleListConstIter = ExampleList::const_iterator;

#endif // DALI_DEMO_SHARED_EXAMPLE_H
