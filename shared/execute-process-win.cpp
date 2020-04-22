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

 // FILE HEADER
#include "execute-process.h"

 // EXTERNAL INCLUDES
#include <windows.h>
#include <dali/public-api/common/dali-common.h>

namespace
{
const std::string PATH_SEPARATOR( "\\" );
}

void ExecuteProcess( const std::string& processName, Dali::Application& application )
{
  std::string processPathName;

  const bool isRelativePath = '.' == DEMO_EXAMPLE_BIN[0];
  if (isRelativePath)
  {
    char currentPath[MAX_PATH];
    DWORD numberOfCharacters = GetCurrentDirectory( MAX_PATH, currentPath );

    if( 0u == numberOfCharacters )
    {
      DALI_ASSERT_ALWAYS( !"Failed to retrieve the current working directory" );
    }

    currentPath[numberOfCharacters] = '\0';
    processPathName = std::string(currentPath) + PATH_SEPARATOR + DEMO_EXAMPLE_BIN + PATH_SEPARATOR + processName + ".exe";
  }
  else
  {
    processPathName = DEMO_EXAMPLE_BIN + PATH_SEPARATOR + processName + ".exe";
  }

  STARTUPINFO info = { sizeof( info ) };
  PROCESS_INFORMATION processInfo;
  if( CreateProcess( processPathName.c_str(), nullptr, nullptr, nullptr, TRUE, 0, nullptr, nullptr, &info, &processInfo ) )
  {
    WaitForSingleObject( processInfo.hProcess, INFINITE );
    CloseHandle( processInfo.hProcess );
    CloseHandle( processInfo.hThread );
  }
}
