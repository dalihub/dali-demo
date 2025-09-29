# TBM library dependencies for native-image-test
# This file is only included when TBM library is available
if(ENABLE_TBM)
  message(STATUS "Including dependencies for ${EXAMPLE}")
  message(STATUS " Link options: -ltbm")
  set(EXTRA_EXAMPLE_LDFLAGS ${EXTRA_EXAMPLE_LDFLAGS} -ltbm)
endif()
