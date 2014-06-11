#ifndef DALI_DEMO_RADIAL_SWEEP_VIEW_IMPL_H
#define DALI_DEMO_RADIAL_SWEEP_VIEW_IMPL_H

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
                              Dali::Degree initialAngle,
                              Dali::Degree finalAngle,
                              Dali::Degree initialSector,
                              Dali::Degree finalSector );

  RadialSweepViewImpl( float duration,
                       float diameter,
                       Dali::Degree initialAngle,
                       Dali::Degree finalAngle,
                       Dali::Degree initialSector,
                       Dali::Degree finalSector );

  void SetDuration(float duration);
  void SetEasingFunction( Dali::AlphaFunction easingFunction );

  void SetDiameter(float diameter);
  void SetInitialAngle( Dali::Degree initialAngle);
  void SetFinalAngle( Dali::Degree finalAngle);
  void SetInitialSector( Dali::Degree initialSector);
  void SetFinalSector( Dali::Degree finalSector);
  void SetInitialActorAngle( Dali::Degree initialAngle );
  void SetFinalActorAngle( Dali::Degree finalAngle );

  float GetDuration( );
  float GetDiameter( );
  Dali::Degree GetInitialAngle( );
  Dali::Degree GetFinalAngle( );
  Dali::Degree GetInitialSector( );
  Dali::Degree GetFinalSector( );
  Dali::Degree GetInitialActorAngle( );
  Dali::Degree GetFinalActorAngle( );

  void RotateActorsWithStencil(bool rotate);

  void Add( Dali::Actor actor );

  void Activate( Dali::Animation anim = Dali::Animation(), float offsetTime=0, float duration=2.0f );

  void Deactivate();

private:

  /**
   * Create the stencil mask
   */
  void CreateStencil(Dali::Degree initialSector );

private:
  Dali::Layer           mLayer;
  Dali::Animation       mAnim;
  float                 mDuration;
  float                 mDiameter;
  Dali::Degree          mInitialAngle;
  Dali::Degree          mFinalAngle;
  Dali::Degree          mInitialSector;
  Dali::Degree          mFinalSector;
  Dali::Degree          mInitialActorAngle;
  Dali::Degree          mFinalActorAngle;
  Dali::AlphaFunction   mEasingFunction;
  Dali::MeshActor       mStencilActor;       ///< Stencil actor which generates mask
  Dali::Material        mMaterial;           ///< Material for drawing mesh actor
  Dali::AnimatableMesh  mMesh;               ///< Animatable mesh
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
