/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include "dali-table-view.h"
#include "shared/dali-demo-strings.h"

using namespace Dali;

int main(int argc, char **argv)
{
  // Configure gettext for internalization
  bindtextdomain(DALI_DEMO_DOMAIN_LOCAL, DALI_LOCALE_DIR);
  textdomain(DALI_DEMO_DOMAIN_LOCAL);
  setlocale(LC_ALL, DALI_LANG);

  Application app = Application::New(&argc, &argv, DALI_DEMO_THEME_PATH);

  // Create the demo launcher
  DaliTableView demo(app);

  demo.AddExample(Example("bubble-effect.example", DALI_DEMO_STR_TITLE_BUBBLES));
  demo.AddExample(Example("blocks.example", DALI_DEMO_STR_TITLE_BLOCKS));
  demo.AddExample(Example("cube-transition-effect.example", DALI_DEMO_STR_TITLE_CUBE_TRANSITION));
  demo.AddExample(Example("dissolve-effect.example", DALI_DEMO_STR_TITLE_DISSOLVE_TRANSITION));
  demo.AddExample(Example("item-view.example", DALI_DEMO_STR_TITLE_ITEM_VIEW));
  demo.AddExample(Example("magnifier.example", DALI_DEMO_STR_TITLE_MAGNIFIER));
  demo.AddExample(Example("model3d-view.example", DALI_DEMO_STR_TITLE_MODEL_3D_VIEWER));
  demo.AddExample(Example("motion-blur.example", DALI_DEMO_STR_TITLE_MOTION_BLUR));
  demo.AddExample(Example("motion-stretch.example", DALI_DEMO_STR_TITLE_MOTION_STRETCH));
  demo.AddExample(Example("page-turn-view.example", DALI_DEMO_STR_TITLE_PAGE_TURN_VIEW));
  demo.AddExample(Example("radial-menu.example", DALI_DEMO_STR_TITLE_RADIAL_MENU));
  demo.AddExample(Example("refraction-effect.example", DALI_DEMO_STR_TITLE_REFRACTION));
  demo.AddExample(Example("scroll-view.example", DALI_DEMO_STR_TITLE_SCROLL_VIEW));
  demo.AddExample(Example("shadow-bone-lighting.example", DALI_DEMO_STR_TITLE_LIGHTS_AND_SHADOWS));
  demo.AddExample(Example("builder.example", DALI_DEMO_STR_TITLE_SCRIPT_BASED_UI));
  demo.AddExample(Example("image-scaling-and-filtering.example", DALI_DEMO_STR_TITLE_IMAGE_FITTING_SAMPLING));
  demo.AddExample(Example("image-scaling-irregular-grid.example", DALI_DEMO_STR_TITLE_IMAGE_SCALING));
  demo.AddExample(Example("text-field.example", DALI_DEMO_STR_TITLE_TEXT_FIELD));
  demo.AddExample(Example("text-label.example", DALI_DEMO_STR_TITLE_TEXT_LABEL));
  demo.AddExample(Example("text-label-multi-language.example", DALI_DEMO_STR_TITLE_TEXT_LABEL_MULTI_LANGUAGE));
  demo.AddExample(Example("text-label-emojis.example", DALI_DEMO_STR_TITLE_EMOJI_TEXT));
  demo.AddExample(Example("size-negotiation.example", DALI_DEMO_STR_TITLE_NEGOTIATE_SIZE));
  demo.AddExample(Example("popup.example", DALI_DEMO_STR_TITLE_POPUP));
  demo.AddExample(Example("buttons.example", DALI_DEMO_STR_TITLE_BUTTONS));
  demo.AddExample(Example("logging.example", DALI_DEMO_STR_TITLE_LOGGING));
  demo.AddExample(Example("mesh-morph.example", DALI_DEMO_STR_TITLE_MESH_MORPH));
  demo.AddExample(Example("mesh-sorting.example", DALI_DEMO_STR_TITLE_MESH_SORTING));
  demo.AddExample(Example("textured-mesh.example", DALI_DEMO_STR_TITLE_TEXTURED_MESH));
  demo.AddExample(Example("line-mesh.example", DALI_DEMO_STR_TITLE_LINE_MESH));
  demo.AddExample(Example("gradients.example", DALI_DEMO_STR_TITLE_COLOR_GRADIENT));
  demo.AddExample(Example("image-view.example", DALI_DEMO_STR_TITLE_IMAGE_VIEW));

  demo.SortAlphabetically( true );

  // Start the event loop
  app.MainLoop();

  return 0;
}
