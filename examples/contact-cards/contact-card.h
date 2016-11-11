#ifndef CONTACT_CARD_H
#define CONTACT_CARD_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <string>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/events/tap-gesture-detector.h>
#include <dali/public-api/object/ref-object.h>
#include <dali-toolkit/public-api/controls/control.h>

class ContactCardLayoutInfo;

/**
 * @brief Creates and sets up animations for a contact card
 *
 * Each contact card has two states, folded and unfolded.
 * In this context, "unfolded" means when all the details, including the name, address and image are shown.
 * In this scenario, the control takes up most of the screen.
 *
 * When the contact card is "folded", this means when only brief information is shown to the user, i.e. the image and name.
 * In this scenario, the control is small and there should be several of these contact cards visible on the screen.
 *
 * The contact card creates several controls that it requires to appropriately display itself in both of these states.
 */
class ContactCard : public Dali::RefObject
{
public:

  /**
   * @brief Constructor.
   *
   * This will create all the controls and add them to the stage so should only be called after the init-signal from the Application has been received.
   *
   * @param[in]  contactCardLayoutInfo  Reference to the common data used by all contact cards.
   * @param[in]  contactName            The name of the contact to display.
   * @param[in]  contactAddress         The address of the contact to display.
   * @param[in]  imagePath              The path to the image to display.
   * @param[in]  position               The unique folded position of this particular contact-card.
   */
  ContactCard( const ContactCardLayoutInfo& contactCardLayoutInfo, const std::string& contactName, const std::string& contactAddress, const std::string& imagePath, const Dali::Vector2& position );

private:

  /**
   * @brief Private Destructor. Will only be deleted when ref-count goes to 0.
   *
   * Unparent the created contact card (i.e. remove from stage).
   */
  ~ContactCard();

  /**
   * @brief Called when this contact card is tapped.
   * @param[in] actor The tapped actor.
   * @param[in] gesture The tap gesture.
   */
  void OnTap( Dali::Actor actor, const Dali::TapGesture& gesture );

  /**
   * @brief Called when the animation finishes.
   * @param[in] animation The animation which has just finished.
   */
  void OnAnimationFinished( Dali::Animation& animation );

  Dali::TapGestureDetector mTapDetector; ///< Used for tap detection.
  Dali::Toolkit::Control mContactCard; ///< Used for the background and to clip the contents.
  Dali::Toolkit::Control mHeader; ///< Header shown when unfolded.
  Dali::Toolkit::Control mClippedImage; ///< The image representing the contact (whose clipping can be animated).
  Dali::Toolkit::Control mNameText; ///< The text shown when folded.
  Dali::Toolkit::Control mDetailText; ///< The text shown when unfolded.

  Dali::SlotDelegate< ContactCard > mSlotDelegate; ///< Used to automatically disconnect our member functions from signals that this class connects to upon destruction. Can be used instead of inheriting from ConnectionTracker.

  const ContactCardLayoutInfo& mContactCardLayoutInfo; ///< Reference to the common data used by all contact cards.
  const Dali::Vector2 foldedPosition; ///< The unique position of this card when it is folded.
  Dali::Property::Index mClippedImagePropertyIndex; ///< Index used to animate the clipping of mClippedImage.
  bool mFolded; ///< Whether the contact card is folded or not.
};

#endif // CONTACT_CARD_H
