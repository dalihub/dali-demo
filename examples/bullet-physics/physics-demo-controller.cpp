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

#include <dali-toolkit/dali-toolkit.h>
#include <dali/dali.h>
#include "dali-physics/public-api/physics-actor.h"
#include "dali-physics/public-api/physics-adaptor.h"

#include <dali/devel-api/adaptor-framework/key-devel.h>
#include <dali/devel-api/events/hit-test-algorithm.h>

#include <iostream>
#include <string>

#include <btBulletDynamicsCommon.h>

#include "ball-renderer.h"
#include "cube-renderer.h"

using namespace Dali;
using namespace Dali::Toolkit::Physics;

namespace KeyModifier
{
enum Key
{
  CONTROL_L = DevelKey::DALI_KEY_CONTROL_LEFT,
  CONTROL_R = DevelKey::DALI_KEY_CONTROL_RIGHT,
  SHIFT_L   = 50,
  SHIFT_R   = 62,
  ALT_L     = 64,
  ALT_R     = 108,
  SUPER_L   = 133,
  SUPER_R   = 134,
  MENU      = 135,
};
}

const std::string BRICK_WALL    = DEMO_IMAGE_DIR "/brick-wall.jpg";
const std::string BALL_IMAGE    = DEMO_IMAGE_DIR "/blocks-ball.png";
const std::string BRICK_URIS[4] = {
  DEMO_IMAGE_DIR "/blocks-brick-1.png", DEMO_IMAGE_DIR "/blocks-brick-2.png", DEMO_IMAGE_DIR "/blocks-brick-3.png", DEMO_IMAGE_DIR "/blocks-brick-4.png"};

class PhysicsDemoController : public ConnectionTracker
{
public:
  PhysicsDemoController(Application& app)
  : mApplication(app)
  {
    app.InitSignal().Connect(this, &PhysicsDemoController::OnInit);
    app.TerminateSignal().Connect(this, &PhysicsDemoController::OnTerminate);
  }

  ~PhysicsDemoController() override
  {
  }

  void OnInit(Application& application)
  {
    mWindow = application.GetWindow();
    mWindow.ResizeSignal().Connect(this, &PhysicsDemoController::OnWindowResize);
    mWindow.KeyEventSignal().Connect(this, &PhysicsDemoController::OnKeyEv);
    Stage::GetCurrent().KeepRendering(30);
    mWindow.SetBackgroundColor(Color::DARK_SLATE_GRAY);
    Window::WindowSize windowSize = mWindow.GetSize();

    auto cameraActor                                    = mWindow.GetRenderTaskList().GetTask(0).GetCameraActor();
    cameraActor[CameraActor::Property::FIELD_OF_VIEW]   = Math::PI / 2.5f; // 72 degrees
    cameraActor[Actor::Property::POSITION_X]            = 500;
    float z                                             = cameraActor[Actor::Property::POSITION_Z];
    cameraActor[Actor::Property::POSITION_Z]            = z - 100;
    cameraActor[CameraActor::Property::TARGET_POSITION] = Vector3();
    cameraActor[CameraActor::Property::TYPE]            = Camera::LOOK_AT_TARGET;
    mPhysicsTransform.SetIdentityAndScale(Vector3(1.0f, -1.0f, 1.0f));
    mPhysicsTransform.SetTranslation(Vector3(windowSize.GetWidth() * 0.5f,
                                             windowSize.GetHeight() * 0.5f,
                                             -100.0f));

    mPhysicsAdaptor = PhysicsAdaptor::New(mPhysicsTransform, windowSize);
    mPhysicsRoot    = mPhysicsAdaptor.GetRootActor();

    mPhysicsRoot.TouchedSignal().Connect(this, &PhysicsDemoController::OnTouched);
    mPhysicsRoot.WheelEventSignal().Connect(this, &PhysicsDemoController::OnWheel);

    mWindow.Add(mPhysicsRoot);

    auto scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor();
    auto bulletWorld    = scopedAccessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    bulletWorld->setGravity(btVector3(0, -200, 0));

    CreateGround(scopedAccessor, windowSize);
    mBrick         = CreateLargeBrick(scopedAccessor);
    mSelectedActor = mBrick;

    CreateBall(scopedAccessor);
    CreateBrickPyramid(scopedAccessor, windowSize);

    mPhysicsAdaptor.CreateSyncPoint();
  }

