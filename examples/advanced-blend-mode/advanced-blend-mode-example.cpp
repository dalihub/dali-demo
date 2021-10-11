/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/actors/actor-devel.h>

#include <dali/devel-api/common/capabilities.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

// This example shows how to create and display Hello World! using a simple TextActor
//
class AdvancedBlendModeController : public ConnectionTracker
{
public:
  AdvancedBlendModeController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &AdvancedBlendModeController::Create);
  }

  ~AdvancedBlendModeController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the stage
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::BLACK);

    Toolkit::ImageView imageView = Toolkit::ImageView::New();
    Property::Map      imagePropertyMap;
    imagePropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
    imagePropertyMap.Insert(Toolkit::ImageVisual::Property::URL, DEMO_IMAGE_DIR "gallery-large-19.jpg");
    imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imagePropertyMap);
    imageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
    imageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    imageView.SetProperty(Actor::Property::SIZE, Vector2(600, 600));
    window.Add(imageView);

    Toolkit::Control control_1 = Toolkit::Control::New();
    Property::Map    colorVisualMap_1;
    colorVisualMap_1.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
    colorVisualMap_1.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Dali::Color::RED);
    colorVisualMap_1.Insert(Toolkit::Visual::Property::PREMULTIPLIED_ALPHA, true);
    control_1.SetProperty(Toolkit::Control::Property::BACKGROUND, colorVisualMap_1);
    control_1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
    control_1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    control_1.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
    control_1.SetProperty(Actor::Property::SIZE, Vector2(600, 200));
    if(Dali::Capabilities::IsBlendEquationSupported(Dali::DevelBlendEquation::SCREEN))
    {
      control_1.SetProperty(Dali::DevelActor::Property::BLEND_EQUATION, Dali::DevelBlendEquation::LUMINOSITY);
    }
    window.Add(control_1);

    Toolkit::Control control_2 = Toolkit::Control::New();
    Property::Map    colorVisualMap_2;
    colorVisualMap_2.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
    colorVisualMap_2.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Dali::Color::GREEN);
    colorVisualMap_2.Insert(Toolkit::Visual::Property::PREMULTIPLIED_ALPHA, true);
    control_2.SetProperty(Toolkit::Control::Property::BACKGROUND, colorVisualMap_2);
    control_2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
    control_2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    control_2.SetProperty(Actor::Property::POSITION, Vector2(0, 200));
    control_2.SetProperty(Actor::Property::SIZE, Vector2(600, 200));
    if(Dali::Capabilities::IsBlendEquationSupported(Dali::DevelBlendEquation::SCREEN))
    {
      control_2.SetProperty(Dali::DevelActor::Property::BLEND_EQUATION, Dali::DevelBlendEquation::LUMINOSITY);
    }
    window.Add(control_2);

    Toolkit::Control control_3 = Toolkit::Control::New();
    Property::Map    colorVisualMap_3;
    colorVisualMap_3.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
    colorVisualMap_3.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, Dali::Color::BLUE);
    colorVisualMap_3.Insert(Toolkit::Visual::Property::PREMULTIPLIED_ALPHA, true);
    control_3.SetProperty(Toolkit::Control::Property::BACKGROUND, colorVisualMap_3);
    control_3.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
    control_3.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    control_3.SetProperty(Actor::Property::POSITION, Vector2(0, 400));
    control_3.SetProperty(Actor::Property::SIZE, Vector2(600, 200));
    if(Dali::Capabilities::IsBlendEquationSupported(Dali::DevelBlendEquation::SCREEN))
    {
      control_3.SetProperty(Dali::DevelActor::Property::BLEND_EQUATION, Dali::DevelBlendEquation::LUMINOSITY);
    }
    window.Add(control_3);

    // Add text to explain what's being seen.
    auto label = Toolkit::TextLabel::New("If your device supports advanced blending, this shows an image at different levels of luminosity. If not, it instead shows red/green/blue sections.");

    label[Toolkit::TextLabel::Property::MULTI_LINE] = true;
    label[Toolkit::TextLabel::Property::TEXT_COLOR] = Color::WHITE;
    label[Toolkit::TextLabel::Property::POINT_SIZE] = 12.0f;
    label[Actor::Property::PARENT_ORIGIN]           = ParentOrigin::BOTTOM_CENTER;
    label[Actor::Property::ANCHOR_POINT]            = AnchorPoint::BOTTOM_CENTER;
    label.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    window.Add(label);

    // Respond to a touch anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &AdvancedBlendModeController::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &AdvancedBlendModeController::OnKeyEvent);
  }

  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                 application = Application::New(&argc, &argv);
  AdvancedBlendModeController test(application);
  application.MainLoop();
  return 0;
}
