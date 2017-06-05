#ifndef LOOK_CAMERA_H
#define LOOK_CAMERA_H

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

#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/math/vector2.h>

/**
 * @brief The LookCamera class
 *
 * LookCamera handles user input to change the orientation of the default camera.
 */
class LookCamera : public Dali::ConnectionTracker
{
public:

  /**
   * Creates an instance of LookCamera
   */
  LookCamera();

  /**
   * Destroys an instance of LookCamera
   */
  ~LookCamera();

  /**
   * Initialise with given position, fovY, near, far
   * @param[in] position Position of the camera.
   * @param[in] fovY Field of view in degrees
   * @param[in] near Near plane
   * @param[in] far Far Plane
   */
  void Initialise( const Dali::Vector3& position, float fov, float near, float far );

  /**
   * Retrieves actor associated with camera object
   * @return Returns camera actor
   */
  Dali::CameraActor GetCameraActor();

private:

  /**
   * Sets up a perspective camera using Dali default camera
   */
  void InitialiseDefaultCamera();

  /**
   * Creates 'interceptor' actor. Interceptor actor is always parallel
   * to the camera and positioned little bit in front of it in order to
   * intercept user input.
   */
  void CreateInterceptorActor();

  /**
   * Handles onTouch signal on the 'interceptor' actor
   * @param[in] actor Actor receiving signal
   * @param[in] touch Touch data
   */
  bool OnTouch( Dali::Actor actor, const Dali::TouchData& touch );

  /**
   * Handles camera tick() update
   * @return true if continue running timer, false otherwise
   */
  bool OnTick();

private:

  Dali::CameraActor mCameraActor; /// Camera actor
  Dali::Actor mInterceptorActor; /// Actor intercepting user input

  Dali::Timer mTimer; /// Per-frame timer

  Dali::Vector2 mScreenLookDelta; /// Look delta vector in screen space
  Dali::Vector2 mOldTouchLookPosition; /// Previous look vector in screen space

  Dali::Vector2 mCameraYawPitch; /// Camera yaw-pitch angles

  float mFovY; /// Camera field-of-view
  float mNear; /// Near plane
  float mFar; /// Far plane

  Dali::Vector3 mCameraPosition; /// Current camera position ( shadowing the actor position )
};

#endif
