/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <string>
#include <stdio.h>
#include <sys/stat.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include "launcher.h"

namespace
{
bool CheckIfFileExists( const std::string& filename )
{
  struct stat buf;
  // fstat returns -1 on error
  if (stat( filename.c_str(), &buf) != -1)
  {
      return true;
  }
  return false;
}

}
int DALI_EXPORT_API main( int argc, char* argv[] )
{
  // pull out the JSON file and JavaScript file from the command line arguments
  std::string javaScriptFileName;
  std::string jSONFileName;

  for( int i = 1 ; i < argc ; ++i )
  {
    std::string arg( argv[i] );

    size_t idx = arg.find( ".json" );
    if( idx != std::string::npos )
    {
      jSONFileName = arg;
    }
    else
    {
      idx = arg.find( ".js" );
      if( idx != std::string::npos )
      {
        javaScriptFileName = arg;
      }
    }
  }

  if( !jSONFileName.empty() )
  {
    bool exists = CheckIfFileExists( jSONFileName );
    if( !exists )
    {
      DALI_ASSERT_ALWAYS( 0 && "JSON file not found ")
    }
  }
  if( !javaScriptFileName.empty() )
  {
    bool exists = CheckIfFileExists( javaScriptFileName );
    if( !exists )
    {
        DALI_ASSERT_ALWAYS( 0 && "JavaScript file not found ")
    }
  }
  if( jSONFileName.empty() && javaScriptFileName.empty() )
  {
    printf("Please specify a JSON and/or JavaScript file to load, e.g. scripting.example mylayout.json my-test.js\n");
    return -1;
  }


  Launcher daliApplication( Dali::Application::New( &argc, &argv, DEMO_THEME_PATH ), jSONFileName, javaScriptFileName );

  daliApplication.MainLoop();

  return 0;
}
