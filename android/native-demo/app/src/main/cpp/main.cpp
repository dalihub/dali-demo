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
 *
 */

// EXTERNAL INCLUDES
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <android/log.h>
#include <android_native_app_glue.h>
#include <dali/devel-api/adaptor-framework/application-devel.h>
#include <dali/integration-api/debug.h>

// from android_native_app_glue.c
#ifndef NDEBUG
#  define LOGV(...)  ((void)__android_log_print(ANDROID_LOG_VERBOSE, "threaded_app", __VA_ARGS__))
#else
#  define LOGV(...)  ((void)0)
#endif

namespace
{
    void free_saved_state(struct android_app *android_app) {
        pthread_mutex_lock(&android_app->mutex);
        if (android_app->savedState != NULL) {
            free(android_app->savedState);
            android_app->savedState = NULL;
            android_app->savedStateSize = 0;
        }
        pthread_mutex_unlock(&android_app->mutex);
    }

    void android_app_destroy(struct android_app *android_app) {
        LOGV("android_app_destroy!");
        free_saved_state(android_app);
        pthread_mutex_lock(&android_app->mutex);
        if (android_app->inputQueue != NULL) {
            AInputQueue_detachLooper(android_app->inputQueue);
        }
        AConfiguration_delete(android_app->config);
        android_app->destroyed = 1;
        pthread_cond_broadcast(&android_app->cond);
        pthread_mutex_unlock(&android_app->mutex);
        // Can't touch android_app object after this.
    }
}

//#include <examples/hello-world/hello-world-example.cpp>
//#include <examples/homescreen-benchmark/homescreen-benchmark.cpp>
//#include <examples/benchmark/benchmark.cpp>

std::string startOnQuit;
#include <shared/dali-table-view.cpp>
#include <shared/file-wrapper.cpp>
#include <demo/dali-demo.cpp>
#include <examples/animated-gradient-call-active/animated-gradient-call-active.cpp>
#include <examples/animated-gradient-card-active/animated-gradient-card-active.cpp>
#include <examples/blocks/blocks-example.cpp>
#include <examples/bezier-curve/bezier-curve-example.cpp>
#include <examples/bubble-effect/bubble-effect-example.cpp>
#include <examples/contact-cards/contact-cards-example.cpp>
#include <examples/cube-transition-effect/cube-transition-effect-example.cpp>
#include <examples/fpp-game/fpp-game-example.cpp>
#include <examples/item-view/item-view-example.cpp>
#include <examples/mesh-visual/mesh-visual-example.cpp>
#include <examples/metaball-explosion/metaball-explosion-example.cpp>
#include <examples/metaball-refrac/metaball-refrac-example.cpp>
#include <examples/motion-blur/motion-blur-example.cpp>
#include <examples/refraction-effect/refraction-effect-example.cpp>
#include <examples/renderer-stencil/renderer-stencil-example.cpp>
#include <examples/rendering-skybox/rendering-skybox.cpp>
#include <examples/rendering-basic-pbr/rendering-basic-pbr-example.cpp>
#include <examples/shadows-and-lights/shadows-and-lights-example.cpp>
#include <examples/sparkle/sparkle-effect-example.cpp>

void AutoHideNavBar(struct android_app* state)
{
    JNIEnv* env;
    state->activity->vm->AttachCurrentThread(&env, NULL);

    jclass activityClass = env->FindClass("android/app/NativeActivity");
    jmethodID getWindow = env->GetMethodID(activityClass, "getWindow", "()Landroid/view/Window;");

    jclass windowClass = env->FindClass("android/view/Window");
    jmethodID getDecorView = env->GetMethodID(windowClass, "getDecorView", "()Landroid/view/View;");

    jclass viewClass = env->FindClass("android/view/View");
    jmethodID setSystemUiVisibility = env->GetMethodID(viewClass, "setSystemUiVisibility", "(I)V");

    jobject window = env->CallObjectMethod(state->activity->clazz, getWindow);

    jobject decorView = env->CallObjectMethod(window, getDecorView);

    jfieldID flagFullscreenID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_FULLSCREEN", "I");
    jfieldID flagHideNavigationID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I");
    jfieldID flagImmersiveStickyID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I");

    const int flagFullscreen = env->GetStaticIntField(viewClass, flagFullscreenID);
    const int flagHideNavigation = env->GetStaticIntField(viewClass, flagHideNavigationID);
    const int flagImmersiveSticky = env->GetStaticIntField(viewClass, flagImmersiveStickyID);
    const int flag = flagFullscreen | flagHideNavigation | flagImmersiveSticky;

    env->CallVoidMethod(decorView, setSystemUiVisibility, flag);

    state->activity->vm->DetachCurrentThread();
}

