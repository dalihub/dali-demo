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

#include "beat-control.h"
#include "beat-control-impl.h"

namespace Demo
{

BeatControl::BeatControl()
{
}

BeatControl::BeatControl( const BeatControl& beatControl )
: Control( beatControl )
{
}

BeatControl& BeatControl::operator= ( const BeatControl& rhs )
{
  if( &rhs != this )
  {
    Control::operator=( rhs );
  }
  return *this;
}

BeatControl::~BeatControl()
{
}

BeatControl BeatControl::New()
{
  BeatControl beatControl = Internal::BeatControl::New();
  return beatControl;
}

BeatControl BeatControl::New( const std::string& url )
{
  BeatControl beatControl = Internal::BeatControl::New();
  return beatControl;
}

BeatControl BeatControl::DownCast( BaseHandle handle )
{
  return Control::DownCast< BeatControl, Internal::BeatControl > ( handle );
}

void BeatControl::StartBounceAnimation()
{
  GetImpl(*this).StartBounceAnimation();
}

void BeatControl::StartXAnimation()
{
  GetImpl(*this).StartXAnimation();
}
void BeatControl::StartYAnimation()
{
  GetImpl(*this).StartYAnimation();
}
void BeatControl::StartFadeAnimation()
{
  GetImpl(*this).StartFadeAnimation();
}

BeatControl::BeatControl( Internal::BeatControl& implementation )
: Control( implementation )
{
}

BeatControl::BeatControl( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer< Internal::BeatControl >( internal ) ;
}


} //namespace Demo
