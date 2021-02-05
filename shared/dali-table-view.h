#ifndef DALI_DEMO_TABLEVIEW_H
#define DALI_DEMO_TABLEVIEW_H

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
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include <dali/dali.h>

// INTERNAL INCLUDES
#include "bubble-animator.h"
#include "example.h"

/**
 * Dali-Demo instance
 */
class DaliTableView : public Dali::ConnectionTracker
{
public:
  /**
   * Constructor
   *
   * @param application A reference to the application class
   */
  DaliTableView(Dali::Application& application);

  /**
   * Destructor
   */
  ~DaliTableView() = default;

public:
  /**
   * Adds an Example to our demo showcase
   *
   * @param[in] example The Example description.
   *
   * @note Should be called before the Application MainLoop is started.
   */
  void AddExample(Example example);

  /**
   * Sorts the example list alphabetically by Title if parameter is true.
   *
   * @param[in] sortAlphabetically If true, example list is sorted alphabetically.
   *
   * @note Should be called before the Application MainLoop is started.
   * @note By default the examples are NOT sorted alphabetically by Title.
   */
  void SortAlphabetically(bool sortAlphabetically);

private:                                                          // Application callbacks & implementation
  static constexpr unsigned int FOCUS_ANIMATION_ACTOR_NUMBER = 2; ///< The number of elements used to form the custom focus effect

  /**
   * Initialize application.
   *
   * @param[in] app Application instance
   */
  void Initialize(Dali::Application& app);

  /**
   * Populates the contents (ScrollView) with all the
   * Examples that have been Added using the AddExample(...)
   * call
   */
  void Populate();

  /**
   * Creates a tile for the main menu.
   *
   * @param[in] name The unique name for this Tile
   * @param[in] title The text caption that appears on the Tile
   * @param[in] parentSize Tile's parent size.
   * @param[in] position The tiles relative position within a page
   *
   * @return The Actor for the created tile.
   */
  Dali::Actor CreateTile(const std::string& name, const std::string& title, const Dali::Vector3& sizeMultiplier, Dali::Vector2& position);

  // Signal handlers

  /**
   * Signal emitted when any tile has been pressed
   *
   * @param[in] actor The Actor representing this tile.
   * @param[in] event The Touch information.
   *
   * @return Consume flag
   */
  bool OnTilePressed(Dali::Actor actor, const Dali::TouchEvent& event);

  /**
   * Called by OnTilePressed & Accessibility to do the appropriate action.
   *
   * @param[in] actor The Actor representing this tile.
   * @param[in] state The Touch state
   *
   * @return Consume flag
   */
  bool DoTilePress(Dali::Actor actor, Dali::PointState::Type state);

  /**
   * Signal emitted when any tile has been hovered
   *
   * @param[in] actor The Actor representing this tile.
   * @param[in] event The HoverEvent
   *
   * @return Consume flag
   */
  bool OnTileHovered(Dali::Actor actor, const Dali::HoverEvent& event);

  /**
   * Signal emitted when the pressed animation has completed.
   *
   * @param[in] source The animation source.
   */
  void OnPressedAnimationFinished(Dali::Animation& source);

  /**
   * Signal emitted when scrolling has started.
   *
   * @param[in] position The current position of the scroll contents.
   */
  void OnScrollStart(const Dali::Vector2& position);

  /**
   * Signal emitted when scrolling has completed.
   *
   * @param[in] position The current position of the scroll contents.
   */
  void OnScrollComplete(const Dali::Vector2& position);

  /**
   * Signal emitted when any Sensitive Actor has been touched
   * (other than those touches consumed by OnTilePressed)
   *
   * @param[in] actor The Actor touched.
   * @param[in] event The Touch information.
   *
   * @return Consume flag
   */
  bool OnScrollTouched(Dali::Actor actor, const Dali::TouchEvent& event);

  /**
   * Setup the effect on the scroll view
   */
  void ApplyScrollViewEffect();

  /**
   * Apply the cube effect to all the page actors
   */
  void ApplyCubeEffectToPages();

  /**
   * Key event handler
   */
  void OnKeyEvent(const Dali::KeyEvent& event);

  /**
   * @brief Creates and sets up the custom effect used for the keyboard (and mouse) focus.
   */
  void CreateFocusEffect();

  /**
   * Callback when the keyboard focus is going to be changed.
   *
   * @param[in] current The current focused actor
   * @param[in] proposed The actor proposed by the keyboard focus manager to move the focus to
   * @param[in] direction The direction to move the focus
   * @return The actor to move the keyboard focus to.
   */
  Dali::Actor OnKeyboardPreFocusChange(Dali::Actor current, Dali::Actor proposed, Dali::Toolkit::Control::KeyboardFocus::Direction direction);

  /**
   * Callback when the keyboard focused actor is activated.
   *
   * @param[in] activatedActor The activated actor
   */
  void OnFocusedActorActivated(Dali::Actor activatedActor);

  /**
   * Callback when the keyboard focus indicator is enabled.
   *
   * @param[in] actor The keyboard focus indicator.
   */
  void OnFocusIndicatorEnabled(Dali::Actor actor);

  /**
   * Callback when the keyboard focus indicator is disabled.
   *
   * @param[in] actor The keyboard focus indicator.
   */
  void OnFocusIndicatorDisabled(Dali::Actor actor);

  /**
   * Called when the logo is tapped
   *
   * @param[in]  actor  The tapped actor
   * @param[in]  tap    The tap information.
   */
  void OnLogoTapped(Dali::Actor actor, const Dali::TapGesture& tap);

private:
  Dali::Application&              mApplication;      ///< Application instance.
  Dali::Actor                     mRootActor;        ///< All content (excluding background is anchored to this Actor)
  Dali::Animation                 mPressedAnimation; ///< Button press scaling animation.
  Dali::Toolkit::ScrollView       mScrollView;       ///< ScrollView container (for all Examples)
  Dali::Toolkit::ScrollViewEffect mScrollViewEffect; ///< Effect to be applied to the scroll view
  Dali::Actor                     mPressedActor;     ///< The currently pressed actor.
  Dali::TapGestureDetector        mLogoTapDetector;  ///< To detect taps on the logo
  Dali::Toolkit::Popup            mVersionPopup;     ///< Displays DALi library version information
  BubbleAnimator                  mBubbleAnimator;   ///< Provides bubble animations.

  /**
   * This struct encapsulates all data relevant to each of the elements used within the custom keyboard focus effect.
   */
  struct FocusEffect
  {
    Dali::Toolkit::ImageView actor;     ///< The parent keyboard focus highlight actor
    Dali::Animation          animation; ///< The animation for the parent keyboard focus highlight actor
  };
  FocusEffect mFocusEffect[FOCUS_ANIMATION_ACTOR_NUMBER]; ///< The elements used to create the custom focus effect

  std::vector<Dali::Actor> mPages;       ///< List of pages.
  ExampleList              mExampleList; ///< List of examples.

  float mPageWidth;  ///< The width of a page within the scroll-view, used to calculate the domain
  int   mTotalPages; ///< Total pages within scrollview.

  bool mScrolling : 1;          ///< Flag indicating whether view is currently being scrolled
  bool mSortAlphabetically : 1; ///< Sort examples alphabetically.
};

#endif // DALI_DEMO_TABLEVIEW_H
