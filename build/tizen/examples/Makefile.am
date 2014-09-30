# Copyright (c) 2014 Samsung Electronics Co., Ltd.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


examples_src_dir = ../../../examples

bin_PROGRAMS = \
               blocks.example \
               bubble-effect.example \
               cluster.example \
               cube-transition-effect.example \
               dissolve-effect.example \
               hello-world.example \
               item-view.example \
               magnifier.example \
               motion-blur.example \
               motion-stretch.example \
               page-turn-view.example \
               radial-menu.example \
               scroll-view.example \
               shadow-bone-lighting.example \
               dali-builder \
               builder.example \
               image-scaling-irregular-grid.example \
               buttons.example \
               text-view.example


daliimagedir = $(appdatadir)/images/
dalimodeldir = $(appdatadir)/models/
daliscriptdir = $(appdatadir)/scripts/

BASE_CXXFLAGS = -I../../../examples \
                   -DDALI_IMAGE_DIR="\"${daliimagedir}\"" \
                   -DDALI_MODEL_DIR="\"${dalimodeldir}\"" \
                   -DDALI_SCRIPT_DIR="\"${daliscriptdir}\"" \
                   $(DALIDEMO_CFLAGS) \
                   $(ECORE_X_CFLAGS) \
                   $(CAPI_MEDIA_PLAYER_CFLAGS) \
                   $(CAPI_APPFW_APPLICATION_CFLAGS) \
                   -I/usr/include/media \
                   -Werror -Wall

EXAMPLE_CXXFLAGS      = $(DALI_CFLAGS) \
                        $(DALI_TOOLKIT_CFLAGS) \
                        $(BASE_CXXFLAGS)


EXAMPLE_DEPS =

EXAMPLE_LDADD = $(DALI_LIBS) $(DALI_TOOLKIT_LIBS) $(ECORE_X_LIBS) $(CAPI_MEDIA_PLAYER_LIBS) $(CAPI_APPFW_APPLICATION_LIBS) -lrt -lEGL


blocks_example_SOURCES = $(examples_src_dir)/blocks/blocks-example.cpp
blocks_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
blocks_example_DEPENDENCIES = $(EXAMPLE_DEPS)
blocks_example_LDADD = $(EXAMPLE_LDADD)

bubble_effect_example_SOURCES = $(examples_src_dir)/shader-effect/bubble-effect-example.cpp
bubble_effect_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
bubble_effect_example_DEPENDENCIES = $(EXAMPLE_DEPS)
bubble_effect_example_LDADD = $(EXAMPLE_LDADD)

cluster_example_SOURCES = $(examples_src_dir)/cluster/cluster-example.cpp
cluster_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
cluster_example_DEPENDENCIES = $(EXAMPLE_DEPS)
cluster_example_LDADD = $(EXAMPLE_LDADD)

cube_transition_effect_example_SOURCES = $(examples_src_dir)/transition/cube-transition-effect-example.cpp
cube_transition_effect_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
cube_transition_effect_example_DEPENDENCIES = $(EXAMPLE_DEPS)
cube_transition_effect_example_LDADD = $(EXAMPLE_LDADD)

dissolve_effect_example_SOURCES = $(examples_src_dir)/shader-effect/dissolve-effect-example.cpp
dissolve_effect_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
dissolve_effect_example_DEPENDENCIES = $(EXAMPLE_DEPS)
dissolve_effect_example_LDADD = $(EXAMPLE_LDADD)

hello_world_example_SOURCES = $(examples_src_dir)/hello-world/hello-world-example.cpp
hello_world_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
hello_world_example_DEPENDENCIES = $(EXAMPLE_DEPS)
hello_world_example_LDADD = $(EXAMPLE_LDADD)

item_view_example_SOURCES = $(examples_src_dir)/item-view/item-view-example.cpp
item_view_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
item_view_example_DEPENDENCIES = $(EXAMPLE_DEPS)
item_view_example_LDADD = $(EXAMPLE_LDADD)

magnifier_example_SOURCES = $(examples_src_dir)/magnifier/magnifier-example.cpp
magnifier_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
magnifier_example_DEPENDENCIES = $(EXAMPLE_DEPS)
magnifier_example_LDADD = $(EXAMPLE_LDADD)

motion_blur_example_SOURCES = $(examples_src_dir)/motion/motion-blur-example.cpp
motion_blur_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
motion_blur_example_DEPENDENCIES = $(EXAMPLE_DEPS)
motion_blur_example_LDADD = $(EXAMPLE_LDADD)

motion_stretch_example_SOURCES = $(examples_src_dir)/motion/motion-stretch-example.cpp
motion_stretch_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
motion_stretch_example_DEPENDENCIES = $(EXAMPLE_DEPS)
motion_stretch_example_LDADD = $(EXAMPLE_LDADD)

page_turn_view_example_SOURCES = $(examples_src_dir)/page-turn-view/page-turn-view-example.cpp
page_turn_view_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
page_turn_view_example_DEPENDENCIES = $(EXAMPLE_DEPS)
page_turn_view_example_LDADD = $(EXAMPLE_LDADD)

radial_menu_example_SOURCES = $(examples_src_dir)/radial-menu/radial-menu-example.cpp \
                              $(examples_src_dir)/radial-menu/radial-sweep-view.cpp \
                              $(examples_src_dir)/radial-menu/radial-sweep-view-impl.cpp
radial_menu_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
radial_menu_example_DEPENDENCIES = $(EXAMPLE_DEPS)
radial_menu_example_LDADD = $(EXAMPLE_LDADD)

scroll_view_example_SOURCES = $(examples_src_dir)/scroll-view/scroll-view-example.cpp
scroll_view_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
scroll_view_example_DEPENDENCIES = $(EXAMPLE_DEPS)
scroll_view_example_LDADD = $(EXAMPLE_LDADD)

shadow_bone_lighting_example_SOURCES = $(examples_src_dir)/shadows/shadow-bone-lighting-example.cpp
shadow_bone_lighting_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
shadow_bone_lighting_example_DEPENDENCIES = $(EXAMPLE_DEPS)
shadow_bone_lighting_example_LDADD = $(EXAMPLE_LDADD)

dali_builder_SOURCES = $(examples_src_dir)/builder/dali-builder.cpp
dali_builder_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
dali_builder_DEPENDENCIES = $(EXAMPLE_DEPS)
dali_builder_LDADD = $(EXAMPLE_LDADD)

builder_example_SOURCES = $(examples_src_dir)/builder/examples.cpp
builder_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
builder_example_DEPENDENCIES = $(EXAMPLE_DEPS)
builder_example_LDADD = $(EXAMPLE_LDADD)

image_scaling_irregular_grid_example_SOURCES = $(examples_src_dir)/image/image-scaling-irregular-grid/image-scaling-irregular-grid-example.cpp
image_scaling_irregular_grid_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
image_scaling_irregular_grid_example_DEPENDENCIES = $(EXAMPLE_DEPS)
image_scaling_irregular_grid_example_LDADD = $(EXAMPLE_LDADD)

buttons_example_SOURCES = $(examples_src_dir)/buttons/buttons-example.cpp
buttons_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
buttons_example_DEPENDENCIES = $(EXAMPLE_DEPS)
buttons_example_LDADD = $(EXAMPLE_LDADD)

text_view_example_SOURCES = $(examples_src_dir)/text-view/text-view-example.cpp
text_view_example_CXXFLAGS = $(EXAMPLE_CXXFLAGS)
text_view_example_DEPENDENCIES = $(EXAMPLE_DEPS)
text_view_example_LDADD = $(EXAMPLE_LDADD)
