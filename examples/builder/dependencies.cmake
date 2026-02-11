# Add link dependency to dali-demo-controls library with --whole-archive to include all object files
# This ensures all controls are available for dynamic creation from JSON
# Only use --whole-archive on Linux/Unix (not macOS/Windows)
IF(UNIX AND NOT APPLE)
  TARGET_LINK_LIBRARIES(${EXAMPLE}.example -Wl,--whole-archive dali-demo-controls -Wl,--no-whole-archive)
ELSE()
  # On macOS/Windows, link normally
  TARGET_LINK_LIBRARIES(${EXAMPLE}.example dali-demo-controls)
ENDIF()
MESSAGE(STATUS "Included dali-demo-controls dependency for ${EXAMPLE}")
