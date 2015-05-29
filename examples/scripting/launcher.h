#ifndef DALI_APP_H
#define DALI_APP_H

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
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/scripting/script.h>
#include <dali-toolkit/devel-api/builder/builder.h>

/**
 * Example app that can load both JSON and JavaScript files from command line
 * E.g.   scripting.example my-first.js  my-first.json
 * See dali-demo/resources/scripts for example JSON and JavaScript files
 */
class Launcher: public Dali::ConnectionTracker
{
public:

  /**
   * @brief Construcctor
   * @param application application
   * @param layoutFileName JSON file to run
   * @param scriptFileName JavaScript file to run
   */
  Launcher( Dali::Application application, std::string layoutFileName, std::string scriptFileName );

  /**
   * @brief destructor
   */
  ~Launcher();

  /**
   * @brief create app
   */
  void Create( Dali::Application& application );

  /**
   * @brief run application MainLoop
   */
  void MainLoop();

private:

  Dali::Toolkit::Script mScript;      ///< Used to load and execute JavaScript
  Dali::Toolkit::Builder mBuilder;    ///< Used to parse JSON
  Dali::Application mApplication;     ///< application
  std::string mJSONFileName;          ///< JSON filename
  std::string mJavaScriptFileName;    ///< JavaScript filename
};

#endif // header
