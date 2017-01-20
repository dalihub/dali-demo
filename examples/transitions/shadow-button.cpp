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

#include "shadow-button.h"
#include "shadow-button-impl.h"

namespace Demo
{

ShadowButton::ShadowButton()
{
}

ShadowButton::ShadowButton( const ShadowButton& control )
: Control( control )
{
}

ShadowButton& ShadowButton::operator= ( const ShadowButton& rhs )
{
  if( &rhs != this )
  {
    Control::operator=( rhs );
  }
  return *this;
}

ShadowButton::~ShadowButton()
{
}

ShadowButton ShadowButton::New()
{
  ShadowButton control = Internal::ShadowButton::New();
  return control;
}

ShadowButton ShadowButton::New( const std::string& url )
{
  ShadowButton control = Internal::ShadowButton::New();
  return control;
}

ShadowButton ShadowButton::DownCast( BaseHandle handle )
{
  return Control::DownCast< ShadowButton, Internal::ShadowButton > ( handle );
}

void ShadowButton::SetActiveState( bool active )
{
  GetImpl(*this).SetActiveState( active );
}

bool ShadowButton::GetActiveState()
{
  return GetImpl(*this).GetActiveState();
}

void ShadowButton::SetCheckState( bool checkState )
{
  GetImpl(*this).SetCheckState( checkState );
}

bool ShadowButton::GetCheckState()
{
  return GetImpl(*this).GetCheckState();
}

ShadowButton::ShadowButton( Internal::ShadowButton& implementation )
: Control( implementation )
{
}

ShadowButton::ShadowButton( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer< Internal::ShadowButton >( internal ) ;
}


} //namespace Demo