  btRigidBody* CreateRigidBody(btDiscreteDynamicsWorld* bulletWorld, float mass, const btTransform& bulletTransform, btCollisionShape* shape)
  {
    bool      isDynamic = (mass != 0.0);
    btVector3 localInertia(0.0f, 0.0f, 0.0f);
    if(isDynamic)
    {
      shape->calculateLocalInertia(mass, localInertia);
    }

    auto*                                    motionState = new btDefaultMotionState(bulletTransform);
    btRigidBody::btRigidBodyConstructionInfo rigidBodyConstructionInfo(mass, motionState, shape, localInertia);
    auto*                                    body = new btRigidBody(rigidBodyConstructionInfo);

    bulletWorld->addRigidBody(body);
    return body;
  }

  void CreateGround(PhysicsAdaptor::ScopedPhysicsAccessorPtr& scopedAccessor, Dali::Window::WindowSize windowSize)
  {
    Dali::Vector3 size(2 * windowSize.GetWidth(), 10.f, 2 * windowSize.GetWidth());
    Actor         groundActor = CubeRenderer::CreateActor(size, BRICK_WALL);

    auto physicsActor = CreateBrick(scopedAccessor, groundActor, 0.0f, 0.1f, 0.9f, size);
    physicsActor.AsyncSetPhysicsPosition(Vector3(0.0f, 0.5f * windowSize.GetHeight(), 0.0f));
  }

  void CreateBall(PhysicsAdaptor::ScopedPhysicsAccessorPtr& scopedAccessor)
  {
    const float BALL_MASS       = 1.0f;
    const float BALL_RADIUS     = 50.0f;
    const float BALL_ELASTICITY = 0.5f;
    const float BALL_FRICTION   = 0.5f;
    auto        actor           = BallRenderer::CreateActor(Vector3(BALL_RADIUS * 2, BALL_RADIUS * 2, BALL_RADIUS * 2), Dali::Color::WHITE);

    btSphereShape* ball = new btSphereShape(BALL_RADIUS); // @todo Fix leak
    btTransform    transform;
    transform.setIdentity();

    auto         bulletWorld = scopedAccessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    btRigidBody* body        = CreateRigidBody(bulletWorld, BALL_MASS, transform, ball);

    body->setFriction(BALL_FRICTION);
    body->setRestitution(BALL_ELASTICITY);

    auto physicsActor = mPhysicsAdaptor.AddActorBody(actor, body);
    actor.RegisterProperty("uBrightness", 0.0f);

    physicsActor.AsyncSetPhysicsPosition(Vector3(0.0f, -400.0f, -150.0f));
  }

  PhysicsActor CreateBrick(PhysicsAdaptor::ScopedPhysicsAccessorPtr& scopedAccessor,
                           Dali::Actor                               actor,
                           float                                     mass,
                           float                                     elasticity,
                           float                                     friction,
                           Vector3                                   size)
  {
    btVector3   halfExtents(size.width * 0.5f, size.height * 0.5f, size.depth * 0.5f);
    btBoxShape* shape = new btBoxShape(halfExtents); // @todo Fix leak
    btVector3   inertia;
    inertia.setZero();
    shape->calculateLocalInertia(mass, inertia);

    btTransform startTransform;
    startTransform.setIdentity();
    auto         bulletWorld = scopedAccessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    btRigidBody* body        = CreateRigidBody(bulletWorld, mass, startTransform, shape);

    body->setFriction(friction);
    body->setRestitution(elasticity);

    actor.RegisterProperty("uBrightness", 0.0f);

    auto physicsActor = mPhysicsAdaptor.AddActorBody(actor, body);
    return physicsActor;
  }

