cmake_minimum_required(VERSION 3.4.1)

set(DALI_ANDROID "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../..")

# build native_app_glue as a static lib
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

add_definitions(-DAPPLICATION_RESOURCE_PATH=\"assets/resources/\")
add_definitions(-DDEMO_IMAGE_DIR=\"assets/resources/images/\")
add_definitions(-DDEMO_THEME_PATH=\"assets/resources/style/demo-theme.json\")
add_definitions(-DDEMO_STYLE_DIR=\"assets/resources/style/\")
#add_definitions(-DDEMO_MODEL_DIR=\"assets/resources/models/\")
#add_definitions(-DDEMO_SHADER_DIR=\"assets/resources/shaders/\")
add_definitions(-DDEMO_GAME_DIR=\"/data/data/com.sec.dali_demo/files/resources/game/\")
add_definitions(-DDEMO_MODEL_DIR=\"/data/data/com.sec.dali_demo/files/resources/models/\")
add_definitions(-DDEMO_SHADER_DIR=\"/data/data/com.sec.dali_demo/files/resources/shaders/\")
add_definitions(-DDEMO_EXAMPLE_BIN=\"\")

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -frtti -w -Wall -std=gnu++11")

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(native-activity SHARED main.cpp)

target_include_directories(native-activity PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue)

# DALi
include_directories(${DALI_ANDROID}/dali-env/android/include)
include_directories(${DALI_ANDROID}/android-dependencies/libexif)
include_directories(${DALI_ANDROID}/android-dependencies/libfontconfig)
include_directories(${DALI_ANDROID}/android-dependencies/libfreetype)
include_directories(${DALI_ANDROID}/android-dependencies/libgif)
include_directories(${DALI_ANDROID}/android-dependencies/libjpeg-turbo)
include_directories(${DALI_ANDROID}/android-dependencies/libpng)
include_directories(${DALI_ANDROID}/dali-demo)

add_library(contact-cards-example STATIC
        ${DALI_ANDROID}/dali-demo/examples/contact-cards/clipped-image.cpp
        ${DALI_ANDROID}/dali-demo/examples/contact-cards/masked-image.cpp
        ${DALI_ANDROID}/dali-demo/examples/contact-cards/contact-card-layouter.cpp
        ${DALI_ANDROID}/dali-demo/examples/contact-cards/contact-card.cpp
        ${DALI_ANDROID}/dali-demo/examples/contact-cards/contact-data.cpp)

add_library(rendering-skybox-example STATIC
        ${DALI_ANDROID}/dali-demo/examples/rendering-skybox/look-camera.cpp)

add_library(rendering-basic-pbr-example STATIC
        ${DALI_ANDROID}/dali-demo/examples/rendering-basic-pbr/ktx-loader.cpp
        ${DALI_ANDROID}/dali-demo/examples/rendering-basic-pbr/model-pbr.cpp
        ${DALI_ANDROID}/dali-demo/examples/rendering-basic-pbr/model-skybox.cpp
        ${DALI_ANDROID}/dali-demo/examples/rendering-basic-pbr/obj-loader.cpp)

add_library(fpp-game-example STATIC
        ${DALI_ANDROID}/dali-demo/examples/fpp-game/fpp-game-tutorial-controller.cpp
        ${DALI_ANDROID}/dali-demo/examples/fpp-game/game-camera.cpp
        ${DALI_ANDROID}/dali-demo/examples/fpp-game/game-entity.cpp
        ${DALI_ANDROID}/dali-demo/examples/fpp-game/game-model.cpp
        ${DALI_ANDROID}/dali-demo/examples/fpp-game/game-renderer.cpp
        ${DALI_ANDROID}/dali-demo/examples/fpp-game/game-scene.cpp
        ${DALI_ANDROID}/dali-demo/examples/fpp-game/game-texture.cpp
        ${DALI_ANDROID}/dali-demo/examples/fpp-game/game-utils.cpp)

add_library(dali
        STATIC
        IMPORTED )

set_target_properties(dali
        PROPERTIES IMPORTED_LOCATION
        ${DALI_ANDROID}/dali-demo/android/native-demo/dali/obj/local/${ANDROID_ABI}/libdali.a )

#add_library(dali-core
#        STATIC
#        IMPORTED )

#set_target_properties(dali-core
#        PROPERTIES IMPORTED_LOCATION
#        ${DALI_ANDROID}/dali-core/build/android/obj/local/${ANDROID_ABI}/libdali-core.a )

#add_library(dali-adaptor
#        STATIC
#        IMPORTED )

#set_target_properties(dali-adaptor
#        PROPERTIES IMPORTED_LOCATION
#        ${DALI_ANDROID}/dali-adaptor/build/android/obj/local/${ANDROID_ABI}/libdali-adaptor.a )

#add_library(dali-toolkit
#        STATIC
#        IMPORTED )

#set_target_properties(dali-toolkit
#        PROPERTIES IMPORTED_LOCATION
#        ${DALI_ANDROID}/dali-toolkit/build/android/obj/local/${ANDROID_ABI}/libdali-toolkit.a )

add_library(exif
        STATIC
        IMPORTED )

set_target_properties(exif
        PROPERTIES IMPORTED_LOCATION
        ${DALI_ANDROID}/android-dependencies/libexif/obj/local/${ANDROID_ABI}/libexif.a )

add_library(freetype
        STATIC
        IMPORTED )

set_target_properties(freetype
        PROPERTIES IMPORTED_LOCATION
        ${DALI_ANDROID}/android-dependencies/libfreetype/obj/local/${ANDROID_ABI}/libfreetype.a )

add_library(fontconfig
        STATIC
        IMPORTED )

set_target_properties(fontconfig
        PROPERTIES IMPORTED_LOCATION
        ${DALI_ANDROID}/android-dependencies/libfontconfig/obj/local/${ANDROID_ABI}/libfontconfig.a )

add_library(harfbuzz
        STATIC
        IMPORTED )

set_target_properties(harfbuzz
        PROPERTIES IMPORTED_LOCATION
        ${DALI_ANDROID}/android-dependencies/libharfbuzz/obj/local/${ANDROID_ABI}/libharfbuzz.a )

add_library(gif
        STATIC
        IMPORTED )

set_target_properties(gif
        PROPERTIES IMPORTED_LOCATION
        ${DALI_ANDROID}/android-dependencies/libgif/obj/local/${ANDROID_ABI}/libgif.a )

add_library(jpeg-turbo
        STATIC
        IMPORTED )

set_target_properties(jpeg-turbo
        PROPERTIES IMPORTED_LOCATION
        ${DALI_ANDROID}/android-dependencies/libjpeg-turbo/obj/local/${ANDROID_ABI}/libjpeg-turbo.a )

add_library(png
        STATIC
        IMPORTED )

set_target_properties(png
        PROPERTIES IMPORTED_LOCATION
        ${DALI_ANDROID}/android-dependencies/libpng/obj/local/${ANDROID_ABI}/libpng.a )

# add lib dependencies
target_link_libraries(native-activity
        contact-cards-example
        fpp-game-example
        rendering-basic-pbr-example
        rendering-skybox-example
        dali
        android
        native_app_glue
        EGL
        GLESv3
        z
        exif
        gif
        jpeg-turbo
        png
        log
        fontconfig
        freetype
        harfbuzz)
