if [ ! -d "$ANDROID_SDK" ]; then
  # try default path
  if [ -d "$HOME/Android/Sdk" ]; then
    export ANDROID_SDK=$HOME/Android/Sdk
  fi
fi

if [ ! -d "$ANDROID_NDK" ]; then
  if [ -d $ANDROID_SDK ]; then
    NDK_DIR=$(find $ANDROID_SDK -maxdepth 2 -name ndk-build | sed 's/\/ndk-build//')
    if [ -d "$NDK_DIR" ]; then
      export ANDROID_NDK=$NDK_DIR
    fi
  fi
fi

if [ ! -d "$DALI_DIR" ]; then
export DALI_DIR=$(cd ../../../../; pwd)
fi

if [ ! -d "$DALI_ENV_DIR" ]; then
export DALI_ENV_DIR=$DALI_DIR/dali-env/android
fi
