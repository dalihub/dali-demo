/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/adaptor-framework/file-stream.h>
#include <dali/integration-api/debug.h>
#include <inttypes.h>
#include <stdio.h>

#include "game-utils.h"

namespace GameUtils
{
bool LoadFile(const char* filename, ByteArray& bytes)
{
  Dali::FileStream fileStream(filename, Dali::FileStream::READ | Dali::FileStream::BINARY);
  FILE*            fin = fileStream.GetFile();

  if(fin)
  {
    if(fseek(fin, 0, SEEK_END))
    {
      return false;
    }
    bytes.resize(ftell(fin));
    std::fill(bytes.begin(), bytes.end(), 0);
    if(fseek(fin, 0, SEEK_SET))
    {
      return false;
    }
    size_t result = fread(bytes.data(), 1, bytes.size(), fin);
    return (result != 0);
  }

  return false;
}

size_t HashString(const char* str)
{
  size_t hash = 5381;
  int    c;
  while((c = *str++))
  {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

} // namespace GameUtils
