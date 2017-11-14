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

#include "my-control-impl.h"

// EXTERNAL INCLUDES

#include <dali/devel-api/scripting/enum-helper.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace Demo
{
namespace Internal
{

namespace
{


Dali::BaseHandle Create()
{
  return Demo::MyControl::New();
}

// Required code for Property set up.

DALI_TYPE_REGISTRATION_BEGIN( MyControl, Dali::Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Demo, MyControl, "iconVisual", MAP, ICON_VISUAL )

DALI_TYPE_REGISTRATION_END();

// Add an enum to string conversion entry for the control's visuals.  In this case just the icon visual.
// Enables Setting of the property using enums or strings.
DALI_ENUM_TO_STRING_TABLE_BEGIN( VISUAL_PROPERTIES )
{ "iconVisual", Demo::MyControl::Property::ICON_VISUAL },
DALI_ENUM_TO_STRING_TABLE_END( VISUAL_PROPERTIES )


} // anonymous namespace


Internal::MyControl::MyControl()
: Control( ControlBehaviour( REQUIRES_STYLE_CHANGE_SIGNALS ) )
{
}

Demo::MyControl Internal::MyControl::New()
{
  IntrusivePtr<Internal::MyControl> impl = new Internal::MyControl();
  Demo::MyControl handle = Demo::MyControl( *impl );
  impl->Initialize();
  return handle;
}

void MyControl::OnInitialize()
{
  Dali::Actor self = Self();
  self.SetKeyboardFocusable( true );
}

void MyControl::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Demo::MyControl myControl = Demo::MyControl::DownCast( Dali::BaseHandle( object ) );

  if( myControl )
  {
    MyControl& impl = GetImpl( myControl );
    switch ( index )
    {
      case Demo::MyControl::Property::ICON_VISUAL:
      {
        Toolkit::Visual::Base iconVisual;
        Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
        Property::Map *map = value.GetMap();
        if( map && !map->Empty() )
        {
          iconVisual = visualFactory.CreateVisual( *map );
        }

        if ( iconVisual )
        {
          DevelControl::RegisterVisual( impl, index, iconVisual );
        }
        break;
      }
    }
  }
}

Property::Value MyControl::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Demo::MyControl myControl = Demo::MyControl::DownCast( Dali::BaseHandle( object ) );

  if ( myControl )
  {
    switch ( propertyIndex )
    {
      case Demo::MyControl::Property::ICON_VISUAL:
      {
        Property::Map map;
        MyControl& impl = GetImpl( myControl );
        Toolkit::Visual::Base visual =  DevelControl::GetVisual( impl, propertyIndex );
        if ( visual )
        {
          visual.CreatePropertyMap( map ); // Creates a Property map containing the Visual that ICON_VISUAL currently is. Can change if state changes.
          value = map;
        }
        break;
      }
      default:
        break;
    }
  }

  return value;
}

} // Internal
} // Demo
