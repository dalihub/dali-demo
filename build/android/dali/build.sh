#!/bin/bash
if [ -z "$ANDROID_SDK" ]; then
  . ./env.sh
fi

if [ ! -d "$ANDROID_SDK" ]; then
  echo "Please install Android SDK into "$ANDROID_SDK
  exit 1
fi

if [ ! -d "$ANDROID_NDK" ]; then
  echo "Please install Android NDK into "$ANDROID_NDK
  exit 1
fi

if [ "$1" = "clean" ]; then
  echo "Cleaning dali..."
  rm -rf ./dali-core
  rm -rf ./dali-adaptor
  rm -rf ./dali-toolkit
  rm -rf ./dali-demo
  exit 0
fi

if [ ! -z "$DEBUG" ]; then
  export ENABLE_TRACE=ON
fi

export ANDROID_CMAKE_COMMAND=${ANDROID_SDK}/cmake/3.22.1/bin/cmake
export ANDROID_PLATFORM=26
export ANDROID_ABI=${TARGET}

./build_core.sh || exit 1
./build_adaptor.sh || exit 1
./build_toolkit.sh || exit 1
./build_demo.sh || exit 1
