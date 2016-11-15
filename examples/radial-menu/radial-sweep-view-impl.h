#ifndef DALI_DEMO_RADIAL_SWEEP_VIEW_IMPL_H
#define DALI_DEMO_RADIAL_SWEEP_VIEW_IMPL_H

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

#include <dali-toolkit/dali-toolkit.h>
#include "radial-sweep-view.h"


/********************************************************************************
 * Class to implement a layer with a radial sweep stencil mask and an actor tree
 */
class RadialSweepViewImpl : public Dali::Toolkit::Internal::Control
{
public:
  static RadialSweepView New();

  static RadialSweepView New( float duration,
                              float diameter,
                              Dali::Radian initialAngle,
                              Dali::Radian finalAngle,
                              Dali::Radian initialSector,
                              Dali::Radian finalSector );

  RadialSweepViewImpl( float duration,
                       float diameter,
                       Dali::Radian initialAngle,
                       Dali::Radian finalAngle,
                       Dali::Radian initialSector,
                       Dali::Radian finalSector );

  void SetDuration(float duration);
  void SetEasingFunction( Dali::AlphaFunction easingFunction );

  void SetDiameter(float diameter);
  void SetInitialAngle( Dali::Radian initialAngle);
  void SetFinalAngle( Dali::Radian finalAngle);
  void SetInitialSector( Dali::Radian initialSector);
  void SetFinalSector( Dali::Radian finalSector);
  void SetInitialActorAngle( Dali::Radian initialAngle );
  void SetFinalActorAngle( Dali::Radian finalAngle );

  float GetDuration( );
  float GetDiameter( );
  Dali::Radian GetInitialAngle( );
  Dali::Radian GetFinalAngle( );
  Dali::Radian GetInitialSector( );
  Dali::Radian GetFinalSector( );
  Dali::Radian GetInitialActorAngle( );
  Dali::Radian GetFinalActorAngle( );

  void RotateActorsWithStencil(bool rotate);

  void Add( Dali::Actor actor );

  void Activate( Dali::Animation anim = Dali::Animation(), float offsetTime=0, float duration=2.0f );

  void Deactivate();

private:

  /**
   * Create the stencil mask
   */
  void CreateStencil(Dali::Radian initialSector );

private:
  Dali::Layer           mLayer;
  Dali::Animation       mAnim;
  float                 mDuration;
  float                 mDiameter;
  Dali::Radian          mInitialAngle;
  Dali::Radian          mFinalAngle;
  Dali::Radian          mInitialSector;
  Dali::Radian          mFinalSector;
  Dali::Radian          mInitialActorAngle;
  Dali::Radian          mFinalActorAngle;
  Dali::AlphaFunction   mEasingFunction;
  Dali::Actor           mStencilActor;       ///< Stencil actor which generates mask
  Dali::Property::Index mStartAngleIndex;    ///< Index of start-angle property
  Dali::Property::Index mRotationAngleIndex; ///< Index of rotation-angle property
  bool                  mRotateActorsWithStencil:1;
  bool                  mRotateActors;
};


inline RadialSweepViewImpl& GetImpl( RadialSweepView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<RadialSweepViewImpl&>(handle);
}

inline const RadialSweepViewImpl& GetImpl( const RadialSweepView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const RadialSweepViewImpl&>(handle);
}



#endif // DALI_DEMO_RADIAL_SWEEP_VIEW_IMPL_H
