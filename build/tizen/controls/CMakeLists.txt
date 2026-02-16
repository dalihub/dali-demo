cmake_minimum_required(VERSION 3.8.2)

# Include directories
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${ROOT_SRC_DIR}
)

# Collect all control sources
file(GLOB_RECURSE CONTROLS_SOURCES
  ${ROOT_SRC_DIR}/controls/slider/*.cpp
  ${ROOT_SRC_DIR}/controls/progress-bar/*.cpp
)

# Create static library for all controls
add_library(dali-demo-controls STATIC ${CONTROLS_SOURCES})

# Enable function and data sections for linker GC (Linux/Unix only)
IF(UNIX AND NOT APPLE)
  target_compile_options(dali-demo-controls PRIVATE
    -ffunction-sections
    -fdata-sections
  )

  # Enable linker garbage collection to remove unused sections
  target_link_options(dali-demo-controls PUBLIC
    -Wl,--gc-sections
  )
ENDIF()

# Link libraries
target_link_libraries(dali-demo-controls
  ${REQUIRED_LIBS}
)

# Set include directories for examples
target_include_directories(dali-demo-controls PUBLIC
  ${ROOT_SRC_DIR}/controls
)
