#ifndef CONTACT_CARD_LAYOUT_INFO_H
#define CONTACT_CARD_LAYOUT_INFO_H

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
#include <dali/public-api/math/vector2.h>

/**
 * @brief This is the common data that is used by all contact cards.
 *
 * In this context, "unfolded" means when all the details, including the name, address and image are shown.
 * In this scenario, the control takes up most of the screen and a header is also shown.
 *
 * When the contact card is "folded", this means when only brief information is shown to the user, i.e. the image and name.
 * In this scenario, the control is small and there should be several of these contact cards visible on the screen.
 */
struct ContactCardLayoutInfo
{
  Dali::Vector2 unfoldedPosition; ///< The position of the entire contact card when all details (unfolded) are shown
  Dali::Vector2 unfoldedSize;     ///< The size of the entire contact card when all details (unfolded) are shown
  Dali::Vector2 foldedSize;       ///< The size of each contact card when only the brief information is shown (folded)

  Dali::Vector2 padding; ///< The default padding to use throughout

  Dali::Vector2 headerSize;             ///< The size of the header area (only shown when unfolded)
  Dali::Vector2 headerFoldedPosition;   ///< The position of the header area when folded - required for animation purposes only as it's actually clipped
  Dali::Vector2 headerUnfoldedPosition; ///< The position of the header area when unfolded

  Dali::Vector2 imageSize;             ///< The size of the image
  Dali::Vector2 imageFoldedPosition;   ///< The position of the image when folded
  Dali::Vector2 imageUnfoldedPosition; ///< The position of the image when unfolded

  Dali::Vector2 textFoldedPosition;   ///< The position of the text when folded
  Dali::Vector2 textUnfoldedPosition; ///< The position of the text when unfolded
};

#endif // CONTACT_CARD_LAYOUT_INFO_H

