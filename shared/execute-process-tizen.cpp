/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>
#include <app_control.h>

namespace {

#define MATCH_THEN_RETURN_ERROR_NAME(ret, error_type) \
do                                                    \
{                                                     \
  if(ret == error_type)                               \
  {                                                   \
   return #error_type;                                \
  }                                                   \
} while(0)

const char* GetAppControlErrorName(int ret)
{
  MATCH_THEN_RETURN_ERROR_NAME(ret, APP_CONTROL_ERROR_NONE);
  MATCH_THEN_RETURN_ERROR_NAME(ret, APP_CONTROL_ERROR_INVALID_PARAMETER);
  MATCH_THEN_RETURN_ERROR_NAME(ret, APP_CONTROL_ERROR_OUT_OF_MEMORY);
  MATCH_THEN_RETURN_ERROR_NAME(ret, APP_CONTROL_ERROR_APP_NOT_FOUND);
  MATCH_THEN_RETURN_ERROR_NAME(ret, APP_CONTROL_ERROR_KEY_NOT_FOUND);
  MATCH_THEN_RETURN_ERROR_NAME(ret, APP_CONTROL_ERROR_KEY_REJECTED);
  MATCH_THEN_RETURN_ERROR_NAME(ret, APP_CONTROL_ERROR_INVALID_DATA_TYPE);
  MATCH_THEN_RETURN_ERROR_NAME(ret, APP_CONTROL_ERROR_LAUNCH_REJECTED);
  MATCH_THEN_RETURN_ERROR_NAME(ret, APP_CONTROL_ERROR_PERMISSION_DENIED);
  MATCH_THEN_RETURN_ERROR_NAME(ret, APP_CONTROL_ERROR_LAUNCH_FAILED);
  MATCH_THEN_RETURN_ERROR_NAME(ret, APP_CONTROL_ERROR_TIMED_OUT);
  MATCH_THEN_RETURN_ERROR_NAME(ret, APP_CONTROL_ERROR_IO_ERROR);
  return "Unknown error!";
}

} // unnamed namespace

void ExecuteProcess(const std::string& processName, Dali::Application& application)
{
  app_control_h handle;
  app_control_create(&handle);
  app_control_set_app_id(handle, processName.c_str());

  DALI_LOG_RELEASE_INFO("Try to launch app[%s]\n", processName.c_str());
  int ret = app_control_send_launch_request(handle, NULL, NULL);
  if(ret == APP_CONTROL_ERROR_NONE)
  {
    DALI_LOG_RELEASE_INFO("Succeeded to launch app[%s]\n", processName.c_str());
  }
  else
  {
    DALI_LOG_ERROR("Fail to launch app[%s] ret=%d(%s)\n",processName.c_str(), ret, GetAppControlErrorName(ret));
    DALI_ASSERT_ALWAYS(false && "Send launch request failed!");
  }
  app_control_destroy(handle);
}
