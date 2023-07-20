#ifndef DALI_PHYSICS_DEMO_PHYSICS_IMPL_H
#define DALI_PHYSICS_DEMO_PHYSICS_IMPL_H
/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

#include <dali/dali.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/common/stage-devel.h>

#include <map>
#include <chipmunk/chipmunk.h>

#include "physics-actor.h"
#include "frame-callback.h"


class PhysicsImpl : public Dali::ConnectionTracker
{
public:
  Dali::Actor Initialize(Dali::Window window);

  /**
   * Create a layer & debug renderer
   */
  Dali::Layer CreateDebug(Dali::Vector2 windowSize);

  /**
   * Converts a point in RootActor local coords (e.g. gesture)
   * into physics space coords.
   * @param vector The point to convert
   * @return The converted point
   */
  Dali::Vector3 TranslateToPhysicsSpace(Dali::Vector3 vector);

  /**
   * Converts a point in physics space coords.
   * into RootActor local coords
   * @param vector The point to convert
   * @return The converted point
   */
  Dali::Vector3 TranslateFromPhysicsSpace(Dali::Vector3 vector);

  /**
   * Converts a vector in DALi space into physics space.
   * @param vector The vector to convert
   * @return The converted vector
   */
  Dali::Vector3 ConvertVectorToPhysicsSpace(Dali::Vector3 vector);

  /**
   * Converts a vector in physics space to DALi space
   * @param vector The vector to convert
   * @return The converted vector
   */
  Dali::Vector3 ConvertVectorFromPhysicsSpace(Dali::Vector3 vector);

  /**
   * Set up the transform from world space to physics space
   * @param[in] worldSize The 2d bounding box of the world in screen space
   */
  void SetTransform(Dali::Vector2 worldSize);

  /**
   * Run the physics integration over the given timestep.
   * @param timeStep
   */
  void Integrate(float timeStep);

  /**
   * Toggle the integration state. If it's turned on, physics will run
   * during the frame callback.
   */
  void ToggleIntegrateState()
  {
    mPhysicsIntegrateState ^= true;
  }

  /**
   * Toggle the debug state. If debug is turned on, use the physics engine
   * debug to show wireframes.
   */
  void ToggleDebugState()
  {
    mPhysicsDebugState ^= true;
  }

  void CreateWorldBounds(Dali::Window::WindowSize size);
  cpShape* AddBound(cpBody* staticBody, cpVect start, cpVect end);
  PhysicsActor& AddBall(Dali::Actor actor, float mass, float radius, float elasticity, float friction);
  PhysicsActor& AddBrick(Dali::Actor actor,float mass, float elasticity, float friction, Dali::Vector3 size);

  cpBody* AddMouseBody();

  /**
   * @param[in] screenCoords The touch point in screen coordinates
   * @param[in] origin The camera origin in DALi world space
   * @param[in] direction The ray direction in DALi world space
   * @param[out] localPivot The hit point local to the body
   * @param[out] distanceFromCamera The distance of the pick point from the camera
   * @return nullptr if no dynamic body found, otherwise a valid ptr to the hit body.
   */
  cpBody* HitTest(Dali::Vector2 screenCoords, Dali::Vector3 origin, Dali::Vector3 direction,
                  Dali::Vector3& localPivot, float& distanceFromCamera);

  cpConstraint* AddPivotJoint(cpBody* body1, cpBody* body2, Dali::Vector3 localPivot);

  void MoveMouseBody(cpBody* mouseBody, Dali::Vector3 position);

  void MoveConstraint(cpConstraint* constraint, Dali::Vector3 newPosition);

  void ReleaseConstraint(cpConstraint* constraint);

  /**
   * Ensure that the physics body does not go to sleep
   * @param[in] body The physics body
   * @return The old state
   */
  int ActivateBody(cpBody* body);

  /**
   * Restore the state of the physics body
   * @param[in] body The physics body
   * @param[in] oldState The previous state to restore
   */
  void RestoreBodyState(cpBody* body, int oldState);

  /**
   * Get the physics actor associated with the given body
   * @param[in] body The physics body
   * @return the associated physics actor
   */
  PhysicsActor* GetPhysicsActor(cpBody* body);

  /**
   * Set the highlight state of the actor associated with the physics body
   * @param[in] body The physics body
   * @param[in] highlight Whether to turn the highlight on or off.
   */
  void HighlightBody(cpBody* body, bool highlight);


public:
  std::map<uint32_t, PhysicsActor> mPhysicsActors;
  bool mPhysicsIntegrateState{true};
  bool mPhysicsDebugState{true};

  cpSpace* mSpace;
  cpShape* mLeftBound{nullptr};
  cpShape* mRightBound{nullptr};
  cpShape* mTopBound{nullptr};
  cpShape* mBottomBound{nullptr};

  Dali::Window mWindow;
  Dali::Mutex mMutex;

  Dali::Actor mPhysicsRoot;
  Dali::Vector2 mWorldOffset;
  FrameCallback* mFrameCallback;
};

#endif // DALI_PHYSICS_DEMO_PHYSICS_IMPL_H
