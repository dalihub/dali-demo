SET(TESTS_REEL_SRC_DIR ${ROOT_SRC_DIR}/tests-reel)

AUX_SOURCE_DIRECTORY(${TESTS_REEL_SRC_DIR} TESTS_REEL_SRCS)

SET(TESTS_REEL_SRCS
  ${TESTS_REEL_SRCS}
  "${ROOT_SRC_DIR}/shared/resources-location.cpp"
  "${ROOT_SRC_DIR}/shared/dali-table-view.cpp"
)

ADD_EXECUTABLE(dali-tests ${TESTS_REEL_SRCS})
TARGET_LINK_LIBRARIES(dali-tests ${REQUIRED_PKGS_LDFLAGS} -pie)

INSTALL(TARGETS dali-tests DESTINATION ${BINDIR})
