#ifndef DALI_DEMO_INTERNAL_IMAGE_CHANNEL_CONTROL_IMPL_H
#define DALI_DEMO_INTERNAL_IMAGE_CHANNEL_CONTROL_IMPL_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include "image-channel-control.h"
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>

namespace Demo
{

namespace Internal // To use TypeRegistry, handle and body classes need the same name
{

class ImageChannelControl : public Dali::Toolkit::Internal::Control
{
public:
  /**
   * Instantiate a new ImageChannelControl object
   */
  static Demo::ImageChannelControl New();
  ImageChannelControl();
  ~ImageChannelControl();

public: // API
  /**
   * @copydoc ImageChannelControl::SetImage
   */
  void SetImage( const std::string& url );

public:  // Properties
  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value );

  /**
   * Called to retrieve a property of an object of this type.
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

  /**
   * @copydoc Toolkit::Control::OnStageConnect()
   */
  virtual void OnStageConnection( int depth );

  /**
   * @copydoc Toolkit::Control::OnStageDisconnection()
   */
  virtual void OnStageDisconnection();

  /**
   * @copydoc Toolkit::Control::OnSizeSet()
   */
  virtual void OnSizeSet( const Dali::Vector3& targetSize );

  /**
   * @copydoc Toolkit::Control::GetNaturalSize
   */
  virtual Dali::Vector3 GetNaturalSize();

private:
  //undefined
  ImageChannelControl( const ImageChannelControl& );
  ImageChannelControl& operator=( const ImageChannelControl& );

private:
  // Implementation details
  std::string mUrl;
  Dali::Toolkit::Visual::Base mVisual;
  Dali::Vector3 mChannels;
  Dali::Property::Index mChannelIndex;
};

} // Internal

inline Internal::ImageChannelControl& GetImpl( Demo::ImageChannelControl& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  Dali::RefObject& object = handle.GetImplementation();
  return static_cast<Internal::ImageChannelControl&>(object);
}

inline const Internal::ImageChannelControl& GetImpl( const Demo::ImageChannelControl& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  const Dali::RefObject& object = handle.GetImplementation();
  return static_cast<const Internal::ImageChannelControl&>(object);
}

} // Demo

#endif //  DALI_DEMO_IMAGE_CHANNEL_CONTROL_IMPL_H
