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
#include <sstream>
#include <unistd.h>
#include <dali/public-api/common/dali-common.h>

void ExecuteProcess( const std::string& processName, Dali::Application& application )
{
  std::stringstream stream;
  stream << "am start -a android.intent.action.MAIN -n com.sec.dalidemo/.DaliDemoNativeActivity --user 0 --es start " << processName.c_str();
  pid_t parentPID = getpid();

  pid_t pid = fork();
  if( pid == 0 )
  {
    do
    {
      sleep( 1 );
    }
    while( kill( parentPID, 0 ) == 0 );

    system( stream.str().c_str() );
    exit( 0 );
  }
  else
  {
    application.Quit();
  }
}
