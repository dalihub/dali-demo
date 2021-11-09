SET(EXAMPLES_REEL_SRC_DIR ${ROOT_SRC_DIR}/examples-reel)

AUX_SOURCE_DIRECTORY(${EXAMPLES_REEL_SRC_DIR} EXAMPLES_REEL_SRCS)

SET(EXAMPLES_REEL_SRCS
  ${EXAMPLES_REEL_SRCS}
  "${ROOT_SRC_DIR}/shared/resources-location.cpp"
  "${ROOT_SRC_DIR}/shared/dali-table-view.cpp"
  "${ROOT_SRC_DIR}/shared/bubble-animator.cpp"
)

IF(WIN32)
  SET(EXAMPLES_REEL_SRCS
    ${EXAMPLES_REEL_SRCS}
    "${ROOT_SRC_DIR}/shared/execute-process-win.cpp"
  )
ELSEIF(ANDROID)
  SET(EXAMPLES_REEL_SRCS
    ${EXAMPLES_REEL_SRCS}
    "${ROOT_SRC_DIR}/shared/execute-process-android.cpp"
  )
ELSEIF(TIZEN)
  SET(EXAMPLES_REEL_SRCS
    ${EXAMPLES_REEL_SRCS}
    "${ROOT_SRC_DIR}/shared/execute-process-tizen.cpp"
  )
ELSE()
  SET(EXAMPLES_REEL_SRCS
    ${EXAMPLES_REEL_SRCS}
    "${ROOT_SRC_DIR}/shared/execute-process-unix.cpp"
  )
ENDIF()

IF(SHARED)
  ADD_LIBRARY(dali-examples SHARED ${EXAMPLES_REEL_SRCS})
ELSE()
  ADD_EXECUTABLE(dali-examples ${EXAMPLES_REEL_SRCS})
ENDIF()

TARGET_LINK_LIBRARIES(dali-examples ${REQUIRED_LIBS})

INSTALL(TARGETS dali-examples DESTINATION ${BINDIR})

