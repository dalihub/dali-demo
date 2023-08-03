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

#include "frame-callback.h"
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/update/update-proxy.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/math/vector3.h>
#include "physics-impl.h"

using Dali::Quaternion;
using Dali::Vector3;

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gPhysicsDemo;
#endif

FrameCallback::FrameCallback(PhysicsImpl& physicsImpl)
: mPhysicsImpl(physicsImpl)
{
}

bool FrameCallback::Update(Dali::UpdateProxy& updateProxy, float elapsedSeconds)
{
  static int numCalls = 0;

  numCalls++;
  if(numCalls % 30 == 0)
  {
    DALI_LOG_INFO(gPhysicsDemo, Debug::Concise, "Physics frame update\n");
  }

  Dali::Mutex::ScopedLock lock(mPhysicsImpl.mMutex);
  static float            frameTime = 0;
  frameTime += elapsedSeconds;
  do
  {
    mPhysicsImpl.Integrate(mPhysicsTimeStep);
    frameTime -= mPhysicsTimeStep;
  } while(frameTime > 0);

  for(auto&& actor : mPhysicsImpl.mPhysicsActors)
  {
    // Get position, orientation from physics world.
    Vector3 position = actor.second.GetActorPosition();
    updateProxy.BakePosition(actor.first, position);
    Quaternion rotation = actor.second.GetActorRotation();
    updateProxy.BakeOrientation(actor.first, rotation);
  }

  return true;
}
