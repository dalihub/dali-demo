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

#include "physics-impl.h"
#include "physics-actor.h"

#include <devel-api/common/stage.h>
#include <iostream>
#include <map>
#include <utility>

using Dali::Actor;
using Dali::Layer;
using Dali::Stage;
using Dali::Vector2;
using Dali::Vector3;
using Dali::Window;
using namespace Dali::DevelStage;

#define GRABBABLE_MASK_BIT (1u << 31)
cpShapeFilter GRAB_FILTER          = {CP_NO_GROUP, GRABBABLE_MASK_BIT, GRABBABLE_MASK_BIT};
cpShapeFilter NOT_GRABBABLE_FILTER = {CP_NO_GROUP, ~GRABBABLE_MASK_BIT, ~GRABBABLE_MASK_BIT};

Actor PhysicsImpl::Initialize(Window window)
{
  mWindow = window;
  mSpace  = cpSpaceNew();
  cpSpaceSetIterations(mSpace, 30);
  cpSpaceSetSleepTimeThreshold(mSpace, 0.5f);
  cpSpaceSetGravity(mSpace, cpv(0, -200));

  auto windowSize = window.GetSize();
  CreateWorldBounds(windowSize);

  // Create an actor that can handle mouse events.
  mPhysicsRoot                                 = Layer::New();
  mPhysicsRoot[Actor::Property::SIZE]          = Vector2(windowSize.GetWidth(), windowSize.GetHeight());
  mPhysicsRoot[Actor::Property::ANCHOR_POINT]  = Dali::AnchorPoint::CENTER;
  mPhysicsRoot[Actor::Property::PARENT_ORIGIN] = Dali::ParentOrigin::CENTER;

  mFrameCallback = new FrameCallback(*this);
  AddFrameCallback(Stage::GetCurrent(), *mFrameCallback, window.GetRootLayer());
  Stage::GetCurrent().KeepRendering(30);

  return mPhysicsRoot;
}

Layer PhysicsImpl::CreateDebug(Vector2 windowSize)
{
  return Layer();
}

void PhysicsImpl::CreateWorldBounds(Window::WindowSize size)
{
  // Physics origin is 0,0,0 in DALi coords.
  // But, Y is inverted, so bottom is -ve, top is +ve.
  // Perform this correction when applying position to actor.
  // But, can't use actors in update, so cache transform.
  SetTransform(Vector2(size.GetWidth(), size.GetHeight()));

  int xBound = size.GetWidth() / 2;
  int yBound = size.GetHeight() / 2;

  cpBody* staticBody = cpSpaceGetStaticBody(mSpace);

  if(mLeftBound)
  {
    cpSpaceRemoveShape(mSpace, mLeftBound);
    cpSpaceRemoveShape(mSpace, mRightBound);
    cpSpaceRemoveShape(mSpace, mTopBound);
    cpSpaceRemoveShape(mSpace, mBottomBound);
    cpShapeFree(mLeftBound);
    cpShapeFree(mRightBound);
    cpShapeFree(mTopBound);
    cpShapeFree(mBottomBound);
  }
  mLeftBound   = AddBound(staticBody, cpv(-xBound, -yBound), cpv(-xBound, yBound));
  mRightBound  = AddBound(staticBody, cpv(xBound, -yBound), cpv(xBound, yBound));
  mTopBound    = AddBound(staticBody, cpv(-xBound, -yBound), cpv(xBound, -yBound));
  mBottomBound = AddBound(staticBody, cpv(-xBound, yBound), cpv(xBound, yBound));
}

void PhysicsImpl::SetTransform(Vector2 worldSize)
{
  mWorldOffset.x = worldSize.x * 0.5f;
  mWorldOffset.y = worldSize.y * 0.5f;
  // y is always inverted.
}

cpShape* PhysicsImpl::AddBound(cpBody* staticBody, cpVect start, cpVect end)
{
  cpShape* shape = cpSpaceAddShape(mSpace, cpSegmentShapeNew(staticBody, start, end, 0.0f));
  cpShapeSetElasticity(shape, 1.0f);
  cpShapeSetFriction(shape, 1.0f);
  cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);
  return shape;
}

