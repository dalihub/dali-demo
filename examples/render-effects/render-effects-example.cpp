/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include <dali/dali.h>
#include <devel-api/actors/actor-devel.h>
#include <public-api/events/key-event.h>

#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>

#include <unordered_map>

using namespace Dali;

namespace
{
const char* BACKGROUND_IMAGE(DEMO_IMAGE_DIR "background-3.jpg");
const char* SUN_CLOUD_ICON_IMAGE(DEMO_IMAGE_DIR "light-icon-front.png");
} // namespace

class RenderEffectController : public Dali::ConnectionTracker
{
public:
  RenderEffectController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &RenderEffectController::Create);
  }

  ~RenderEffectController()
  {
  }

  void Create(Dali::Application& application)
  {
    Window window          = application.GetWindow();
    Layer  backgroundLayer = Layer::New();

    Vector2 size = window.GetSize();

    // Background image
    {
      backgroundLayer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      backgroundLayer.SetProperty(Actor::Property::SIZE, size);

      Toolkit::ImageView backgroundImage = Toolkit::ImageView::New(BACKGROUND_IMAGE);
      backgroundImage.SetProperty(Actor::Property::SIZE, size);
      backgroundImage.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      backgroundLayer.Add(backgroundImage);
      window.Add(backgroundLayer);
    }

    // UI panel
    float unitSizeWidth  = 180.0f;
    float unitSizeHeight = 180.0f;

    Toolkit::Control UIPanel = Toolkit::Control::New();
    UIPanel.SetProperty(Actor::Property::SIZE, size * 0.8f);
    UIPanel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    window.Add(UIPanel);
    UIPanel.SetRenderEffect(Toolkit::BackgroundBlurEffect::New(0.4f, 40, 10.0f));

    // Welcome message
    {
      Toolkit::TextLabel label = SetUpTextLabelProperties("Welcome, You.", Vector4::ONE, "BEGIN", 20.0f);
      label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
      label.SetProperty(Actor::Property::SIZE, Vector2(unitSizeWidth * 2.0f, unitSizeHeight + 20.0f));
      label.SetProperty(Actor::Property::POSITION_Y, 50.0f);
      UIPanel.Add(label);
    }

    // Weather panel
    {
      Toolkit::Control weatherPanel = Toolkit::Control::New();
      weatherPanel.SetProperty(Actor::Property::SIZE, Vector2(unitSizeWidth * 2.0f, unitSizeHeight + 10.0f));
      weatherPanel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
      weatherPanel.SetProperty(Actor::Property::POSITION, Vector2(0, size.y * 0.15f));

      Property::Map colorVisualPropertyMap;
      colorVisualPropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
      colorVisualPropertyMap.Insert(Toolkit::Visual::Property::MIX_COLOR, Color::BLACK);
      colorVisualPropertyMap.Insert(Toolkit::Visual::Property::OPACITY, 0.2f);
      colorVisualPropertyMap.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, 30.0f);
      weatherPanel.SetProperty(Toolkit::Control::Property::BACKGROUND, colorVisualPropertyMap);

      Toolkit::TextLabel label = SetUpTextLabelProperties("10:21", Color::WHITE, "BEGIN", 36.0f);
      label.SetProperty(Actor::Property::SIZE_WIDTH, 250.0f);
      label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      label.SetProperty(Actor::Property::POSITION_Y, -10.0f);
      weatherPanel.Add(label);

      label = SetUpTextLabelProperties("June 4th, 2024", Color::WHITE, "BEGIN", 13.0f);
      label.SetProperty(Actor::Property::SIZE_WIDTH, 250.0f);
      label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      label.SetProperty(Actor::Property::POSITION_Y, 30.0f);
      weatherPanel.Add(label);

      Toolkit::ImageView weatherIcon = Toolkit::ImageView::New(SUN_CLOUD_ICON_IMAGE);
      weatherIcon.SetProperty(Actor::Property::SIZE, Vector2(120.0f, 100.0f));
      weatherIcon.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      weatherIcon.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_LEFT);
      weatherIcon.SetProperty(Actor::Property::POSITION, Vector2(30.0f, 30.0f));
      weatherPanel.Add(weatherIcon);

      label = SetUpTextLabelProperties("18", Color::WHITE, "END", 25.0f);
      label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
      label.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      label.SetProperty(Actor::Property::POSITION_X, 100.0f);
      label.SetProperty(Actor::Property::POSITION_Y, -50.0f);
      weatherPanel.Add(label);

      Toolkit::TextLabel unitLabel = SetUpTextLabelProperties("°C", Color::WHITE, "BEGIN");
      unitLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_RIGHT);
      unitLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
      label.Add(unitLabel);

      UIPanel.Add(weatherPanel);
      weatherPanel.SetRenderEffect(Toolkit::BackgroundBlurEffect::New(0.4f, 40, 10.0f));
    }

    // Icon mini panels
    {
      Vector2 iconPanelSize = Vector2(unitSizeWidth, unitSizeHeight);
      float   x_incrementer = iconPanelSize.x / 2.0f + 10.0f;
      float   y_incrementer = iconPanelSize.y + 20.f;
      float   y_starter     = size.y * .33f;

      Toolkit::Control control = CreateIconPanel("Security", "4 rooms", false, DEMO_IMAGE_DIR "application-icon-7.png", iconPanelSize);
      control.SetProperty(Actor::Property::POSITION, Vector2(-x_incrementer, y_starter));
      control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
      UIPanel.Add(control);
      control.SetRenderEffect(Toolkit::BackgroundBlurEffect::New(0.4f, 40, 10.0f));

      control = CreateIconPanel("BlueTooth", "2 devices", true, DEMO_IMAGE_DIR "application-icon-14.png", iconPanelSize);
      control.SetProperty(Actor::Property::POSITION, Vector2(x_incrementer, y_starter));
      control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
      UIPanel.Add(control);
      control.SetRenderEffect(Toolkit::BackgroundBlurEffect::New(0.4f, 40, 10.0f));

      control = CreateIconPanel("Wi-Fi", "TizenUIFW", true, DEMO_IMAGE_DIR "application-icon-55.png", iconPanelSize);
      control.SetProperty(Actor::Property::POSITION, Vector2(-x_incrementer, y_starter + y_incrementer));
      control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
      UIPanel.Add(control);
      control.SetRenderEffect(Toolkit::BackgroundBlurEffect::New(0.4f, 40, 10.0f));

      control = CreateIconPanel("Lighting", "5 devices", true, DEMO_IMAGE_DIR "application-icon-21.png", iconPanelSize);
      control.SetProperty(Actor::Property::POSITION, Vector2(x_incrementer, y_starter + y_incrementer));
      control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
      UIPanel.Add(control);
      control.SetRenderEffect(Toolkit::BackgroundBlurEffect::New(0.4f, 40, 10.0f));
    }

    // Air conditioner
    {
      Toolkit::Control airConPanel = Toolkit::Control::New();
      airConPanel.SetProperty(Actor::Property::SIZE, Vector2(unitSizeWidth * 2.0f, unitSizeHeight + 10.0f));
      airConPanel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
      airConPanel.SetProperty(Actor::Property::POSITION_Y, -unitSizeHeight + 20.0f);
      airConPanel.SetProperty(DevelActor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);

      Toolkit::TextLabel label = SetUpTextLabelProperties("Air Conditioner", Color::WHITE, "BEGIN", 15.0f);
      label.SetProperty(Actor::Property::SIZE_WIDTH, unitSizeWidth * 2.0f - 50.0f);
      label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      label.SetProperty(Actor::Property::POSITION_Y, -unitSizeHeight * 0.3f);
      airConPanel.Add(label);

      label = SetUpTextLabelProperties("24", Color::WHITE, "END", 36.0f);
      label.SetProperty(Actor::Property::SIZE_WIDTH, 36.0f * 2.0f); // maximum two characters
      label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      label.SetProperty(Actor::Property::POSITION_Y, unitSizeHeight * 0.05f);
      label.SetProperty(Actor::Property::POSITION_X, -unitSizeWidth * 0.75f);
      airConPanel.Add(label);

      Toolkit::TextLabel unitLabel = SetUpTextLabelProperties("°C", Color::WHITE, "BEGIN");
      unitLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_RIGHT);
      unitLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
      label.Add(unitLabel);

      label = SetUpTextLabelProperties("Living room", Color::WHITE, "BEGIN", 13.0f);
      label.SetProperty(Actor::Property::SIZE_WIDTH, unitSizeWidth * 2.0f - 50.0f);
      label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      label.SetProperty(Actor::Property::POSITION_Y, unitSizeHeight * 0.3f);
      airConPanel.Add(label);

      Toolkit::ImageView airConImage = Toolkit::ImageView::New(DEMO_IMAGE_DIR "application-icon-24.png");
      airConImage.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      airConImage.SetProperty(Actor::Property::SIZE, Vector2(unitSizeWidth, unitSizeHeight) * 0.5f);
      airConImage.SetProperty(Actor::Property::POSITION_X, unitSizeWidth * .5f);
      airConPanel.Add(airConImage);

      Property::Map airConPanelDimmer;
      airConPanelDimmer.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
      airConPanelDimmer.Insert(Toolkit::Visual::Property::MIX_COLOR, Color::BLACK);
      airConPanelDimmer.Insert(Toolkit::Visual::Property::OPACITY, 0.2f);
      airConPanelDimmer.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, 30.0f);
      airConPanel.SetProperty(Toolkit::Control::Property::BACKGROUND, airConPanelDimmer);

      UIPanel.Add(airConPanel);
      airConPanel.SetRenderEffect(Toolkit::BackgroundBlurEffect::New(0.4f, 40, 10.0f));
    }

    // lower background layer
    backgroundLayer.LowerBelow(window.GetRootLayer());

    // Connect signals
    application.GetWindow().KeyEventSignal().Connect(this, &RenderEffectController::OnKeyEvent);
  }

  Toolkit::Control CreateIconPanel(std::string title, std::string detail, bool isOn, std::string iconURL, Vector2 size)
  {
    Toolkit::Control panel = Toolkit::Control::New();
    panel.SetProperty(Actor::Property::SIZE, size);

    Property::Map colorVisualPropertyMap;
    colorVisualPropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
    colorVisualPropertyMap.Insert(Toolkit::Visual::Property::OPACITY, 0.3f);
    colorVisualPropertyMap.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, 30.0f);
    panel.SetProperty(Toolkit::Control::Property::BACKGROUND, colorVisualPropertyMap);

    // TOP
    Toolkit::ImageView icon = Toolkit::ImageView::New(iconURL);
    icon.SetProperty(Actor::Property::SIZE, Vector2(50.0f, 50.0f));
    icon.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    icon.SetProperty(Actor::Property::POSITION, size * 0.05f);
    panel.Add(icon);

    Toolkit::TextLabel isOnLabel;
    if(isOn)
    {
      isOnLabel = SetUpTextLabelProperties("On", Vector4::ONE, "END", 13.0f);
    }
    else
    {
      isOnLabel = SetUpTextLabelProperties("Off", Vector4::ONE, "END", 13.0f);
    }
    isOnLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
    isOnLabel.SetProperty(Actor::Property::POSITION_Y, size.y * 0.15f);
    isOnLabel.SetProperty(Actor::Property::SIZE_WIDTH, size.x * 0.7f);
    panel.Add(isOnLabel);

    // MIDDLE
    Toolkit::TextLabel textLabel = SetUpTextLabelProperties(title, Vector4::ONE, "BEGIN");
    textLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    textLabel.SetProperty(Actor::Property::PADDING, Vector4(20.0f, 20.0f, 20.0f, 20.0f));
    panel.Add(textLabel);

    Toolkit::TextLabel detailLabel = SetUpTextLabelProperties(detail, Vector4::ONE, "BEGIN", 13.0f);
    detailLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    detailLabel.SetProperty(Actor::Property::PADDING, Vector4(20.0f, 20.0f, 20.0f, 20.0f));
    detailLabel.SetProperty(Actor::Property::POSITION_Y, 30.0f);
    panel.Add(detailLabel);

    return panel;
  }

  Toolkit::TextLabel SetUpTextLabelProperties(std::string text, Vector4 color, std::string alignment, float fontSize = 15.0f)
  {
    Toolkit::TextLabel label = Toolkit::TextLabel::New(text);
    label.SetProperty(Toolkit::TextLabel::Property::TEXT_COLOR, color);
    label.SetProperty(Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, alignment);
    label.SetProperty(Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
    label.SetProperty(Toolkit::TextLabel::Property::POINT_SIZE, fontSize);
    return label;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
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
  Application            application = Application::New(&argc, &argv, DEMO_THEME_PATH);
  RenderEffectController test(application);
  application.MainLoop();
  return 0;
}
