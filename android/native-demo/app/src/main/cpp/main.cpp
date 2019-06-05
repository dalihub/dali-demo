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

#include <examples/alpha-blending-cpu/alpha-blending-cpu-example.cpp>
#include <examples/animated-gradient-call-active/animated-gradient-call-active.cpp>
#include <examples/animated-gradient-card-active/animated-gradient-card-active.cpp>
#include <examples/animated-images/animated-images-example.cpp>
#include <examples/animated-shapes/animated-shapes-example.cpp>
#include <examples/animated-vector-images/animated-vector-images-example.cpp>
#include <examples/bezier-curve/bezier-curve-example.cpp>
#include <examples/blocks/blocks-example.cpp>
#include <examples/bubble-effect/bubble-effect-example.cpp>
#include <examples/buttons/buttons-example.cpp>
#include <examples/clipping/clipping-example.cpp>
#include <examples/clipping-draw-order/clipping-draw-order.cpp>
#include <examples/compressed-texture-formats/compressed-texture-formats-example.cpp>
#include <examples/contact-cards/contact-cards-example.cpp>
#include <examples/cube-transition-effect/cube-transition-effect-example.cpp>
#include <examples/dissolve-effect/dissolve-effect-example.cpp>
#include <examples/drag-and-drop/drag-and-drop-example.cpp>
#include <examples/effects-view/effects-view-example.cpp>
#include <examples/flex-container/flex-container-example.cpp>
#include <examples/focus-integration/focus-integration.cpp>
#include <examples/fpp-game/fpp-game-example.cpp>
#include <examples/frame-callback/frame-callback-example.cpp>
#include <examples/gestures/gesture-example.cpp>
#include <examples/gradients/gradients-example.cpp>
#include <examples/hello-world/hello-world-example.cpp>
#include <examples/image-policies/image-policies-example.cpp>
#include <examples/image-scaling-and-filtering/image-scaling-and-filtering-example.cpp>
#include <examples/image-scaling-irregular-grid/image-scaling-irregular-grid-example.cpp>
#include <examples/image-view-alpha-blending/image-view-alpha-blending-example.cpp>
#include <examples/image-view/image-view-example.cpp>
#include <examples/image-view-pixel-area/image-view-pixel-area-example.cpp>
#include <examples/image-view-svg/image-view-svg-example.cpp>
#include <examples/image-view-url/image-view-url-example.cpp>
#include <examples/item-view/item-view-example.cpp>
#include <examples/line-mesh/line-mesh-example.cpp>
#include <examples/magnifier/magnifier-example.cpp>
#include <examples/mesh-morph/mesh-morph-example.cpp>
#include <examples/mesh-visual/mesh-visual-example.cpp>
#include <examples/metaball-explosion/metaball-explosion-example.cpp>
#include <examples/metaball-refrac/metaball-refrac-example.cpp>
#include <examples/model3d-view/model3d-view-example.cpp>
#include <examples/motion-blur/motion-blur-example.cpp>
#include <examples/motion-stretch/motion-stretch-example.cpp>
#include <examples/native-image-source/native-image-source-example.cpp>
#include <examples/pivot/pivot-example.cpp>
#include <examples/point-mesh/point-mesh-example.cpp>
#include <examples/popup/popup-example.cpp>
#include <examples/pre-render-callback/pre-render-callback-example.cpp>
#include <examples/primitive-shapes/primitive-shapes-example.cpp>
#include <examples/progress-bar/progress-bar-example.cpp>
#include <examples/property-notification/property-notification-example.cpp>
#include <examples/ray-marching/ray-marching-example.cpp>
#include <examples/refraction-effect/refraction-effect-example.cpp>
#include <examples/remote-image-loading/remote-image-loading-example.cpp>
#include <examples/renderer-stencil/renderer-stencil-example.cpp>
#include <examples/rendering-basic-light/rendering-basic-light-example.cpp>
#include <examples/rendering-basic-pbr/rendering-basic-pbr-example.cpp>
#include <examples/rendering-cube/rendering-cube.cpp>
#include <examples/rendering-line/rendering-line.cpp>
#include <examples/rendering-radial-progress/radial-progress.cpp>
#include <examples/rendering-skybox/rendering-skybox.cpp>
#include <examples/rendering-textured-cube/rendering-textured-cube.cpp>
#include <examples/rendering-triangle/rendering-triangle.cpp>
// #include <examples/scripting/scripting-example.cpp> - multiple definition of 'DemoHelper::
// ./dali-adaptor/dali/public-api/adaptor-framework/native-image-source.cpp:111: error: undefined reference to 'Dali::Internal::Adaptor::GetNativeImageSourceFactory()'
#include <examples/scroll-view/scroll-view-example.cpp>
#include <examples/shadows-and-lights/shadows-and-lights-example.cpp>
#include <examples/simple-bitmap-font-text-label/simple-text-label-example.cpp>
#include <examples/simple-text-label/simple-text-label-example.cpp>
/*
#include <examples/simple-text-renderer/simple-text-renderer-example.cpp>
 error: undefined reference to 'Dali::TextAbstraction::Internal::TextRenderer
*/
#include <examples/simple-text-visual/simple-text-visual-example.cpp>
#include <examples/simple-visuals-control/simple-visuals-example.cpp>
#include <examples/size-negotiation/size-negotiation-example.cpp>
#include <examples/sparkle/sparkle-effect-example.cpp>
#include <examples/styling/style-example.cpp>
#include <examples/text-editor/text-editor-example.cpp>
#include <examples/text-field/text-field-example.cpp>
#include <examples/text-fonts/text-fonts-example.cpp>
#include <examples/text-label-emojis/text-label-emojis.cpp>
#include <examples/text-label-multi-language/text-label-multi-language-example.cpp>
#include <examples/text-label/text-label-example.cpp>
#include <examples/text-memory-profiling/text-memory-profiling-example.cpp>
#include <examples/text-overlap/text-overlap-example.cpp>
#include <examples/text-scrolling/text-scrolling-example.cpp>
#include <examples/textured-mesh/textured-mesh-example.cpp>
/*
#include <examples/tilt/tilt-example.cpp>
tilt-sensor.cpp:37: error: undefined reference to 'Dali::Internal::Adaptor::TiltSensorFactory::Get()'
*/
#include <examples/tooltip/tooltip-example.cpp>
#include <examples/transitions/transition-example.cpp>
/* no video support
#include <examples/video-view/video-view-example.cpp>
*/
#include <examples/visual-transitions/transition-example.cpp>


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
    std::string callParam = GetCallParameter( state );
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
    else if ( callParam == "animated-gradient-call-active.example" )
      status = AnimatedGradientCallActiveExample::main( 0, nullptr );
    else if ( callParam == "animated-gradient-card-active.example" )
      status = AnimatedGradientCardActiveExample::main( 0, nullptr );
    else if ( callParam == "animated-images.example" )
      status = AnimatedImagesExample::main( 0, nullptr );
    else if ( callParam == "animated-shapes.example" )
      status = AnimatedShapesExample::main( 0, nullptr );
    else if ( callParam == "animated-vector-images.example" )
      status = AnimatedVectorImagesExample::main( 0, nullptr );
    else if ( callParam == "alpha-blending-cpu.example" )
      status = AlphaBlendingCpuExample::main( 0, nullptr );
    else if ( callParam == "buttons.example" )
      status = ButtonsExample::main( 0, nullptr );
    else if ( callParam == "clipping.example" )
      status = ClippingExample::main( 0, nullptr );
    else if ( callParam == "clipping-draw-order.example" )
      status = ClippingDrawOrderExample::main( 0, nullptr );
    else if ( callParam == "dissolve-effect.example" )
      status = DissolveEffectExample::main( 0, nullptr );
    else if ( callParam == "drag-and-drop.example" )
      status = DragAndDropExample::main( 0, nullptr );
    else if ( callParam == "effects-view.example" )
      status = EffectsViewExample::main( 0, nullptr );
    else if ( callParam == "flex-container.example" )
      status = FlexContainerExample::main( 0, nullptr );
    else if ( callParam == "frame-callback.example" )
      status = FrameCallbackExample::main( 0, nullptr );
    else if ( callParam == "focus-integration.example" )
      status = FocusIntegrationExample::main( 0, nullptr );
    else if ( callParam == "gestures.example" )
      status = GestureExample::main( 0, nullptr );
    else if ( callParam == "gradients.example" )
      status = GradientsExample::main( 0, nullptr );
    else if ( callParam == "hello-world.example" )
      status = HelloWorldExample::main( 0, nullptr );
    else if ( callParam == "image-policies.example" )
      status = ImagePoliciesExample::main( 0, nullptr );
    else if ( callParam == "image-scaling-and-filtering.example" )
      status = ImageScalingAndFilteringExample::main( 0, nullptr );
    else if ( callParam == "image-scaling-irregular-grid.example" )
      status = ImageScalingIrregularGridExample::main( 0, nullptr );
    else if ( callParam == "image-view.example" )
      status = ImageViewExample::main( 0, nullptr );
    else if ( callParam == "image-view-alpha-blending.example" )
      status = ImageViewPixelAreaExample::main( 0, nullptr );
    else if ( callParam == "image-view-pixel-area.example" )
      status = ImageViewPixelAreaExample::main( 0, nullptr );
    else if ( callParam == "image-view-svg.example" )
      status = ImageViewSvgExample::main( 0, nullptr );
    else if ( callParam == "image-view-url.example" )
      status = ImageViewUrlExample::main( 0, nullptr );
    else if ( callParam == "line-mesh.example" )
      status = LineMeshExample::main( 0, nullptr );
    else if ( callParam == "magnifier.example" )
      status = MagnifierExample::main( 0, nullptr );
    else if ( callParam == "mesh-morph.example" )
      status = MeshMorphExample::main( 0, nullptr );
    else if ( callParam == "motion-stretch.example" )
      status = MotionStretchExample::main( 0, nullptr );
    else if ( callParam == "native-image-source.example" )
      status = NativeImageSourceExample::main( 0, nullptr );
    else if ( callParam == "popup.example" )
      status = PopupExample::main( 0, nullptr );
    else if ( callParam == "pivot.example" )
      status = PivotExample::main( 0, nullptr );
    else if ( callParam == "primitive-shapes.example" )
      status = PrimitiveShapesExample::main( 0, nullptr );
    else if ( callParam == "progress-bar.example" )
      status = ProgressBarExample::main( 0, nullptr );
    else if ( callParam == "rendering-basic-light.example" )
      status = RenderingBasicLightExample::main( 0, nullptr );
    else if ( callParam == "rendering-line.example" )
      status = RenderingLineExample::main( 0, nullptr );
    else if ( callParam == "rendering-triangle.example" )
      status = RenderingTriangleExample::main( 0, nullptr );
    else if ( callParam == "rendering-cube.example" )
      status = RenderingCubeExample::main( 0, nullptr );
    else if ( callParam == "rendering-textured-cube.example" )
      status = RenderingTexturedCubeExample::main( 0, nullptr );
    else if ( callParam == "rendering-radial-progress.example" )
      status = RenderingRadialProgressExample::main( 0, nullptr );
    else if ( callParam == "ray-marching.example" )
      status = RayMarchingExample::main( 0, nullptr );
    else if ( callParam == "scroll-view.example" )
      status = ScrollViewExample::main( 0, nullptr );
    else if ( callParam == "size-negotiation.example" )
      status = SizeNegotiationExample::main( 0, nullptr );
    else if ( callParam == "styling.example" )
      status = StyleExample::main( 0, nullptr );
    else if ( callParam == "text-editor.example" )
      status = TextEditorExample::main( 0, nullptr );
    else if ( callParam == "text-field.example" )
      status = TextFieldExample::main( 0, nullptr );
    else if ( callParam == "text-label.example" )
      status = TextLabelExample::main( 0, nullptr );
    else if ( callParam == "text-label-multi-language.example" )
      status = TextLabelMultiLanguageExample::main( 0, nullptr );
    else if ( callParam == "text-label-emojis.example" )
      status = TextLabelEmojisExample::main( 0, nullptr );
    else if ( callParam == "text-scrolling.example" )
      status = TextScrollingExample::main( 0, nullptr );
    else if ( callParam == "remote-image-loading.example" )
      status = RemoteImageLoadingExample::main( 0, nullptr );
    else if ( callParam == "textured-mesh.example" )
      status = TexturedMeshExample::main( 0, nullptr );
    else if ( callParam == "tooltip.example" )
      status = TooltipExample::main( 0, nullptr );
    else if ( callParam == "transitions.example" )
      status = TransitionExample::main( 0, nullptr );
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