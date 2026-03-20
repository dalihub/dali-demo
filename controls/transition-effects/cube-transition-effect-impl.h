#ifndef DALI_DEMO_CONTROLS_INTERNAL_CUBE_TRANSITION_EFFECT_H
#define DALI_DEMO_CONTROLS_INTERNAL_CUBE_TRANSITION_EFFECT_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <controls/transition-effects/cube-transition-effect.h>

namespace Dali::Demo
{
class CubeTransitionEffect;

namespace Internal
{
/**
 * CubeTransitionEffect implementation class
 */
class CubeTransitionEffect : public Toolkit::ControlImpl
{
public:
  /**
   * Destructor
   */
  ~CubeTransitionEffect();

  /**
   * @copydoc Demo::CubeTransitionEffect::SetTransitionDuration
   */
  void SetTransitionDuration(float duration);

  /**
   * @copydoc Demo::CubeTransitionEffect::GetTransitionDuration
   */
  float GetTransitionDuration() const;

  /**
   * @copydoc Demo::CubeTransitionEffect::SetCubeDisplacement
   */
  void SetCubeDisplacement(float displacement);

  /**
   * @copydoc Demo::CubeTransitionEffect::GetCubeDisplacement
   */
  float GetCubeDisplacement() const;

  /**
   * @copydoc Demo::CubeTransitionEffect::IsTransitioning
   */
  bool IsTransitioning();

  /**
   * @copydoc Demo::CubeTransitionEffect::SetCurrentTexture
   */
  void SetCurrentTexture(Texture texture);

  /**
   * @copydoc Demo::CubeTransitionEffect::SetTargetTexture
   */
  void SetTargetTexture(Texture texture);

  /**
   * @copydoc Demo::CubeTransitionEffect::StartTransition(bool)
   */
  void StartTransition(bool toNextImage = true);

  /**
   * @copydoc Demo::CubeTransitionEffect::StartTransition(Vector2, Vector2)
   */
  void StartTransition(Vector2 panPosition, Vector2 panDisplacement);

  /**
   * @copydoc Demo::CubeTransitionEffect::PauseTransition()
   */
  void PauseTransition();

  /**
   * @copydoc Demo::CubeTransitionEffect::ResumeTransition()
   */
  void ResumeTransition();

  /**
   * @copydoc Demo::CubeTransitionEffect::StopTransition()
   */
  void StopTransition();

public: // Signal
  /**
   * @copydoc Demo::CubeTransitionEffect::TransitionCompletedSignal()
   */
  Demo::CubeTransitionEffect::TransitionCompletedSignalType& TransitionCompletedSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const Dali::String& signalName, FunctorDelegate* functor);

protected:
  /**
   * @copydoc Toolkit::ControlImpl::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc Toolkit::ControlImpl::OnSceneDisconnection()
   */
  void OnSceneDisconnection() override;

protected:
  /**
   * Construct a new CubeTransitionEffect object
   * Called in the constructor of subclasses
   * @param[in] numRows How many rows of cubes
   * @param[in] numColumns How many columns of cubes
   */
  CubeTransitionEffect(unsigned int numRows, unsigned int numColumns);

  /**
   * Initialization steps: creating a layer, two groups of tiles,
   * and one group of actors (cubes) serving as parents of every two tiles (one from each image).
   */
  void Initialize();

protected:
  void SetTargetLeft(unsigned int idx);
  void SetTargetRight(unsigned int idx);
  void SetTargetTop(unsigned int idx);
  void SetTargetBottom(unsigned int idx);

private:
  /**
   * Callback function of transition animation finished
   * Hide transition layer, show current image, and set isAnimating flag to false
   * @param[in] source The cube transition animation
   */
  void OnTransitionFinished(Animation& source);

  /**
   * This method is called after the CubeTransitionEffect has been initialized.  Derived classes should do
   * any second phase initialization by overriding this method.
   */
  virtual void OnInitialize()
  {
  }

  /**
   * This method is called after the a new transition is activated.
   * Derived classes should do any specialized transition process by overriding this method.
   * @param[in] panPosition The press down position of panGesture
   * @param[in] panDisplacement The displacement vector of panGesture
   */
  virtual void OnStartTransition(Vector2 panPosition, Vector2 panDisplacement)
  {
  }

  /**
   * This method is called when the transition is forced stop in the middle of animation.
   * Derived classed should set the rotation status of the cubes to the same as the final state when the animation is finished completely.
   * So that the next transition would be started correctly.
   */
  virtual void OnStopTransition()
  {
  }

  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  void ResetToInitialState();

protected:
  typedef std::vector<Actor> ActorArray;
  enum FACE
  {
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
  };

  ActorArray   mBoxes;
  Vector<FACE> mBoxType;
  ActorArray   mCurrentTiles;
  ActorArray   mTargetTiles;

  Actor mBoxRoot;

  unsigned int mRows;
  unsigned int mColumns;

  Renderer mCurrentRenderer;
  Renderer mTargetRenderer;

  Texture mCurrentTexture;
  Texture mTargetTexture;

  Animation mAnimation;

  Vector2 mTileSize;

  bool mIsAnimating;
  bool mIsPaused;

  float mAnimationDuration;
  float mCubeDisplacement;

  static const Vector4 FULL_BRIGHTNESS;
  static const Vector4 HALF_BRIGHTNESS;

private:
  Demo::CubeTransitionEffect::TransitionCompletedSignalType mTransitionCompletedSignal;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::CubeTransitionEffect& GetImpl(Dali::Demo::CubeTransitionEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::RefObject& handle = obj.GetImplementation();

  return static_cast<Internal::CubeTransitionEffect&>(handle);
}

inline const Internal::CubeTransitionEffect& GetImpl(const Dali::Demo::CubeTransitionEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::RefObject& handle = obj.GetImplementation();

  return static_cast<const Internal::CubeTransitionEffect&>(handle);
}

} // namespace Dali::Demo

#endif // DALI_DEMO_CONTROLS_INTERNAL_CUBE_TRANSITION_EFFECT_H
