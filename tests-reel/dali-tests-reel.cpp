/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// EXTERNAL INCLUDES
#include <dali/dali.h>

// INTERNAL INCLUDES
#include "shared/dali-demo-strings.h"
#include "shared/dali-table-view.h"

using namespace Dali;

int DALI_EXPORT_API main(int argc, char** argv)
{
  // Configure gettext for internalization
#if INTERNATIONALIZATION_ENABLED
  bindtextdomain(DALI_DEMO_DOMAIN_LOCAL, DEMO_LOCALE_DIR);
  textdomain(DALI_DEMO_DOMAIN_LOCAL);
  setlocale(LC_ALL, DEMO_LANG);
#endif
  Application app = Application::New(&argc, &argv, DEMO_STYLE_DIR "/tests-theme.json");

  // Create the demo launcher
  DaliTableView demo(app);

  demo.AddExample(Example("benchmark.example", DALI_DEMO_STR_TITLE_BENCHMARK));
  demo.AddExample(Example("benchmark-2dphysics.example", DALI_DEMO_STR_TITLE_BENCHMARK_2D_PHYSICS));
  demo.AddExample(Example("benchmark-color.example", DALI_DEMO_STR_TITLE_BENCHMARK_COLOR));
  demo.AddExample(Example("camera-test.example", DALI_DEMO_STR_TITLE_CAMERA_TEST));
  demo.AddExample(Example("compressed-texture-formats.example", DALI_DEMO_STR_TITLE_COMPRESSED_TEXTURE_FORMATS));
  demo.AddExample(Example("frame-callback-toggle.example", DALI_DEMO_STR_TITLE_FRAME_CALLBACK_TOGGLE));
  demo.AddExample(Example("homescreen-benchmark.example", DALI_DEMO_STR_TITLE_HOMESCREEN));
  demo.AddExample(Example("image-view-yuv.example", DALI_DEMO_STR_TITLE_IMAGE_VIEW_YUV));
  demo.AddExample(Example("inherit-test.example", DALI_DEMO_STR_TITLE_INHERIT_TEST));
  demo.AddExample(Example("instance-rendering.example", DALI_DEMO_STR_TITLE_INSTANCE_RENDERING));
  demo.AddExample(Example("pre-render-callback.example", DALI_DEMO_STR_TITLE_PRE_RENDER_CALLBACK));
  demo.AddExample(Example("precompile-shader.example", DALI_DEMO_STR_TITLE_PRECOMPILE_SHADER));
  demo.AddExample(Example("perf-scroll.example", DALI_DEMO_STR_TITLE_PERF_SCROLL));
  demo.AddExample(Example("perf-view-creation.example", DALI_DEMO_STR_TITLE_PERF_VIEW_CREATION));
  demo.AddExample(Example("point-mesh.example", DALI_DEMO_STR_TITLE_POINT_MESH));
  demo.AddExample(Example("property-notification.example", DALI_DEMO_STR_TITLE_PROPERTY_NOTIFICATION));
  demo.AddExample(Example("simple-visuals-control.example", DALI_DEMO_STR_TITLE_SIMPLE_VISUALS_CONTROL));
  demo.AddExample(Example("text-fonts.example", DALI_DEMO_STR_TITLE_TEXT_FONTS));
  demo.AddExample(Example("text-memory-profiling.example", DALI_DEMO_STR_TITLE_TEXT_MEMORY_PROFILING));
  demo.AddExample(Example("text-overlap.example", DALI_DEMO_STR_TITLE_TEXT_OVERLAP));
  demo.AddExample(Example("visual-fitting-mode.example", DALI_DEMO_STR_TITLE_VISUAL_FITTING_MODE));
  demo.AddExample(Example("visual-transitions.example", DALI_DEMO_STR_TITLE_VISUAL_TRANSITIONS));
  demo.AddExample(Example("scene-view-test.example", DALI_DEMO_STR_TITLE_SCENE_VIEW_TEST));
  demo.AddExample(Example("simple-scroll-view.example", DALI_DEMO_STR_TITLE_SIMPLE_SCROLL_VIEW));
  demo.AddExample(Example("simple-text-label.example", DALI_DEMO_STR_TITLE_TEXT_LABEL));
  demo.AddExample(Example("simple-text-field.example", DALI_DEMO_STR_TITLE_TEXT_FIELD));
  demo.AddExample(Example("simple-text-visual.example", DALI_DEMO_STR_TITLE_TEXT_VISUAL));
  demo.AddExample(Example("simple-bitmap-font-text-label.example", DALI_DEMO_STR_TITLE_TEXT_LABEL_BITMAP_FONT));
  demo.AddExample(Example("touch.example", DALI_DEMO_STR_TITLE_TOUCH));
  demo.AddExample(Example("uniform-blocks.example", DALI_DEMO_STR_TITLE_UNIFORM_BLOCKS));

  demo.SortAlphabetically(true);

  // Start the event loop
  app.MainLoop();

  return 0;
}