PhysicsActor& PhysicsImpl::AddBall(::Actor actor, float mass, float radius, float elasticity, float friction)
{
  Dali::Mutex::ScopedLock lock(mMutex);
  cpBody*                 body = cpSpaceAddBody(mSpace, cpBodyNew(mass, cpMomentForCircle(mass, 0.0f, radius, cpvzero)));
  cpBodySetPosition(body, cpv(0, 0));
  cpBodySetVelocity(body, cpv(0, 0));

  cpShape* shape = cpSpaceAddShape(mSpace, cpCircleShapeNew(body, radius, cpvzero));
  cpShapeSetElasticity(shape, elasticity);
  cpShapeSetFriction(shape, friction);

  int                   id    = actor[Actor::Property::ID];
  Dali::Property::Index index = actor.RegisterProperty("uBrightness", 0.0f);
  mPhysicsActors.insert(std::make_pair(id, PhysicsActor{actor, body, this, index}));
  actor[Actor::Property::PARENT_ORIGIN] = Dali::ParentOrigin::TOP_LEFT;
  actor[Actor::Property::ANCHOR_POINT]  = Dali::AnchorPoint::CENTER;
  mPhysicsRoot.Add(actor);
  return mPhysicsActors.at(id);
}

PhysicsActor& PhysicsImpl::AddBrick(Dali::Actor actor, float mass, float elasticity, float friction, Vector3 size)
{
  Dali::Mutex::ScopedLock lock(mMutex);
  cpBody*                 body = cpSpaceAddBody(mSpace, cpBodyNew(mass, cpMomentForBox(mass, size.width, size.height)));
  cpBodySetPosition(body, cpv(0, 0));
  cpBodySetVelocity(body, cpv(0, 0));

  cpShape* shape = cpSpaceAddShape(mSpace, cpBoxShapeNew(body, size.width, size.height, 0.0f));
  cpShapeSetFriction(shape, friction);
  cpShapeSetElasticity(shape, elasticity);

  int                   id    = actor[Actor::Property::ID];
  Dali::Property::Index index = actor.RegisterProperty("uBrightness", 0.0f);
  mPhysicsActors.insert(std::make_pair(id, PhysicsActor{actor, body, this, index}));
  actor[Actor::Property::PARENT_ORIGIN] = Dali::ParentOrigin::TOP_LEFT;
  actor[Actor::Property::ANCHOR_POINT]  = Dali::AnchorPoint::CENTER;
  mPhysicsRoot.Add(actor);
  return mPhysicsActors.at(id);
}

cpBody* PhysicsImpl::AddMouseBody()
{
  Dali::Mutex::ScopedLock lock(mMutex);
  auto                    kinematicBody = cpBodyNewKinematic(); // Mouse actor is a kinematic body that is not integrated
  return kinematicBody;
}

PhysicsActor* PhysicsImpl::GetPhysicsActor(cpBody* body)
{
  return reinterpret_cast<PhysicsActor*>(cpBodyGetUserData(body));
}

void PhysicsImpl::HighlightBody(cpBody* body, bool highlight)
{
  auto physicsActor = GetPhysicsActor(body);
  if(physicsActor)
  {
    Actor actor = mPhysicsRoot.FindChildById(physicsActor->GetId());
    if(actor)
    {
      actor[physicsActor->GetBrightnessIndex()] = highlight ? 1.0f : 0.0f;
    }
  }
}

// Convert from root actor local space to physics space
Vector3 PhysicsImpl::TranslateToPhysicsSpace(Vector3 vector)
{
  // root actor origin is top left, DALi Y is inverted.
  // Physics origin is center. Y: 0->1 => 0.5=>-0.5
  return Vector3(vector.x - mWorldOffset.x, mWorldOffset.y - vector.y, vector.z);
}

// Convert from physics space to root actor local space
Vector3 PhysicsImpl::TranslateFromPhysicsSpace(Vector3 vector)
{
  return Vector3(vector.x + mWorldOffset.x, mWorldOffset.y - vector.y, vector.z);
}

