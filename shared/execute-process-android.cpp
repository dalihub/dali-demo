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
#include <unistd.h>
#include <sstream>

#include <dali/integration-api/adaptor-framework/android/android-framework.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>

#include <android_native_app_glue.h>
#include <dali-demo-native-activity-jni.h>

void ExecuteProcess(const std::string& processName, Dali::Application& application)
{
  struct android_app* nativeApp = Dali::Integration::AndroidFramework::Get().GetNativeApplication();
  if(!nativeApp)
  {
    DALI_LOG_ERROR("Couldn't get native app.");
    return;
  }

  DaliDemoNativeActivity nativeActivity(nativeApp->activity);
  nativeActivity.LaunchExample(processName);
}