std::string GetAppCacheDir( struct android_app *app ) {
    JNIEnv* env;
    app->activity->vm->AttachCurrentThread( &env, NULL );

    jclass activityClass = env->FindClass( "android/app/NativeActivity" );
    jmethodID getCacheDir = env->GetMethodID( activityClass, "getCacheDir", "()Ljava/io/File;" );
    jobject cache_dir = env->CallObjectMethod( app->activity->clazz, getCacheDir );

    jclass fileClass = env->FindClass( "java/io/File" );
    jmethodID getPath = env->GetMethodID( fileClass, "getPath", "()Ljava/lang/String;" );
    jstring path_string = (jstring)env->CallObjectMethod( cache_dir, getPath );

    const char *path_chars = env->GetStringUTFChars( path_string, NULL );
    std::string temp_folder( path_chars );

    env->ReleaseStringUTFChars( path_string, path_chars );
    app->activity->vm->DetachCurrentThread();
    return temp_folder;
}

void ExtractAsset( struct android_app* state, std::string assetPath, std::string filePath )
{
    AAsset* asset = AAssetManager_open( state->activity->assetManager, assetPath.c_str(), AASSET_MODE_BUFFER );
    if( asset )
    {
        size_t length = AAsset_getLength( asset ) + 1;

        char* buffer = new char[ length ];
        length = AAsset_read( asset, buffer, length );

        FILE* file = fopen( filePath.c_str(), "wb" );
        if( file )
        {
          fwrite( buffer, 1, length, file );
          fclose( file );
        }

        delete[] buffer;
        AAsset_close( asset );
    }
}

void ExtractAssets( struct android_app* state, std::string assetDirPath, std::string filesDirPath )
{
  AAssetDir* assetDir = AAssetManager_openDir( state->activity->assetManager, assetDirPath.c_str() );
  if( assetDir )
  {
    if( mkdir( filesDirPath.c_str(), S_IRWXU ) != -1 )
    {
      const char *filename = NULL;
      std::string assetPath = assetDirPath + "/";
      while ( ( filename = AAssetDir_getNextFileName( assetDir ) ) != NULL )
      {
        ExtractAsset( state, assetPath + filename, filesDirPath + "/" + filename );
      }
    }

    AAssetDir_close( assetDir );
  }
}

std::string GetCallParameter( struct android_app* state )
{
    JNIEnv *env;
    state->activity->vm->AttachCurrentThread( &env, NULL) ;
    jclass activityClass = env->GetObjectClass( state->activity->clazz );
    jmethodID getIntent = env->GetMethodID( activityClass, "getIntent",
                                            "()Landroid/content/Intent;" );

    jobject intent = env->CallObjectMethod( state->activity->clazz, getIntent );
    jclass intentClass = env->GetObjectClass( intent );
    jmethodID getStringExtra = env->GetMethodID( intentClass, "getStringExtra",
                                                 "(Ljava/lang/String;)Ljava/lang/String;" );

    jstring jsCallParam = (jstring)env->CallObjectMethod( intent, getStringExtra, env->NewStringUTF( "start" ) );
    if( jsCallParam )
    {
        const char *callParam = env->GetStringUTFChars( jsCallParam, 0 );
        std::string callParamString = std::string( callParam );
        env->ReleaseStringUTFChars( jsCallParam, callParam );
        return callParamString;
    }
    else
    {
        return std::string();
    }
}

// "am start -a android.intent.action.MAIN -n com.sec.dali_demo/android.app.NativeActivity --es "start" "blocks.example""
// adb shell settings put global policy_control immersive.full=com.sec.dali_demo

