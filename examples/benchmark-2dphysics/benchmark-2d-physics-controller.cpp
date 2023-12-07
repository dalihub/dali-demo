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
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace Dali;
using namespace Dali::Toolkit::Physics;
using namespace Dali::ParentOrigin;
using namespace Dali::AnchorPoint;

#if defined(DEBUG_ENABLED)
Debug::Filter* gPhysicsDemo = Debug::Filter::New(Debug::Concise, false, "LOG_PHYSICS_EXAMPLE");
#endif

const float    MAX_ANIMATION_DURATION{60.0f};
const uint32_t ANIMATION_TIME{30000};
const uint32_t DEFAULT_BALL_COUNT{500};

#if defined(_ARCH_ARM_)
#define DEMO_ICON_DIR "/usr/share/icons"
#else
#define DEMO_ICON_DIR DEMO_IMAGE_DIR
#endif

const std::string BALL_IMAGES[] = {DEMO_IMAGE_DIR "/blocks-ball.png",
                                   DEMO_ICON_DIR "/dali-tests.png",
                                   DEMO_ICON_DIR "/dali-examples.png",
                                   DEMO_ICON_DIR "/com.samsung.dali-demo.png"};

const Vector2 BALL_SIZE{26.0f, 26.0f};

// Groups that can collide with each other:
const cpGroup BALL_GROUP{1 << 0};
const cpGroup BOUNDS_GROUP{1 << 1};

const cpBitmask COLLISION_MASK{0xfF};

const cpBitmask BALL_COLLIDES_WITH{BALL_GROUP | BOUNDS_GROUP};

enum BenchmarkType
{
  ANIMATION,
  PHYSICS_2D,
};

/**
 * @brief The physics demo using Chipmunk2D APIs.
 */
class Physics2dBenchmarkController : public ConnectionTracker
{
public:
  Physics2dBenchmarkController(Application& app, BenchmarkType startType, int numberOfBalls)
  : mApplication(app),
    mType(startType),
    mBallNumber(numberOfBalls)
  {
    app.InitSignal().Connect(this, &Physics2dBenchmarkController::OnInit);
    app.TerminateSignal().Connect(this, &Physics2dBenchmarkController::OnTerminate);
  }

  ~Physics2dBenchmarkController() = default;

  void OnInit(Application& application)
  {
    mWindow = application.GetWindow();
    mWindow.ResizeSignal().Connect(this, &Physics2dBenchmarkController::OnWindowResize);
    mWindow.KeyEventSignal().Connect(this, &Physics2dBenchmarkController::OnKeyEv);
    mWindow.GetRootLayer().TouchedSignal().Connect(this, &Physics2dBenchmarkController::OnTouched);
    mWindow.SetBackgroundColor(Color::DARK_SLATE_GRAY);

    CreateSimulation();

    mTimer = Timer::New(ANIMATION_TIME);
    mTimer.TickSignal().Connect(this, &Physics2dBenchmarkController::AnimationSimFinished);
    mTimer.Start();
  }

  void CreateSimulation()
  {
    switch(mType)
    {
      case BenchmarkType::ANIMATION:
      default:
      {
        DALI_LOG_ERROR("CreateAnimationSimulation\n");
        CreateAnimationSimulation();
        break;
      }
      case BenchmarkType::PHYSICS_2D:
      {
        DALI_LOG_ERROR("CreatePhysicsSimulation\n");
        CreatePhysicsSimulation();
        break;
      }
    }
  }

  bool AnimationSimFinished()
  {
    switch(mType)
    {
      case BenchmarkType::ANIMATION:
      default:
      {
        UnparentAndReset(mAnimationSimRootActor);
        for(auto&& animation : mBallAnimations)
        {
          animation.Stop();
          animation.Clear();
        }
        mBallAnimations.clear();

        mType = BenchmarkType::PHYSICS_2D;

        CreateSimulation();
        return true;
      }
      case BenchmarkType::PHYSICS_2D:
      {
        mApplication.Quit();
        break;
      }
    }
    return false;
  }

  void OnTerminate(Application& application)
  {
    UnparentAndReset(mAnimationSimRootActor);
    UnparentAndReset(mPhysicsRoot);
  }