  PhysicsActor CreateLargeBrick(PhysicsAdaptor::ScopedPhysicsAccessorPtr& scopedAccessor)
  {
    const float BRICK_MASS       = 1.0f;
    const float BRICK_ELASTICITY = 0.1f;
    const float BRICK_FRICTION   = 0.6f;
    const int   BRICK_WIDTH      = 400;
    const int   BRICK_HEIGHT     = 150;
    const int   BRICK_DEPTH      = 200;

    auto brick        = CubeRenderer::CreateActor(Vector3(BRICK_WIDTH, BRICK_HEIGHT, BRICK_DEPTH), Dali::Color::WHITE);
    auto physicsActor = CreateBrick(scopedAccessor, brick, BRICK_MASS, BRICK_ELASTICITY, BRICK_FRICTION, Vector3(BRICK_WIDTH, BRICK_HEIGHT, BRICK_DEPTH));

    physicsActor.AsyncSetPhysicsPosition(Vector3(0, 0, -300));
    return physicsActor;
  }

  void CreateBrickPyramid(PhysicsAdaptor::ScopedPhysicsAccessorPtr& scopedAccessor, Dali::Window::WindowSize windowSize)
  {
    const float BRICK_MASS       = 1.0f;
    const float BRICK_ELASTICITY = 0.1f;
    const float BRICK_FRICTION   = 0.6f;
    const int   BRICK_WIDTH      = 60;
    const int   BRICK_HEIGHT     = 30;
    const int   BRICK_DEPTH      = 30;
    const int   BRICK_GAP        = 12;

    Dali::Vector4 colors[5] = {Dali::Color::AQUA_MARINE, Dali::Color::DARK_SEA_GREEN, Dali::Color::BLUE_VIOLET, Dali::Color::MISTY_ROSE, Dali::Color::ORCHID};

    int numberOfRows = 10;
    int oY           = -(1 + numberOfRows) * (BRICK_HEIGHT + BRICK_GAP);
    for(int i = 0; i < numberOfRows; ++i)
    {
      int   w  = (i + 1) * BRICK_WIDTH + i * BRICK_GAP;
      float oX = w * -0.5f;
      for(int j = 0; j < i + 1; ++j)
      {
        auto brick        = CubeRenderer::CreateActor(Vector3(BRICK_WIDTH, BRICK_HEIGHT, BRICK_DEPTH), colors[(i + j) % 5]);
        auto physicsActor = CreateBrick(scopedAccessor, brick, BRICK_MASS, BRICK_ELASTICITY, BRICK_FRICTION, Vector3(BRICK_WIDTH, BRICK_HEIGHT, BRICK_DEPTH));

        physicsActor.AsyncSetPhysicsPosition(Vector3(oX + j * (BRICK_WIDTH + BRICK_GAP), oY + i * (BRICK_HEIGHT + BRICK_GAP), -300.0f));
        // Create slight rotation offset to trigger automatic collapse
        auto axis = Vector3(Random::Range(-0.2f, 0.2f), // Roughly the z axis
                            Random::Range(-0.2f, 0.2f),
                            Random::Range(0.8f, 1.2f));
        axis.Normalize();
        physicsActor.AsyncSetPhysicsRotation(Quaternion(Radian(Random::Range(-0.3f, 0.3f)), axis));
      }
    }
  }

  void HighlightBody(btRigidBody* body, bool highlight)
  {
    auto physicsActor = mPhysicsAdaptor.GetPhysicsActor(body);
    if(physicsActor)
    {
      Actor actor = mPhysicsAdaptor.GetRootActor().FindChildById(physicsActor.GetId());
      if(actor)
      {
        actor["uBrightness"] = highlight ? 1.0f : 0.0f;
      }
    }
  }

