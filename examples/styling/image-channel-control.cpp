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
#include "image-channel-control-impl.h"

namespace Demo
{

ImageChannelControl::ImageChannelControl()
{
}

ImageChannelControl::ImageChannelControl( const ImageChannelControl& imageChannelControl )
: Control( imageChannelControl )
{
}

ImageChannelControl& ImageChannelControl::operator= ( const ImageChannelControl& rhs )
{
  if( &rhs != this )
  {
    Control::operator=( rhs );
  }
  return *this;
}

ImageChannelControl::~ImageChannelControl()
{
}

ImageChannelControl ImageChannelControl::New()
{
  ImageChannelControl imageChannelControl = Internal::ImageChannelControl::New();
  return imageChannelControl;
}

ImageChannelControl ImageChannelControl::New( const std::string& url )
{
  ImageChannelControl imageChannelControl = Internal::ImageChannelControl::New();
  imageChannelControl.SetImage( url );
  return imageChannelControl;
}

ImageChannelControl ImageChannelControl::DownCast( BaseHandle handle )
{
  return Control::DownCast< ImageChannelControl, Internal::ImageChannelControl > ( handle );
}

void ImageChannelControl::SetImage( const std::string& url )
{
  GetImpl( *this ).SetImage( url );
}

void ImageChannelControl::SetVisibility( bool visibility )
{
  GetImpl( *this ).SetVisibility( visibility );
}

ImageChannelControl::ImageChannelControl( Internal::ImageChannelControl& implementation )
: Control( implementation )
{
}

ImageChannelControl::ImageChannelControl( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer< Internal::ImageChannelControl >( internal ) ;
}


} //namespace Demo
