#ifndef DALI_DEMO_INTERNAL_BEAT_CONTROL_IMPL_H
#define DALI_DEMO_INTERNAL_BEAT_CONTROL_IMPL_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include "beat-control.h"
#include <dali/public-api/animation/animation.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>
#include <dali-toolkit/public-api/align-enumerations.h>

namespace Demo
{

namespace Internal // To use TypeRegistry, handle and body classes need the same name
{

class BeatControl : public Dali::Toolkit::Internal::Control
{
public:
  /**
   * Instantiate a new BeatControl object
   */
  static Demo::BeatControl New();
  BeatControl();
  ~BeatControl();

public: // API
  void StartBounceAnimation();

  void StartXAnimation();

  void StartYAnimation();

  void StartFadeAnimation();

public:  // Properties
  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value );

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Dali::Property::Value GetProperty( Dali::BaseObject* object, Dali::Property::Index propertyIndex );

private: // From Control
  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Control::OnStageConnect()
   */
  virtual void OnStageConnection( int depth );

  /**
   * @copydoc Toolkit::Control::OnStageDisconnection()
   */
  virtual void OnStageDisconnection();

  /**
   * @copydoc Toolkit::Control::OnSizeSet()
   */
  virtual void OnSizeSet( const Dali::Vector3& targetSize );

  /**
   * @copydoc Toolkit::Control::OnRelayout()
   */
  virtual void OnRelayout( const Dali::Vector2& targetSize, Dali::RelayoutContainer& container );
  /**
   * @copydoc Toolkit::Control::GetNaturalSize
   */
  virtual Dali::Vector3 GetNaturalSize();

  /**
   * @copydoc Toolkit::Control::OnStyleChange
   */
  virtual void OnStyleChange( Dali::Toolkit::StyleManager styleManager, Dali::StyleChange::Type change );

private:
  void OnBounceAnimationFinished( Dali::Animation& handle );
  void OnXAnimationFinished( Dali::Animation& src );
  void OnYAnimationFinished( Dali::Animation& src );
  void OnFadeAnimationFinished( Dali::Animation& src );

  /**
   * Relayout the visuals as a result of size negotiation
   */
  void RelayoutVisuals( const Dali::Vector2& targetSize );

private:
  //undefined
  BeatControl( const BeatControl& );
  BeatControl& operator=( const BeatControl& );

private:
  // Implementation details
  Dali::Toolkit::Visual::Base mVisual;
  Dali::Toolkit::TransitionData mBounceTransition;
  Dali::Toolkit::TransitionData mLeftTransition;
  Dali::Toolkit::TransitionData mUpTransition;
  Dali::Toolkit::TransitionData mFadeTransition;
  Dali::Animation mAnimation;
  Dali::Animation mXAnimation;
  Dali::Animation mYAnimation;
  Dali::Animation mFadeAnimation;
  Dali::Vector2 mTransformSize;
  Dali::Toolkit::Align::Type mTransformOrigin;
  Dali::Toolkit::Align::Type mTransformAnchorPoint;
  int mAnimationPlaying;
};

} // Internal

inline Internal::BeatControl& GetImpl( Demo::BeatControl& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  Dali::RefObject& object = handle.GetImplementation();
  return static_cast<Internal::BeatControl&>(object);
}

inline const Internal::BeatControl& GetImpl( const Demo::BeatControl& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  const Dali::RefObject& object = handle.GetImplementation();
  return static_cast<const Internal::BeatControl&>(object);
}

} // Demo

#endif //  DALI_DEMO_BEAT_CONTROL_IMPL_H