  void OnTerminate(Application& application)
  {
    UnparentAndReset(mPhysicsRoot);
  }

  void OnWindowResize(Window window, Window::WindowSize newSize)
  {
    Vector2 size(newSize.GetWidth(), newSize.GetHeight());
    window.GetRenderTaskList().GetTask(0).GetCameraActor().SetPerspectiveProjection(size);
    mPhysicsAdaptor.SetTransformAndSize(mPhysicsTransform, newSize);
  }

  bool OnTouched(Dali::Actor actor, const Dali::TouchEvent& touch)
  {
    static enum {
      None,
      MoveCameraXZ,
      MovePivot,
    } state = None;

    const float MOUSE_CLAMPING(30.0f);
    const float TAU(0.001f);
    //static float cameraY{0.0f};
    auto    renderTask   = mWindow.GetRenderTaskList().GetTask(0);
    auto    screenCoords = touch.GetScreenPosition(0);
    Vector3 origin, direction;
    Dali::HitTestAlgorithm::BuildPickingRay(renderTask, screenCoords, origin, direction);

    switch(state)
    {
      case None:
      {
        if(touch.GetState(0) == Dali::PointState::STARTED)
        {
          if(mCtrlDown)
          {
            state = MoveCameraXZ;
            // local to top left
            //cameraY = touch.GetLocalPosition(0).y;
            // Could move on fixed plane, e.g. y=0.
            // position.Y corresponds to a z value depending on perspective
            // position.X scales to an x value depending on perspective
          }
          else
          {
            state               = MovePivot;
            auto scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor();
            auto bulletWorld    = scopedAccessor->GetNative().Get<btDiscreteDynamicsWorld*>();

            Vector3 localPivot;
            Vector3 rayPhysicsOrigin;
            Vector3 rayPhysicsEnd;
            mPhysicsAdaptor.BuildPickingRay(origin, direction, rayPhysicsOrigin, rayPhysicsEnd);
            auto body = scopedAccessor->HitTest(rayPhysicsOrigin, rayPhysicsEnd, localPivot, mOldPickingDistance);
            if(!body.Empty())
            {
              mPickedBody = body.Get<btRigidBody*>();
              HighlightBody(mPickedBody, true);
              mSelectedActor = mPhysicsAdaptor.GetPhysicsActor(mPickedBody);

              mPickedSavedState = mPickedBody->getActivationState();
              mPickedBody->setActivationState(DISABLE_DEACTIVATION);

              btVector3 pivot(localPivot.x, localPivot.y, localPivot.z);
              auto      p2p = new btPoint2PointConstraint(*mPickedBody, pivot);
              bulletWorld->addConstraint(p2p, true);
              p2p->m_setting.m_impulseClamp = MOUSE_CLAMPING;
              p2p->m_setting.m_tau          = TAU;

              mPickedConstraint = p2p;
            }
          }
        }
        break;
      }
      case MovePivot:
      {
        if(touch.GetState(0) == Dali::PointState::MOTION)
        {
          if(mPickedBody && mPickedConstraint)
          {
            if(!mShiftDown)
            {
              // Move point in XY plane, projected into scene
              auto scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor(); // Ensure we get a lock

              Vector3                  newPosition = mPhysicsAdaptor.ProjectPoint(origin, direction, mOldPickingDistance);
              btPoint2PointConstraint* p2p         = static_cast<btPoint2PointConstraint*>(mPickedConstraint);
              if(p2p)
              {
                // @todo Add inline memory cast... (only viable if physics uses floats not doubles)
                p2p->setPivotB(btVector3(newPosition.x, newPosition.y, newPosition.z));
              }
            }
            // @todo Add code to move objects in XZ plane
          }
        }
        else if(touch.GetState(0) == Dali::PointState::FINISHED ||
                touch.GetState(0) == Dali::PointState::INTERRUPTED)
        {
          if(mPickedConstraint)
          {
            auto scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor(); // Ensure we get a lock
            auto bulletWorld    = scopedAccessor->GetNative().Get<btDiscreteDynamicsWorld*>();
            HighlightBody(mPickedBody, false);
            mPickedBody->forceActivationState(mPickedSavedState);
            mPickedBody->activate();
            bulletWorld->removeConstraint(mPickedConstraint);

            mPickedConstraint = nullptr;
            mPickedBody       = nullptr;
          }
          state = None;
        }
        break;
      }
      case MoveCameraXZ:
      {
        if(touch.GetState(0) == Dali::PointState::MOTION)
        {
          // Move camera in XZ plane
          //float y = cameraY; // touch point in Y. Move camera in an XZ plane on this point.
        }
        else if(touch.GetState(0) == Dali::PointState::FINISHED ||
                touch.GetState(0) == Dali::PointState::INTERRUPTED)
        {
          state = None;
        }
        break;
      }
    }

    static int reduceSpam = 0;
    ++reduceSpam;
    if(reduceSpam == 30)
    {
      reduceSpam = 0;
      std::cout << "Dali scrnpos:" << screenCoords << "\nDali pos:    " << mBrick.GetActorPosition() << "\nDali rot:    " << mBrick.GetActorRotation() << "\nPhys pos:    " << mBrick.GetPhysicsPosition() << "\nPhys rot:    " << mBrick.GetPhysicsRotation() << "\n";
    }

    Stage::GetCurrent().KeepRendering(30.0f);

    return true;
  }

