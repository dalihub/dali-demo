#ifndef ITEM_VIEW_ORIENTATION_CONSTRAINT_H
#define ITEM_VIEW_ORIENTATION_CONSTRAINT_H

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
 *
 */

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector3.h>

/**
 * @brief Constraint used to constrain the orientation of the item-view depending on the position within the layout.
 */
class ItemViewOrientationConstraint
{
public:

  /**
   * @brief Constructor.
   * @param[in] maximumRotationInDegrees       The maximum rotation (in degrees) that we should rotate the item-view by.
   * @param[in] layoutPositionChangeMultiplier This value is used to multiply the change in layout position
   *                                           (in order to exaggerate the amount moved so it's more visible).
   */
  ItemViewOrientationConstraint( float maximumRotationInDegrees, float layoutPositionChangeMultiplier )
  : mMaximumRotationInDegrees( maximumRotationInDegrees ),
    mLayoutPositionChangeMultiplier( layoutPositionChangeMultiplier ),
    mStartingLayoutPosition( 0.0f ),
    mStartingAngle( 0.0f ),
    mFirstCall( true )
  {
  }

  /**
   * @brief Will be called by the Constraint.
   *
   * The first time this operator is called, it uses the values as it's base reference.
   * Thereafter, the position in the layout is used to determine the rotation around the X-Axis.
   *
   * @param[in] rotation The rotation of the item-view.
   * @param[in] inputs   The constraint inputs:
   *                     [0] ItemView::Property::LAYOUT_POSITION, float
   */
  void operator()( Dali::Quaternion& rotation, const Dali::PropertyInputContainer& inputs )
  {
    const float& layoutPosition = inputs[ 0 ]->GetFloat();

    // Store values for base reference when called the first call.
    if( mFirstCall )
    {
      mStartingLayoutPosition = layoutPosition;

      Dali::Vector3 axis;
      Dali::Radian angleInRadians;
      rotation.ToAxisAngle( axis, angleInRadians );
      Dali::Degree angleInDegrees( angleInRadians ); // Convert to Degrees

      mStartingAngle = angleInDegrees.degree;
      if( axis.x < 0.0f ) // We only rotate round the X-Axis. So if the X-Axis is negative, then the angle is also a negative angle.
      {
        mStartingAngle = -mStartingAngle;
      }

      mFirstCall = false;
    }
    else
    {
      // All subsequent calls should tilt the orientation of the item-view around the X-Axis depending on how much our position has changed in the layout.

      Dali::Degree angle( mStartingAngle + mLayoutPositionChangeMultiplier * ( mStartingLayoutPosition - layoutPosition ) );
      Dali::ClampInPlace( angle.degree, -mMaximumRotationInDegrees, mMaximumRotationInDegrees ); // Ensure the angle does not exceed maximum specified (in both directions).
      rotation = Dali::Quaternion( angle, Dali::Vector3::XAXIS );
    }
  }

private:

  const float mMaximumRotationInDegrees; ///< The maximum allowable rotation of the item-view.
  const float mLayoutPositionChangeMultiplier; ///< This value is used to multiply the change in layout position.
  float mStartingLayoutPosition; ///< The starting layout position.
  float mStartingAngle; ///< The starting angle (in degrees) of the item-view.
  bool mFirstCall; ///< A boolean to state whether this is the first time the operator() is called. Allows us to set the starting values.
};

#endif // ITEM_VIEW_ORIENTATION_CONSTRAINT_H
