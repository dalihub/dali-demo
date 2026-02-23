SET(BUILDER_SRC_DIR ${ROOT_SRC_DIR}/builder)

SET(DALI_BUILDER_SRCS ${BUILDER_SRC_DIR}/dali-builder.cpp)
SET(DALI_BUILDER_SRCS ${DALI_BUILDER_SRCS} "${ROOT_SRC_DIR}/shared/resources-location.cpp")
IF(SHARED)
  ADD_LIBRARY(dali-builder SHARED ${DALI_BUILDER_SRCS})
ELSE()
  ADD_EXECUTABLE(dali-builder ${DALI_BUILDER_SRCS})
ENDIF()

# Link dali-demo-controls with --whole-archive to include all object files
# This ensures all controls are available for dynamic creation from JSON
# Only use --whole-archive on Linux/Unix (not macOS/Windows)
IF(UNIX AND NOT APPLE)
  target_link_libraries(dali-builder
    -Wl,--whole-archive
    dali-demo-controls
    -Wl,--no-whole-archive
    ${REQUIRED_LIBS}
  )
ELSE()
  # On macOS/Windows, link normally
  target_link_libraries(dali-builder
    dali-demo-controls
    ${REQUIRED_LIBS}
  )
ENDIF()

INSTALL(TARGETS dali-builder DESTINATION ${BINDIR})
