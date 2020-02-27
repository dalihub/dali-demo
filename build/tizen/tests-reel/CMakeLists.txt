SET(TESTS_REEL_SRC_DIR ${ROOT_SRC_DIR}/tests-reel)

AUX_SOURCE_DIRECTORY(${TESTS_REEL_SRC_DIR} TESTS_REEL_SRCS)

SET(TESTS_REEL_SRCS
  ${TESTS_REEL_SRCS}
  "${ROOT_SRC_DIR}/shared/resources-location.cpp"
  "${ROOT_SRC_DIR}/shared/dali-table-view.cpp"
)

IF(WIN32)
  SET(TESTS_REEL_SRCS
    ${TESTS_REEL_SRCS}
    "${ROOT_SRC_DIR}/shared/execute-process-win.cpp"
  )
ELSEIF(ANDROID)
  SET(TESTS_REEL_SRCS
    ${TESTS_REEL_SRCS}
    "${ROOT_SRC_DIR}/shared/execute-process-android.cpp"
  )
ELSE()
  SET(TESTS_REEL_SRCS
    ${TESTS_REEL_SRCS}
    "${ROOT_SRC_DIR}/shared/execute-process-unix.cpp"
  )
ENDIF()

IF(SHARED)
  ADD_LIBRARY(dali-tests SHARED ${TESTS_REEL_SRCS})
ELSE()
  ADD_EXECUTABLE(dali-tests ${TESTS_REEL_SRCS})
ENDIF()

TARGET_LINK_LIBRARIES(dali-tests ${REQUIRED_LIBS})

INSTALL(TARGETS dali-tests DESTINATION ${BINDIR})

