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

#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include <vector>
#include <inttypes.h>
#include <stdlib.h>

namespace GameUtils
{
typedef std::vector<char> ByteArray;

/**
 * Loads file from the storage and returns byte array
 */
bool LoadFile( const char* filename, ByteArray& out );

/**
 * Computes hash value from string using djb2 algorithm
 * @return hash value
 */
size_t HashString( const char* str );
}

#endif

