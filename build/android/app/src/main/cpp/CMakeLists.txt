CMAKE_MINIMUM_REQUIRED(VERSION 3.4.1)

SET(DALI_ENV_DIR $ENV{DALI_ENV_DIR})
SET(ENV{PKG_CONFIG_PATH} "${DALI_ENV_DIR}/lib/${ANDROID_ABI}/pkgconfig:$ENV{PKG_CONFIG_PATH}")
FIND_PACKAGE(PkgConfig REQUIRED)

SET(ANDROID_SDK $ENV{ANDROID_SDK})
SET(ANDROID_NDK $ENV{ANDROID_NDK})

# build native_app_glue as a static lib
ADD_LIBRARY(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# now build app's shared lib
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -frtti -w -Wall -std=gnu++11")

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
SET(CMAKE_SHARED_LINKER_FLAGS
  "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

ADD_LIBRARY(native-activity SHARED main.cpp)

TARGET_INCLUDE_DIRECTORIES(native-activity PRIVATE
  ${ANDROID_NDK}/sources/android/native_app_glue)

INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(${DALI_ENV_DIR}/include)
INCLUDE_DIRECTORIES(${DALI_ENV_DIR}/include/dali)

IF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  SET(ENABLE_TRACE ON)
ELSE()
  SET(ENABLE_TRACE OFF)
ENDIF()

IF(EXISTS "${DALI_ENV_DIR}")
  PKG_CHECK_MODULES(DALI_CORE dali-core REQUIRED NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH)
  PKG_CHECK_MODULES(DALI_ADAPTOR dali-adaptor REQUIRED NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH)
  PKG_CHECK_MODULES(DALI_TOOLKIT dali-toolkit REQUIRED NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH)
ELSE()
  MESSAGE(WARNING "${DALI_ENV_DIR} not found!")
ENDIF()

# add lib dependencies
TARGET_LINK_LIBRARIES(native-activity
        ${DALI_TOOLKIT_LDFLAGS}
        ${DALI_ADAPTOR_LDFLAGS}
        ${DALI_CORE_LDFLAGS}
        native_app_glue
        z
        android
        log
        EGL
        GLESv3)
