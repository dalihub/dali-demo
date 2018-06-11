cmake_minimum_required(VERSION 3.11)

if (WIN32)

PROJECT (dali)

add_subdirectory(dali-core)
add_subdirectory(dali-adaptor)
add_subdirectory(dali-toolkit)

SET( SOURCES ${SOURCES}
dali-demo/main/main-example.cpp
)

ADD_DEFINITIONS(
#-D _CRT_SECURE_NO_WARNINGS
-DAPP_DATA_RES_DIR="${dali_SOURCE_DIR}/dali-env/opt/share/com.samsung.dali-demo/res"
-DIMAGES_DIR=APP_DATA_RES_DIR"/images/"
-DSTYLE_DIR=APP_DATA_RES_DIR"/style/"
-DDEMO_THEME_PATH=STYLE_DIR"demo-theme.json"
-DDEMO_IMAGE_DIR=IMAGES_DIR
-DdataReadOnlyDir="${dali_SOURCE_DIR}/dali-env/opt/share/dali/"
-Ddaliimagedir=dataReadOnlyDir"toolkit/images/"
-DDALI_IMAGE_DIR=daliimagedir
-DDEMO_STYLE_DIR=APP_DATA_RES_DIR"/style/"
-DAPPLICATION_RESOURCE_PATH="${dali_SOURCE_DIR}/dali-env/opt/share/com.samsung.dali-demo/res"
-DDEMO_VIDEO_DIR="${dali_SOURCE_DIR}/dali-env/opt/share/com.samsung.dali-demo/res/videos/"
-DDEMO_MODEL_DIR="${dali_SOURCE_DIR}/dali-env/opt/share/com.samsung.dali-demo/res/models/"
-DDEMO_SHADER_DIR="${dali_SOURCE_DIR}/dali-env/opt/share/com.samsung.dali-demo/res/shaders/"
-DDEMO_SCRIPT_DIR="${dali_SOURCE_DIR}/dali-env/opt/share/com.samsung.dali-demo/res/scripts/"
-DDEMO_GAME_DIR="${dali_SOURCE_DIR}/dali-env/opt/share/com.samsung.dali-demo/res/game"
/vmg
/FI"${dali_SOURCE_DIR}/dali-adaptor/dali-windows-backend/ExInclude/PreprocessorDefinitions.h"
)
#head file path  
INCLUDE_DIRECTORIES(  
${dali_SOURCE_DIR}/dali-demo
${dali_SOURCE_DIR}/dali-env/opt/include
${dali_SOURCE_DIR}/dali-demo/examples/text-label-emojis
${dali_SOURCE_DIR}/dali-core
${dali_SOURCE_DIR}/dali-adaptor
${dali_SOURCE_DIR}/dali-toolkit
${dali_SOURCE_DIR}/dali-adaptor/dali-windows-backend/ExInclude
)

link_directories(
bin/Debug
dali-adaptor/dali-windows-backend/ExLib
dali-adaptor/dali-windows-backend/ExLib/Debug
)

set(EXECUTABLE_OUTPUT_PATH bin)

add_executable(dali-demo ${SOURCES})

target_link_libraries(dali-demo dali-core.lib)
target_link_libraries(dali-demo dali-adaptor.lib)
target_link_libraries(dali-demo dali-toolkit.lib)
target_link_libraries(dali-demo Win32File.lib)
target_link_libraries(dali-demo WindowsPlatform.lib)
target_link_libraries(dali-demo libjpeg.lib)
target_link_libraries(dali-demo libcurl_a.lib)
target_link_libraries(dali-demo turbojpeg.lib)
target_link_libraries(dali-demo getopt.lib)
target_link_libraries(dali-demo giflib.lib)
target_link_libraries(dali-demo libEGL.lib)
target_link_libraries(dali-demo libexif.lib)
target_link_libraries(dali-demo libGLESv2.lib)
target_link_libraries(dali-demo pthread.lib)
target_link_libraries(dali-demo libpng.lib)
target_link_libraries(dali-demo zlibstat.lib)
target_link_libraries(dali-demo legacy_stdio_definitions.lib)
target_link_libraries(dali-demo freetype.lib)
target_link_libraries(dali-demo FontConfig.lib)
target_link_libraries(dali-demo libexpat.lib)
target_link_libraries(dali-demo fribidi.lib)
target_link_libraries(dali-demo harfbuzz-1.4.7.lib)
target_link_libraries(dali-demo dlfcn.lib)
target_link_libraries(dali-demo Iphlpapi.lib)
target_link_libraries(dali-demo Psapi.lib)
target_link_libraries(dali-demo Userenv.lib)

add_dependencies(dali-demo dali-core)
add_dependencies(dali-demo dali-adaptor)
add_dependencies(dali-demo dali-toolkit)

endif()
