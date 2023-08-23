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
#include <dali-physics/dali-physics.h>

#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali/devel-api/adaptor-framework/key-devel.h>
#include <dali/devel-api/events/hit-test-algorithm.h>
#include <dali/integration-api/debug.h>

#include <iostream>
#include <string>
#include <chipmunk/chipmunk.h>

using namespace Dali;
using namespace Dali::Toolkit::Physics;

#if defined(DEBUG_ENABLED)
Debug::Filter* gPhysicsDemo = Debug::Filter::New(Debug::Concise, false, "LOG_PHYSICS_EXAMPLE");
#endif

#define GRABBABLE_MASK_BIT (1u << 31)
cpShapeFilter NOT_GRABBABLE_FILTER = {CP_NO_GROUP, ~GRABBABLE_MASK_BIT, ~GRABBABLE_MASK_BIT};

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

/**
 * @brief The physics demo using Chipmunk2D APIs.
 */
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

    // Map Physics space (origin bottom left, +ve Y up)
    // to DALi space (origin center, +ve Y down)
    mPhysicsTransform.SetIdentityAndScale(Vector3(1.0f, -1.0f, 1.0f));
    mPhysicsTransform.SetTranslation(Vector3(windowSize.GetWidth() * 0.5f,
                                             windowSize.GetHeight() * 0.5f,
                                             0.0f));

    mPhysicsAdaptor = PhysicsAdaptor::New(mPhysicsTransform, windowSize);
    mPhysicsRoot = mPhysicsAdaptor.GetRootActor();
    mPhysicsRoot.TouchedSignal().Connect(this, &PhysicsDemoController::OnTouched);

    mWindow.Add(mPhysicsRoot);

    auto scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor();
    cpSpace* space = scopedAccessor->GetNative().Get<cpSpace*>();

    CreateBounds(space, windowSize);
    // Ball area = 2*PI*26^2 ~= 6.28*26*26 ~= 5400
    // Fill quarter of the screen...
    int numBalls = 10 + windowSize.GetWidth() * windowSize.GetHeight() / 20000;
    for(int i = 0; i < numBalls; ++i)
    {
      CreateBall(space);
    }

    // For funky mouse drag
    mMouseBody = cpBodyNewKinematic(); // Mouse actor is a kinematic body that is not integrated

    // Process any async queued methods next frame
    mPhysicsAdaptor.CreateSyncPoint();
  }

  void CreateBall(cpSpace* space)
  {
    const float BALL_MASS       = 10.0f;
    const float BALL_RADIUS     = 26.0f;
    const float BALL_ELASTICITY = 0.5f;
    const float BALL_FRICTION   = 0.5f;

    auto ball = Toolkit::ImageView::New(BALL_IMAGE);

    cpBody* body = cpSpaceAddBody(space, cpBodyNew(BALL_MASS, cpMomentForCircle(BALL_MASS, 0.0f, BALL_RADIUS, cpvzero)));

    cpShape* shape = cpSpaceAddShape(space, cpCircleShapeNew(body, BALL_RADIUS, cpvzero));
    cpShapeSetElasticity(shape, BALL_ELASTICITY);
    cpShapeSetFriction(shape, BALL_FRICTION);

    ball.RegisterProperty("uBrightness", 0.0f);

    PhysicsActor physicsBall = mPhysicsAdaptor.AddActorBody(ball, body);

    Window::WindowSize windowSize  = mWindow.GetSize();

    const float        fw          = 0.5f*(windowSize.GetWidth() - BALL_RADIUS);
    const float        fh          = 0.5f*(windowSize.GetHeight() - BALL_RADIUS);

    // Example of setting physics property on update thread
    physicsBall.AsyncSetPhysicsPosition(Vector3(Random::Range(-fw, fw), Random::Range(-fh, fh), 0.0f));

    // Example of queuing a chipmunk method to run on the update thread
    mPhysicsAdaptor.Queue([body](){
      cpBodySetVelocity(body, cpv(Random::Range(-100.0, 100.0), Random::Range(-100.0, 100.0)));
    });
  }

  void CreateBounds(cpSpace* space, Window::WindowSize size)
  {
    // We're working in physics space here - coords are: origin: bottom left, +ve Y: up
    int xBound = size.GetWidth();
    int yBound = size.GetHeight();

    cpBody* staticBody = cpSpaceGetStaticBody(space);

    if(mLeftBound)
    {
      cpSpaceRemoveShape(space, mLeftBound);
      cpSpaceRemoveShape(space, mRightBound);
      cpSpaceRemoveShape(space, mTopBound);
      cpSpaceRemoveShape(space, mBottomBound);
      cpShapeFree(mLeftBound);
      cpShapeFree(mRightBound);
      cpShapeFree(mTopBound);
      cpShapeFree(mBottomBound);
    }
    mLeftBound   = AddBound(space, staticBody, cpv(0, 0), cpv(0, yBound));
    mRightBound  = AddBound(space, staticBody, cpv(xBound, 0), cpv(xBound, yBound));
    mTopBound    = AddBound(space, staticBody, cpv(0, 0), cpv(xBound, 0));
    mBottomBound = AddBound(space, staticBody, cpv(0, yBound), cpv(xBound, yBound));
  }

  cpShape* AddBound(cpSpace* space, cpBody* staticBody, cpVect start, cpVect end)
  {
    cpShape* shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, start, end, 0.0f));
    cpShapeSetElasticity(shape, 1.0f);
    cpShapeSetFriction(shape, 1.0f);

    cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);
    return shape;
  }

  void MoveMouseBody(cpBody* mouseBody, Vector3 position)
  {
    cpVect cpPosition = cpv(position.x, position.y);
    cpVect newPoint   = cpvlerp(cpBodyGetPosition(mouseBody), cpPosition, 0.25f);
    cpBodySetVelocity(mouseBody, cpvmult(cpvsub(newPoint, cpBodyGetPosition(mouseBody)), 60.0f));
    cpBodySetPosition(mouseBody, newPoint);
  }

  cpConstraint* AddPivotJoint(cpSpace* space, cpBody* body1, cpBody* body2, Vector3 localPivot)
  {
    cpVect        pivot{localPivot.x, localPivot.y};
    cpConstraint* joint = cpPivotJointNew2(body2, body1, cpvzero, pivot);
    cpConstraintSetMaxForce(joint, 50000.0f); // Magic numbers for mouse feedback.
    cpConstraintSetErrorBias(joint, cpfpow(1.0f - 0.15f, 60.0f));
    cpConstraint* constraint = cpSpaceAddConstraint(space, joint);
    return constraint; // Constraint & joint are the same...
  }

  void OnTerminate(Application& application)
  {
    UnparentAndReset(mPhysicsRoot);
  }

  void OnWindowResize(Window window, Window::WindowSize newSize)
  {
    auto scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor();
    cpSpace* space = scopedAccessor->GetNative().Get<cpSpace*>();

    CreateBounds(space, newSize);
  }

  bool OnTouched(Dali::Actor actor, const Dali::TouchEvent& touch)
  {
    static enum {
      None,
      MovePivot,
    } state = None;

    auto renderTask   = mWindow.GetRenderTaskList().GetTask(0);
    auto screenCoords = touch.GetScreenPosition(0);
    // In this demo, physics space is equivalent to screen space with y inverted
    auto windowSize = mWindow.GetSize();
    Vector3 rayPhysicsOrigin(screenCoords.x, windowSize.GetHeight() - screenCoords.y, 0.0f);

    switch(state)
    {
      case None:
      {
        if(touch.GetState(0) == Dali::PointState::STARTED)
        {
          state = MovePivot;

          auto scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor();
          cpSpace* space = scopedAccessor->GetNative().Get<cpSpace*>();

          Vector3 localPivot;
          float   pickingDistance;

          auto body = scopedAccessor->HitTest(rayPhysicsOrigin, rayPhysicsOrigin, localPivot, pickingDistance);
          if(!body.Empty())
          {
            mPickedBody = body.Get<cpBody*>();
            mSelectedActor = mPhysicsAdaptor.GetPhysicsActor(mPickedBody);

            mPickedSavedState = cpBodyIsSleeping(mPickedBody);
            cpBodyActivate(mPickedBody);
            mPickedConstraint = AddPivotJoint(space, mPickedBody, mMouseBody, localPivot);
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
            // Ensure we get a lock before altering constraints
            auto scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor();

            // Move point in physics coords
            MoveMouseBody(mMouseBody, rayPhysicsOrigin);
          }
        }
        else if(touch.GetState(0) == Dali::PointState::FINISHED ||
                touch.GetState(0) == Dali::PointState::INTERRUPTED)
        {
          if(mPickedConstraint)
          {
            auto scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor();
            cpSpace* space = scopedAccessor->GetNative().Get<cpSpace*>();

            if(mPickedSavedState)
            {
              cpBodyActivate(mPickedBody);
            }
            else
            {
              cpBodySleep(mPickedBody);
            }

            cpSpaceRemoveConstraint(space, mPickedConstraint);
            cpConstraintFree(mPickedConstraint);
            mPickedConstraint = nullptr;
            mPickedBody       = nullptr;
          }
          state = None;
        }
        break;
      }
    }


    Stage::GetCurrent().KeepRendering(30.0f);

    return true;
  }

  void OnKeyEv(const Dali::KeyEvent& event)
  {
    static bool integrateState{true};

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
            integrateState = true^integrateState;
            mPhysicsAdaptor.SetIntegrationState(integrateState?
                                                PhysicsAdaptor::IntegrationState::ON:
                                                PhysicsAdaptor::IntegrationState::OFF);

          }
          else if(!event.GetKeyString().compare("w"))
          {
            if(mSelectedActor)
            {
              Vector3 pos = mSelectedActor.GetActorPosition();
              mSelectedActor.AsyncSetPhysicsPosition(pos + Vector3(0, -10, 0));
              cpBody* body = mSelectedActor.GetBody().Get<cpBody*>();
              mPhysicsAdaptor.Queue([body]() { cpBodyActivate(body); });
              mPhysicsAdaptor.CreateSyncPoint();
            }
          }
          else if(!event.GetKeyString().compare("s"))
          {
            if(mSelectedActor)
            {
              Vector3 pos = mSelectedActor.GetActorPosition();
              mSelectedActor.AsyncSetPhysicsPosition(pos + Vector3(0, 10, 0));
              cpBody* body = mSelectedActor.GetBody().Get<cpBody*>();
              mPhysicsAdaptor.Queue([body]() { cpBodyActivate(body); });
              mPhysicsAdaptor.CreateSyncPoint();
            }
          }
          else if(!event.GetKeyString().compare("a"))
          {
            if(mSelectedActor)
            {
              Vector3 pos = mSelectedActor.GetActorPosition();
              mSelectedActor.AsyncSetPhysicsPosition(pos + Vector3(-10, 0, 0));
              cpBody* body = mSelectedActor.GetBody().Get<cpBody*>();
              mPhysicsAdaptor.Queue([body]() { cpBodyActivate(body); });
              mPhysicsAdaptor.CreateSyncPoint();
            }
          }
          else if(!event.GetKeyString().compare("d"))
          {
            if(mSelectedActor)
            {
              Vector3 pos = mSelectedActor.GetActorPosition();
              mSelectedActor.AsyncSetPhysicsPosition(pos + Vector3(10, 0, 0));
              cpBody* body = mSelectedActor.GetBody().Get<cpBody*>();
              mPhysicsAdaptor.Queue([body]() { cpBodyActivate(body); });
              mPhysicsAdaptor.CreateSyncPoint();
            }
          }
          else if(!event.GetKeyString().compare("q"))
          {
            // Rotate anti-clockwise
            if(mSelectedActor)
            {
              Quaternion quaternion = mSelectedActor.GetActorRotation();
              quaternion *= Quaternion(Degree(-15.0f), Vector3::ZAXIS);
              mSelectedActor.AsyncSetPhysicsRotation(quaternion);
              cpBody* body = mSelectedActor.GetBody().Get<cpBody*>();
              mPhysicsAdaptor.Queue([body]() { cpBodyActivate(body); });
              mPhysicsAdaptor.CreateSyncPoint();
            }
          }
          else if(!event.GetKeyString().compare("e"))
          {
            // Rotate clockwise using native physics APIs
            if(mSelectedActor)
            {
              cpBody* body = mSelectedActor.GetBody().Get<cpBody*>();
              float angle = cpBodyGetAngle(body);
              mPhysicsAdaptor.Queue([body, angle]() { cpBodySetAngle(body, angle-Math::PI/12.0f); });
              mPhysicsAdaptor.Queue([body]() { cpBodyActivate(body); });
              mPhysicsAdaptor.CreateSyncPoint();
            }
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
  }

private:
  Application& mApplication;
  Window       mWindow;

  PhysicsAdaptor mPhysicsAdaptor;
  PhysicsActor   mSelectedActor;
  Matrix        mPhysicsTransform;
  Actor         mPhysicsRoot;
  cpBody*       mMouseBody{nullptr};
  cpBody*       mPickedBody{nullptr};
  cpConstraint* mPickedConstraint{nullptr};
  int           mPickedSavedState = -1; /// 0 : Active, 1 : Sleeping

  cpShape* mLeftBound{nullptr};
  cpShape* mRightBound{nullptr};
  cpShape* mTopBound{nullptr};
  cpShape* mBottomBound{nullptr};

  bool mCtrlDown{false};
  bool mAltDown{false};
  bool mShiftDown{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application           application = Application::New(&argc, &argv);
  PhysicsDemoController controller(application);
  application.MainLoop();
  return 0;
}
