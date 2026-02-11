# Add link dependency to dali-demo-controls library for Slider
TARGET_LINK_LIBRARIES(${EXAMPLE}.example dali-demo-controls)
MESSAGE(STATUS "Included dali-demo-controls dependency for ${EXAMPLE}")
