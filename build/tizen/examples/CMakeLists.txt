SET(EXAMPLES_SRC_DIR ${ROOT_SRC_DIR}/examples)

SET(BLOCKS_SRCS ${EXAMPLES_SRC_DIR}/blocks/blocks-example.cpp)
ADD_EXECUTABLE(blocks.example ${BLOCKS_SRCS})
TARGET_LINK_LIBRARIES(blocks.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS blocks.example DESTINATION ${BINDIR})

SET(BUBBLE_EFFECT_SRCS ${EXAMPLES_SRC_DIR}/shader-effect/bubble-effect-example.cpp)
ADD_EXECUTABLE(bubble-effect.example ${BUBBLE_EFFECT_SRCS})
TARGET_LINK_LIBRARIES(bubble-effect.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS bubble-effect.example DESTINATION ${BINDIR})

SET(CLUSTER_SRCS ${EXAMPLES_SRC_DIR}/cluster/cluster-example.cpp)
ADD_EXECUTABLE(cluster.example ${CLUSTER_SRCS})
TARGET_LINK_LIBRARIES(cluster.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS cluster.example DESTINATION ${BINDIR})

SET(CUBE_TRANSITION_EFFECT_SRCS ${EXAMPLES_SRC_DIR}/transition/cube-transition-effect-example.cpp)
ADD_EXECUTABLE(cube-transition-effect.example ${CUBE_TRANSITION_EFFECT_SRCS})
TARGET_LINK_LIBRARIES(cube-transition-effect.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS cube-transition-effect.example DESTINATION ${BINDIR})

SET(DISSOLVE_EFFECT_SRCS ${EXAMPLES_SRC_DIR}/shader-effect/dissolve-effect-example.cpp)
ADD_EXECUTABLE(dissolve-effect.example ${DISSOLVE_EFFECT_SRCS})
TARGET_LINK_LIBRARIES(dissolve-effect.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS dissolve-effect.example DESTINATION ${BINDIR})

SET(HELLO_WORLD_SRCS ${EXAMPLES_SRC_DIR}/hello-world/hello-world-example.cpp)
ADD_EXECUTABLE(hello-world.example ${HELLO_WORLD_SRCS})
TARGET_LINK_LIBRARIES(hello-world.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS hello-world.example DESTINATION ${BINDIR})

SET(ITEM_VIEW_SRCS ${EXAMPLES_SRC_DIR}/item-view/item-view-example.cpp)
ADD_EXECUTABLE(item-view.example ${ITEM_VIEW_SRCS})
TARGET_LINK_LIBRARIES(item-view.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS item-view.example DESTINATION ${BINDIR})

SET(MAGNIFIER_SRCS ${EXAMPLES_SRC_DIR}/magnifier/magnifier-example.cpp)
ADD_EXECUTABLE(magnifier.example ${MAGNIFIER_SRCS})
TARGET_LINK_LIBRARIES(magnifier.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS magnifier.example DESTINATION ${BINDIR})

SET(MOTION_BLUR_SRCS ${EXAMPLES_SRC_DIR}/motion/motion-blur-example.cpp)
ADD_EXECUTABLE(motion-blur.example ${MOTION_BLUR_SRCS})
TARGET_LINK_LIBRARIES(motion-blur.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS motion-blur.example DESTINATION ${BINDIR})

SET(MOTION_STRETCH_SRCS ${EXAMPLES_SRC_DIR}/motion/motion-stretch-example.cpp)
ADD_EXECUTABLE(motion-stretch.example ${MOTION_STRETCH_SRCS})
TARGET_LINK_LIBRARIES(motion-stretch.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS motion-stretch.example DESTINATION ${BINDIR})

SET(NEW_WINDOW_SRCS ${EXAMPLES_SRC_DIR}/new-window/new-window-example.cpp)
ADD_EXECUTABLE(new-window.example ${NEW_WINDOW_SRCS})
TARGET_LINK_LIBRARIES(new-window.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS new-window.example DESTINATION ${BINDIR})

