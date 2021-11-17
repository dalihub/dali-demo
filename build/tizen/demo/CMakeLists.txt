SET(DEMO_SRC_DIR ${ROOT_SRC_DIR}/demo)

AUX_SOURCE_DIRECTORY(${DEMO_SRC_DIR} DEMO_SRCS)

SET(DEMO_SRCS
  ${DEMO_SRCS}
  "${ROOT_SRC_DIR}/shared/resources-location.cpp"
  "${ROOT_SRC_DIR}/shared/dali-table-view.cpp"
  "${ROOT_SRC_DIR}/shared/bubble-animator.cpp"
)

IF(WIN32)
  SET(DEMO_SRCS
    ${DEMO_SRCS}
    "${ROOT_SRC_DIR}/shared/execute-process-win.cpp"
  )
ELSEIF(ANDROID)
  SET(DEMO_SRCS
    ${DEMO_SRCS}
    "${ROOT_SRC_DIR}/shared/execute-process-android.cpp"
  )
ELSEIF(TIZEN)
  SET(DEMO_SRCS
    ${DEMO_SRCS}
    "${ROOT_SRC_DIR}/shared/execute-process-tizen.cpp"
  )
ELSE()
  SET(DEMO_SRCS
    ${DEMO_SRCS}
    "${ROOT_SRC_DIR}/shared/execute-process-unix.cpp"
  )
ENDIF()

IF(SHARED)
  ADD_LIBRARY(${PROJECT_NAME} SHARED ${DEMO_SRCS})
ELSE()
  ADD_EXECUTABLE(${PROJECT_NAME} ${DEMO_SRCS})
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${REQUIRED_LIBS})

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${BINDIR})

