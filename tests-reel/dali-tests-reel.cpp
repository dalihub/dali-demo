/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include "shared/dali-table-view.h"
#include "shared/dali-demo-strings.h"

using namespace Dali;

int DALI_EXPORT_API main(int argc, char **argv)
{
  // Configure gettext for internalization
  bindtextdomain(DALI_DEMO_DOMAIN_LOCAL, DEMO_LOCALE_DIR);
  textdomain(DALI_DEMO_DOMAIN_LOCAL);
  setlocale(LC_ALL, DEMO_LANG);

  Application app = Application::New(&argc, &argv, DEMO_THEME_PATH);

  // Create the demo launcher
  DaliTableView demo(app);

  demo.AddExample(Example("benchmark.example", DALI_DEMO_STR_TITLE_BENCHMARK));
  demo.AddExample(Example("compressed-texture-formats.example", DALI_DEMO_STR_TITLE_COMPRESSED_TEXTURE_FORMATS));
  demo.AddExample(Example("homescreen-benchmark.example", DALI_DEMO_STR_TITLE_HOMESCREEN));
  demo.AddExample(Example("perf-scroll.example", DALI_DEMO_STR_TITLE_PERF_SCROLL));
  demo.AddExample(Example("point-mesh.example", DALI_DEMO_STR_TITLE_POINT_MESH));
  demo.AddExample(Example("property-notification.example", DALI_DEMO_STR_TITLE_PROPERTY_NOTIFICATION));
  demo.AddExample(Example("simple-layout.example", DALI_DEMO_STR_TITLE_SIMPLE_LAYOUT));
  demo.AddExample(Example("simple-visuals-control.example", DALI_DEMO_STR_TITLE_SIMPLE_VISUALS_CONTROL));
  demo.AddExample(Example("text-fonts.example", DALI_DEMO_STR_TITLE_TEXT_FONTS));
  demo.AddExample(Example("text-memory-profiling.example", DALI_DEMO_STR_TITLE_TEXT_MEMORY_PROFILING));
  demo.AddExample(Example("text-overlap.example", DALI_DEMO_STR_TITLE_TEXT_OVERLAP));
  demo.AddExample(Example("visual-transitions.example", DALI_DEMO_STR_TITLE_VISUAL_TRANSITIONS));

  demo.SortAlphabetically( true );

  // Start the event loop
  app.MainLoop();

  return 0;
}