SET(PAGE_TURN_VIEW_SRCS ${EXAMPLES_SRC_DIR}/page-turn-view/page-turn-view-example.cpp)
ADD_EXECUTABLE(page-turn-view.example ${PAGE_TURN_VIEW_SRCS})
TARGET_LINK_LIBRARIES(page-turn-view.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS page-turn-view.example DESTINATION ${BINDIR})

AUX_SOURCE_DIRECTORY(${EXAMPLES_SRC_DIR}/radial-menu RADIAL_MENU_SRCS)
ADD_EXECUTABLE(radial-menu.example ${RADIAL_MENU_SRCS})
TARGET_LINK_LIBRARIES(radial-menu.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS radial-menu.example DESTINATION ${BINDIR})

SET(REFRACTION_EFFECT_SRCS ${EXAMPLES_SRC_DIR}/shader-effect/refraction-effect-example.cpp)
ADD_EXECUTABLE(refraction-effect.example ${REFRACTION_EFFECT_SRCS})
TARGET_LINK_LIBRARIES(refraction-effect.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS refraction-effect.example DESTINATION ${BINDIR})

SET(SCROLL_VIEW_SRCS ${EXAMPLES_SRC_DIR}/scroll-view/scroll-view-example.cpp)
ADD_EXECUTABLE(scroll-view.example ${SCROLL_VIEW_SRCS})
TARGET_LINK_LIBRARIES(scroll-view.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS scroll-view.example DESTINATION ${BINDIR})

SET(SHADOW_BONE_LIGHTING_SRCS ${EXAMPLES_SRC_DIR}/shadows/shadow-bone-lighting-example.cpp)
ADD_EXECUTABLE(shadow-bone-lighting.example ${SHADOW_BONE_LIGHTING_SRCS})
TARGET_LINK_LIBRARIES(shadow-bone-lighting.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS shadow-bone-lighting.example DESTINATION ${BINDIR})

SET(DALI_BUILDER_SRCS ${EXAMPLES_SRC_DIR}/builder/dali-builder.cpp)
ADD_EXECUTABLE(dali-builder ${DALI_BUILDER_SRCS})
TARGET_LINK_LIBRARIES(dali-builder ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS dali-builder DESTINATION ${BINDIR})

SET(BUILDER_SRCS ${EXAMPLES_SRC_DIR}/builder/examples.cpp)
ADD_EXECUTABLE(builder.example ${BUILDER_SRCS})
TARGET_LINK_LIBRARIES(builder.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS builder.example DESTINATION ${BINDIR})

SET(IMAGE_SCALING_IRREGULAR_GRID_SRCS ${EXAMPLES_SRC_DIR}/image/image-scaling-irregular-grid/image-scaling-irregular-grid-example.cpp)
ADD_EXECUTABLE(image-scaling-irregular-grid.example ${IMAGE_SCALING_IRREGULAR_GRID_SRCS})
TARGET_LINK_LIBRARIES(image-scaling-irregular-grid.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS image-scaling-irregular-grid.example DESTINATION ${BINDIR})

SET(BUTTONS_SRCS ${EXAMPLES_SRC_DIR}/buttons/buttons-example.cpp)
ADD_EXECUTABLE(buttons.example ${BUTTONS_SRCS})
TARGET_LINK_LIBRARIES(buttons.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS buttons.example DESTINATION ${BINDIR})

SET(TEXT_VIEW_SRCS ${EXAMPLES_SRC_DIR}/text-view/text-view-example.cpp)
ADD_EXECUTABLE(text-view.example ${TEXT_VIEW_SRCS})
TARGET_LINK_LIBRARIES(text-view.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS text-view.example DESTINATION ${BINDIR})

SET(LOGGING_SRCS ${EXAMPLES_SRC_DIR}/logging/logging-example.cpp)
ADD_EXECUTABLE(logging.example ${LOGGING_SRCS})
TARGET_LINK_LIBRARIES(logging.example ${REQUIRED_PKGS_LDFLAGS})
INSTALL(TARGETS logging.example DESTINATION ${BINDIR})
