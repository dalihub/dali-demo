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
   * [Creating an example](#creating-an-example)

# Build Instructions

## 1. Building for Ubuntu desktop

### Requirements

 - Ubuntu 16.04 or later
 - Environment created using dali_env script in dali-core repository
 - GCC version 6

DALi requires a compiler supporting C++11 features.
Ubuntu 16.04 is the first version to offer this by default (GCC v5.4.0).

GCC version 6 is recommended since it has fixes for issues in version 5
e.g. it avoids spurious 'defined but not used' warnings in header files.

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

## 2. GBS Builds

### NON-SMACK Targets

         $ gbs build -A [TARGET_ARCH]

### SMACK enabled Targets

         $ gbs build -A [TARGET_ARCH] --define "%enable_dali_smack_rules 1"

### DEBUG Builds

         $ gbs build -A [TARGET_ARCH] --define "%enable_debug 1"

# Creating an example

 - Make a directory in the "examples" directory. Only one example will be created per directory.
 - The executable installed will have a ".example" appended to it, e.g. a "blocks" directory produces "blocks.example".
 - Add all source files for the required example in this directory.
 - Modify "com.samsung.dali-demo.xml" to include your example so that it can be launched on target.
 - No changes are required to the make system as long as the above is followed, your example will be automatically built & installed.

## 3. Building for Android

### Requirements

 - Ubuntu 16.04 or later
 - GCC version 6
 - Android DALi dependencies

### Building the Repository
To download Android DALi dependencies clone https://github.com/dalihub/android-dependencies
in your DALi folder (folder containing dali-demo).

         $ git clone https://github.com/dalihub/android-dependencies.git

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


