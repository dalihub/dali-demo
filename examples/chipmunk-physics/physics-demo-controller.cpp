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

#include <dali-physics/dali-physics.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali/devel-api/adaptor-framework/key-devel.h>
#include <dali/devel-api/events/hit-test-algorithm.h>
#include <dali/integration-api/debug.h>

#include <chipmunk/chipmunk.h>
#include <iostream>
#include <string>
#include "letter-a.h"
#include "letter-d.h"
#include "letter-i.h"
#include "letter-l.h"
#include "split-letter-d.h"

using namespace Dali;
using namespace Dali::Toolkit::Physics;

#if defined(DEBUG_ENABLED)
Debug::Filter* gPhysicsDemo = Debug::Filter::New(Debug::Concise, false, "LOG_PHYSICS_EXAMPLE");
#endif

const bool DEBUG_STATE{false};

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

const std::string LETTER_IMAGES[4] = {
  DEMO_IMAGE_DIR "/dali-logo-d.png",
  DEMO_IMAGE_DIR "/dali-logo-a.png",
  DEMO_IMAGE_DIR "/dali-logo-l.png",
  DEMO_IMAGE_DIR "/dali-logo-i.png"};

const std::string BRICK_WALL = DEMO_IMAGE_DIR "/brick-wall.jpg";
const std::string BALL_IMAGE = DEMO_IMAGE_DIR "/blocks-ball.png";

#if defined(_ARCH_ARM_)
#define DEMO_ICON_DIR "/usr/share/icons"
#else
#define DEMO_ICON_DIR DEMO_IMAGE_DIR
#endif

const std::string BALL_IMAGES[] = {DEMO_IMAGE_DIR "/blocks-ball.png",
                                   DEMO_ICON_DIR "/dali-tests.png",
                                   DEMO_ICON_DIR "/dali-examples.png",
                                   DEMO_ICON_DIR "/com.samsung.dali-demo.png"};

const std::string LOGO          = DEMO_IMAGE_DIR "/Logo-for-demo.png";
const std::string BRICK_URIS[4] = {
  DEMO_IMAGE_DIR "/blocks-brick-1.png",
  DEMO_IMAGE_DIR "/blocks-brick-2.png",
  DEMO_IMAGE_DIR "/blocks-brick-3.png",
  DEMO_IMAGE_DIR "/blocks-brick-4.png"};

using Verts = double*;

const Verts LETTER_VERTICES[] = {&letter_d0[0], &letter_d1[0], &letter_a[0], &letter_l[0], &letter_i[0]};
//const Verts LETTER_VERTS[4] = {&letter_d[0], &letter_a[0], &letter_l[0], &letter_i[0]};
const size_t NUMBER_OF_VERTICES[] = {
  sizeof(letter_d0) / (2 * sizeof(double)),
  sizeof(letter_d1) / (2 * sizeof(double)),
  sizeof(letter_a) / (2 * sizeof(double)),
  sizeof(letter_l) / (2 * sizeof(double)),
  sizeof(letter_i) / (2 * sizeof(double))};

// Indexed by letter - index into VERTICES / NUMBER_OF_VERTICES arrays
const std::vector<int> LETTER_SHAPE_INDEXES[]{{0, 1}, {2}, {3}, {4}};

// Groups that can collide with each other:
const cpGroup BALL_GROUP{1 << 0};
const cpGroup LETTER_GROUP_1{1 << 1};
const cpGroup LETTER_GROUP_2{1 << 2};
const cpGroup LETTER_GROUP_3{1 << 3};
const cpGroup LETTER_GROUP_4{1 << 4};
const cpGroup BOUNDS_GROUP{1 << 5};

const cpBitmask COLLISION_MASK{0x3F};

