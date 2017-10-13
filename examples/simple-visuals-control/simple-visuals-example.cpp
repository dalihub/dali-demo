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
 */

/**
 * @file transition-example.cpp
 * @brief Example of a control built with visuals
 */

// EXTERNAL INCLUDES
#include <dali/dali.h>

// INTERNAL INCLUDES
#include "simple-visuals-application.h"

namespace
{
// Style sheet to be used by this application
const char* SIMPLE_DEMO_THEME( DEMO_STYLE_DIR "simple-example-theme.json" );
}

int DALI_EXPORT_API main( int argc, char** argv )
{
  Application application = Application::New( &argc, &argv, SIMPLE_DEMO_THEME ); // Use the above defined style sheet for this application.
  Demo::SimpleVisualsApplication simpleVisualsApplication( application );
  application.MainLoop();
  return 0;
}
