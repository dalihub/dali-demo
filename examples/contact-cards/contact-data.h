#ifndef CONTACT_DATA_H
#define CONTACT_DATA_H

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
#include <cstddef>

namespace ContactData
{

/**
 * @brief The information for an individual contact.
 */
struct Item
{
  const char * const name; ///< The name of the contact.
  const char * const address; ///< The address of the contact.
  const char * const imagePath; ///< The path to the image that represents the contact.
};

extern const Item TABLE[]; ///< The table that has the information for all the contacts.
extern const size_t TABLE_SIZE; ///< The size of TABLE. Can use this to iterate through TABLE.

} // namespace ContactData

#endif // CONTACT_DATA_H
