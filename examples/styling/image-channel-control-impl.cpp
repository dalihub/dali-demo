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

#include "image-channel-control-impl.h"
#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>

using namespace Dali; // Needed for macros

namespace Demo
{
namespace Internal
{

namespace
{

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform mediump vec4 uColor;\n
  uniform mediump vec3 uChannels;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor * vec4(uChannels, 1.0) ;\n
  }\n
);

Dali::BaseHandle Create()
{
  return Demo::ImageChannelControl::New();
}

DALI_TYPE_REGISTRATION_BEGIN( ImageChannelControl, Dali::Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Demo, ImageChannelControl, "url", STRING, RESOURCE_URL );
DALI_PROPERTY_REGISTRATION( Demo, ImageChannelControl, "redChannel", FLOAT, RED_CHANNEL );
DALI_PROPERTY_REGISTRATION( Demo, ImageChannelControl, "greenChannel", FLOAT, GREEN_CHANNEL );
DALI_PROPERTY_REGISTRATION( Demo, ImageChannelControl, "blueChannel", FLOAT, BLUE_CHANNEL );

DALI_TYPE_REGISTRATION_END();

} // anonymous namespace


Internal::ImageChannelControl::ImageChannelControl()
: Control( ControlBehaviour( REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mChannels( 1.0f, 1.0f, 1.0f )
{
}

Internal::ImageChannelControl::~ImageChannelControl()
{
}

Demo::ImageChannelControl Internal::ImageChannelControl::New()
{
  IntrusivePtr<Internal::ImageChannelControl> impl = new Internal::ImageChannelControl();
  Demo::ImageChannelControl handle = Demo::ImageChannelControl( *impl );
  impl->Initialize();
  return handle;
}

void ImageChannelControl::SetImage( const std::string& url )
{
  mUrl = url;

  Actor self = Self();

  Property::Map properties;
  Property::Map shader;
  shader[Dali::Toolkit::Visual::Shader::Property::FRAGMENT_SHADER] = FRAGMENT_SHADER;
  properties[Dali::Toolkit::Visual::Property::TYPE] = Dali::Toolkit::Visual::IMAGE;
  properties[Dali::Toolkit::Visual::Property::SHADER]=shader;
  properties[Dali::Toolkit::ImageVisual::Property::URL] = url;

  Dali::Toolkit::InitializeVisual( self, mVisual, properties );

  RelayoutRequest();
}

void ImageChannelControl::OnInitialize()
{
  Actor self = Self();
  mChannelIndex = self.RegisterProperty( "uChannels", Vector3(1.0f, 1.0f, 1.0f) );
}

void ImageChannelControl::OnStageConnection( int depth )
{
  Control::OnStageConnection( depth );

  if( mVisual )
  {
    CustomActor self = Self();
    mVisual.SetOnStage( self );
  }
}

void ImageChannelControl::OnStageDisconnection()
{
  if( mVisual )
  {
    CustomActor self = Self();
    mVisual.SetOffStage( self );
  }

  Control::OnStageDisconnection();
}

void ImageChannelControl::OnSizeSet( const Vector3& targetSize )
{
  Control::OnSizeSet( targetSize );

  if( mVisual )
  {
    Vector2 size( targetSize );
    mVisual.SetSize( size );
  }
}

Vector3 ImageChannelControl::GetNaturalSize()
{
  if( mVisual )
  {
    Vector2 naturalSize;
    mVisual.GetNaturalSize(naturalSize);
    return Vector3(naturalSize);
  }
  return Vector3::ZERO;
}


///////////////////////////////////////////////////////////
//
// Properties
//

void ImageChannelControl::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Demo::ImageChannelControl imageChannelControl = Demo::ImageChannelControl::DownCast( Dali::BaseHandle( object ) );

  if ( imageChannelControl )
  {
    ImageChannelControl& impl = GetImpl( imageChannelControl );
    Actor self = impl.Self();
    switch ( index )
    {
      case Demo::ImageChannelControl::Property::RED_CHANNEL:
      {
        impl.mChannels[0] = value.Get<float>();
        self.SetProperty( impl.mChannelIndex, impl.mChannels );
        break;
      }
      case Demo::ImageChannelControl::Property::GREEN_CHANNEL:
      {
        impl.mChannels[1] = value.Get<float>();
        self.SetProperty( impl.mChannelIndex, impl.mChannels );
        break;
      }
      case Demo::ImageChannelControl::Property::BLUE_CHANNEL:
      {
        impl.mChannels[2] = value.Get<float>();
        self.SetProperty( impl.mChannelIndex, impl.mChannels );
        break;
      }
    }
  }
}

Property::Value ImageChannelControl::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Demo::ImageChannelControl imageChannelControl = Demo::ImageChannelControl::DownCast( Dali::BaseHandle( object ) );

  if ( imageChannelControl )
  {
    ImageChannelControl& impl = GetImpl( imageChannelControl );
    switch ( propertyIndex )
    {
      case Demo::ImageChannelControl::Property::RED_CHANNEL:
      {
        value = impl.mChannels[0];
        break;
      }
      case Demo::ImageChannelControl::Property::GREEN_CHANNEL:
      {
        value = impl.mChannels[1];
        break;
      }
      case Demo::ImageChannelControl::Property::BLUE_CHANNEL:
      {
        value = impl.mChannels[2];
        break;
      }
    }
  }

  return value;
}

} // Internal
} // Demo
