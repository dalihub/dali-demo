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

#include "radial-sweep-view.h"
#include "radial-sweep-view-impl.h"

using namespace Dali;

RadialSweepView::RadialSweepView()
{
}

RadialSweepView::RadialSweepView(const RadialSweepView& handle)
: Control(handle)
{
}

RadialSweepView& RadialSweepView::operator=(const RadialSweepView& rhs)
{
  if( &rhs != this )
  {
    Control::operator=(rhs);
  }
  return *this;
}

RadialSweepView::~RadialSweepView()
{
}

RadialSweepView RadialSweepView::DownCast( BaseHandle handle )
{
  return Control::DownCast<RadialSweepView, RadialSweepViewImpl>(handle);
}

RadialSweepView RadialSweepView::New( )
{
  return RadialSweepViewImpl::New();
}

RadialSweepView RadialSweepView::New( float duration,
                                      float diameter,
                                      Radian initialAngle,
                                      Radian finalAngle,
                                      Radian initialSector,
                                      Radian finalSector )
{
  return RadialSweepViewImpl::New(duration, diameter, initialAngle, finalAngle, initialSector, finalSector );
}

RadialSweepView::RadialSweepView( RadialSweepViewImpl& impl )
: Control( impl )
{
}

RadialSweepView::RadialSweepView( Dali::Internal::CustomActor* impl )
: Control( impl )
{
  VerifyCustomActorPointer<RadialSweepViewImpl>(impl);
}

void RadialSweepView::SetDuration(float duration)
{
  GetImpl(*this).SetDuration(duration);
}

void RadialSweepView::SetEasingFunction( Dali::AlphaFunction easingFunction )
{
  GetImpl(*this).SetEasingFunction( easingFunction );
}

void RadialSweepView::SetDiameter(float diameter)
{
  GetImpl(*this).SetDiameter(diameter);
}

void RadialSweepView::SetInitialAngle( Dali::Radian initialAngle)
{
  GetImpl(*this).SetInitialAngle(initialAngle);
}

void RadialSweepView::SetFinalAngle( Dali::Radian finalAngle)
{
  GetImpl(*this).SetFinalAngle(finalAngle);
}

void RadialSweepView::SetInitialSector( Dali::Radian initialSector)
{
  GetImpl(*this).SetInitialSector(initialSector);
}

void RadialSweepView::SetFinalSector( Dali::Radian finalSector)
{
  GetImpl(*this).SetFinalSector(finalSector);
}

void RadialSweepView::SetInitialActorAngle( Dali::Radian initialAngle )
{
  GetImpl(*this).SetInitialActorAngle(initialAngle);
}

void RadialSweepView::SetFinalActorAngle( Dali::Radian finalAngle )
{
  GetImpl(*this).SetFinalActorAngle(finalAngle);
}

float RadialSweepView::GetDuration( )
{
  return GetImpl(*this).GetDuration();
}

float RadialSweepView::GetDiameter( )
{
  return GetImpl(*this).GetDiameter();
}

Dali::Radian RadialSweepView::GetInitialAngle( )
{
  return GetImpl(*this).GetInitialAngle();
}

Dali::Radian RadialSweepView::GetFinalAngle( )
{
  return GetImpl(*this).GetFinalAngle();
}

Dali::Radian RadialSweepView::GetInitialSector( )
{
  return GetImpl(*this).GetInitialSector();
}

Dali::Radian RadialSweepView::GetFinalSector( )
{
  return GetImpl(*this).GetFinalSector();
}

Dali::Radian RadialSweepView::GetInitialActorAngle( )
{
  return GetImpl(*this).GetInitialActorAngle();
}

Dali::Radian RadialSweepView::GetFinalActorAngle( )
{
  return GetImpl(*this).GetFinalActorAngle();
}

void RadialSweepView::RotateActorsWithStencil(bool rotate)
{
  GetImpl(*this).RotateActorsWithStencil(rotate);
}

void RadialSweepView::Add(Actor actor)
{
  GetImpl(*this).Add(actor);
}

void RadialSweepView::Activate()
{
  GetImpl(*this).Activate();
}

void RadialSweepView::Activate( Dali::Animation anim, float offsetTime, float duration )
{
  GetImpl(*this).Activate(anim, offsetTime, duration);
}

void RadialSweepView::Deactivate()
{
  GetImpl(*this).Deactivate();
}
