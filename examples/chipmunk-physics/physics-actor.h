#ifndef DALI_PHYSICS_DEMO_PHYSICS_ACTOR_H
#define DALI_PHYSICS_DEMO_PHYSICS_ACTOR_H
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

#include <chipmunk/chipmunk.h>
#include <cstdint>
#include <dali/dali.h>
#include <iosfwd>

// Forward declarations
class PhysicsImpl;

/**
 * Class that associates an actor with a physics body. (Initially, rigid body)
 */
class PhysicsActor
{
public:
  PhysicsActor() = default;
  PhysicsActor(Dali::Actor& actor, cpBody* body, PhysicsImpl* impl, Dali::Property::Index brightnessId)
  : mImpl(impl),
    mActorId(actor.GetProperty<int>(Dali::Actor::Property::ID)),
    mBody(body),
    mBrightnessIndex(brightnessId)
  {
    cpBodySetUserData(mBody, this);
  }

  PhysicsActor(const PhysicsActor& rhs)=delete;
  PhysicsActor& operator=(const PhysicsActor& rhs)=delete;

  PhysicsActor(const PhysicsActor&& rhs)
  {
    if(this != &rhs)
    {
      mImpl = rhs.mImpl;
      mActorId = rhs.mActorId;
      mBody = rhs.mBody;
      cpBodySetUserData(mBody, this);
      mBrightnessIndex = rhs.mBrightnessIndex;
    }
  }

  PhysicsActor& operator=(const PhysicsActor&& rhs)
  {
    if(this != &rhs)
    {
      mActorId = rhs.mActorId;
      mBody    = rhs.mBody;
      mImpl = rhs.mImpl;
      mBrightnessIndex = rhs.mBrightnessIndex;
      cpBodySetUserData(mBody, this);
    }
    return *this;
  }

  uint32_t GetId()
  {
    return mActorId;
  }

  cpBody* GetBody()
  {
    return mBody;
  }

  Dali::Property::Index GetBrightnessIndex()
  {
    return mBrightnessIndex;
  }

  Dali::Vector3 GetPhysicsPosition();
  Dali::Quaternion GetPhysicsRotation();

  void SetPhysicsPosition(Dali::Vector3 actorPosition);
  void SetPhysicsVelocity(Dali::Vector3 actorVelocity);
  void SetPhysicsAngularVelocity(Dali::Vector3 actorVelocity);
  void SetPhysicsRotation(Dali::Quaternion actorRotation);
  Dali::Vector3 GetActorPosition();
  Dali::Vector3 GetActorVelocity();
  Dali::Quaternion GetActorRotation();
  void ClearForces();

private:
  PhysicsImpl* mImpl{nullptr};
  uint32_t mActorId{0};
  cpBody* mBody{nullptr};
  Dali::Property::Index mBrightnessIndex{Dali::Property::INVALID_INDEX};
};

#endif // DALI_PHYSICS_DEMO_PHYSICS_ACTOR_H
