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
 * @brief Example of stylable transitions.
 */

// External includes
#include <dali/dali.h>

// Internal includes
#include "transition-application.h"


int DALI_EXPORT_API main( int argc, char** argv )
{
  const char* themeName = Demo::TransitionApplication::DEMO_THEME_ONE_PATH;

  Application application = Application::New( &argc, &argv, themeName );
  Demo::TransitionApplication transitionApplication( application );
  application.MainLoop();
  return 0;
}
