#ifndef DALI_DEMO_BUBBLE_ANIMATOR_H
#define DALI_DEMO_BUBBLE_ANIMATOR_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <vector>

/**
 * Creates and animates random sized bubbles
 */
class BubbleAnimator : public Dali::ConnectionTracker
{
public:
  /**
   * @brief Initilizes the bubble background
   *
   * @param parent The actor to add all the bubbles to
   * @param scrollView If provided, does a parallax effect when scrolling using this scroll-view (optional)
   */
  void Initialize(Dali::Actor parent, Dali::Actor scrollView = Dali::Actor());

  /**
   * @brief Plays the bubble animation
   */
  void PlayAnimation();

private:
  /**
   * @brief Used by the timer to pause the animation
   *
   * @return Returns false to cancel the timer
   */
  bool PauseAnimation();

  /**
   * @brief Initializes the background actors
   *
   * @param actor Actor which contains all the children
   */
  void InitializeBackgroundActors(Dali::Actor actor);

  /**
   * Create background actors for the given layer
   *
   * @param[in] layer The layer to add the actors to
   * @param[in] count The number of actors to generate
   */
  void AddBackgroundActors(Dali::Actor layer, int count);

private:
  using AnimationList = std::vector<Dali::Animation>;

  Dali::WeakHandle<Dali::Actor> mScrollView;                    ///< Weak handle to the scroll view used to apply a parallax effect when scrolling.
  AnimationList                 mBackgroundAnimations;          ///< List of background bubble animations.
  Dali::Timer                   mAnimationTimer;                ///< Timer used to turn off animation after a specific time period.
  bool                          mBackgroundAnimsPlaying{false}; ///< Are background animations playing.
};

#endif // DALI_DEMO_BUBBLE_ANIMATOR_H
