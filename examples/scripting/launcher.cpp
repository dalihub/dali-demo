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

// CLASS HEADER
#include "launcher.h"

// EXTERNAL INCLUDES
#include <fstream>
#include <sys/stat.h>
#include <dali/integration-api/debug.h>



using namespace Dali;

#define TOKEN_STRING(x) #x

namespace
{
std::string GetFileContents(const std::string& filename)
{
  std::ifstream t(filename.c_str());
  return std::string((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
};

} // unnamed namespace

Launcher::Launcher( Dali::Application application, std::string layoutFileName, std::string scriptFileName )
: mApplication( application ),
  mJSONFileName(layoutFileName ),
  mJavaScriptFileName( scriptFileName )
{

  mApplication.InitSignal().Connect( this, &Launcher::Create );
}

Launcher::~Launcher()
{
}

void Launcher::Create( Dali::Application& application )
{
  // Get a handle to the stage
  Stage stage = Stage::GetCurrent();

  // change the background color to purple
  Stage::GetCurrent().SetBackgroundColor( Vector4(0.2,0.2,0.4,1.0) );

  // Try loading a JSON file
  if( !mJSONFileName.empty() )
  {
    mBuilder = Toolkit::Builder::New();

    Property::Map defaultDirs;
    defaultDirs[ TOKEN_STRING(DALI_IMAGE_DIR) ]  = DALI_IMAGE_DIR;
    defaultDirs[ TOKEN_STRING(DALI_MODEL_DIR) ]  = DALI_MODEL_DIR;
    defaultDirs[ TOKEN_STRING(DALI_SCRIPT_DIR) ] = DALI_SCRIPT_DIR;
    mBuilder.AddConstants( defaultDirs );

    std::string json_data(GetFileContents( mJSONFileName ));
    mBuilder.LoadFromString(json_data);
    mBuilder.AddActors( stage.GetRootLayer() );
  }

  // Try load a JavaScript file
  if( !mJavaScriptFileName.empty() )
  {
    // execute the script
    mScript = Toolkit::Script::New();

    mScript.ExecuteFile( mJavaScriptFileName);
  }
}

void Launcher::MainLoop()
{
  mApplication.MainLoop();
}
