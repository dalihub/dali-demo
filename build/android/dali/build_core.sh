
if [ -d ./dali-core/cmake/${ANDROID_ABI} ]; then
  cd ./dali-core/cmake/${ANDROID_ABI}
else
  mkdir -p ./dali-core/cmake/${ANDROID_ABI}
  cd ./dali-core/cmake/${ANDROID_ABI}

  if [ ! -z "$DEBUG" ]; then
    export CMAKE_BUILD_TYPE=Debug
  else
    export CMAKE_BUILD_TYPE=Release
  fi

  if [ -z "$ENABLE_TRACE" ]; then
    export ENABLE_TRACE=OFF
  fi

  export PREFIX=${DALI_ENV_DIR}
  export PKG_CONFIG_LIBDIR=${ANDROID_NDK}/prebuilt/linux-x86_64/lib/pkgconfig
  export PKG_CONFIG_PATH=${PREFIX}/lib/${ANDROID_ABI}/pkgconfig
  ${ANDROID_SDK}/cmake/3.10.2.4988404/bin/cmake ${DALI_DIR}/dali-core/build/tizen -DCMAKE_C_COMPILER=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang -DCMAKE_CXX_COMPILER=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake -DANDROID_PLATFORM=${ANDROID_PLATFORM} -DANDROID_ABI=${ANDROID_ABI} -DANDROID_NDK=${ANDROID_NDK} -DANDROID_STL=c++_shared -DCMAKE_CXX_FLAGS='-fexceptions -frtti -w -Wall -std=c++17' -DINCLUDE_DIR=${PREFIX}/include -DLIB_DIR=${PREFIX}/lib/${ANDROID_ABI} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DENABLE_TRACE=${ENABLE_TRACE}
fi

make -j8 || exit 1
make install || exit 1

cd -