// Convert a vector from dali space to physics space
Vector3 PhysicsImpl::ConvertVectorToPhysicsSpace(Vector3 vector)
{
  // root actor origin is top left, DALi Y is inverted.
  // @todo Add space config scale.
  return Vector3(vector.x, -vector.y, vector.z);
}

// Convert a vector physics space to root actor local space
Vector3 PhysicsImpl::ConvertVectorFromPhysicsSpace(Vector3 vector)
{
  return Vector3(vector.x, -vector.y, vector.z);
}

void PhysicsImpl::Integrate(float timestep)
{
  if(mPhysicsIntegrateState)
  {
    cpSpaceStep(mSpace, timestep);
  }
  //  if(mDynamicsWorld->getDebugDrawer() && mPhysicsDebugState)
  //  {
  //    mDynamicsWorld->debugDrawWorld();
  //  }
}

cpBody* PhysicsImpl::HitTest(Vector2 screenCoords, Vector3 origin, Vector3 direction, Vector3& localPivot, float& distanceFromCamera)
{
  Vector3          spacePosition = TranslateToPhysicsSpace(Vector3{screenCoords});
  cpVect           mousePosition = cpv(spacePosition.x, spacePosition.y);
  cpFloat          radius        = 5.0f;
  cpPointQueryInfo info          = {0};
  cpShape*         shape         = cpSpacePointQueryNearest(mSpace, mousePosition, radius, GRAB_FILTER, &info);

  cpBody* body{nullptr};

  if(shape && cpBodyGetMass(cpShapeGetBody(shape)) < INFINITY)
  {
    // Use the closest point on the surface if the click is outside the shape.
    cpVect nearest = (info.distance > 0.0f ? info.point : mousePosition);
    body           = cpShapeGetBody(shape);
    cpVect local   = cpBodyWorldToLocal(body, nearest);
    localPivot.x   = local.x;
    localPivot.y   = local.y;
    localPivot.z   = 0.0;
  }
  return body;
}

cpConstraint* PhysicsImpl::AddPivotJoint(cpBody* body1, cpBody* body2, Vector3 localPivot)
{
  cpVect        pivot{localPivot.x, localPivot.y};
  cpConstraint* joint = cpPivotJointNew2(body2, body1, cpvzero, pivot);
  cpConstraintSetMaxForce(joint, 50000.0f); // Magic numbers for mouse feedback.
  cpConstraintSetErrorBias(joint, cpfpow(1.0f - 0.15f, 60.0f));
  cpConstraint* constraint = cpSpaceAddConstraint(mSpace, joint);
  return constraint; // Constraint & joint are the same...
}

void PhysicsImpl::MoveMouseBody(cpBody* mouseBody, Vector3 position)
{
  cpVect cpPosition = cpv(position.x, position.y);
  cpVect newPoint   = cpvlerp(cpBodyGetPosition(mouseBody), cpPosition, 0.25f);
  cpBodySetVelocity(mouseBody, cpvmult(cpvsub(newPoint, cpBodyGetPosition(mouseBody)), 60.0f));
  // Normally, kinematic body's position would be calculated by engine.
  // For mouse, though, we want to set it.
  cpBodySetPosition(mouseBody, newPoint);
}

void PhysicsImpl::MoveConstraint(cpConstraint* constraint, Vector3 newPosition)
{
}

void PhysicsImpl::ReleaseConstraint(cpConstraint* constraint)
{
  cpSpaceRemoveConstraint(mSpace, constraint);
  cpConstraintFree(constraint);
}

int PhysicsImpl::ActivateBody(cpBody* body)
{
  int oldState = cpBodyIsSleeping(body);
  cpBodyActivate(body);

  return oldState;
}

void PhysicsImpl::RestoreBodyState(cpBody* body, int oldState)
{
  if(oldState)
  {
    cpBodyActivate(body);
  }
  else
  {
    cpBodySleep(body);
  }
}
