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

// CLASS HEADER
#include "bubble-animator.h"

#include <dali-toolkit/devel-api/shader-effects/distance-field-effect.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/math/random.h>
#include <dali/public-api/rendering/shader.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* const BUBBLE_COLOR_STYLE_NAME[] =
  {
    "BubbleColor1",
    "BubbleColor2",
    "BubbleColor3",
    "BubbleColor4"};
constexpr int NUMBER_OF_BUBBLE_COLORS(sizeof(BUBBLE_COLOR_STYLE_NAME) / sizeof(BUBBLE_COLOR_STYLE_NAME[0]));

const char* const SHAPE_IMAGE_TABLE[] =
  {
    DEMO_IMAGE_DIR "shape-circle.png",
    DEMO_IMAGE_DIR "shape-bubble.png"};
constexpr int NUMBER_OF_SHAPE_IMAGES(sizeof(SHAPE_IMAGE_TABLE) / sizeof(SHAPE_IMAGE_TABLE[0]));

constexpr int   NUM_BACKGROUND_IMAGES   = 18;
constexpr float BACKGROUND_SPREAD_SCALE = 1.5f;

constexpr unsigned int BACKGROUND_ANIMATION_DURATION = 15000; // 15 secs

constexpr float BUBBLE_MIN_Z = -1.0;
constexpr float BUBBLE_MAX_Z = 0.0f;

/**
 * Constraint to return a position for a bubble based on the scroll value and vertical wrapping
 */
struct AnimateBubbleConstraint
{
public:
  AnimateBubbleConstraint(const Vector3& initialPos, float scale)
  : mInitialX(initialPos.x),
    mScale(scale)
  {
  }

  void operator()(Vector3& position, const PropertyInputContainer& inputs)
  {
    const Vector3& parentSize = inputs[1]->GetVector3();
    const Vector3& childSize  = inputs[2]->GetVector3();

    // Wrap bubbles vertically.
    float range = parentSize.y + childSize.y;
    // This performs a float mod (we don't use fmod as we want the arithmetic modulus as opposed to the remainder).
    position.y -= range * (floor(position.y / range) + 0.5f);

    // Bubbles X position moves parallax to horizontal
    // panning by a scale factor unique to each bubble.
    position.x = mInitialX + (inputs[0]->GetVector2().x * mScale);
  }

private:
  float mInitialX;
  float mScale;
};

} // unnamed namespace

void BubbleAnimator::Initialize(Dali::Actor parent, Dali::Actor scrollView)
{
  mScrollView = scrollView;

  // Populate background and bubbles - needs to be scrollViewLayer so scroll ends show
  Actor bubbleContainer = Actor::New();
  bubbleContainer.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  bubbleContainer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  bubbleContainer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  AddBackgroundActors(bubbleContainer, NUM_BACKGROUND_IMAGES);
  parent.Add(bubbleContainer);

  // Background animation
  mAnimationTimer = Timer::New(BACKGROUND_ANIMATION_DURATION);
  mAnimationTimer.TickSignal().Connect(this, &BubbleAnimator::PauseAnimation);
  mAnimationTimer.Start();
  mBackgroundAnimsPlaying = true;
}

bool BubbleAnimator::PauseAnimation()
{
  if(mBackgroundAnimsPlaying)
  {
    for(auto&& anim : mBackgroundAnimations)
    {
      anim.Stop();
    }

    mBackgroundAnimsPlaying = false;
  }
  return false;
}

void BubbleAnimator::PlayAnimation()
{
  if(!mBackgroundAnimsPlaying)
  {
    for(auto&& anim : mBackgroundAnimations)
    {
      anim.Play();
    }

    mBackgroundAnimsPlaying = true;
  }

  mAnimationTimer.SetInterval(BACKGROUND_ANIMATION_DURATION);
}
void BubbleAnimator::InitializeBackgroundActors(Dali::Actor actor)
{
  // Delete current animations
  mBackgroundAnimations.clear();

  // Create new animations
  const Vector3 size = actor.GetTargetSize();

  for(unsigned int i = 0, childCount = actor.GetChildCount(); i < childCount; ++i)
  {
    Actor child = actor.GetChildAt(i);

    // Calculate a random position
    Vector3 childPos(Random::Range(-size.x * 0.5f * BACKGROUND_SPREAD_SCALE, size.x * 0.85f * BACKGROUND_SPREAD_SCALE),
                     Random::Range(-size.y, size.y),
                     Random::Range(BUBBLE_MIN_Z, BUBBLE_MAX_Z));

    child.SetProperty(Actor::Property::POSITION, childPos);

    // Define bubble horizontal parallax and vertical wrapping
    Actor scrollView = mScrollView.GetHandle();
    if(scrollView)
    {
      Constraint animConstraint = Constraint::New<Vector3>(child, Actor::Property::POSITION, AnimateBubbleConstraint(childPos, Random::Range(-0.85f, 0.25f)));
      animConstraint.AddSource(Source(scrollView, ScrollView::Property::SCROLL_POSITION));
      animConstraint.AddSource(Dali::ParentSource(Dali::Actor::Property::SIZE));
      animConstraint.AddSource(Dali::LocalSource(Dali::Actor::Property::SIZE));
      animConstraint.SetRemoveAction(Constraint::DISCARD);
      animConstraint.Apply();
    }

    // Kickoff animation
    Animation animation = Animation::New(Random::Range(30.0f, 160.0f));
    animation.AnimateBy(Property(child, Actor::Property::POSITION), Vector3(0.0f, -2000.0f, 0.0f), AlphaFunction::LINEAR);
    animation.SetLooping(true);
    animation.Play();
    mBackgroundAnimations.push_back(animation);
  }
}

void BubbleAnimator::AddBackgroundActors(Actor layer, int count)
{
  for(int i = 0; i < count; ++i)
  {
    float randSize  = Random::Range(10.0f, 400.0f);
    int   shapeType = static_cast<int>(Random::Range(0.0f, NUMBER_OF_SHAPE_IMAGES - 1) + 0.5f);

    ImageView dfActor = ImageView::New();
    dfActor.SetProperty(Actor::Property::SIZE, Vector2(randSize, randSize));
    dfActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

    // Set the Image URL and the custom shader at the same time
    Dali::Property::Map effect = Toolkit::CreateDistanceFieldEffect();
    Property::Map       imageMap;
    imageMap.Add(ImageVisual::Property::URL, SHAPE_IMAGE_TABLE[shapeType]);
    imageMap.Add(Toolkit::Visual::Property::SHADER, effect);
    dfActor.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);

    dfActor.SetStyleName(BUBBLE_COLOR_STYLE_NAME[i % NUMBER_OF_BUBBLE_COLORS]);

    layer.Add(dfActor);
  }

  // Positioning will occur when the layer is relaid out
  layer.OnRelayoutSignal().Connect(this, &BubbleAnimator::InitializeBackgroundActors);
}
