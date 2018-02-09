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

  demo.AddExample(Example("blocks.example", DALI_DEMO_STR_TITLE_BLOCKS));
  demo.AddExample(Example("bezier-curve.example", DALI_DEMO_STR_TITLE_BEZIER_CURVE));
  demo.AddExample(Example("bubble-effect.example", DALI_DEMO_STR_TITLE_BUBBLES));
  demo.AddExample(Example("contact-cards.example", DALI_DEMO_STR_TITLE_CONTACT_CARDS));
  demo.AddExample(Example("cube-transition-effect.example", DALI_DEMO_STR_TITLE_CUBE_TRANSITION));
  demo.AddExample(Example("fpp-game.example", DALI_DEMO_STR_TITLE_FPP_GAME));
  demo.AddExample(Example("item-view.example", DALI_DEMO_STR_TITLE_ITEM_VIEW));
  demo.AddExample(Example("mesh-visual.example", DALI_DEMO_STR_TITLE_MESH_VISUAL));
  demo.AddExample(Example("motion-blur.example", DALI_DEMO_STR_TITLE_MOTION_BLUR));
  demo.AddExample(Example("refraction-effect.example", DALI_DEMO_STR_TITLE_REFRACTION));
  demo.AddExample(Example("renderer-stencil.example", DALI_DEMO_STR_TITLE_RENDERER_STENCIL));
  demo.AddExample(Example("shadows-and-lights.example", DALI_DEMO_STR_TITLE_LIGHTS_AND_SHADOWS));
  demo.AddExample(Example("sparkle.example", DALI_DEMO_STR_TITLE_SPARKLE));
  demo.AddExample(Example("rendering-skybox.example", DALI_DEMO_STR_TITLE_SKYBOX));
  demo.AddExample(Example("rendering-basic-pbr.example", DALI_DEMO_STR_TITLE_PBR));
  demo.AddExample(Example("animated-gradient-call-active.example", DALI_DEMO_STR_TITLE_CALL_ACTIVE));
  demo.AddExample(Example("animated-gradient-card-active.example", DALI_DEMO_STR_TITLE_CARD_ACTIVE));

  demo.SortAlphabetically( true );

  // Start the event loop
  app.MainLoop();

  return 0;
}
