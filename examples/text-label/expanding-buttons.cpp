/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include "expanding-buttons-impl.h"
#include "expanding-buttons.h"

namespace Demo
{

ExpandingButtons::ExpandingButtons()
{
}

ExpandingButtons::ExpandingButtons( const ExpandingButtons& expandingButtons )
: Control( expandingButtons )
{
}

ExpandingButtons& ExpandingButtons::operator= ( const ExpandingButtons& rhs )
{
  if( &rhs != this )
  {
    Control::operator=( rhs );
  }
  return *this;
}

ExpandingButtons::~ExpandingButtons()
{
}

ExpandingButtons ExpandingButtons::New()
{
  ExpandingButtons expandingButtons = Internal::ExpandingButtons::New();
  return expandingButtons;
}

ExpandingButtons ExpandingButtons::DownCast( BaseHandle handle )
{
  return Control::DownCast< ExpandingButtons, Internal::ExpandingButtons > ( handle );
}

void ExpandingButtons::RegisterButton( Control& control )
{
  GetImpl(*this).RegisterButton( control );
}

void ExpandingButtons::Expand()
{
  GetImpl(*this).Expand();
}
void ExpandingButtons::Collapse()
{
  GetImpl(*this).Collapse();
}

Demo::ExpandingButtons::ExpandingButtonsSignalType& ExpandingButtons::CollapsingSignal()
{
  return GetImpl(*this).CollapsingSignal();
}

ExpandingButtons::ExpandingButtons( Internal::ExpandingButtons& implementation )
: Control( implementation )
{
}

ExpandingButtons::ExpandingButtons( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer< Internal::ExpandingButtons >( internal ) ;
}


} //namespace Demo
