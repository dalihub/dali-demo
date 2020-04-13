[ -z $ROOT_DIR ] && ROOT_DIR=$HOME
echo "Using ROOT_DIR=\"$ROOT_DIR\""

[ -z $DALI_DIR ] && DALI_DIR=$(cd ../../../;pwd)
echo "Using DALI_DIR=\"$DALI_DIR\""
export DALI_DIR=$DALI_DIR

[ -z $DALI_ENV_DIR ] && DALI_ENV_DIR=$DALI_DIR/dali-env/android
echo "Using DALI_ENV_DIR=\"$DALI_ENV_DIR\""
export DALI_ENV_DIR=$DALI_ENV_DIR

[ -z $TARGET ] && export TARGET=armeabi-v7a
echo "Using TARGET=\"$TARGET\""

if [ ! -z $http_proxy ]
then
  proxyFull=${http_proxy/http:\/\/}
  proxyHost=$(echo $proxyFull | cut -d: -f 1)
  proxyPort=$(echo $proxyFull | cut -d: -f 2)
  SdbProxyOptions="--proxy=http --proxy_host=$proxyHost --proxy_port=$proxyPort"
  echo "Proxy detected Host:$proxyHost Port:$proxyPort"
fi

if [ ! -d "$ANDROID_SDK" ]; then
  if [ ! -d "$ROOT_DIR/Android/Sdk" ]; then
    mkdir -p "$ROOT_DIR/Android/Sdk"
    cd "$ROOT_DIR/Android/Sdk"
    wget https://dl.google.com/android/repository/sdk-tools-linux-4333796.zip
    unzip sdk-tools-linux-4333796.zip
    tools/bin/sdkmanager --update $SdbProxyOptions
    yes | tools/bin/sdkmanager $SdbProxyOptions "patcher;v4" "platform-tools" "platforms;android-29" "build-tools;29.0.2" "lldb;3.1" "cmake;3.10.2.4988404" "ndk-bundle" "ndk;20.1.5948944"
    cd -
  fi
fi

if [ ! -d "$ANDROID_SDK" ]; then
# try default path
  if [ -d "$ROOT_DIR/Android/Sdk" ]; then
    export ANDROID_SDK=$ROOT_DIR/Android/Sdk
  fi
fi

if [ ! -d "$ANDROID_NDK" ]; then
  if [ -d "$ANDROID_SDK" ]; then
    NDK_DIR=$(find $ANDROID_SDK -maxdepth 2 -name ndk-build | sed 's/\/ndk-build//')
    # Some sdk folder structures have extra <version> dir for ndk folders.
    if [ ! -d "$NDK_DIR" ]; then
      NDK_DIR=$(find $ANDROID_SDK -maxdepth 3 -name ndk-build | sed 's/\/ndk-build//')
    fi
    if [ -d "$NDK_DIR" ]; then
      export ANDROID_NDK=$NDK_DIR
    fi
  fi
fi

if [ ! -d "$ROOT_DIR/gradle/gradle-5.4.1" ]; then
  mkdir -p $ROOT_DIR/gradle
  cd $ROOT_DIR/gradle
  wget https://services.gradle.org/distributions/gradle-5.4.1-bin.zip
  unzip gradle-5.4.1-bin.zip
  cd -
fi

GRADLE_PROPERTIES_FILE=gradle.properties
if [ ! -f $GRADLE_PROPERTIES_FILE ]
then
  echo "org.gradle.jvmargs=-Xmx1536m" > $GRADLE_PROPERTIES_FILE
  if [ ! -z $http_proxy ]
  then
    echo "systemProp.http.proxyHost=$proxyHost" >> $GRADLE_PROPERTIES_FILE
    echo "systemProp.http.proxyPort=$proxyPort" >> $GRADLE_PROPERTIES_FILE

    if [ ! -z $https_proxy ]
    then
      httpsProxyFull=${https_proxy/https:\/\/}
      httpsProxyHost=$(echo $httpsProxyFull | cut -d: -f 1)
      httpsProxyPort=$(echo $httpsProxyFull | cut -d: -f 2)
      echo "systemProp.https.proxyHost=$httpsProxyHost" >> $GRADLE_PROPERTIES_FILE
      echo "systemProp.https.proxyPort=$httpsProxyPort" >> $GRADLE_PROPERTIES_FILE
    fi
  fi
fi

export PATH=$PATH:$ROOT_DIR/gradle/gradle-5.4.1/bin
[ ! -f local.properties ] && echo 'sdk.dir='$(echo $ANDROID_SDK) > local.properties

gradle wrapper
if [ "$1" = "clean" ]; then
  ./gradlew clean
else
  if [ -z "$DEBUG" ]; then
    ./gradlew assembleRelease
  else
    ./gradlew assembleDebug
  fi
fi

