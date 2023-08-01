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

#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali/devel-api/adaptor-framework/key-devel.h>
#include <dali/devel-api/events/hit-test-algorithm.h>

#include <iostream>
#include <string>

#include "generated/rendering-textured-shape-frag.h"
#include "generated/rendering-textured-shape-vert.h"
#include "physics-actor.h"
#include "physics-impl.h"

using namespace Dali;

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

    mPhysicsRoot = mPhysicsImpl.Initialize(mWindow);
    mPhysicsRoot.TouchedSignal().Connect(this, &PhysicsDemoController::OnTouched);

    mWindow.Add(mPhysicsRoot);

    CreateBall();
    CreateBrickPyramid(windowSize);

    // For funky mouse drag
    mMouseBody = mPhysicsImpl.AddMouseBody();
  }

  void CreateBall()
  {
    const float BALL_MASS       = 10.0f;
    const float BALL_RADIUS     = 26.0f;
    const float BALL_ELASTICITY = 0.5f;
    const float BALL_FRICTION   = 0.5f;

    Property::Value v{std::string{SHADER_RENDERING_TEXTURED_SHAPE_VERT}};
    Property::Value f{std::string{SHADER_RENDERING_TEXTURED_SHAPE_FRAG}};

    auto image = Property::Map{{Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE},
                               {Toolkit::ImageVisual::Property::URL, BALL_IMAGE},
                               {Toolkit::Visual::Property::SHADER, {{Toolkit::Visual::Shader::Property::FRAGMENT_SHADER, f}}}};

    auto ball                                 = Toolkit::ImageView::New();
    ball[Toolkit::ImageView::Property::IMAGE] = image;
    mPhysicsImpl.AddBall(ball, BALL_MASS, BALL_RADIUS, BALL_ELASTICITY, BALL_FRICTION);
  }

  void CreateBrickPyramid(Dali::Window::WindowSize windowSize)
  {
    const float BRICK_MASS       = 30.0f;
    const float BRICK_ELASTICITY = 0.0f;
    const float BRICK_FRICTION   = 0.9f;
    const int   BRICK_WIDTH      = 128;
    const int   BRICK_HEIGHT     = 64;
    const int   BRICK_GAP        = 8;

    Property::Value v{std::string{SHADER_RENDERING_TEXTURED_SHAPE_VERT}};
    Property::Value f{std::string{SHADER_RENDERING_TEXTURED_SHAPE_FRAG}};

    int uriIndex     = 0;
    int numberOfRows = windowSize.GetWidth() / (BRICK_WIDTH + BRICK_GAP) - 2;
    int oY           = windowSize.GetHeight() - (1 + numberOfRows) * BRICK_HEIGHT;
    for(int i = 0; i < numberOfRows; ++i)
    {
      // Row start: i+1 is brick number. i is gap#
      int w  = (i + 1) * BRICK_WIDTH + i * BRICK_GAP;
      int oX = (windowSize.GetWidth() - w) / 2;
      for(int j = 0; j < i + 1; ++j)
      {
        auto image = Property::Map{{Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE},
                                   {Toolkit::ImageVisual::Property::URL, BRICK_URIS[uriIndex]},
                                   {Toolkit::Visual::Property::SHADER, {{Toolkit::Visual::Shader::Property::FRAGMENT_SHADER, f}}}};

        auto brick                                 = Toolkit::ImageView::New();
        brick[Toolkit::ImageView::Property::IMAGE] = image;
        auto& physicsActor                         = mPhysicsImpl.AddBrick(brick, BRICK_MASS, BRICK_ELASTICITY, BRICK_FRICTION, Vector3(BRICK_WIDTH, BRICK_HEIGHT, BRICK_HEIGHT));
        physicsActor.SetPhysicsPosition(Vector3(oX + j * (BRICK_WIDTH + BRICK_GAP), oY + i * BRICK_HEIGHT, 0.0f));
        uriIndex += 1;
        uriIndex %= 4;
      }
    }
  }
  void OnTerminate(Application& application)
  {
    UnparentAndReset(mPhysicsRoot);
  }

  void OnWindowResize(Window window, Window::WindowSize newSize)
  {
    mPhysicsImpl.CreateWorldBounds(newSize);
  }

  bool OnTouched(Dali::Actor actor, const Dali::TouchEvent& touch)
  {
    static enum {
      None,
      MoveCameraXZ,
      MovePivot,
    } state = None;

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
            state = MovePivot;
            Dali::Mutex::ScopedLock lock(mPhysicsImpl.mMutex);

            Vector3 localPivot;
            float   pickingDistance;
            auto    body = mPhysicsImpl.HitTest(screenCoords, origin, direction, localPivot, pickingDistance);
            if(body)
            {
              mPickedBody = body;
              mPhysicsImpl.HighlightBody(mPickedBody, true);
              mPickedSavedState = mPhysicsImpl.ActivateBody(mPickedBody);
              mPickedConstraint = mPhysicsImpl.AddPivotJoint(mPickedBody, mMouseBody, localPivot);
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
              Dali::Mutex::ScopedLock lock(mPhysicsImpl.mMutex);

              Vector3 position = mPhysicsImpl.TranslateToPhysicsSpace(Vector3(screenCoords));
              mPhysicsImpl.MoveMouseBody(mMouseBody, position);
            }
            else
            {
              // Move point in XZ plane
              // Above vanishing pt, it's on top plane of frustum; below vanishing pt it's on bottom plane.
              // Kind of want to project onto the plane using initial touch xy, rather than top/bottom.
              // Whole new projection code needed.

              // Cheat!
            }
          }
        }
        else if(touch.GetState(0) == Dali::PointState::FINISHED ||
                touch.GetState(0) == Dali::PointState::INTERRUPTED)
        {
          if(mPickedConstraint)
          {
            mPhysicsImpl.HighlightBody(mPickedBody, false);

            Dali::Mutex::ScopedLock lock(mPhysicsImpl.mMutex);
            mPhysicsImpl.RestoreBodyState(mPickedBody, mPickedSavedState);
            mPhysicsImpl.ReleaseConstraint(mPickedConstraint);
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

    //std::cout<<"Touch State: "<<state<<std::endl;
    Stage::GetCurrent().KeepRendering(30.0f);

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
            mPhysicsImpl.ToggleIntegrateState();
          }
          else if(!event.GetKeyString().compare("m"))
          {
            mPhysicsImpl.ToggleDebugState();
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

  PhysicsImpl   mPhysicsImpl;
  Actor         mPhysicsRoot;
  cpBody*       mMouseBody{nullptr};
  cpBody*       mPickedBody{nullptr};
  cpConstraint* mPickedConstraint{nullptr};
  int           mPickedSavedState = -1; /// 0 : Active, 1 : Sleeping

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
