/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/color-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* IMAGE_FILE(DEMO_IMAGE_DIR "gallery-medium-1.jpg");

const float BLUR_RADIUS_VALUE(15.0f);
const Vector2 BLUR_OFFSET_VALUE(0.05f, 0.05f);
const Vector2 BLUR_SIZE_VALUE(1.1f, 1.1f);
const Vector2 NO_BLUR_SIZE_VALUE(1.05f, 1.05f);
const float ANIMATION_DURATION(2.0f);

const Property::Value SHADOW{
  {Visual::Property::TYPE, Visual::COLOR},
  {Visual::Property::MIX_COLOR, Vector4(0.0f, 0.0f, 0.0f, 0.5f)},
  {Visual::Property::TRANSFORM, Property::Map{{Visual::Transform::Property::SIZE, NO_BLUR_SIZE_VALUE},
                                              {Visual::Transform::Property::ORIGIN, Align::CENTER},
                                              {Visual::Transform::Property::ANCHOR_POINT, Align::CENTER}}}};

} // namespace

// This example shows the blur radius property of the color visual and animates it.
//
class ColorVisualExample : public ConnectionTracker
{
public:
  ColorVisualExample(Application& application)
  : mApplication(application),
    mBlurEnabled(false)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ColorVisualExample::Create);
  }

  ~ColorVisualExample()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    mImageView = ImageView::New(IMAGE_FILE);
    mImageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mImageView.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
    mImageView.SetProperty(DevelControl::Property::SHADOW, SHADOW);

    window.Add(mImageView);

    // Respond to a click anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &ColorVisualExample::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &ColorVisualExample::OnKeyEvent);
  }

  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      Animation animation = Animation::New(ANIMATION_DURATION);

      if(!mBlurEnabled)
      {
        animation.AnimateTo(DevelControl::GetVisualProperty(mImageView, ImageView::Property::IMAGE, Visual::Property::MIX_COLOR), Vector3(1.0f, 0.0f, 0.0f));
        animation.AnimateTo(DevelControl::GetVisualProperty(mImageView, ImageView::Property::IMAGE, Visual::Property::OPACITY), 0.5f);
        animation.AnimateTo(DevelControl::GetVisualProperty(mImageView, ImageView::Property::IMAGE, DevelVisual::Property::CORNER_RADIUS), 30.0f);
        animation.AnimateTo(DevelControl::GetVisualProperty(mImageView, DevelControl::Property::SHADOW, ColorVisual::Property::MIX_COLOR), Vector3(0.0f, 0.0f, 1.0f));
        animation.AnimateTo(DevelControl::GetVisualProperty(mImageView, DevelControl::Property::SHADOW, DevelColorVisual::Property::BLUR_RADIUS), BLUR_RADIUS_VALUE);
        animation.AnimateTo(DevelControl::GetVisualProperty(mImageView, DevelControl::Property::SHADOW, Visual::Transform::Property::OFFSET), BLUR_OFFSET_VALUE);
        animation.AnimateTo(DevelControl::GetVisualProperty(mImageView, DevelControl::Property::SHADOW, Visual::Transform::Property::SIZE), BLUR_SIZE_VALUE);
      }
      else
      {
        animation.AnimateTo(DevelControl::GetVisualProperty(mImageView, ImageView::Property::IMAGE, Visual::Property::MIX_COLOR), Vector3(1.0f, 1.0f, 1.0f));
        animation.AnimateTo(DevelControl::GetVisualProperty(mImageView, ImageView::Property::IMAGE, Visual::Property::OPACITY), 1.0f);
        animation.AnimateTo(DevelControl::GetVisualProperty(mImageView, ImageView::Property::IMAGE, DevelVisual::Property::CORNER_RADIUS), 0.0f);
        animation.AnimateTo(DevelControl::GetVisualProperty(mImageView, DevelControl::Property::SHADOW, ColorVisual::Property::MIX_COLOR), Vector3(0.0f, 0.0f, 0.0f));
        animation.AnimateTo(DevelControl::GetVisualProperty(mImageView, DevelControl::Property::SHADOW, DevelColorVisual::Property::BLUR_RADIUS), 0.0f);
        animation.AnimateTo(DevelControl::GetVisualProperty(mImageView, DevelControl::Property::SHADOW, Visual::Transform::Property::OFFSET), Vector2::ZERO);
        animation.AnimateTo(DevelControl::GetVisualProperty(mImageView, DevelControl::Property::SHADOW, Visual::Transform::Property::SIZE), NO_BLUR_SIZE_VALUE);
      }
      animation.Play();

      mBlurEnabled = !mBlurEnabled;
    }
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
  ImageView    mImageView;
  bool         mBlurEnabled;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application        application = Application::New(&argc, &argv);
  ColorVisualExample test(application);
  application.MainLoop();
  return 0;
}
