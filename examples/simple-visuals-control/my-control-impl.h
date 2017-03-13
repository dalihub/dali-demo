#ifndef DALI_DEMO_INTERNAL_MY_CONTROL_IMPL_H
#define DALI_DEMO_INTERNAL_MY_CONTROL_IMPL_H

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
 */

// CLASS HEADER
#include "my-control.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>

namespace Demo
{

namespace Internal // To use TypeRegistry, handle and body classes need the same name
{

/**
 * @brief A Simple Control to show use of visuals with a style sheet and changing visuals with a state change
 */

class MyControl : public Dali::Toolkit::Internal::Control
{
public:
  /**
   * @brief Instantiate a new ContentView object
   */
  static Demo::MyControl New();

  /**
   * @brief Default constructor
   */
  MyControl();

public:  // Properties
  /**
   * @brief Called when a property of an object of this type is set.
   *
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value );

  /**
   * @brief Called to retrieve a property of an object of this type.
   *
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Dali::Property::Value GetProperty( Dali::BaseObject* object, Dali::Property::Index propertyIndex );

private: // From Control
  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

private:
  /**
   *  undefined constructor and operator=
   */
  MyControl( const MyControl& );
  MyControl& operator=( const MyControl& );

private:
};

} // Internal

inline Internal::MyControl& GetImpl( Demo::MyControl& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  Dali::RefObject& object = handle.GetImplementation();
  return static_cast<Internal::MyControl&>(object);
}

inline const Internal::MyControl& GetImpl( const Demo::MyControl& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  const Dali::RefObject& object = handle.GetImplementation();
  return static_cast<const Internal::MyControl&>(object);
}

} // Demo

#endif //  DALI_DEMO_INTERNAL_MY_CONTROL_IMPL_H
