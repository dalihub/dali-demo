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
 */

#ifndef ANDROID_DALI_DEMO_NATIVE_ACTIVITY_JNI_H
#define ANDROID_DALI_DEMO_NATIVE_ACTIVITY_JNI_H

#include <android_native_app_glue.h>
#include <string>

class DaliDemoNativeActivity
{
public:
  DaliDemoNativeActivity(ANativeActivity* activity)
  : activity(activity)
  {
  }

  class JString
  {
  public:
    JString(JNIEnv* env, const std::string& str)
    : env(env),
      string(env->NewStringUTF(str.c_str()))
    {
    }

    JString(JNIEnv* env, jstring str)
    : env(env),
      string(str)
    {
    }

    std::string ToString()
    {
      std::string out;
      if(string)
      {
        const char* utf = env->GetStringUTFChars(string, 0);
        out             = std::string(utf);
        env->ReleaseStringUTFChars(string, utf);
      }
      return out;
    }

    ~JString()
    {
      if(string)
      {
        env->DeleteLocalRef(string);
      }
    }

  private:
    friend class DaliDemoNativeActivity;
    JNIEnv* env;
    jstring string;
  };

  class NativeActivityJNI
  {
  public:
    NativeActivityJNI(ANativeActivity* activity)
    : activity(activity)
    {
      activity->vm->AttachCurrentThread(&env, nullptr);
      clazz = env->GetObjectClass(activity->clazz);
    }

    ~NativeActivityJNI()
    {
      activity->vm->DetachCurrentThread();
    }

    std::string CallStringMethod(const std::string& name, const std::string& arg)
    {
      jmethodID methodID = env->GetMethodID(clazz, name.c_str(), "(Ljava/lang/String;)Ljava/lang/String;");
      JString   argument(env, arg);
      JString   returnValue(env, (jstring)env->CallObjectMethod(activity->clazz, methodID, argument.string));
      return returnValue.ToString();
    }

    void CallVoidMethod(const std::string& name, const std::string& arg)
    {
      jmethodID methodID = env->GetMethodID(clazz, name.c_str(), "(Ljava/lang/String;)V");
      JString   argument(env, arg);
      env->CallVoidMethod(activity->clazz, methodID, argument.string);
    }

  private:
    ANativeActivity* activity;
    JNIEnv*          env;
    jclass           clazz;
  };

  std::string GetMetaData(const std::string& key)
  {
    NativeActivityJNI nativeActivityJNI(activity);
    return nativeActivityJNI.CallStringMethod("getMetaData", key);
  }

  std::string GetIntentStringExtra(const std::string& key)
  {
    NativeActivityJNI nativeActivityJNI(activity);
    return nativeActivityJNI.CallStringMethod("getIntentStringExtra", key);
  }

  void LaunchExample(const std::string& exampleName)
  {
    NativeActivityJNI nativeActivityJNI(activity);
    return nativeActivityJNI.CallVoidMethod("launchExample", exampleName);
  }

private:
  ANativeActivity* activity;
};

#endif //ANDROID_DALI_DEMO_NATIVE_ACTIVITY_JNI_H
