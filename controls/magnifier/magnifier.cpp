/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include "magnifier.h"
#include "magnifier-impl.h"

using namespace Dali;

namespace Dali::Demo
{
///////////////////////////////////////////////////////////////////////////////////////////////////
// Magnifier
///////////////////////////////////////////////////////////////////////////////////////////////////

Magnifier::Magnifier()
{
}

Magnifier::Magnifier(const Magnifier& handle) = default;

Magnifier& Magnifier::operator=(const Magnifier& handle) = default;

Magnifier::Magnifier(Magnifier&& rhs) = default;

Magnifier& Magnifier::operator=(Magnifier&& rhs) = default;

Magnifier::Magnifier(Internal::Magnifier& implementation)
: Toolkit::Control(implementation)
{
}

Magnifier::Magnifier(Dali::Internal::CustomActor* internal)
: Toolkit::Control(internal)
{
  VerifyCustomActorPointer<Internal::Magnifier>(internal);
}

Magnifier Magnifier::New()
{
  return Internal::Magnifier::New();
}

Magnifier::~Magnifier()
{
}

Magnifier Magnifier::DownCast(BaseHandle handle)
{
  return Toolkit::Control::DownCast<Magnifier, Internal::Magnifier>(handle);
}

void Magnifier::SetSourceActor(Actor actor)
{
  GetImpl(*this).SetSourceActor(actor);
}

} // namespace Dali::Demo
