#ifndef DALI_DEMO_LAYOUTING_EXAMPLE_H
#define DALI_DEMO_LAYOUTING_EXAMPLE_H

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

// EXTERNAL INCLUDES
#include <string>

namespace Demo
{

/**
 * @brief Abstract base class for layouting examples.
 */
class Example
{
public:
  /// Should be overridden by deriving classes to create the required Layouting example
  virtual void Create() = 0;

  /// Should be overridden by deriving classes to remove their layouting example from stage
  virtual void Remove() = 0;

  /// Virtual destructor
  virtual ~Example() = default;

  /**
   * Gets the title set for this example
   * @return title
   */
  const std::string& GetExampleTitle() const
  {
    return mTitle;
  }

protected:
  /**
   * Constructor for Example
   * @param[in] title Title to be used for the example
   */
  Example( const std::string& title )
  : mTitle( title )
  {
  }

private:
  const std::string mTitle;
};

} // namespace Demo

#endif // DALI_DEMO_LAYOUTING_EXAMPLE_H
