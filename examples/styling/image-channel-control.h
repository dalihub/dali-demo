#ifndef DALI_DEMO_IMAGE_CHANNEL_CONTROL_H
#define DALI_DEMO_IMAGE_CHANNEL_CONTROL_H

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

#include <dali-toolkit/dali-toolkit.h>
#include <string>

namespace Demo
{

namespace Internal
{
// All type registered types need to have the same name for the body and the handle
class ImageChannelControl;
}

/**
 * Control that allows the RGB channels of an image to be altered.
 */
class ImageChannelControl : public Dali::Toolkit::Control
{
public:
  /**
   * The start and end property ranges for this control
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Dali::Toolkit::Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000,
    ANIMATABLE_PROPERTY_START_INDEX = Dali::ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,
    ANIMATABLE_PROPERTY_END_INDEX = ANIMATABLE_PROPERTY_START_INDEX+1000
  };

  struct Property
  {
    enum
    {
      RESOURCE_URL = PROPERTY_START_INDEX,
      RED_CHANNEL,
      GREEN_CHANNEL,
      BLUE_CHANNEL,
      VISIBILITY,
      ENABLE_VISIBILITY_TRANSITION,
      DISABLE_VISIBILITY_TRANSITION,
      IMAGE_VISUAL
    };
  };

public: // Construction / destruction

  /**
   * Create an uninitialized handle
   */
  ImageChannelControl();

  /**
   * Create a new image channel control without an image. Use
   * SetImage to give this control an image
   */
  static ImageChannelControl New();

  /**
   * Create a new image channel control from a given URL
   */
  static ImageChannelControl New( const std::string& url );

  /**
   * Destructor. This is non-virtual since derived Handle types must not
   * contain data or virtual methods
   */
  ~ImageChannelControl();

  /**
   * Copy Constructor
   */
  ImageChannelControl( const ImageChannelControl& imageChannelControl );

  /**
   * Assignment Operator
   */
  ImageChannelControl& operator=( const ImageChannelControl& imageChannelControl );

  /**
   * Downcast
   */
  static ImageChannelControl DownCast( BaseHandle handle );

public: // API

  /**
   * Set the image for this ImageChannelControl
   * @param[in] url The url of the image resource
   */
  void SetImage( const std::string& url );

  /**
   * Set the visibility of this control
   */
  void SetVisibility( bool visibility );

public: // Not for public use
  /**
   * Create a handle from an implementation
   */
  ImageChannelControl( Internal::ImageChannelControl& implementation );

  /**
   * Allow the creation of an ImageChannelControl handle from an internal CustomActor pointer
   */
  ImageChannelControl( Dali::Internal::CustomActor* internal );
};

} // namespace Demo

#endif // DALI_DEMO_IMAGE_CHANNEL_CONTROL_H
