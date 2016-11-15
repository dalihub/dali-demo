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

// CLASS HEADER
#include "contact-card-layouter.h"

// EXTERNAL INCLUDES
#include <dali/public-api/common/stage.h>

// INTERNAL INCLUDES
#include "contact-card.h"

using namespace Dali;

namespace
{
const float DEFAULT_PADDING = 25.0f;

const float MINIMUM_ITEMS_PER_ROW_OR_COLUMN( 3.0f );

const float HEADER_HEIGHT_TO_UNFOLDED_SIZE_RATIO( 0.1f );
const Vector2 HEADER_FOLDED_POSITION_AS_RATIO_OF_SIZE( -0.05f, -1.5f );
const Vector2 HEADER_UNFOLDED_POSITION( Vector2::ZERO );

const float IMAGE_SIZE_AS_RATIO_TO_FOLDED_SIZE( 0.5f );
const Vector2 IMAGE_FOLDED_POSITION_AS_RATIO_OF_SIZE( 0.5f, 0.25f );

const float FOLDED_TEXT_POSITION_AS_RATIO_OF_IMAGE_SIZE( 1.01f );
} // unnamed namespace

ContactCardLayouter::ContactCardLayouter()
: mContactCardLayoutInfo(),
  mContactCards(),
  mLastPosition(),
  mPositionIncrementer(),
  mItemsPerRow( 0 ),
  mInitialized( false )
{
}

ContactCardLayouter::~ContactCardLayouter()
{
  // Nothing to do as ContactCardContainer uses intrusive pointers so they will be automatically deleted
}

void ContactCardLayouter::AddContact( const std::string& contactName, const std::string& contactAddress, const std::string& imagePath )
{
  if( ! mInitialized )
  {
    // Set up the common layouting info shared between all contact cards when first called

    mContactCardLayoutInfo.unfoldedPosition = mContactCardLayoutInfo.padding = Vector2( DEFAULT_PADDING, DEFAULT_PADDING );
    mContactCardLayoutInfo.unfoldedSize = Stage::GetCurrent().GetSize() - mContactCardLayoutInfo.padding * ( MINIMUM_ITEMS_PER_ROW_OR_COLUMN - 1.0f );

    // Calculate the size of the folded card (use the minimum of width/height as size)
    mContactCardLayoutInfo.foldedSize = ( mContactCardLayoutInfo.unfoldedSize - ( mContactCardLayoutInfo.padding * ( MINIMUM_ITEMS_PER_ROW_OR_COLUMN - 1.0f ) ) ) / MINIMUM_ITEMS_PER_ROW_OR_COLUMN;
    mContactCardLayoutInfo.foldedSize.width = mContactCardLayoutInfo.foldedSize.height = std::min( mContactCardLayoutInfo.foldedSize.width, mContactCardLayoutInfo.foldedSize.height );

    // Set the size and positions of the header
    mContactCardLayoutInfo.headerSize.width = mContactCardLayoutInfo.unfoldedSize.width;
    mContactCardLayoutInfo.headerSize.height = mContactCardLayoutInfo.unfoldedSize.height * HEADER_HEIGHT_TO_UNFOLDED_SIZE_RATIO;
    mContactCardLayoutInfo.headerFoldedPosition = mContactCardLayoutInfo.headerSize * HEADER_FOLDED_POSITION_AS_RATIO_OF_SIZE;
    mContactCardLayoutInfo.headerUnfoldedPosition = HEADER_UNFOLDED_POSITION;

    // Set the image size and positions
    mContactCardLayoutInfo.imageSize = mContactCardLayoutInfo.foldedSize * IMAGE_SIZE_AS_RATIO_TO_FOLDED_SIZE;
    mContactCardLayoutInfo.imageFoldedPosition = mContactCardLayoutInfo.imageSize * IMAGE_FOLDED_POSITION_AS_RATIO_OF_SIZE;
    mContactCardLayoutInfo.imageUnfoldedPosition.x = mContactCardLayoutInfo.padding.width;
    mContactCardLayoutInfo.imageUnfoldedPosition.y = mContactCardLayoutInfo.headerSize.height + mContactCardLayoutInfo.padding.height;

    // Set the positions of the contact name
    mContactCardLayoutInfo.textFoldedPosition.x = 0.0f;
    mContactCardLayoutInfo.textFoldedPosition.y = mContactCardLayoutInfo.imageFoldedPosition.x + mContactCardLayoutInfo.imageSize.height * FOLDED_TEXT_POSITION_AS_RATIO_OF_IMAGE_SIZE;
    mContactCardLayoutInfo.textUnfoldedPosition.x = mContactCardLayoutInfo.padding.width;
    mContactCardLayoutInfo.textUnfoldedPosition.y = mContactCardLayoutInfo.imageUnfoldedPosition.y + mContactCardLayoutInfo.imageSize.height + mContactCardLayoutInfo.padding.height;

    // Figure out the positions of the contact cards
    mItemsPerRow = ( mContactCardLayoutInfo.unfoldedSize.width + mContactCardLayoutInfo.padding.width ) / ( mContactCardLayoutInfo.foldedSize.width + mContactCardLayoutInfo.padding.width );
    mLastPosition = mContactCardLayoutInfo.unfoldedPosition;
    mPositionIncrementer.x = mContactCardLayoutInfo.foldedSize.width + mContactCardLayoutInfo.padding.width;
    mPositionIncrementer.y = mContactCardLayoutInfo.foldedSize.height + mContactCardLayoutInfo.padding.height;

    mInitialized = true;
  }

  // Create a new contact card and add to our container
  mContactCards.push_back( new ContactCard( mContactCardLayoutInfo, contactName, contactAddress, imagePath, NextCardPosition() ) );
}

const Vector2& ContactCardLayouter::NextCardPosition()
{
  size_t currentNumOfCards = mContactCards.size();

  if( currentNumOfCards )
  {
    if( currentNumOfCards % mItemsPerRow )
    {
      mLastPosition.x += mPositionIncrementer.x;
    }
    else // go to the next row
    {
      mLastPosition.x = mContactCardLayoutInfo.unfoldedPosition.x;
      mLastPosition.y += mPositionIncrementer.y;
    }
  }
  return mLastPosition;
}
