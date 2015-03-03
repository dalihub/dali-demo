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
#include "center-layout.h"

// INTERNAL INCLUDES
#include "center-layout-impl.h"

namespace Dali
{

namespace Toolkit
{

CenterLayout CenterLayout::New()
{
  return Internal::CenterLayout::New();
}

CenterLayout::CenterLayout()
{
}

CenterLayout::CenterLayout( const CenterLayout& handle )
: Control( handle )
{
}

CenterLayout& CenterLayout::operator=( const CenterLayout& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

CenterLayout CenterLayout::DownCast( BaseHandle handle )
{
  return Control::DownCast<CenterLayout, Internal::CenterLayout>( handle );
}

CenterLayout::~CenterLayout()
{
}

CenterLayout::CenterLayout( Internal::CenterLayout& internal )
: Control( internal )
{
}

CenterLayout::CenterLayout( Dali::Internal::CustomActor* internal )
: Control( internal )
{
}

Internal::CenterLayout& CenterLayout::GetImpl( CenterLayout& verticalLayout )
{
  DALI_ASSERT_ALWAYS( verticalLayout );

  Dali::RefObject& handle = verticalLayout.GetImplementation();

  return static_cast<Internal::CenterLayout&>(handle);
}

} // namespace Toolkit

} // namespace Dali
