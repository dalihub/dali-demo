IF(ANDROID)
  SET(EXTRA_EXAMPLE_LDFLAGS -lGLESv3)
ELSEIF(NOT WIN32 AND NOT APPLE)
  PKG_CHECK_MODULES(GLESV2 REQUIRED glesv2)
  SET(EXTRA_EXAMPLE_LDFLAGS ${GLESV2_LIBRARIES})
ENDIF()