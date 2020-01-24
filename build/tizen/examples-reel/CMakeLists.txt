SET(EXAMPLES_REEL_SRC_DIR ${ROOT_SRC_DIR}/examples-reel)

AUX_SOURCE_DIRECTORY(${EXAMPLES_REEL_SRC_DIR} EXAMPLES_REEL_SRCS)

SET(EXAMPLES_REEL_SRCS
  ${EXAMPLES_REEL_SRCS}
  "${ROOT_SRC_DIR}/shared/resources-location.cpp"
  "${ROOT_SRC_DIR}/shared/dali-table-view.cpp"
  "${ROOT_SRC_DIR}/shared/execute-process-unix.cpp"
)

ADD_EXECUTABLE(dali-examples ${EXAMPLES_REEL_SRCS})
TARGET_LINK_LIBRARIES(dali-examples ${REQUIRED_PKGS_LDFLAGS} -pie)

INSTALL(TARGETS dali-examples DESTINATION ${BINDIR})

