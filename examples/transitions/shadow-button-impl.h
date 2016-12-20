#ifndef DALI_DEMO_INTERNAL_SHADOW_BUTTON_IMPL_H
#define DALI_DEMO_INTERNAL_SHADOW_BUTTON_IMPL_H

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

#include "shadow-button.h"
#include <dali/public-api/animation/animation.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>

namespace Demo
{

namespace Internal // To use TypeRegistry, handle and body classes need the same name
{

class ShadowButton : public Dali::Toolkit::Internal::Control
{
public:
  /**
   * Instantiate a new ShadowButton object
   */
  static Demo::ShadowButton New();
  ShadowButton();
  ~ShadowButton();

public: // API
  /**
   * @brief Set the button to be active or inactive.
   *
   * The button will perform a transition if there is a state change.
   * @param[in] active The active state
   */
  void SetActiveState( bool active );

  /**
   * Get the active state
   * @return the active state
   */
  bool GetActiveState();

  /**
   * Set the check state
   * @param[in] checkState The state of the checkbox
   */
  void SetCheckState( bool checkState );

  /**
   * Get the check state
   * @return the check state
   */
  bool GetCheckState();

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
   * @copydoc Toolkit::Button::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Button::OnStageConnect()
   */
  virtual void OnStageConnection( int depth );

  /**
   * @copydoc Toolkit::Button::OnStageDisconnection()
   */
  virtual void OnStageDisconnection();

  /**
   * @copydoc Toolkit::Button::OnSizeSet()
   */
  virtual void OnSizeSet( const Dali::Vector3& targetSize );

  /**
   * @copydoc Toolkit::Button::OnRelayout()
   */
  virtual void OnRelayout( const Dali::Vector2& targetSize, Dali::RelayoutContainer& container );
  /**
   * @copydoc Toolkit::Button::GetNaturalSize
   */
  virtual Dali::Vector3 GetNaturalSize();

  /**
   * @copydoc Toolkit::Button::OnStyleChange
   */
  virtual void OnStyleChange( Dali::Toolkit::StyleManager styleManager, Dali::StyleChange::Type change );

private:
  struct Transition
  {
    Dali::Property::Index mTransitionId;
    Dali::Toolkit::TransitionData mTransitionData;
    Dali::Animation mAnimation;
    bool mPlaying;

    Transition( Dali::Property::Index index, Dali::Toolkit::TransitionData transitionData )
    : mTransitionId( index ),
      mTransitionData( transitionData ),
      mPlaying(false)
    {
    }
  private:
    Transition();
  };

  typedef std::vector<Transition> Transitions;

  struct Transform
  {
    Dali::Property::Index mTransformId;
    Dali::Property::Map   mTransform;

    Transform( Dali::Property::Index index, Dali::Property::Map& map )
    : mTransformId(index),
      mTransform( map )
    {
    }
  };
  typedef std::vector<Transform> Transforms;

private:
  void StartTransition( Dali::Property::Index transitionId );

  void OnTransitionFinished( Dali::Animation& handle );

  Transitions::iterator FindTransition( Dali::Property::Index index );

  Transforms::iterator FindTransform( Dali::Property::Index index );

  /**
   * Relayout the visuals as a result of size negotiation using
   * the transforms provided in the stylesheet
   */
  void RelayoutVisuals( const Dali::Vector2& targetSize );

  /**
   * Relayout the visuals as a result of size negotiation using
   * programmatically generated transforms
   */
  void RelayoutVisualsManually( const Dali::Vector2& targetSize );

  void ResetVisual( Dali::Property::Index        index,
                    Dali::Toolkit::Visual::Base& visual,
                    const Dali::Property::Value& value );

  void ResetTransition( Dali::Property::Index        index,
                        const Dali::Property::Value& value );

  void StoreTargetLayouts( Dali::Toolkit::TransitionData transitionData );

private:
  // undefined
  ShadowButton( const ShadowButton& );
  ShadowButton& operator=( const ShadowButton& );

private:
  // Data
  Dali::Toolkit::Visual::Base mBackgroundVisual;
  Dali::Toolkit::Visual::Base mCheckboxBgVisual;
  Dali::Toolkit::Visual::Base mCheckboxFgVisual;
  Dali::Toolkit::Visual::Base mLabelVisual;

  Transitions mTransitions;
  Transforms  mTransforms;
  bool        mCheckState;
  bool        mActiveState;
};

} // Internal

inline Internal::ShadowButton& GetImpl( Demo::ShadowButton& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  Dali::RefObject& object = handle.GetImplementation();
  return static_cast<Internal::ShadowButton&>(object);
}

inline const Internal::ShadowButton& GetImpl( const Demo::ShadowButton& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  const Dali::RefObject& object = handle.GetImplementation();
  return static_cast<const Internal::ShadowButton&>(object);
}

} // Demo

#endif //  DALI_DEMO_SHADOW_BUTTON_IMPL_H
