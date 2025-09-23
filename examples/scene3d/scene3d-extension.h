/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/dali-toolkit.h>
#include <dali/dali.h>
#include <cstring>

class Scene3DExtension : public Dali::ConnectionTracker
{
public:
  Scene3DExtension()
  : mSceneLoader(nullptr),
    mCurrentAnimationIndex(ANIMATION_IDLE)
  {
  }

  ~Scene3DExtension() = default; // Nothing to do in destructor

  void SetSceneLoader(Scene3DExample* scene3D)
  {
    mSceneLoader = scene3D;
  }

  void ConnectTouchSignals()
  {
    // This is a temporary hack for now to manually connect these signals.
    // We should connect these signals automatically when loading the scene.

    if(mSceneLoader)
    {
      ConnectTouchSignal(ICON_IDLE);
      ConnectTouchSignal(ICON_SQUAT);
      ConnectTouchSignal(ICON_JUMPING_JACK);
      ConnectTouchSignal(ICON_LUNGE);
    }
  }

  void OnKey(const Dali::KeyEvent& e)
  {
    // This is a temporary hack for now to manually handle these key events.
    // We should links them to the animations automatically when loading the scene.

    switch(e.GetKeyCode())
    {
      case KEY_ONE:
      {
        PlaySceneAnimation(ANIMATION_IDLE);
        break;
      }
      case KEY_TWO:
      {
        PlaySceneAnimation(ANIMATION_IDLE_TO_SQUAT);
        break;
      }
      case KEY_THREE:
      {
        PlaySceneAnimation(ANIMATION_IDLE_TO_JUMPING_JACK);
        break;
      }
      case KEY_FOUR:
      {
        PlaySceneAnimation(ANIMATION_IDLE_TO_LUNGE);
        break;
      }
      default:
        break;
    }
  }

private:
  bool PlaySceneAnimation(unsigned int animationIndex)
  {
    if(mSceneLoader && mSceneLoader->mScene)
    {
      if(mSceneLoader->mCurrentAnimation &&
         mCurrentAnimationIndex != ANIMATION_IDLE &&
         mSceneLoader->mCurrentAnimation.GetState() != Dali::Animation::STOPPED)
      {
        return false;
      }

      if(mSceneLoader->mSceneAnimations.size() > animationIndex)
      {
        mCurrentAnimationIndex          = animationIndex;
        mSceneLoader->mCurrentAnimation = mSceneLoader->mSceneAnimations[animationIndex];
        if(mSceneLoader->mCurrentAnimation.FinishedSignal().GetConnectionCount() == 0)
        {
          mSceneLoader->mCurrentAnimation.FinishedSignal().Connect(this, &Scene3DExtension::OnAnimationFinished);
        }
        mSceneLoader->mCurrentAnimation.Play();
      }
    }

    return true;
  }

  void ConnectTouchSignal(const std::string actorName)
  {
    if(mSceneLoader && mSceneLoader->mScene)
    {
      auto actor = mSceneLoader->mScene.FindChildByName(actorName);
      if(actor)
      {
        actor.TouchedSignal().Connect(this, &Scene3DExtension::OnTouch);
      }
    }
  }

  void OnAnimationFinished(Dali::Animation& source)
  {
    switch(mCurrentAnimationIndex)
    {
      case ANIMATION_IDLE_TO_SQUAT:
      {
        PlaySceneAnimation(ANIMATION_SQUAT_TO_IDLE);
        break;
      }
      case ANIMATION_IDLE_TO_JUMPING_JACK:
      {
        PlaySceneAnimation(ANIMATION_JUMPING_JACK);
        break;
      }
      case ANIMATION_JUMPING_JACK:
      {
        PlaySceneAnimation(ANIMATION_JUMPING_JACK_TO_IDLE);
        break;
      }
      case ANIMATION_IDLE_TO_LUNGE:
      {
        PlaySceneAnimation(ANIMATION_LUNGE);
        break;
      }
      case ANIMATION_LUNGE:
      {
        PlaySceneAnimation(ANIMATION_LUNGE_TO_IDLE);
        break;
      }
      default:
      {
        mCurrentAnimationIndex = ANIMATION_IDLE;
        break;
      }
      break;
    }
  }

  bool OnTouch(Dali::Actor actor, const Dali::TouchEvent& touch)
  {
    bool processed = false;

    if(touch.GetPointCount() > 0)
    {
      if(touch.GetState(0) == Dali::PointState::DOWN)
      {
        auto actorName = actor.GetProperty<std::string>(Dali::Actor::Property::NAME);

        if(ICON_IDLE == actorName)
        {
          processed = PlaySceneAnimation(ANIMATION_IDLE);
        }
        else if(ICON_SQUAT == actorName)
        {
          processed = PlaySceneAnimation(ANIMATION_IDLE_TO_SQUAT);
        }
        else if(ICON_JUMPING_JACK == actorName)
        {
          processed = PlaySceneAnimation(ANIMATION_IDLE_TO_JUMPING_JACK);
        }
        else if(ICON_LUNGE == actorName)
        {
          processed = PlaySceneAnimation(ANIMATION_IDLE_TO_LUNGE);
        }
      }
    }
    return processed;
  }

private:
  static constexpr unsigned int KEY_ONE   = 10;
  static constexpr unsigned int KEY_TWO   = 11;
  static constexpr unsigned int KEY_THREE = 12;
  static constexpr unsigned int KEY_FOUR  = 13;

  // Idle animation
  static constexpr unsigned int ANIMATION_IDLE = 0;
  // Squat animation
  static constexpr unsigned int ANIMATION_IDLE_TO_SQUAT = 3;
  static constexpr unsigned int ANIMATION_SQUAT_TO_IDLE = 15;
  // JumpingJack animation
  static constexpr unsigned int ANIMATION_IDLE_TO_JUMPING_JACK = 1;
  static constexpr unsigned int ANIMATION_JUMPING_JACK         = 5;
  static constexpr unsigned int ANIMATION_JUMPING_JACK_TO_IDLE = 6;
  // Lunge animation
  static constexpr unsigned int ANIMATION_IDLE_TO_LUNGE = 2;
  static constexpr unsigned int ANIMATION_LUNGE         = 9;
  static constexpr unsigned int ANIMATION_LUNGE_TO_IDLE = 10;

  inline static const std::string ICON_IDLE         = "Idle";
  inline static const std::string ICON_SQUAT        = "Squat";
  inline static const std::string ICON_JUMPING_JACK = "JumpingJack";
  inline static const std::string ICON_LUNGE        = "Lunge";

  Scene3DExample* mSceneLoader;
  unsigned int    mCurrentAnimationIndex;
};
