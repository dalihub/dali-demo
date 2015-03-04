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
#include "edit-layout.h"

// INTERNAL INCLUDES
#include "edit-layout-impl.h"

namespace Dali
{

namespace Toolkit
{

EditLayout EditLayout::New()
{
  return Internal::EditLayout::New();
}

void EditLayout::SetTopPanel( Control panel )
{
  GetImpl( *this ).SetTopPanel( panel );
}

EditLayout::EditLayout()
{
}

EditLayout::EditLayout( const EditLayout& handle )
: Control( handle )
{
}

EditLayout& EditLayout::operator=( const EditLayout& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

EditLayout EditLayout::DownCast( BaseHandle handle )
{
  return Control::DownCast<EditLayout, Internal::EditLayout>( handle );
}

EditLayout::~EditLayout()
{
}

EditLayout::EditLayout( Internal::EditLayout& internal )
: Control( internal )
{
}

EditLayout::EditLayout( Dali::Internal::CustomActor* internal )
: Control( internal )
{
}

Internal::EditLayout& EditLayout::GetImpl( EditLayout& verticalLayout )
{
  DALI_ASSERT_ALWAYS( verticalLayout );

  Dali::RefObject& handle = verticalLayout.GetImplementation();

  return static_cast<Internal::EditLayout&>(handle);
}

} // namespace Toolkit

} // namespace Dali
