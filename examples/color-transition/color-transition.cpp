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
#include "utils.h"
#include "color-transition-controller.h"
#include "dali/dali.h"
#include "dali-toolkit/dali-toolkit.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const float TRANSITION_DURATION = 1.f;

const Vector3 INITIAL_COLOR{ 1.f, 1.f, .25f };

const char* const FLOW_MAPS[] = {
  "circular",
  "multi-circle",
  "concentric",
  "spiral",
  "conical",
  "blinds",
  "radial",
  "swipe",
  "bubbles",
  "image"
};

Texture LoadTexture(const std::string& path)
{
  PixelData pixelData = SyncImageLoader::Load(path);

  Texture texture = Texture::New(TextureType::TEXTURE_2D, pixelData.GetPixelFormat(),
    pixelData.GetWidth(), pixelData.GetHeight());
  texture.Upload(pixelData);
  return texture;
}

TextLabel MakeTextLabel(const char* text, const Vector4& color, float pointSize, const Vector2& size)
{
  auto tl = TextLabel::New(text);
  CenterActor(tl);
  tl.SetProperty(Actor::Property::SIZE, size);
  tl.SetProperty(TextLabel::Property::POINT_SIZE, pointSize);
  tl.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
  tl.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
  tl.SetProperty(TextLabel::Property::MULTI_LINE, true);
  tl.SetProperty(TextLabel::Property::TEXT_COLOR, color);
  return tl;
}

}

/**
 * Demonstrates colour transition using flow maps and uv rotation / scaling.
 *
 * Flow maps are greyscale images where the value of the pixels signifies the
 * progress of the animation, which is added to an animated value which we
 * use to lerp between old and new colour.
 *
 * The colour of the content is used to scale the source / target colour, i.e.
 * white is affected most, dark is affected less.
 *
 * Controls:
 * - Double-tap middle (33%) of screen: transition using random flow map;
 * - Double-tap outside the middle of screen: transition using one of the
 *   effects (flow maps) mapped to the given segment of the screen;
 */
class ColorTransition: public ConnectionTracker
{
public:
  ColorTransition(Application& app)
  : mApp(app)
  {
    app.InitSignal().Connect(this, &ColorTransition::OnInit);
    app.TerminateSignal().Connect(this, &ColorTransition::OnTerminate);
  }

private:
  static void OnTransitionFinished(void* data)
  {
    auto& me = *static_cast<ColorTransition*>(data);
    me.OnTransitionFinished();
  }

  void OnInit(Application& app)
  {
    auto window = mApp.GetWindow();
    auto windowSize = Vector2{ window.GetSize() };

    // create content root
    Actor content = Actor::New();
    CenterActor(content);
    content.SetProperty(Actor::Property::SIZE, windowSize);
    window.Add(content);
    mColorTransitionContent = content;

    // create some example content
    TextLabel tlHello = MakeTextLabel("<b>HELLO</b>", Vector4::ONE, 60.f, windowSize);
    tlHello.SetProperty(TextLabel::Property::ENABLE_MARKUP, true);
    tlHello.SetProperty(Actor::Property::POSITION_Y, -60.f);
    content.Add(tlHello);

    TextLabel tlWorld = MakeTextLabel("<i>WORLD</i>", Vector4(1.f, 1.f, 1.f, .5f), 60.f, windowSize);
    tlWorld.SetProperty(TextLabel::Property::ENABLE_MARKUP, true);
    tlWorld.SetProperty(Actor::Property::POSITION_Y, 60.f);
    content.Add(tlWorld);

    ImageView ivIcon = ImageView::New(Application::GetResourcePath() + "images/application-icon-2.png");
    CenterActor(ivIcon);
    ivIcon.SetProperty(Actor::Property::POSITION_Y, 120.f);
    content.Add(ivIcon);

    // create main root
    Actor mainRoot = Actor::New();
    CenterActor(mainRoot);
    window.Add(mainRoot);
    mMainRoot = mainRoot;

    auto renderTasks = window.GetRenderTaskList();
    auto weakRenderTasks = WeakHandle<RenderTaskList>(renderTasks);
    auto controller = new ColorTransitionController(weakRenderTasks, content, window.GetRenderTaskList(), INITIAL_COLOR);
    mController.reset(controller);
    mainRoot.Add(mController->GetComposite());

    // add content unaffected by effect, to scene.
    TextLabel tlInstructions = MakeTextLabel("Double tap to change Color", Vector4(.5f, .5f, .5f, 1.f), 10.f, windowSize);
    tlInstructions.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "TOP");
    mainRoot.Add(tlInstructions);