  void OnWindowResize(Window window, Window::WindowSize newSize)
  {
    switch(mType)
    {
      case BenchmarkType::ANIMATION:
      default:
      {
        // TODO : Implement here if you want.
        break;
      }
      case BenchmarkType::PHYSICS_2D:
      {
        if(mPhysicsAdaptor)
        {
          auto     scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor();
          cpSpace* space          = scopedAccessor->GetNative().Get<cpSpace*>();

          CreateBounds(space, newSize);
        }
        break;
      }
    }
  }

  bool OnTouched(Dali::Actor actor, const Dali::TouchEvent& touch)
  {
    mApplication.Quit();
    return false;
  }

  void OnKeyEv(const Dali::KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

  // BenchmarkType::ANIMATION

  void CreateAnimationSimulation()
  {
    DALI_LOG_RELEASE_INFO("Creating animation simulation with %d balls\n", mBallNumber);

    Window::WindowSize windowSize = mWindow.GetSize();
    mBallActors.resize(mBallNumber);
    mBallVelocity.resize(mBallNumber);
    mBallAnimations.resize(mBallNumber);

    mAnimationSimRootActor = Layer::New();
    mAnimationSimRootActor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    mAnimationSimRootActor[Actor::Property::PARENT_ORIGIN] = Dali::ParentOrigin::CENTER;
    mAnimationSimRootActor[Actor::Property::ANCHOR_POINT]  = Dali::AnchorPoint::CENTER;

    mWindow.Add(mAnimationSimRootActor);
    std::ostringstream oss;
    oss << "Animation simulation of " << mBallNumber << " balls";
    auto title = Toolkit::TextLabel::New(oss.str());
    mAnimationSimRootActor.Add(title);
    title[Toolkit::TextLabel::Property::TEXT_COLOR]           = Color::WHITE;
    title[Actor::Property::PARENT_ORIGIN]                     = Dali::ParentOrigin::TOP_CENTER;
    title[Actor::Property::ANCHOR_POINT]                      = Dali::AnchorPoint::TOP_CENTER;
    title[Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT] = HorizontalAlignment::CENTER;
    title.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);

    const float margin(BALL_SIZE.width * 0.5f);

    for(int i = 0; i < mBallNumber; ++i)
    {
      Actor ball = mBallActors[i]          = Toolkit::ImageView::New(BALL_IMAGES[rand() % 4]);
      ball[Actor::Property::PARENT_ORIGIN] = Dali::ParentOrigin::CENTER;
      ball[Actor::Property::ANCHOR_POINT]  = Dali::AnchorPoint::CENTER;

      ball[Actor::Property::NAME]     = "Ball";
      ball[Actor::Property::SIZE]     = BALL_SIZE; // Halve the image size
      int width                       = windowSize.GetWidth() / 2;
      int height                      = windowSize.GetHeight() / 2;
      ball[Actor::Property::POSITION] = Vector3(Random::Range(margin - width, width - margin), Random::Range(margin - height, height - margin), 0.0f);
      ball.RegisterProperty("index", i);
      mAnimationSimRootActor.Add(ball);

      mBallVelocity[i] = Vector3(Random::Range(-25.0f, 25.0f), Random::Range(-25.0f, 25.0f), 0.0f);
      mBallVelocity[i].Normalize();
      mBallVelocity[i] = mBallVelocity[i] * Random::Range(15.0f, 50.0f);

      PropertyNotification leftNotify = mBallActors[i].AddPropertyNotification(Actor::Property::POSITION_X, LessThanCondition(margin - width));
      leftNotify.NotifySignal().Connect(this, &Physics2dBenchmarkController::OnHitLeftWall);

      PropertyNotification rightNotify = mBallActors[i].AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(width - margin));
      rightNotify.NotifySignal().Connect(this, &Physics2dBenchmarkController::OnHitRightWall);

      PropertyNotification topNotify = mBallActors[i].AddPropertyNotification(Actor::Property::POSITION_Y, LessThanCondition(margin - height));
      topNotify.NotifySignal().Connect(this, &Physics2dBenchmarkController::OnHitTopWall);

      PropertyNotification bottomNotify = mBallActors[i].AddPropertyNotification(Actor::Property::POSITION_Y, GreaterThanCondition(height - margin));
      bottomNotify.NotifySignal().Connect(this, &Physics2dBenchmarkController::OnHitBottomWall);

      ContinueAnimation(i);
    }