void android_main( struct android_app* state )
{
    DALI_LOG_ERROR( "\n" );

    std::string callParam = GetCallParameter( state );
    DALI_LOG_ERROR( "%s\n", callParam.c_str() );

    std::string filesDir = state->activity->internalDataPath;

    struct stat st = { 0 };
    std::string path = filesDir + "/fonts";

    if( stat( path.c_str(), &st ) == -1 )
    {
      mkdir( filesDir.c_str(), S_IRWXU );
      ExtractAssets( state, "fonts", filesDir + "/fonts" );
      ExtractAssets( state, "fonts/dejavu", filesDir + "/fonts/dejavu" );
      ExtractAssets( state, "fonts/tizen", filesDir + "/fonts/tizen" );
    }
/*
    path = filesDir + "/toolkit";
    if( stat( path.c_str(), &st ) == -1 )
    {
      mkdir( filesDir.c_str(), S_IRWXU );
      ExtractAssets( state, "toolkit", filesDir + "/toolkit" );
      ExtractAssets( state, "toolkit/images", filesDir + "/toolkit/images" );
      ExtractAssets( state, "toolkit/sounds", filesDir + "/toolkit/sounds" );
      ExtractAssets( state, "toolkit/styles", filesDir + "/toolkit/styles" );
      ExtractAssets( state, "toolkit/styles/images", filesDir + "/toolkit/styles/images" );
    }
*/
    path = filesDir + "/resources";
    if( stat( path.c_str(), &st ) == -1 )
    {
        mkdir( filesDir.c_str(), S_IRWXU );
        ExtractAssets( state, "resources", filesDir + "/resources" );
        ExtractAssets( state, "resources/game", filesDir + "/resources/game" );
        ExtractAssets( state, "resources/models", filesDir + "/resources/models" );
        ExtractAssets( state, "resources/scripts", filesDir + "/resources/scripts" );
        ExtractAssets( state, "resources/shaders", filesDir + "/resources/shaders" );
        //ExtractAssets( state, "resources/images", filesDir + "/resources/images" );
        //ExtractAssets( state, "resources/style", filesDir + "/resources/style" );
        //ExtractAssets( state, "resources/style/images", filesDir + "/resources/style/images" );
    }

    AutoHideNavBar( state );

    Dali::DevelApplication::SetApplicationContext( state );

    int status = 0;
    if( callParam == "blocks.example" )
      status = BlocksExample::main( 0, nullptr );
    else if ( callParam == "bezier-curve.example" )
      status = BezierCurveExample::main( 0, nullptr );
    else if ( callParam == "bubble-effect.example")
      status = BubbleEffectExample::main( 0, nullptr );
    else if ( callParam == "contact-cards.example")
      status = ContactCardsExample::main( 0, nullptr );
    else if ( callParam == "cube-transition-effect.example")
      status = CubeTransitionEffectExample::main( 0, nullptr );
    else if ( callParam == "fpp-game.example")
      status = FppGameExample::main( 0, nullptr );
    else if ( callParam == "item-view.example")
      status = ItemViewExample::main( 0, nullptr );
    else if ( callParam == "mesh-visual.example")
      status = MeshVisualExample::main( 0, nullptr );
    else if ( callParam == "metaball-explosion.example")
      status = MetaballExplosionExample::main( 0, nullptr );
    else if ( callParam == "metaball-refrac.example")
      status = MetaballRefracExample::main( 0, nullptr );
    else if ( callParam == "motion-blur.example")
      status = MotionBlurExample::main( 0, nullptr );
    else if ( callParam == "refraction-effect.example")
      status = RefractionEffectExample::main( 0, nullptr );
    else if ( callParam == "renderer-stencil.example")
      status = RendererStencilExample::main( 0, nullptr );
    else if ( callParam == "rendering-basic-pbr.example")
      status = RenderingBasicPbrExample::main( 0, nullptr );
    else if ( callParam == "rendering-skybox.example")
      status = RenderingSkyboxExample::main( 0, nullptr );
    else if ( callParam == "shadows-and-lights.example")
      status = ShadowsAndLightsExample::main( 0, nullptr );
    else if ( callParam == "sparkle.example")
      status = SparkleEffectExample::main( 0, nullptr );
    else if ( callParam == "animated-gradient-call-active.example")
      status = AnimatedGradientCallActiveExample::main( 0, nullptr );
    else if ( callParam == "animated-gradient-card-active.example")
      status = AnimatedGradientCardActiveExample::main( 0, nullptr );
    else
      status = main( 0, nullptr );

    android_app_destroy( state );

    if( !startOnQuit.empty() )
    {
      std::stringstream stream;
      stream << "am start -a android.intent.action.MAIN -n com.sec.dali_demo/android.app.NativeActivity --user 0 --es start " << startOnQuit.c_str();
      system( stream.str().c_str() );
    }

    // TODO: We need to kill the application process manually, DALi cannot restart in the same process due to memory leaks
    std::exit( status );
}

//END_INCLUDE(all)