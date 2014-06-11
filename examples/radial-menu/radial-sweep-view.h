#ifndef DALI_DEMO_RADIAL_SWEEP_VIEW_H
#define DALI_DEMO_RADIAL_SWEEP_VIEW_H

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

class RadialSweepViewImpl;


/********************************************************************************
 * Handle to RadialSweepView implementation
 */
class RadialSweepView : public Dali::Toolkit::Control
{
public:
  /**
   * Create a new RadialSweepView with default parameters (2 second animation,
   * no rotation, sweeping out a full cicle).
   */
  static RadialSweepView New( );

  /**
   * Create a new RadialSweepView.
   * @param[in] duration The duration of the sweep animation
   * @param[in] diameter The diameter of the stencil mask
   * @param[in] initialAngle The initial angle of the anticlockwise line of the sweep sector
   * @param[in] finalAngle The final angle of the anticlockwise line of the sweep sector
   * @param[in] initialSector The angle of the starting sector
   * @param[in] finalSector The angle of the sector at the end of the animation.
   * Note, to cover the entire circle, use a value of 359.9999f, not zero or 360.
   *
   *            initial sector
   *             \   |  .
   *              \  |    .
   * initialAngle  \ |     .   final sector
   *                \|      _|
   *                 .________
   */
  static RadialSweepView New( float duration,
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

  /**
   * @param[in] rotate True if the actors should rotate with the stencil
   */
  void RotateActorsWithStencil(bool rotate);

  /**
   * Add actors to the view
   */
  void Add(Actor actor);

  /**
   * Activate the sweep animation
   */
  void Activate( );

  void Activate( Dali::Animation anim, float offsetTime, float duration );

  /**
   * Deactivate the sweep animation
   */
  void Deactivate();

  /**
   * Default constructor. Create an uninitialized handle.
   */
  RadialSweepView();

  /**
   * Copy constructor
   */
  RadialSweepView(const RadialSweepView& handle);

  /**
   * Assignment operator
   */
  RadialSweepView& operator=(const RadialSweepView& rhs);

  /**
   * Destructor
   */
  virtual ~RadialSweepView();

  /**
   * Downcast method
   */
  static RadialSweepView DownCast( BaseHandle handle );

public: // Not for use by application developers

  RadialSweepView( RadialSweepViewImpl& impl );

  RadialSweepView( Dali::Internal::CustomActor* impl );
};

#endif
