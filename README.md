<img src="https://dalihub.github.io/images/DaliLogo320x200.png">

# Table of Contents

   * [Build Instructions](#build-instructions)
      * [1. Building for Ubuntu desktop](#1-building-for-ubuntu-desktop)
         * [Minimum Requirements](#minimum-requirements)
         * [Building the Repository](#building-the-repository)
         * [DEBUG Builds](#debug-builds)
      * [2. GBS Builds](#2-gbs-builds)
         * [NON-SMACK Targets](#non-smack-targets)
         * [SMACK enabled Targets](#smack-enabled-targets)
         * [DEBUG Builds](#debug-builds-1)
      * [3. Building for Android](#3-building-for-android)
         * [Requirements](#requirements)
         * [Building the Repository](#building-the-repository)
         * [Launching a demo via the ADB command](#launching-a-demo-via-the-adb-command)
      * [4. Building for MS Windows](#4-building-for-ms-windows)
         * [Build with the Visual Studio project](#build-with-the-visual-studio-project)
         * [Build with CMake](#build-with-cmake)
      * [5. Building for MacOS](#5-building-for-macos)
   * [Creating an Example](#creating-an-example)

# Build Instructions

## 1. Building for Ubuntu desktop

### Requirements

 - Ubuntu 16.04 or later
 - Environment created using dali_env script in dali-core repository
 - GCC version 9
 - Cmake version 3.8.2 or later

DALi requires a compiler supporting C++17 features.

### Building the Repository

To build the repository enter the 'build/tizen' folder:

         $ cd dali-demo/build/tizen

Then run the following commands:

         $ cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX .
         $ make install -j8

### DEBUG Builds

Specify a debug build when building for desktop by passing the following parameter to cmake:

         $ cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX -DCMAKE_BUILD_TYPE=Debug .

Before running make install as normal:

         $ make install -j8

### Build Only specific sample

You can set the sample's name and can build that sample only. For example, If you want to build 'hello-world',

         $ cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX -DBUILD_EXAMPLE_NAME=hello-world
         $ make install -j9

## 2. GBS Builds

### NON-SMACK Targets

         $ gbs build -A [TARGET_ARCH]

### SMACK enabled Targets

         $ gbs build -A [TARGET_ARCH] --define "%enable_dali_smack_rules 1"

### DEBUG Builds

         $ gbs build -A [TARGET_ARCH] --define "%enable_debug 1"

### Raspberry Pi builds

         $ gbs build -A [TARGET_ARCH] --define "%rpi_style 1"

### Build specific sample only

         $ gbs build -A [TARGET_ARCH] --define "build_example_name hello-world"

## 3. Building for Android

### Requirements

 - Ubuntu 20.04 or later
 - Open JDK Version 17 or above
 - Android DALi dependencies
 - If you are behind a proxy, please set up the "http_proxy" and "https_proxy" environment variables appropriately
 - Clone dali-core, dali-adaptor, dali-toolkit & android-dependencies to the same folder as dali-demo
   - Android dependencies can be cloned from: https://github.com/dalihub/android-dependencies

### Building the Repository
To build the repository enter the 'build/android' folder:

         $ cd dali-demo/build/android

Then run the following command:

         $ ./build.sh

For debug build:

         $ DEBUG=1 ./build.sh

To debug from Android Studio set Android DALi enviroment using setenv script in 'build/android' and launch Android Studio from the enviroment:

         $ . setenv
         $ <path/to/androidstudio>/bin/studio.sh

To clean the build:

         $ ./build.sh clean

To install apks:

         $ adb install -r ./app/build/outputs/apk/debug/dali-demo-debug.apk
         $ adb install -r ./app/build/outputs/apk/release/dali-demo-release.apk

Manually granting permission for accessing Storage before running the demo
         1. Long press the DALi demo app icon => App Info
         2. Permissions => Storage => Enable

### Launching a demo via the ADB command
A demo can be launched directly via the ADB command. For example, to launch the Benchmark demo:

         $ adb shell am start -n com.sec.dalidemo/.DaliDemoNativeActivity --es start "benchmark.example"

## 4. Building for MS Windows

Third party dependencies are built using vcpkg. Instructions on how to install vcpkg can be found in the
vcpkg-script folder in the windows-dependencies repository.

- Download the windows-dependencies repository from DaliHub

         $ git clone https://github.com/dalihub/windows-dependencies.git

- Read the windows-dependencies/vcpkg-script/Readme.md file for more instructions on how to build and install the third-party dependencies.

### Build with the Visual Studio project
  Read the windows-dependencies/README.md file for more instructions on how to build and run DALi for MS Windows.

### Build with CMake

  * Requirements
    It's required the version 3.12.2 of CMake and a Git Bash Shell.

  * Notes and troubleshoting:
    It should be possible to use the MS Visual studio Developer Command Prompt (https://docs.microsoft.com/en-us/dotnet/framework/tools/developer-command-prompt-for-vs) to build DALi from the command line.
    However, the CMake version installed with MS Visual Studio 2017 is a bit out of date and some VCPKG modules require a higher version.
    This instructions have been tested with CMake 3.12.2 on a Git Bash shell.

  * Define an environment variable to set the path to the VCPKG folder

    $ export VCPKG_FOLDER=C:/Users/username/Workspace/VCPKG_TOOL

  * Define an environment variable to set the path where DALi is going to be installed.

    $ export DALI_ENV_FOLDER=C:/Users/username/Workspace/dali-env

  * Execute the following commands to create the makefiles, build and install DALi.

    $ cmake -g Ninja . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_FOLDER/vcpkg/scripts/buildsystems/vcpkg.cmake -DENABLE_PKG_CONFIGURE=OFF -DINTERNATIONALIZATION=OFF -DCMAKE_INSTALL_PREFIX=$DALI_ENV_FOLDER
    $ cmake --build . --target install

  * Options:
    - CMAKE_TOOLCHAIN_FILE  ---> Needed to find packages installed by VCPKG.
    - ENABLE_PKG_CONFIGURE  ---> Whether to install pkg configure files (not currently working on MS Windows. CMake modules used instead).
    - INTERNATIONALIZATION  ---> Whether to generate and install internazionalization '.po' Portable Object files.
    - CMAKE_INSTALL_PREFIX  ---> Were DALi is installed.
    - ENABLE_DEBUG          ---> Whether to build with debug enabled.
    - BUILD_EXAMPLE_NAME    ---> The name of sample what you want to build. To reduce the memory of rpm file, we didn't copy resources now.

  * If dali-toolkit has been compiled with the USE_DEFAULT_RESOURCE_DIR option disabled then is needed to set global environment variables in order to find the default toolkit resources.

    i.e
    $ setx DALI_ENV_FOLDER C:/Users/username/dali-env
    $ setx DALI_IMAGE_DIR $DALI_ENV_FOLDER/share/dali/toolkit/images/
    $ setx DALI_SOUND_DIR $DALI_ENV_FOLDER/share/dali/toolkit/sounds/
    $ setx DALI_STYLE_DIR $DALI_ENV_FOLDER/share/dali/toolkit/styles/
    $ setx DALI_STYLE_IMAGE_DIR $DALI_ENV_FOLDER/share/dali/toolkit/styles/images/
    $ setx DALI_DATA_READ_ONLY_DIR $DALI_ENV_FOLDER/share/dali/

## 5. Building for MacOS

It is assumed that the DALi environment has been set up & that DALi Core, Adaptor & Toolkit have been built accordingly.

To build the repository enter the 'build/tizen' folder:
```zsh
% cd dali-demo/build/tizen
```
Then run the following command to set up the build:
```zsh
% cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX -DCMAKE_TOOLCHAIN_FILE=$VCPKG_FOLDER/scripts/buildsystems/vcpkg.cmake -DINTERNATIONALIZATION=OFF
```
If a Debug build is required, then add `-DCMAKE_BUILD_TYPE=Debug`

To build, run:
```zsh
% make install -j8
```

# Creating an Example

- Make a directory in the `examples` directory. Only one example will be created per directory.
- The executable installed will have a `.example` appended to it, e.g. a `blocks` directory produces `blocks.example`.
- Add all source files for the required example in this directory.
- Modify `com.samsung.dali-demo.xml` to include your example so that it can be launched on target.
- No changes are required to the make system as long as the above is followed, your example will be automatically built & installed.
- Add a key handler so that the `ESC` or `Back` keys can be used to quit the application.
- Depending on the nature of what you are demonstrating, add a launcher line to either:
  - `demo/dali-demo.cpp`: for graphical showcase demos.
  - `examples-reel/dali-examples-reel.cpp`: generic examples of how to use various features.
  - `tests-reel/dali-tests-reel.cpp`: for testing purposes.
- Add 2 lines to `shared/dali-demo-strings.h` for the title of your application, please keep in alphabetic order,
- Add as many translations of the title as possible to the files in `resources/po`. Currently, this only works on Ubuntu.
- In each example folder, create a README.md with images and a description of what the example is demonstrating.
  Look at [this](examples/particle-system/README.md) for reference.
