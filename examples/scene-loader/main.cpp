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
#include "scene-loader-example.h"

#include "dali/dali.h"

using namespace Dali;

int DALI_EXPORT_API main(int argc, char** argv)
{
  auto app = Application::New(&argc, &argv, DEMO_THEME_PATH);
  SceneLoaderExample sceneLoader(app);
  app.MainLoop();
  return 0;
}
