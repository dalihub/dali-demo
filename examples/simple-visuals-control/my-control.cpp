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

// INTERNAL INCLUDES
#include "my-control-impl.h"

namespace Demo
{

MyControl::MyControl()
{
}

MyControl::MyControl( const MyControl& control )
: Control( control )
{
}

MyControl& MyControl::operator= ( const MyControl& rhs )
{
  if( &rhs != this )
  {
    Control::operator=( rhs );
  }
  return *this;
}

MyControl::~MyControl()
{
}

MyControl MyControl::New()
{
  MyControl control = Internal::MyControl::New();
  return control;
}

MyControl MyControl::DownCast( BaseHandle handle )
{
  return Control::DownCast< MyControl, Internal::MyControl > ( handle );
}

MyControl::MyControl( Internal::MyControl& implementation )
: Control( implementation )
{
}

MyControl::MyControl( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer< Internal::MyControl >( internal ) ;
}


} //namespace Demo
