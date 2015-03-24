/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include "vertical-layout-impl.h"

// INTERNAL INCLUDES
#include "vertical-layout.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

Toolkit::VerticalLayout VerticalLayout::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< VerticalLayout > impl = new VerticalLayout();

  // Pass ownership to CustomActor handle
  Toolkit::VerticalLayout handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

VerticalLayout::VerticalLayout()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_NONE ) )
{
}

VerticalLayout::~VerticalLayout()
{
}

void VerticalLayout::OnInitialize()
{
}

Vector3 VerticalLayout::GetNaturalSize()
{
  Vector3 size = Vector3::ZERO;

  CustomActor self = Self();
  for( unsigned int index = 0u, count = self.GetChildCount(); index < count; ++index )
  {
    Toolkit::TextLabel label = Toolkit::TextLabel::DownCast( self.GetChildAt( index ) );

    if( label )
    {
      Vector3 labelSize = label.GetNaturalSize();

      size.width = ( labelSize.width > size.width ) ? labelSize.width : size.width;
      size.height += labelSize.height;
    }
  }

  return size;
}

float VerticalLayout::GetHeightForWidth( float width )
{
  float height = 0.f;

  CustomActor self = Self();
  for( unsigned int index = 0u, count = self.GetChildCount(); index < count; ++index )
  {
    Toolkit::TextLabel label = Toolkit::TextLabel::DownCast( self.GetChildAt( index ) );

    if( label )
    {
      height += label.GetHeightForWidth( width );
    }
  }

  return height;
}

float VerticalLayout::GetWidthForHeight( float height )
{
  return 0.f;
}

void VerticalLayout::OnFontChange( bool defaultFontChange, bool defaultFontSizeChange )
{
}

void VerticalLayout::OnRelayout( const Vector2& size, ActorSizeContainer& container )
{
  CustomActor self = Self();

  Vector3 position;
  for( unsigned int index = 0u, count = self.GetChildCount(); index < count; ++index )
  {
    Size childSize = size;
    Toolkit::TextLabel label = Toolkit::TextLabel::DownCast( self.GetChildAt( index ) );

    if( label )
    {
      label.SetPosition( position );

      childSize.height = label.GetHeightForWidth( size.width );
      position.height += childSize.height;

      label.SetSize( childSize );
    }

    container.push_back( ActorSizePair( label, childSize ) );
  }
}

void VerticalLayout::AddLabel( Toolkit::TextLabel label )
{
  Self().Add( label );

  RelayoutRequest();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
