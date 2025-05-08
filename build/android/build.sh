[ -z $ROOT_DIR ] && ROOT_DIR=$HOME
echo "Using ROOT_DIR=\"$ROOT_DIR\""

[ -z $DALI_DIR ] && DALI_DIR=$(cd ../../../;pwd)
echo "Using DALI_DIR=\"$DALI_DIR\""
export DALI_DIR=$DALI_DIR

[ -z $DALI_ENV_DIR ] && DALI_ENV_DIR=$DALI_DIR/dali-env/android
echo "Using DALI_ENV_DIR=\"$DALI_ENV_DIR\""
export DALI_ENV_DIR=$DALI_ENV_DIR

[ -z $TARGET ] && export TARGET=arm64-v8a
echo "Using TARGET=\"$TARGET\""

if [ ! -z $http_proxy ]
then
  proxyFull=${http_proxy/http:\/\/}
  proxyHost=$(echo $proxyFull | cut -d: -f 1)
  proxyPort=$(echo $proxyFull | cut -d: -f 2)
  SdkProxyOptions="--proxy=http --proxy_host=$proxyHost --proxy_port=$proxyPort"
  echo "Proxy detected Host:$proxyHost Port:$proxyPort"
fi

SDK_PACKAGES=$(echo "
  'platform-tools'
  'platforms;android-35'
  'build-tools;35.0.1'
  'cmake;3.22.1'
  'ndk;29.0.13113456'
" | xargs)
SDK_MANAGER="$ROOT_DIR/Android/Sdk/cmdline-tools/bin/sdkmanager --sdk_root=$ROOT_DIR/Android/Sdk $SdkProxyOptions"

if [ ! -d "$ANDROID_SDK" ]; then
  if [ ! -d "$ROOT_DIR/Android/Sdk" ]; then
    mkdir -p "$ROOT_DIR/Android/Sdk"
    cd "$ROOT_DIR/Android/Sdk"
    wget --quiet https://developer.android.com/studio/index.html
    if [ ! -f index.html ]; then echo ERROR: Unable to get Android Tools Version; exit 1; fi

    androidCommandLineToolsPkgUrl=$(cat index.html | grep "commandlinetools-linux-" | grep href | cut -d\" -f 2)
    androidCommandLineToolsPkgName=$(echo $androidCommandLineToolsPkgUrl | rev | cut -d\/ -f 1 | rev)
    echo "Android SDK: Downloading Android Command Line Tools from: $androidCommandLineToolsPkgUrl"
    wget --quiet $androidCommandLineToolsPkgUrl
    echo "Android SDK: Unzipping $androidCommandLineToolsPkgName"
    unzip -q $androidCommandLineToolsPkgName
    $SDK_MANAGER --update
    yes | $SDK_MANAGER $SDK_PACKAGES
    cd -
  fi
fi

# Install any missing packages that we require
SDK_PACKAGES_INSTALLED="$($SDK_MANAGER --list_installed)"
for package in $SDK_PACKAGES;
do
  if ! echo -e $SDK_PACKAGES_INSTALLED | grep $package &> /dev/null
  then
    echo "Android SDK: Installing $package"
    yes | $SDK_MANAGER "$package"
  fi
done

if [ ! -d "$ANDROID_SDK" ]; then
# try default path
  if [ -d "$ROOT_DIR/Android/Sdk" ]; then
    export ANDROID_SDK=$ROOT_DIR/Android/Sdk
  fi
fi

if [ ! -d "$ANDROID_NDK" ]; then
  if [ -d "$ANDROID_SDK" ]; then
    # Disregard ndk-bundle and use the latest ndk installed
    NDK_DIR=$(find $ANDROID_SDK -maxdepth 3 -name ndk-build | grep -v ndk-bundle | sort -u | tail -n 1 | sed 's/\/ndk-build//')
    if [ -d "$NDK_DIR" ]; then
      export ANDROID_NDK=$NDK_DIR
    fi
  fi
fi

echo "ANDROID_NDK=$ANDROID_NDK"
echo "ANDROID_SDK=$ANDROID_SDK"
echo "NDK_DIR=$NDK_DIR"

GRADLE_VERSION=8.11.1
if [ ! -d "$ROOT_DIR/gradle/gradle-$GRADLE_VERSION" ]; then
  mkdir -p $ROOT_DIR/gradle
  cd $ROOT_DIR/gradle
  gradleUrl=https://services.gradle.org/distributions/gradle-$GRADLE_VERSION-bin.zip
  gradlePkgName=gradle-$GRADLE_VERSION-bin.zip
  echo "Gradle: Downloading Gradle from: $gradleUrl"
  wget --quiet $gradleUrl
  echo "Unzipping $gradlePkgName"
  unzip -q $gradlePkgName
  cd -
fi

GRADLE_PROPERTIES_FILE=gradle.properties
if [ ! -f $GRADLE_PROPERTIES_FILE ]
then
  echo "Gradle: Creating $GRADLE_PROPERTIES_FILE"
  echo "org.gradle.jvmargs=-Xmx1536m" > $GRADLE_PROPERTIES_FILE
  echo "android.useAndroidX=true" >> $GRADLE_PROPERTIES_FILE
  echo "android.enableJetifier=true" >> $GRADLE_PROPERTIES_FILE
  if [ ! -z $http_proxy ]
  then
    echo "systemProp.http.proxyHost=$proxyHost" >> $GRADLE_PROPERTIES_FILE
    echo "systemProp.http.proxyPort=$proxyPort" >> $GRADLE_PROPERTIES_FILE

    if [ ! -z $https_proxy ]
    then
      httpsProxyFull=$https_proxy
      httpsProxyFull=${httpsProxyFull/https:\/\/}
      httpsProxyFull=${httpsProxyFull/http:\/\/}
      httpsProxyHost=$(echo $httpsProxyFull | cut -d: -f 1)
      httpsProxyPort=$(echo $httpsProxyFull | cut -d: -f 2)
      echo "systemProp.https.proxyHost=$httpsProxyHost" >> $GRADLE_PROPERTIES_FILE
      echo "systemProp.https.proxyPort=$httpsProxyPort" >> $GRADLE_PROPERTIES_FILE
    fi
  fi
fi

echo "Using \"$ROOT_DIR/gradle/gradle-$GRADLE_VERSION/bin/gradle\""
export PATH=$PATH:$ROOT_DIR/gradle/gradle-$GRADLE_VERSION/bin
[ ! -f local.properties ] && echo 'sdk.dir='$(echo $ANDROID_SDK) > local.properties

echo "Gradle: Creating Wrapper"
gradle wrapper || exit 1
if [ "$1" = "clean" ]; then
  ./gradlew clean
else
  if [ -z "$DEBUG" ];
  then
    echo "Gradle: Initiating Release Build"
    ./gradlew assembleRelease
  else
    echo "Gradle: Initiating Debug Build"
    ./gradlew assembleDebug
  fi
fi