const cpBitmask BALL_COLLIDES_WITH{BALL_GROUP | LETTER_GROUP_1 | LETTER_GROUP_2 | LETTER_GROUP_3 | LETTER_GROUP_4 | BOUNDS_GROUP};
const cpBitmask LETTER_1_COLLIDES_WITH{BALL_GROUP | LETTER_GROUP_2 | BOUNDS_GROUP};
const cpBitmask LETTER_2_COLLIDES_WITH{BALL_GROUP | LETTER_GROUP_1 | LETTER_GROUP_3};
const cpBitmask LETTER_3_COLLIDES_WITH{BALL_GROUP | LETTER_GROUP_2 | LETTER_GROUP_4};
const cpBitmask LETTER_4_COLLIDES_WITH{BALL_GROUP | LETTER_GROUP_3 | BOUNDS_GROUP};

static cpFloat SpringForce(cpConstraint* spring, cpFloat distance)
{
  cpFloat clamp = 20.0f;
  return cpfclamp(cpDampedSpringGetRestLength(spring) - distance, -clamp, clamp) *
         cpDampedSpringGetStiffness(spring);
}

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
    mPhysicsRoot    = mPhysicsAdaptor.GetRootActor();
    mPhysicsRoot.TouchedSignal().Connect(this, &PhysicsDemoController::OnTouched);

    mWindow.Add(mPhysicsRoot);
    mPopcornTimer = Timer::New(7000);
    mPopcornTimer.TickSignal().Connect(this, &PhysicsDemoController::OnPopcornTick);
    mPopcornTimer.Start();

    auto     scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor();
    cpSpace* space          = scopedAccessor->GetNative().Get<cpSpace*>();

    CreateBounds(space, windowSize);

    // Ball area = 2*PI*26^2 ~= 6.28*26*26 ~= 5400
    // Fill top quarter of the screen...
    int numBalls = 10 + windowSize.GetWidth() * windowSize.GetHeight() / 20000;
    for(int i = 0; i < numBalls; ++i)
    {
      mBalls.push_back(CreateBall(space));
    }
    //AddSprings(space);
    //CreateLogo(space);
    CreateLetters(space);

    // For funky mouse drag
    mMouseBody = cpBodyNewKinematic(); // Mouse actor is a kinematic body that is not integrated

    // Process any async queued methods next frame
    mPhysicsAdaptor.CreateSyncPoint();

    if(DEBUG_STATE)
    {
      mPhysicsDebugLayer = mPhysicsAdaptor.CreateDebugLayer(mWindow);
      mPhysicsAdaptor.SetDebugState(PhysicsAdaptor::DebugState::ON);
    }
  }

  PhysicsActor CreateBall(cpSpace* space)
  {
    const float BALL_MASS       = 10.0f;
    const float BALL_RADIUS     = 13.0f;
    const float BALL_ELASTICITY = 0.5f;
    const float BALL_FRICTION   = 0.5f;

    auto ball                   = Toolkit::ImageView::New(BALL_IMAGES[rand() % 4]);
    ball[Actor::Property::NAME] = "Ball";
    ball[Actor::Property::SIZE] = Vector2(26, 26); // Halve the image size
    cpBody* body                = cpSpaceAddBody(space, cpBodyNew(BALL_MASS, cpMomentForCircle(BALL_MASS, 0.0f, BALL_RADIUS, cpvzero)));

    cpShape* shape = cpSpaceAddShape(space, cpCircleShapeNew(body, BALL_RADIUS, cpvzero));
    cpShapeSetElasticity(shape, BALL_ELASTICITY);
    cpShapeSetFriction(shape, BALL_FRICTION);
    //cpShapeSetFilter(shape, cpShapeFilterNew(BALL_GROUP, BALL_COLLIDES_WITH, COLLISION_MASK));
    ball.RegisterProperty("uBrightness", 0.0f);

    PhysicsActor physicsBall = mPhysicsAdaptor.AddActorBody(ball, body);

    Window::WindowSize windowSize = mWindow.GetSize();

    const float fw = 0.5f * (windowSize.GetWidth() - BALL_RADIUS);
    const float fh = 0.5f * (windowSize.GetHeight() - BALL_RADIUS);

    // Example of setting physics property on update thread
    physicsBall.AsyncSetPhysicsPosition(Vector3(Random::Range(-fw, fw), Random::Range(-fh, -fh * 0.5), 0.0f));

    // Example of queuing a chipmunk method to run on the update thread
    mPhysicsAdaptor.Queue([body]() {
      cpBodySetVelocity(body, cpv(Random::Range(-100.0, 100.0), Random::Range(-100.0, 100.0)));
    });
    return physicsBall;
  }

  void CreateLogo(cpSpace* space)
  {
    const float MASS = 20.0f;
    auto        logo = Toolkit::ImageView::New(LOGO);
    Vector2     logoSize{368, 208};
    logo[Actor::Property::SIZE] = logoSize; // Double in size

    cpBody*      logoBody  = cpSpaceAddBody(space, cpBodyNew(MASS, cpMomentForBox(MASS, logoSize.width, logoSize.height)));
    cpShape*     logoShape = cpSpaceAddShape(space, cpBoxShapeNew(logoBody, logoSize.width, logoSize.height, 0.0));
    PhysicsActor logoActor = mPhysicsAdaptor.AddActorBody(logo, logoBody);

    cpShapeSetFriction(logoShape, 0.9);
    cpShapeSetElasticity(logoShape, 0.0);
    Window::WindowSize windowSize = mWindow.GetSize();
    Vector3            daliPos(0, -windowSize.GetHeight() / 2 + logoSize.height * 1.3, 0);
    Vector3            physPos = mPhysicsAdaptor.TranslateToPhysicsSpace(daliPos);
    cpBodySetPosition(logoBody, cpv(physPos.x, physPos.y));

    cpBody*       staticBody = cpSpaceGetStaticBody(space);
    cpConstraint* spring     = NewSpring(staticBody, logoBody, cpv(0, 0), cpv(0, logoSize.height / 2));
    cpSpaceAddConstraint(space, spring);
  }

  void CreateLetters(cpSpace* space)
  {
    const float LETTER_MASS = 10.0f;
    const float RADIUS      = 85.0f;
    const float ELASTICITY  = 0.0f;
    const float FRICTION    = 0.9f;

    static const cpShapeFilter FILTERS[4] = {
      cpShapeFilterNew(LETTER_GROUP_1, LETTER_1_COLLIDES_WITH, COLLISION_MASK),
      cpShapeFilterNew(LETTER_GROUP_2, LETTER_2_COLLIDES_WITH, COLLISION_MASK),
      cpShapeFilterNew(LETTER_GROUP_3, LETTER_3_COLLIDES_WITH, COLLISION_MASK),
      cpShapeFilterNew(LETTER_GROUP_4, LETTER_4_COLLIDES_WITH, COLLISION_MASK)};

    static const std::string NAME[4] = {"d", "a", "l", "i"};
    for(int index = 0; index < 4; ++index)
    {
      auto letter                   = Toolkit::ImageView::New(LETTER_IMAGES[index]);
      letter[Actor::Property::NAME] = NAME[index];

      cpBody* body = cpSpaceAddBody(space, cpBodyNew(LETTER_MASS, cpMomentForCircle(LETTER_MASS, 0.0f, RADIUS, cpvzero)));

      for(size_t letterShapeIndex = 0; letterShapeIndex < LETTER_SHAPE_INDEXES[index].size(); ++letterShapeIndex)
      {
        size_t shapeIndex = LETTER_SHAPE_INDEXES[index][letterShapeIndex];

        std::vector<cpVect> scaledVerts;
        size_t              numberOfElements = NUMBER_OF_VERTICES[shapeIndex];
        scaledVerts.resize(numberOfElements);
        for(size_t i = 0; i < numberOfElements; ++i)
        {
          double x       = LETTER_VERTICES[shapeIndex][i * 2 + 0];
          double y       = LETTER_VERTICES[shapeIndex][i * 2 + 1];
          scaledVerts[i] = cpv(x * 122.0f, y * 171.0f); // Verts are normalized to +-0.5
        }
        cpFloat bevel = 1.0;

        cpShape* shape = cpSpaceAddShape(space, cpPolyShapeNew(body, numberOfElements, &scaledVerts[0], cpTransformIdentity, bevel));
        cpShapeSetElasticity(shape, ELASTICITY);
        cpShapeSetFriction(shape, FRICTION);
        cpShapeSetFilter(shape, FILTERS[index]);
      }

      PhysicsActor physicsLetter = mPhysicsAdaptor.AddActorBody(letter, body);

      Window::WindowSize windowSize = mWindow.GetSize();

      // Image is 326x171; center of letter is guessed; each image contains only 1 image.
      // Position the letters into the window

      float   cellW   = (windowSize.GetWidth() - 170) / 4;
      float   cellC   = -windowSize.GetWidth() * 0.5f + cellW * (0.5f + index);
      float   x       = 85 + cellC; // - 61.0f;
      Vector3 physPos = mPhysicsAdaptor.TranslateToPhysicsSpace(Vector3(x, 0, 0.0f));

      // Have to set position before setting constraint
      cpBodySetPosition(body, cpv(physPos.x, physPos.y));

      // Add a fixed pivot at top of shape
      cpBody* staticBody = cpSpaceGetStaticBody(space);

      Vector3 localPivot(x, -70.0f, 0.0f);
      Vector3 pivotPhys = mPhysicsAdaptor.TranslateToPhysicsSpace(localPivot);
      cpSpaceAddConstraint(space, cpPivotJointNew(staticBody, body, cpv(pivotPhys.x, pivotPhys.y)));
    }
  }

  cpConstraint* NewSpring(cpBody* body1, cpBody* body2, cpVect anchor1, cpVect anchor2)
  {
    const cpFloat STIFFNESS{100.0f};
    const cpFloat DAMPING{0.5f};
    cpConstraint* spring = cpDampedSpringNew(body1, body2, anchor1, anchor2, 0.0f, STIFFNESS, DAMPING);
    cpDampedSpringSetSpringForceFunc(spring, SpringForce);
    return spring;
  }

  void AddSprings(cpSpace* space)
  {
    int N         = mBalls.size();
    int randValue = 3 + rand() % (N / 4); // Some number of pairs
    for(int i = 0; i < randValue; ++i)
    {
      int     randIndex = rand() % N;
      cpBody* body1     = mBalls[randIndex].GetBody().Get<cpBody*>();
      cpBody* body2     = mBalls[(randIndex + 1) % N].GetBody().Get<cpBody*>();

      cpConstraint* spring = NewSpring(body1, body2, cpv(25, 0), cpv(-25, 0));
      cpSpaceAddConstraint(space, spring);
    }
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

    cpShapeSetFilter(shape, cpShapeFilterNew(BOUNDS_GROUP, COLLISION_MASK, COLLISION_MASK));
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
    auto     scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor();
    cpSpace* space          = scopedAccessor->GetNative().Get<cpSpace*>();

    CreateBounds(space, newSize);
  }

  bool OnPopcornTick()
  {
    auto scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor();

    // fire off N random balls upwards with a high impulse
    int N         = mBalls.size();
    int randValue = 10 + rand() % (N / 2);

    for(int i = 0; i < randValue; ++i)
    {
      int     randIndex = rand() % N;
      cpBody* body      = mBalls[randIndex].GetBody().Get<cpBody*>();
      cpBodyActivate(body);
      cpBodyApplyImpulseAtLocalPoint(body, cpv(rand() % 200 - 100, -10000), cpv(0, 25));
    }
    return true;
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
    auto    windowSize = mWindow.GetSize();
    Vector3 rayPhysicsOrigin(screenCoords.x, windowSize.GetHeight() - screenCoords.y, 0.0f);

    switch(state)
    {
      case None:
      {
        if(touch.GetState(0) == Dali::PointState::STARTED)
        {
          state = MovePivot;

          auto     scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor();
          cpSpace* space          = scopedAccessor->GetNative().Get<cpSpace*>();

          Vector3 localPivot;
          float   pickingDistance;

          cpShapeFilter ballFilter{CP_NO_GROUP, 1u << 31, 1u << 31};
          auto          body = scopedAccessor->HitTest(rayPhysicsOrigin, rayPhysicsOrigin, ballFilter, localPivot, pickingDistance);
          if(body.Empty())
          {
            cpShapeFilter letterFilter{CP_NO_GROUP, COLLISION_MASK, COLLISION_MASK};
            body = scopedAccessor->HitTest(rayPhysicsOrigin, rayPhysicsOrigin, letterFilter, localPivot, pickingDistance);
          }
          if(!body.Empty())
          {
            mPickedBody    = body.Get<cpBody*>();
            mSelectedActor = mPhysicsAdaptor.GetPhysicsActor(mPickedBody);
            std::cout << "PhysicsActor: " << mPhysicsAdaptor.GetRootActor().FindChildById(mSelectedActor.GetId()).GetProperty<std::string>(Actor::Property::NAME) << std::endl;

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
            auto     scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor();
            cpSpace* space          = scopedAccessor->GetNative().Get<cpSpace*>();

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
    static bool debugState{DEBUG_STATE};

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
            integrateState = true ^ integrateState;
            mPhysicsAdaptor.SetIntegrationState(integrateState ? PhysicsAdaptor::IntegrationState::ON : PhysicsAdaptor::IntegrationState::OFF);
          }
          else if(!event.GetKeyString().compare("m"))
          {
            debugState = true ^ debugState;
            if(debugState && !mPhysicsDebugLayer)
            {
              mPhysicsDebugLayer = mPhysicsAdaptor.CreateDebugLayer(mWindow);
            }
            mPhysicsAdaptor.SetDebugState(debugState ? PhysicsAdaptor::DebugState::ON : PhysicsAdaptor::DebugState::OFF);
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
              // A negative angle should rotate anti-clockwise, which it does,
              // BUT, we mirror in Y axis, so actually, it LOOKS like it rotates clockwise.
              // So, we have to invert angle.

              cpBody* body  = mSelectedActor.GetBody().Get<cpBody*>();
              float   angle = cpBodyGetAngle(body);
              mPhysicsAdaptor.Queue([body, angle]() { cpBodySetAngle(body, angle + Math::PI / 12.0f); });
              mPhysicsAdaptor.Queue([body]() { cpBodyActivate(body); });
              mPhysicsAdaptor.CreateSyncPoint();
            }
          }
          else if(!event.GetKeyString().compare("e"))
          {
            // Rotate clockwise using native physics APIs
            if(mSelectedActor)
            {
              cpBody* body  = mSelectedActor.GetBody().Get<cpBody*>();
              float   angle = cpBodyGetAngle(body);
              mPhysicsAdaptor.Queue([body, angle]() { cpBodySetAngle(body, angle - Math::PI / 12.0f); });
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

  PhysicsAdaptor            mPhysicsAdaptor;
  PhysicsActor              mSelectedActor;
  std::vector<PhysicsActor> mBalls;
  Matrix                    mPhysicsTransform;
  Actor                     mPhysicsRoot;
  Layer                     mPhysicsDebugLayer;
  cpBody*                   mMouseBody{nullptr};
  cpBody*                   mPickedBody{nullptr};
  cpConstraint*             mPickedConstraint{nullptr};
  int                       mPickedSavedState = -1; /// 0 : Active, 1 : Sleeping
  Timer                     mPopcornTimer;

  PhysicsAdaptor::DebugState mDebugState{PhysicsAdaptor::DebugState::OFF};

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