  bool OnWheel(Actor actor, const WheelEvent& event)
  {
    // Move camera along it's fwd axis
    auto            renderTask = mWindow.GetRenderTaskList().GetTask(0);
    auto            camera     = renderTask.GetCameraActor();
    static uint32_t lastTime   = 0;

    int32_t delta = event.GetDelta();
    if(lastTime > 0)
    {
      uint32_t timeDiff = event.GetTime() - lastTime;
      ++timeDiff;       // Can be zero.
      if(timeDiff < 50) // When it's less than some threshold
      {
        // Scale delta by speed (range: 0-6)
        float scale = (50.0f - timeDiff) * 6.0f / 50.0f; // smaller times = bigger factor
        scale *= (scale * scale);                        // Cube it. (Max - 360)
        scale = std::max(1.0f, scale);
        delta *= scale;
      }
    }
    lastTime = event.GetTime();

    float z = camera[Actor::Property::POSITION_Z];
    z += delta;
    camera[Actor::Property::POSITION_Z] = z;
    return true;
  }

  void OnKeyEv(const Dali::KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      switch(event.GetKeyCode())
      {
        case KeyModifier::CONTROL_L:
        case KeyModifier::CONTROL_R:
        {
          mCtrlDown = true;
          break;
        }
        case KeyModifier::ALT_L:
        case KeyModifier::ALT_R:
        {
          mAltDown = true;
          break;
        }
        case KeyModifier::SHIFT_L:
        case KeyModifier::SHIFT_R:
        {
          mShiftDown = true;
          break;
        }
        default:
        {
          if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
          {
            mApplication.Quit();
          }
          else if(!event.GetKeyString().compare(" "))
          {
            if(mIntegrationState == PhysicsAdaptor::IntegrationState::ON)
            {
              mIntegrationState = PhysicsAdaptor::IntegrationState::OFF;
            }
            else
            {
              mIntegrationState = PhysicsAdaptor::IntegrationState::ON;
            }
            mPhysicsAdaptor.SetIntegrationState(mIntegrationState);
          }
          else if(!event.GetKeyString().compare("m"))
          {
            if(mDebugState == PhysicsAdaptor::DebugState::ON)
            {
              mDebugState = PhysicsAdaptor::DebugState::OFF;
            }
            else
            {
              mDebugState = PhysicsAdaptor::DebugState::ON;
              if(!mPhysicsDebugLayer)
              {
                mPhysicsDebugLayer = mPhysicsAdaptor.CreateDebugLayer(mWindow);
              }
            }
            mPhysicsAdaptor.SetDebugState(mDebugState);
          }
          else if(!event.GetKeyString().compare("w"))
          {
            Vector3 pos = mSelectedActor.GetActorPosition();
            mSelectedActor.AsyncSetPhysicsPosition(pos + Vector3(0, 10, 0));
            btRigidBody* body = mSelectedActor.GetBody().Get<btRigidBody*>();
            mPhysicsAdaptor.Queue([body]() { body->activate(true); });
            mPhysicsAdaptor.CreateSyncPoint();
          }
          else if(!event.GetKeyString().compare("s"))
          {
            Vector3 pos = mSelectedActor.GetActorPosition();
            mSelectedActor.AsyncSetPhysicsPosition(pos + Vector3(0, -10, 0));
            btRigidBody* body = mSelectedActor.GetBody().Get<btRigidBody*>();
            mPhysicsAdaptor.Queue([body]() { body->activate(true); });
            mPhysicsAdaptor.CreateSyncPoint();
          }
          else if(!event.GetKeyString().compare("a"))
          {
            Vector3 pos = mSelectedActor.GetActorPosition();
            mSelectedActor.AsyncSetPhysicsPosition(pos + Vector3(-10, 0, 0));
            btRigidBody* body = mSelectedActor.GetBody().Get<btRigidBody*>();
            mPhysicsAdaptor.Queue([body]() { body->activate(true); });
            mPhysicsAdaptor.CreateSyncPoint();
          }
          else if(!event.GetKeyString().compare("d"))
          {
            Vector3 pos = mSelectedActor.GetActorPosition();
            mSelectedActor.AsyncSetPhysicsPosition(pos + Vector3(10, 0, 0));
            btRigidBody* body = mSelectedActor.GetBody().Get<btRigidBody*>();
            mPhysicsAdaptor.Queue([body]() { body->activate(true); });
            mPhysicsAdaptor.CreateSyncPoint();
          }
          else if(!event.GetKeyString().compare("q"))
          {
            Quaternion quat = mSelectedActor.GetActorRotation();
            quat *= Quaternion(Radian(-0.1f), Vector3::ZAXIS);
            mSelectedActor.AsyncSetPhysicsRotation(quat);
            btRigidBody* body = mSelectedActor.GetBody().Get<btRigidBody*>();
            mPhysicsAdaptor.Queue([body]() { body->activate(true); });
            mPhysicsAdaptor.CreateSyncPoint();
          }
          else if(!event.GetKeyString().compare("e"))
          {
            Quaternion quat = mSelectedActor.GetActorRotation();
            quat *= Quaternion(Radian(0.1f), Vector3::ZAXIS);
            mSelectedActor.AsyncSetPhysicsRotation(quat);
            btRigidBody* body = mSelectedActor.GetBody().Get<btRigidBody*>();
            mPhysicsAdaptor.Queue([body]() { body->activate(true); });
            mPhysicsAdaptor.CreateSyncPoint();
          }
          else if(!event.GetKeyString().compare("z"))
          {
            Quaternion quat = mSelectedActor.GetActorRotation();
            quat *= Quaternion(Radian(-0.1f), Vector3::YAXIS);
            mSelectedActor.AsyncSetPhysicsRotation(quat);
            btRigidBody* body = mSelectedActor.GetBody().Get<btRigidBody*>();
            mPhysicsAdaptor.Queue([body]() { body->activate(true); });
            mPhysicsAdaptor.CreateSyncPoint();
          }
          else if(!event.GetKeyString().compare("x"))
          {
            Quaternion quat = mSelectedActor.GetActorRotation();
            quat *= Quaternion(Radian(0.1f), Vector3::YAXIS);
            mSelectedActor.AsyncSetPhysicsRotation(quat);
            btRigidBody* body = mSelectedActor.GetBody().Get<btRigidBody*>();
            mPhysicsAdaptor.Queue([body]() { body->activate(true); });
            mPhysicsAdaptor.CreateSyncPoint();
          }
          else if(!event.GetKeyString().compare("c"))
          {
            Quaternion quat = mSelectedActor.GetActorRotation();
            quat *= Quaternion(Radian(-0.1f), Vector3::XAXIS);
            mSelectedActor.AsyncSetPhysicsRotation(quat);
            btRigidBody* body = mSelectedActor.GetBody().Get<btRigidBody*>();
            mPhysicsAdaptor.Queue([body]() { body->activate(true); });
            mPhysicsAdaptor.CreateSyncPoint();
          }
          else if(!event.GetKeyString().compare("v"))
          {
            Quaternion quat = mSelectedActor.GetActorRotation();
            quat *= Quaternion(Radian(0.1f), Vector3::XAXIS);
            mSelectedActor.AsyncSetPhysicsRotation(quat);
            btRigidBody* body = mSelectedActor.GetBody().Get<btRigidBody*>();
            mPhysicsAdaptor.Queue([body]() { body->activate(true); });
            mPhysicsAdaptor.CreateSyncPoint();
          }
          else if(!event.GetKeyString().compare("p"))
          {
            mSelectedActor.AsyncSetPhysicsPosition(Vector3::ZERO);
            mSelectedActor.AsyncSetPhysicsRotation(Quaternion(Radian(0.001f), Vector3::XAXIS));

            // Example of calling specific function asyncronously:
            btRigidBody* body = mSelectedActor.GetBody().Get<btRigidBody*>();
            mPhysicsAdaptor.Queue([body]() { body->clearForces(); });
            mPhysicsAdaptor.CreateSyncPoint();
          }
          break;
        }
      }
    }
    else if(event.GetState() == KeyEvent::UP)
    {
      switch(event.GetKeyCode())
      {
        case KeyModifier::CONTROL_L:
        case KeyModifier::CONTROL_R:
        {
          mCtrlDown = false;
          break;
        }
        case KeyModifier::ALT_L:
        case KeyModifier::ALT_R:
        {
          mAltDown = false;
          break;
        }
        case KeyModifier::SHIFT_L:
        case KeyModifier::SHIFT_R:
        {
          mShiftDown = false;
          break;
        }
      }
    }

