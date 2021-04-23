
if [ -d ./dali-toolkit/cmake/${ANDROID_ABI} ]; then
  cd ./dali-toolkit/cmake/${ANDROID_ABI}
else
  mkdir -p ./dali-toolkit/cmake/${ANDROID_ABI}
  cd ./dali-toolkit/cmake/${ANDROID_ABI}

  if [ ! -z "$DEBUG" ]; then
    export CMAKE_BUILD_TYPE=Debug
  else
    export CMAKE_BUILD_TYPE=Release
  fi

  if [ -z "$ENABLE_TRACE" ]; then
    export ENABLE_TRACE=OFF
  fi

  if [ -z "$CXX" ]; then
    export CXX="g++"
  fi

  export PREFIX=${DALI_ENV_DIR}
  export PKG_CONFIG_LIBDIR=${ANDROID_NDK}/prebuilt/linux-x86_64/lib/pkgconfig
  export PKG_CONFIG_PATH=${PREFIX}/lib/${ANDROID_ABI}/pkgconfig
  export DALI_DATA_RO_DIR=assets
  export DALI_DATA_RW_DIR=/data/data/com.sec.dali_demo/files
  export DALI_DATA_RO_INSTALL_DIR=${PREFIX}/files
  export DALI_DATA_RW_INSTALL_DIR=${PREFIX}/files

  ${ANDROID_SDK}/cmake/3.10.2.4988404/bin/cmake ${DALI_DIR}/dali-toolkit/build/tizen -DCMAKE_C_COMPILER=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang -DCMAKE_CXX_COMPILER=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake -DANDROID_PLATFORM=${ANDROID_PLATFORM} -DANDROID_ABI=${ANDROID_ABI} -DANDROID_NDK=${ANDROID_NDK} -DANDROID_STL=c++_shared -DCMAKE_CXX_FLAGS='-fexceptions -frtti -w -Wall -std=c++17' -DINCLUDE_DIR=${PREFIX}/include -DLIB_DIR=${PREFIX}/lib/${ANDROID_ABI} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DENABLE_TRACE=${ENABLE_TRACE} -DWITH_STYLE='720x1280' -DANDROID_HOST_COMPILER="$CXX"
fi

make -j8 || exit 1
make install || exit 1
cp -R ${DALI_DIR}/dali-toolkit/automated-tests/resources/fonts/* ${DALI_DIR}/dali-env/android/files/fonts

cd -
