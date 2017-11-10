/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

  demo.AddExample(Example("animated-images.example", DALI_DEMO_STR_TITLE_ANIMATED_IMAGES));
  demo.AddExample(Example("animated-shapes.example", DALI_DEMO_STR_TITLE_ANIMATED_SHAPES));
  demo.AddExample(Example("alpha-blending-cpu.example", DALI_DEMO_STR_TITLE_ALPHA_BLENDING_CPU));
  demo.AddExample(Example("builder.example", DALI_DEMO_STR_TITLE_SCRIPT_BASED_UI));
  demo.AddExample(Example("buttons.example", DALI_DEMO_STR_TITLE_BUTTONS));
  demo.AddExample(Example("clipping.example", DALI_DEMO_STR_TITLE_CLIPPING));
  demo.AddExample(Example("clipping-draw-order.example", DALI_DEMO_STR_TITLE_CLIPPING_DRAW_ORDER));
  demo.AddExample(Example("dissolve-effect.example", DALI_DEMO_STR_TITLE_DISSOLVE_TRANSITION));
  demo.AddExample(Example("effects-view.example", DALI_DEMO_STR_TITLE_EFFECTS_VIEW));
  demo.AddExample(Example("flex-container.example", DALI_DEMO_STR_TITLE_FLEXBOX_PLAYGROUND));
  demo.AddExample(Example("focus-integration.example", DALI_DEMO_STR_TITLE_FOCUS_INTEGRATION));
  demo.AddExample(Example("gradients.example", DALI_DEMO_STR_TITLE_COLOR_GRADIENT));
  demo.AddExample(Example("image-policies.example", DALI_DEMO_STR_TITLE_IMAGE_POLICIES));
  demo.AddExample(Example("image-scaling-and-filtering.example", DALI_DEMO_STR_TITLE_IMAGE_FITTING_SAMPLING));
  demo.AddExample(Example("image-scaling-irregular-grid.example", DALI_DEMO_STR_TITLE_IMAGE_SCALING));
  demo.AddExample(Example("image-view.example", DALI_DEMO_STR_TITLE_IMAGE_VIEW));
  demo.AddExample(Example("image-view-alpha-blending.example", DALI_DEMO_STR_TITLE_IMAGE_VIEW_ALPHA_BLENDING));
  demo.AddExample(Example("image-view-pixel-area.example", DALI_DEMO_STR_TITLE_IMAGE_VIEW_PIXEL_AREA));
  demo.AddExample(Example("image-view-svg.example", DALI_DEMO_STR_TITLE_IMAGE_VIEW_SVG));
  demo.AddExample(Example("image-view-url.example", DALI_DEMO_STR_TITLE_IMAGE_VIEW_URL));
  demo.AddExample(Example("line-mesh.example", DALI_DEMO_STR_TITLE_LINE_MESH));
  demo.AddExample(Example("magnifier.example", DALI_DEMO_STR_TITLE_MAGNIFIER));
  demo.AddExample(Example("mesh-morph.example", DALI_DEMO_STR_TITLE_MESH_MORPH));
  demo.AddExample(Example("motion-stretch.example", DALI_DEMO_STR_TITLE_MOTION_STRETCH));
  demo.AddExample(Example("native-image-source.example", DALI_DEMO_STR_TITLE_NATIVE_IMAGE_SOURCE));
  demo.AddExample(Example("popup.example", DALI_DEMO_STR_TITLE_POPUP));
  demo.AddExample(Example("pivot.example", DALI_DEMO_STR_TITLE_PIVOT));
  demo.AddExample(Example("primitive-shapes.example", DALI_DEMO_STR_TITLE_PRIMITIVE_SHAPES));
  demo.AddExample(Example("progress-bar.example", DALI_DEMO_STR_TITLE_PROGRESS_BAR));
  demo.AddExample(Example("property-notification.example", DALI_DEMO_STR_TITLE_PROPERTY_NOTIFICATION));
  demo.AddExample(Example("rendering-basic-light.example", DALI_DEMO_STR_TITLE_BASIC_LIGHT));
  demo.AddExample(Example("rendering-line.example", DALI_DEMO_STR_TITLE_RENDERING_DRAW_LINE));
  demo.AddExample(Example("rendering-triangle.example", DALI_DEMO_STR_TITLE_RENDERING_DRAW_TRIANGLE));
  demo.AddExample(Example("rendering-cube.example", DALI_DEMO_STR_TITLE_RENDERING_DRAW_CUBE));
  demo.AddExample(Example("rendering-textured-cube.example", DALI_DEMO_STR_TITLE_RENDERING_TEXTURED_CUBE));
  demo.AddExample(Example("rendering-radial-progress.example", DALI_DEMO_STR_TITLE_RENDERING_RADIAL_PROGRESS));
  demo.AddExample(Example("ray-marching.example", DALI_DEMO_STR_TITLE_RENDERING_RAY_MARCHING));
  demo.AddExample(Example("scroll-view.example", DALI_DEMO_STR_TITLE_SCROLL_VIEW));
  demo.AddExample(Example("size-negotiation.example", DALI_DEMO_STR_TITLE_NEGOTIATE_SIZE));
  demo.AddExample(Example("styling.example", DALI_DEMO_STR_TITLE_STYLING));
  demo.AddExample(Example("text-editor.example", DALI_DEMO_STR_TITLE_TEXT_EDITOR));
  demo.AddExample(Example("text-field.example", DALI_DEMO_STR_TITLE_TEXT_FIELD));
  demo.AddExample(Example("text-label.example", DALI_DEMO_STR_TITLE_TEXT_LABEL));
  demo.AddExample(Example("text-label-multi-language.example", DALI_DEMO_STR_TITLE_TEXT_LABEL_MULTI_LANGUAGE));
  demo.AddExample(Example("text-label-emojis.example", DALI_DEMO_STR_TITLE_EMOJI_TEXT));
  demo.AddExample(Example("text-memory-profiling.example", DALI_DEMO_STR_TITLE_TEXT_MEMORY_PROFILING));
  demo.AddExample(Example("text-overlap.example", DALI_DEMO_STR_TITLE_TEXT_OVERLAP));
  demo.AddExample(Example("text-scrolling.example", DALI_DEMO_STR_TITLE_TEXT_SCROLLING));
  demo.AddExample(Example("remote-image-loading.example", DALI_DEMO_STR_TITLE_REMOTE_IMAGE));
  demo.AddExample(Example("textured-mesh.example", DALI_DEMO_STR_TITLE_TEXTURED_MESH));
  demo.AddExample(Example("tilt.example", DALI_DEMO_STR_TITLE_TILT_SENSOR));
  demo.AddExample(Example("tooltip.example", DALI_DEMO_STR_TITLE_TOOLTIP));
  demo.AddExample(Example("transitions.example", DALI_DEMO_STR_TITLE_VISUAL_TRANSITIONS));

  demo.SortAlphabetically( true );

  // Start the event loop
  app.MainLoop();

  return 0;
}