    static int reduceSpam = 0;
    ++reduceSpam;
    if(reduceSpam == 2)
    {
      reduceSpam = 0;
      std::cout << "Dali pos: " << mBrick.GetActorPosition() << "\nDali rot: " << mBrick.GetActorRotation() << "\nPhys pos: " << mBrick.GetPhysicsPosition() << "\nPhys rot: " << mBrick.GetPhysicsRotation() << "\n";
    }
  }

private:
  Application& mApplication;
  Window       mWindow;

  Matrix         mPhysicsTransform;
  PhysicsAdaptor mPhysicsAdaptor;
  Actor          mPhysicsRoot;
  Layer          mPhysicsDebugLayer;
  PhysicsActor   mBrick;
  PhysicsActor   mSelectedActor;

  btRigidBody*                     mPickedBody{nullptr};
  float                            mOldPickingDistance{0.0f};
  int                              mPickedSavedState{0};
  btTypedConstraint*               mPickedConstraint{nullptr};
  PhysicsAdaptor::IntegrationState mIntegrationState{PhysicsAdaptor::IntegrationState::ON};
  PhysicsAdaptor::DebugState       mDebugState{PhysicsAdaptor::DebugState::OFF};

  bool mCtrlDown{false};
  bool mAltDown{false};
  bool mShiftDown{false};
};

int main(int argc, char** argv)
{
  Application           application = Application::New(&argc, &argv);
  PhysicsDemoController controller(application);
  application.MainLoop();
  return 0;
}
