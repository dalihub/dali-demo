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


bin_PROGRAMS = \
                dali-demo

demo_src_dir = ../../../demo
include ../../../demo/file.list

dalidemoimagesdir = $(appdatadir)/images/
dalidemoimages_DATA = $(demo_image_files)

dalidemomodelsdir = $(appdatadir)/models/
dalidemomodels_DATA = $(demo_model_files)

DEMO_CXXFLAGS = -DDALI_IMAGE_DIR="\"$(dalidemoimagesdir)\"" \
                -DDALI_MODEL_DIR="\"$(dalidemomodelsdir)\"" \
                -DDALI_EXAMPLE_BIN="\"$(exedir)\"" \
                -I../../../demo -I../../.. \
                $(DALIDEMO_CFLAGS) \
                $(DALI_TOOLKIT_CFLAGS) \
                $(DALI_CFLAGS) \
                $(AUL_CFLAGS) \
                -Werror -Wall

DEMO_DEPS =

DEMO_LDADD = $(DALI_LIBS) $(DALI_TOOLKIT_LIBS) $(AUL_LIBS)

dali_demo_SOURCES = $(demo_src_files)
dali_demo_CXXFLAGS = $(DEMO_CXXFLAGS)
dali_demo_DEPENDENCIES = $(DEMO_DEPS)
dali_demo_LDADD = $(DEMO_LDADD)
