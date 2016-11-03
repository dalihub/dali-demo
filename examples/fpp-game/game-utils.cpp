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

#include <inttypes.h>
#include <stdio.h>

#include "game-utils.h"

namespace GameUtils
{
bool LoadFile( const char* filename, ByteArray& bytes )
{
  FILE* fin = fopen( filename, "rb" );
  fseek( fin, 0, SEEK_END );
  bytes.resize( ftell( fin ) );
  std::fill( bytes.begin(), bytes.end(), 0 );
  fseek( fin, 0, SEEK_SET );
  size_t result = fread( bytes.data(), 1, bytes.size(), fin );
  fclose( fin );
  return (result != 0);
}

size_t HashString( const char* str )
{
  size_t hash = 5381;
  int c;
  while( ( c = *str++ ) )
  {
     hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

}
