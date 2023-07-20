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

#include "physics-actor.h"
#include "physics-impl.h"
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/quaternion.h>

using Dali::Vector3;
using Dali::Quaternion;
using Dali::Radian;

void PhysicsActor::ClearForces()
{
  printf("Not Implemented\n");
  //mBody->clearForces();
  // No similar API
}

Dali::Vector3 PhysicsActor::GetPhysicsPosition()
{
  cpVect cpPosition = cpBodyGetPosition(mBody);
  return Vector3(cpPosition.x, cpPosition.y, 0.0f);
}

void PhysicsActor::SetPhysicsPosition(Dali::Vector3 actorPosition)
{
  Dali::Mutex::ScopedLock lock(mImpl->mMutex);
  Vector3 physicsPosition = mImpl->TranslateToPhysicsSpace(actorPosition);
  cpBodySetPosition(mBody, cpv(physicsPosition.x, physicsPosition.y));
}

void PhysicsActor::SetPhysicsVelocity(Dali::Vector3 actorVelocity)
{
  Dali::Mutex::ScopedLock lock(mImpl->mMutex);
  Vector3 physicsVelocity = mImpl->ConvertVectorToPhysicsSpace(actorVelocity);
  cpBodySetVelocity(mBody, cpv(physicsVelocity.x, physicsVelocity.y));
}

void PhysicsActor::SetPhysicsAngularVelocity(Dali::Vector3 velocity)
{
  Dali::Mutex::ScopedLock lock(mImpl->mMutex);
  printf("Not Implemented\n");
  //mBody->setAngularVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

Quaternion PhysicsActor::GetPhysicsRotation()
{
  return Quaternion{};
}

void PhysicsActor::SetPhysicsRotation(Dali::Quaternion rotation)
{
  Dali::Mutex::ScopedLock lock(mImpl->mMutex);

  Vector3 axis;
  Radian angle;
  rotation.ToAxisAngle(axis, angle);

  //btQuaternion orn = btQuaternion(btVector3(axis.x, -axis.y, axis.z), btScalar(float(-angle)));
  //btTransform& transform = mBody->getWorldTransform();
  //transform.setRotation(orn);
  printf("Not Implemented\n");
}


Vector3 PhysicsActor::GetActorPosition()
{
  cpVect cpPosition = cpBodyGetPosition(mBody);
  return mImpl->TranslateFromPhysicsSpace(Vector3(cpPosition.x, cpPosition.y, 0.0f));
}

Vector3 PhysicsActor::GetActorVelocity()
{
  cpVect cpVelocity = cpBodyGetVelocity(mBody);
  return mImpl->ConvertVectorFromPhysicsSpace(Vector3(cpVelocity.x, cpVelocity.y, 0.0f));
}

Quaternion PhysicsActor::GetActorRotation()
{
  cpFloat angle = cpBodyGetAngle(mBody);
  return Quaternion(Radian(angle), -Vector3::ZAXIS);
}
