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

// EXTERNAL INCLUDES
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <android/log.h>
#include <android_native_app_glue.h>
#include <dali-demo-native-activity-jni.h>
#include <dali/devel-api/adaptor-framework/application-devel.h>
#include <dali/integration-api/adaptor-framework/android/android-framework.h>
#include <dali/integration-api/debug.h>
#include <dlfcn.h>

// from android_native_app_glue.c

#define TAG "dalidemo"
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,   TAG, ##__VA_ARGS__))
#ifndef NDEBUG
#define LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, TAG, ##__VA_ARGS__))
#else
#define LOGV(...) ((void)0)
#endif

namespace
{
void free_saved_state(struct android_app* android_app)
{
  LOGV("free_saved_state");
  pthread_mutex_lock(&android_app->mutex);

  if(android_app->savedState != NULL)
  {
    free(android_app->savedState);
    android_app->savedState     = NULL;
    android_app->savedStateSize = 0;
  }

  pthread_mutex_unlock(&android_app->mutex);
}

void android_app_destroy(struct android_app* android_app)
{
  LOGV("android_app_destroy");
  free_saved_state(android_app);
  pthread_mutex_lock(&android_app->mutex);

  if(android_app->inputQueue != NULL)
  {
    AInputQueue_detachLooper(android_app->inputQueue);
  }

  AConfiguration_delete(android_app->config);
  android_app->destroyed = 1;

  pthread_cond_broadcast(&android_app->cond);
  pthread_mutex_unlock(&android_app->mutex);

  // Can't touch android_app object after this.
}

} // namespace

void ExtractAsset(struct android_app* state, const std::string& assetPath, const std::string& filePath)
{
  AAsset* asset = AAssetManager_open(state->activity->assetManager, assetPath.c_str(), AASSET_MODE_BUFFER);
  if(asset)
  {
    size_t length = AAsset_getLength(asset) + 1;

    char* buffer = new char[length];
    length       = AAsset_read(asset, buffer, length);

    FILE* file = fopen(filePath.c_str(), "wb");
    if(file)
    {
      fwrite(buffer, 1, length, file);
      fclose(file);
    }

    delete[] buffer;
    AAsset_close(asset);
  }
}

void ExtractAssets(struct android_app* state, const std::string& assetDirPath, const std::string& filesDirPath)
{
  AAssetDir* assetDir = AAssetManager_openDir(state->activity->assetManager, assetDirPath.c_str());
  if(assetDir)
  {
    if(mkdir(filesDirPath.c_str(), S_IRWXU) != -1)
    {
      const char* filename  = NULL;
      std::string assetPath = assetDirPath + "/";
      while((filename = AAssetDir_getNextFileName(assetDir)) != NULL)
      {
        ExtractAsset(state, assetPath + filename, filesDirPath + "/" + filename);
      }
    }

    AAssetDir_close(assetDir);
  }
}

void ExtractFontConfig(struct android_app* state, std::string assetFontConfig, std::string fontsPath)
{
  AAsset* asset = AAssetManager_open(state->activity->assetManager, assetFontConfig.c_str(), AASSET_MODE_BUFFER);
  if(asset)
  {
    size_t length = AAsset_getLength(asset) + 1;

    char* buffer = new char[length];
    length       = AAsset_read(asset, buffer, length);

    std::string fontConfig = std::string(buffer, length);
    int         i          = fontConfig.find("~");
    if(i != std::string::npos)
    {
      std::string filesDir = state->activity->internalDataPath;
      fontConfig.replace(i, 1, filesDir);
    }

    std::string fontsFontConfig = fontsPath;
    FILE*       file            = fopen(fontsFontConfig.c_str(), "wb");
    if(file)
    {
      fwrite(fontConfig.c_str(), 1, fontConfig.size(), file);
      fclose(file);
    }

    delete[] buffer;
    AAsset_close(asset);
  }
}

extern "C" void FcConfigPathInit(const char* path, const char* file);

void android_main(struct android_app* state)
{
  LOGV("android_main() >>");

  std::string filesDir = state->activity->internalDataPath;
  LOGV("filesDir=%s", filesDir.c_str() );

  std::string fontconfigPath = filesDir + "/fonts";
  setenv("FONTCONFIG_PATH", fontconfigPath.c_str(), 1);

  std::string fontconfigFile = fontconfigPath + "/fonts.conf";
  setenv("FONTCONFIG_FILE", fontconfigFile.c_str(), 1);

  struct stat st = {0};
  FcConfigPathInit(fontconfigPath.c_str(), fontconfigFile.c_str());

  if(stat(fontconfigPath.c_str(), &st) == -1)
  {
    mkdir(fontconfigPath.c_str(), S_IRWXU);
    ExtractFontConfig(state, "fonts/fonts.conf", fontconfigPath + "/fonts.conf");
    ExtractFontConfig(state, "fonts/fonts.dtd", fontconfigPath + "/fonts.dtd");
    ExtractFontConfig(state, "fonts/local.conf", fontconfigPath + "/local.conf");
    ExtractAssets(state, "fonts/dejavu", fontconfigPath + "/dejavu");
    ExtractAssets(state, "fonts/tizen", fontconfigPath + "/tizen");
    ExtractAssets(state, "fonts/bitmap", fontconfigPath + "/bitmap");
  }

  Dali::Integration::AndroidFramework::New();
  Dali::Integration::AndroidFramework::Get().SetNativeApplication(state);
  Dali::Integration::AndroidFramework::Get().SetApplicationConfiguration(state->config);
  Dali::Integration::AndroidFramework::Get().SetApplicationAssets(state->activity->assetManager);
  Dali::Integration::AndroidFramework::Get().SetInternalDataPath(filesDir);

  DaliDemoNativeActivity nativeActivity(state->activity);

  int         status    = 0;

  //dali requires Android 8 or higher
  //Android 6+ support loading library directly from apk,
  //therefore no need to extract to filesystem first then open by specifying full path
  //unless there is need to do profiling, or export libraries so that other packages can use
  std::string libpath   = "libdali-demo.so";

  std::string callParam = nativeActivity.GetIntentStringExtra("start");
  if(callParam.empty())
  {
    callParam = nativeActivity.GetMetaData("start");
  }

  if(!callParam.empty())
  {
    libpath = "lib" + callParam + ".so";
  }

  void* handle = dlopen(libpath.c_str(), RTLD_LAZY);
  if(!handle)
  {
    int err = errno;
    LOGE("Err=%d Fail to open lib %s", err, libpath.c_str());
    status = err;
    std::exit(status);
  }

  dlerror(); /* Clear any existing error */

  int (*main)(int, char**) = (int (*)(int, char**))dlsym(handle, "main");
  if(main)
  {
    status = main(0, nullptr);
  }

  if(handle)
  {
    dlclose(handle);
    handle = nullptr;
  }

  android_app_destroy(state);

  Dali::Integration::AndroidFramework::Get().SetNativeApplication(nullptr);
  Dali::Integration::AndroidFramework::Get().SetApplicationConfiguration(nullptr);
  Dali::Integration::AndroidFramework::Get().SetApplicationAssets(nullptr);
  Dali::Integration::AndroidFramework::Delete();

  LOGV("android_main() <<");

  // We need to kill the application process manually, DALi cannot exit the process properly due to memory leaks
  std::exit(status);
}

//END_INCLUDE(all)