    title.RaiseToTop();
  }

  void ContinueAnimation(int index)
  {
    if(mBallAnimations[index])
    {
      mBallAnimations[index].Clear();
    }
    mBallAnimations[index] = Animation::New(MAX_ANIMATION_DURATION);
    mBallAnimations[index].AnimateBy(Property(mBallActors[index], Actor::Property::POSITION), mBallVelocity[index] * MAX_ANIMATION_DURATION);
    mBallAnimations[index].Play();
  }

  void OnHitLeftWall(PropertyNotification& source)
  {
    auto actor = Actor::DownCast(source.GetTarget());
    if(actor)
    {
      int index = actor["index"];
      if(mBallVelocity[index].x < 0.0f)
      {
        mBallVelocity[index].x = fabsf(mBallVelocity[index].x);
        ContinueAnimation(index);
      }
    }
  }

  void OnHitRightWall(PropertyNotification& source)
  {
    auto actor = Actor::DownCast(source.GetTarget());
    if(actor)
    {
      int index = actor["index"];
      if(mBallVelocity[index].x > 0.0f)
      {
        mBallVelocity[index].x = -fabsf(mBallVelocity[index].x);
        ContinueAnimation(index);
      }
    }
  }

  void OnHitBottomWall(PropertyNotification& source)
  {
    auto actor = Actor::DownCast(source.GetTarget());
    if(actor)
    {
      int index = actor["index"];
      if(mBallVelocity[index].y > 0.0f)
      {
        mBallVelocity[index].y = -fabsf(mBallVelocity[index].y);
        ContinueAnimation(index);
      }
    }
  }

  void OnHitTopWall(PropertyNotification& source)
  {
    auto actor = Actor::DownCast(source.GetTarget());
    if(actor)
    {
      int index = actor["index"];
      if(mBallVelocity[index].y < 0.0f)
      {
        mBallVelocity[index].y = fabsf(mBallVelocity[index].y);
        ContinueAnimation(index);
      }
    }
  }

  // BenchmarkType::PHYSICS_2D

  void CreatePhysicsSimulation()
  {
    DALI_LOG_RELEASE_INFO("Creating physics simulation with %d balls\n", mBallNumber);

    Window::WindowSize windowSize = mWindow.GetSize();

    // Map Physics space (origin bottom left, +ve Y up)
    // to DALi space (origin center, +ve Y down)
    mPhysicsTransform.SetIdentityAndScale(Vector3(1.0f, -1.0f, 1.0f));
    mPhysicsTransform.SetTranslation(Vector3(windowSize.GetWidth() * 0.5f,
                                             windowSize.GetHeight() * 0.5f,
                                             0.0f));

    mPhysicsAdaptor = PhysicsAdaptor::New(mPhysicsTransform, windowSize);
    mPhysicsRoot    = mPhysicsAdaptor.GetRootActor();
    mWindow.Add(mPhysicsRoot);

    auto     scopedAccessor = mPhysicsAdaptor.GetPhysicsAccessor();
    cpSpace* space          = scopedAccessor->GetNative().Get<cpSpace*>();
    cpSpaceSetGravity(space, cpv(0, 0));

    CreateBounds(space, windowSize);

    for(int i = 0; i < mBallNumber; ++i)
    {
      mBalls.push_back(CreateBall(space));
    }

    // Process any async queued methods next frame
    mPhysicsAdaptor.CreateSyncPoint();

    std::ostringstream oss;
    oss << "Physics simulation of " << mBallNumber << " balls";
    auto title = Toolkit::TextLabel::New(oss.str());
    mPhysicsRoot.Add(title);
    title[Toolkit::TextLabel::Property::TEXT_COLOR]           = Color::WHITE;
    title[Actor::Property::PARENT_ORIGIN]                     = Dali::ParentOrigin::TOP_CENTER;
    title[Actor::Property::ANCHOR_POINT]                      = Dali::AnchorPoint::TOP_CENTER;
    title[Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT] = HorizontalAlignment::CENTER;
    title.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);
    title.RaiseToTop();
  }

  PhysicsActor CreateBall(cpSpace* space)
  {
    Window::WindowSize windowSize = mWindow.GetSize();
    const float BALL_MASS       = 10.0f;
    const float BALL_RADIUS     = BALL_SIZE.x * 0.25f;
    const float BALL_ELASTICITY = 1.0f;
    const float BALL_FRICTION   = 0.0f;

    auto ball                   = Toolkit::ImageView::New(BALL_IMAGES[rand() % 4]);
    ball[Actor::Property::NAME] = "Ball";
    ball[Actor::Property::SIZE] = BALL_SIZE * 0.5f;
    const float moment = cpMomentForCircle(BALL_MASS, 0.0f, BALL_RADIUS, cpvzero);
    cpBody* body = cpBodyNew(BALL_MASS, moment);
    const float fw = (windowSize.GetWidth() - BALL_RADIUS);
    const float fh = (windowSize.GetHeight() - BALL_RADIUS);
    cpBodySetPosition(body, cpv(Random::Range(0, fw), Random::Range(0, fh)));
    cpBodySetVelocity(body, cpv(Random::Range(-100.0, 100.0), Random::Range(-100.0, 100.0)));
    cpSpaceAddBody(space, body);

    cpShape* shape = cpSpaceAddShape(space, cpCircleShapeNew(body, BALL_RADIUS, cpvzero));
    cpShapeSetElasticity(shape, BALL_ELASTICITY);
    cpShapeSetFriction(shape, BALL_FRICTION);

    PhysicsActor physicsBall = mPhysicsAdaptor.AddActorBody(ball, body);

    return physicsBall;
  }

  void CreateBounds(cpSpace* space, Window::WindowSize size)
  {
    // We're working in physics space here - coords are: origin: bottom left, +ve Y: up
    int32_t xBound = static_cast<int32_t>(static_cast<uint32_t>(size.GetWidth()));
    int32_t yBound = static_cast<int32_t>(static_cast<uint32_t>(size.GetHeight()));

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

private:
  Application& mApplication;
  Window       mWindow;

  BenchmarkType mType{BenchmarkType::ANIMATION};

  PhysicsAdaptor            mPhysicsAdaptor;
  std::vector<PhysicsActor> mBalls;
  Matrix                    mPhysicsTransform;
  Actor                     mPhysicsRoot;
  Layer                     mPhysicsDebugLayer;
  Layer                     mAnimationSimRootActor;
  cpShape*                  mLeftBound{nullptr};
  cpShape*                  mRightBound{nullptr};
  cpShape*                  mTopBound{nullptr};
  cpShape*                  mBottomBound{nullptr};

  std::vector<Actor>     mBallActors;
  std::vector<Vector3>   mBallVelocity;
  std::vector<Animation> mBallAnimations;
  int                    mBallNumber;
  Timer                  mTimer;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  setenv("DALI_FPS_TRACKING", "5", 1);
  Application application = Application::New(&argc, &argv);
  BenchmarkType startType = BenchmarkType::ANIMATION;

  int numberOfBalls = DEFAULT_BALL_COUNT;
  int opt=0;
  optind=1;
  while((opt=getopt(argc, argv, "ap")) != -1)
  {
    switch(opt)
    {
      case 'a':
        startType = BenchmarkType::ANIMATION;
        break;
      case 'p':
        startType = BenchmarkType::PHYSICS_2D;
        break;
      case 1:
        // Should only trigger if optstring argument starts with "-", but check it anyway.
        numberOfBalls = atoi(optarg);
        break;
      default:
        fprintf(stderr, "Usage: %s [-p][-a] [n-balls]\n", argv[0]);
        exit(1);
    }
  }
  if(optind < argc)
  {
    numberOfBalls = atoi(argv[optind]);
  }

  Physics2dBenchmarkController controller(application, startType, numberOfBalls);
  application.MainLoop();
  return 0;
}