    // hook up event handlers
    window.KeyEventSignal().Connect(this, &ColorTransition::OnKeyEvent);

    auto tapDetector = TapGestureDetector::New(2);
    tapDetector.DetectedSignal().Connect(this, &ColorTransition::OnDoubleTap);
    tapDetector.Attach(window.GetRootLayer());
    mDoubleTapDetector = tapDetector;

    mController->SetOnFinished(OnTransitionFinished, this);
  }

  void OnTerminate(Application& app)
  {
    auto window = mApp.GetWindow();
    mDoubleTapDetector.Detach(window.GetRootLayer());

    UnparentAndReset(mColorTransitionContent);
    UnparentAndReset(mMainRoot);

    mController.reset();
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if (event.GetState() == KeyEvent::DOWN)
    {
      if (IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
      {
        mApp.Quit();
      }
    }
  }

  void OnDoubleTap(Actor /*actor*/, const TapGesture& tap)
  {
    auto window = mApp.GetWindow();
    auto windowSize = Vector2{ window.GetSize() };
    Vector2 clip = tap.GetScreenPoint() / windowSize - Vector2::ONE * .5f; // [-.5, .5]
    if (clip.Length() < .333f * .5f)
    {
      LoadFlowMap(FLOW_MAPS[rand() % std::extent<decltype(FLOW_MAPS)>::value]);
    }
    else
    {
      float theta = fmodf((atan2(clip.y, clip.x) + M_PI) / (M_PI * 2.) + .75f, 1.f);
      int i = std::extent<decltype(FLOW_MAPS)>::value * theta;

      LoadFlowMap(FLOW_MAPS[i]);
    }
    RandomizeUvTransform();

    Vector3 color(rand() % 5, rand() % 5, rand() % 5);
    color /= 4.f;

    mDoubleTapDetector.Detach(mApp.GetWindow().GetRootLayer());
    mController->RequestTransition(TRANSITION_DURATION, color);
  }

  void LoadFlowMap(const char* const name)
  {
    DALI_ASSERT_DEBUG(name && "Flow map name must be given");
    if (mLastFlowMap != name)
    {
      std::string flowMapDir = Application::GetResourcePath() + "images/color-transition/";
      std::string flowMapPath = flowMapDir + name + ".png";
      auto flowMap = LoadTexture(flowMapPath);
      DALI_ASSERT_DEBUG(flowMap && "Failed to load flow map.");
      mController->SetFlowMap(flowMap);
    }
  }

  void RandomizeUvTransform()
  {
    mController->SetUvTransform((rand() % 12) * M_PI / 12., 1.f,
      (rand() % 12) * M_PI / 12., .5f);
  }

  void OnTransitionFinished()
  {
    mDoubleTapDetector.Attach(mApp.GetWindow().GetRootLayer());
  }

  Application& mApp;

  const char* mLastFlowMap = nullptr;

  Actor mColorTransitionContent;
  Actor mMainRoot;

  std::unique_ptr<ColorTransitionController> mController;

  TapGestureDetector mDoubleTapDetector;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  auto app = Application::New(&argc, &argv
#ifdef WIN32
    , ".//dali-toolkit-default-theme.json"
#endif
  );
  ColorTransition ct(app);
  app.MainLoop();
  return 0;
}
