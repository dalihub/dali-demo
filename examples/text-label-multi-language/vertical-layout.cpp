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
#include "vertical-layout.h"

// INTERNAL INCLUDES
#include "vertical-layout-impl.h"

namespace Dali
{

namespace Toolkit
{

VerticalLayout VerticalLayout::New()
{
  return Internal::VerticalLayout::New();
}

VerticalLayout::VerticalLayout()
{
}

VerticalLayout::VerticalLayout( const VerticalLayout& handle )
: Control( handle )
{
}

VerticalLayout& VerticalLayout::operator=( const VerticalLayout& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

VerticalLayout VerticalLayout::DownCast( BaseHandle handle )
{
  return Control::DownCast<VerticalLayout, Internal::VerticalLayout>( handle );
}

VerticalLayout::~VerticalLayout()
{
}

VerticalLayout::VerticalLayout( Internal::VerticalLayout& internal )
: Control( internal )
{
}

VerticalLayout::VerticalLayout( Dali::Internal::CustomActor* internal )
: Control( internal )
{
}

void VerticalLayout::AddLabel( TextLabel label )
{
  GetImpl( *this ).AddLabel( label );
}

Internal::VerticalLayout& VerticalLayout::GetImpl( VerticalLayout& verticalLayout )
{
  DALI_ASSERT_ALWAYS( verticalLayout );

  Dali::RefObject& handle = verticalLayout.GetImplementation();

  return static_cast<Internal::VerticalLayout&>(handle);
}

} // namespace Toolkit

} // namespace Dali
