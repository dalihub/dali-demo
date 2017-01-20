#ifndef CLIPPING_ITEM_FACTORY_H
#define CLIPPING_ITEM_FACTORY_H

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
 *
 */

#include <dali-toolkit/public-api/controls/scrollable/item-view/item-factory.h>

/**
 * @brief Factory used to create the items required in the item-view used by this example.
 */
class ClippingItemFactory : public Dali::Toolkit::ItemFactory
{
public:

  /**
   * @brief Constructor
   */
  ClippingItemFactory();

private: // From ItemFactory

  /**
   * Query the number of items available from the factory.
   * The maximum available item has an ID of GetNumberOfItems() - 1.
   */
  virtual unsigned int GetNumberOfItems();

  /**
   * Create an Actor to represent a visible item.
   * @param itemId
   * @return the created actor.
   */
  virtual Dali::Actor NewItem( unsigned int itemId );

private:

  ClippingItemFactory( const ClippingItemFactory& ); ///< Undefined
  ClippingItemFactory& operator=( const ClippingItemFactory& ); ///< Undefined
};

#endif // CLIPPING_ITEM_FACTORY_H
