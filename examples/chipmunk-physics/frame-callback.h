#ifndef PHYSICS_DEMO_FRAME_CALLBACK_H
#define PHYSICS_DEMO_FRAME_CALLBACK_H

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
 */

#include <dali/public-api/adaptor-framework/window.h>
#include <dali/devel-api/update/frame-callback-interface.h>
#include <map>
#include <chrono>

class PhysicsImpl;

class FrameCallback : public Dali::FrameCallbackInterface
{
public:
  /**
   * Constructor
   */
  explicit FrameCallback(PhysicsImpl& physicsImpl);

  /**
   * Set the physics time step
   * @param timeStep (in seconds)
   */
  void SetPhysicsTimeStep(float timeStep)
  {
    mPhysicsTimeStep = timeStep;
  }

private:
  /**
   * Called each frame.
   * @param[in] updateProxy Used to set world matrix and size
   * @param[in] elapsedSeconds Time since last frame
   * @return Whether we should keep rendering.
   */
  bool Update(Dali::UpdateProxy& updateProxy, float elapsedSeconds) override;

private: // Member variables
  PhysicsImpl& mPhysicsImpl;
  float mPhysicsTimeStep{1.0/180.0};
};

#endif //PHYSICS_DEMO_FRAME_CALLBACK_H
