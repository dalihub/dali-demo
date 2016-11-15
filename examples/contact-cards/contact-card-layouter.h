#ifndef CONTACT_CARD_LAYOUTER_H
#define CONTACT_CARD_LAYOUTER_H

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
#include <vector>
#include <string>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES
#include "contact-card-layout-info.h"

class ContactCard;

/**
 * @brief This class lays out contact cards on the screen appropriately.
 *
 * The contact cards are added to the stage directly and it uses the stage size to figure out exactly how to layout them.
 * It supports a minimum of 3 items on each row or column.
 *
 * Relayouting is not supported.
 */
class ContactCardLayouter
{
public:

  /**
   * @brief Constructor.
   */
  ContactCardLayouter();

  /**
   * @brief Destructor.
   */
  ~ContactCardLayouter();

  /**
   * @brief Creates a contact card with the given information.
   * @param[in]  contactName     The name of the contact to display.
   * @param[in]  contactAddress  The address of the contact to display.
   * @param[in]  imagePath       The path to the image to display.
   */
  void AddContact( const std::string& contactName, const std::string& contactAddress, const std::string& imagePath );

private:

  /**
   * @brief Calculates the next position of the contact card that's about to be added to our container.
   * @return A reference to the next position.
   */
  const Dali::Vector2& NextCardPosition();

  ContactCardLayoutInfo mContactCardLayoutInfo; ///< The common layouting information used by all contact cards. Set up when AddContact is first called.

  typedef Dali::IntrusivePtr< ContactCard > ContactCardPtr; ///< Better than raw pointers as these are ref counted and the memory is released when the count reduces to 0.
  typedef std::vector< ContactCardPtr > ContactCardContainer;
  ContactCardContainer mContactCards; ///< Contains all the contact cards.

  Dali::Vector2 mLastPosition; ///< The last position a contact card was added.
  Dali::Vector2 mPositionIncrementer; ///< Calculated once when AddContact is first called.
  size_t mItemsPerRow; ///< Calculated once when AddContact is first called and stores the number of items we have in a row.

  bool mInitialized; ///< Whether initialization has taken place or not.
};



#endif // CONTACT_CARD_LAYOUTER_H
