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

#include <algorithm>

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/gaussian-blur-view/gaussian-blur-view.h>

using namespace Dali;
using Dali::Toolkit::GaussianBlurView;
using Dali::Toolkit::TextLabel;

namespace
{
const char* const BACKGROUND_IMAGE(DEMO_IMAGE_DIR "lake_front.jpg");
const float       BACKGROUND_IMAGE_WIDTH = 2048.0f;

} // namespace

/**
 * This example shows a scrolling background image which can be blurred (on/off) by tapping the screen
 */
class GaussianBlurViewExample : public ConnectionTracker
{
public:
  GaussianBlurViewExample(Application& application)
  : mApplication(application),
    mExcessWidth(0.0f),
    mStrength(1.0f),
    mActivate(false)
  {
    mApplication.InitSignal().Connect(this, &GaussianBlurViewExample::Create);
  }

  ~GaussianBlurViewExample() = default;

private:
  void Create(Application& application)
  {
    auto    window     = application.GetWindow();
    Vector2 windowSize = window.GetSize();
    window.KeyEventSignal().Connect(this, &GaussianBlurViewExample::OnKeyEvent);

    mImageView = Toolkit::ImageView::New(BACKGROUND_IMAGE);
    mImageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mImageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);

    float excessWidth = std::max(0.0f, (BACKGROUND_IMAGE_WIDTH - windowSize.width) * 0.5f);

    if(excessWidth > 0.0f)
    {
      // Move background image to show GaussianBlurView activity

      float pixelsPerSecond = 10.0f;
      float duration        = excessWidth / pixelsPerSecond;
      float qDuration       = duration * 0.25f;

      mAnimation = Animation::New(duration);
      mAnimation.AnimateTo(Property(mImageView, Actor::Property::POSITION_X), excessWidth, TimePeriod(0.0f, qDuration));
      mAnimation.AnimateTo(Property(mImageView, Actor::Property::POSITION_X), 0.0f, TimePeriod(qDuration, qDuration));
      mAnimation.AnimateTo(Property(mImageView, Actor::Property::POSITION_X), -excessWidth, TimePeriod(2.0f * qDuration, qDuration));
      mAnimation.AnimateTo(Property(mImageView, Actor::Property::POSITION_X), 0.0f, TimePeriod(3.0f * qDuration, qDuration));

      mAnimation.SetLooping(true);
      mAnimation.Play();
    }

    Layer onTop = Layer::New();
    onTop.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    onTop.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    onTop.SetProperty(Actor::Property::SIZE, windowSize);
    window.Add(onTop);
    onTop.RaiseToTop();

    mOnLabel = TextLabel::New("Blur ON");
    mOnLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mOnLabel.SetProperty(TextLabel::Property::TEXT_COLOR, Color::GREEN);
    mOnLabel.SetProperty(Actor::Property::VISIBLE, false);
    onTop.Add(mOnLabel);

    mOffLabel = TextLabel::New("Blur OFF");
    mOffLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mOffLabel.SetProperty(TextLabel::Property::TEXT_COLOR, Color::WHITE);
    mOffLabel.SetProperty(Actor::Property::VISIBLE, true);
    onTop.Add(mOffLabel);

    mGaussianBlurView = GaussianBlurView::New(30, 8.0f, Pixel::RGBA8888, 0.5f, 0.5f, false);
    mGaussianBlurView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mGaussianBlurView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mGaussianBlurView.SetProperty(Actor::Property::SIZE, windowSize);
    window.Add(mGaussianBlurView);

    mGaussianBlurView.Add(mImageView);
    mGaussianBlurView.SetProperty(mGaussianBlurView.GetBlurStrengthPropertyIndex(), mStrength);

    window.GetRootLayer().TouchedSignal().Connect(this, &GaussianBlurViewExample::OnTouch);
  }

  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    const PointState::Type state = touch.GetState(0);

    if(PointState::DOWN == state)
    {
      ToggleBlurState();
    }

    return true;
  }

  void ToggleBlurState()
  {
    if(!mActivate)
    {
      mActivate = true;
      mGaussianBlurView.Activate();

      mOnLabel.SetProperty(Actor::Property::VISIBLE, true);
      mOffLabel.SetProperty(Actor::Property::VISIBLE, false);
    }
    else
    {
      mActivate = false;
      mGaussianBlurView.Deactivate();

      mOnLabel.SetProperty(Actor::Property::VISIBLE, false);
      mOffLabel.SetProperty(Actor::Property::VISIBLE, true);
    }
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
      else if(!event.GetKeyName().compare("1"))
      {
        ToggleBlurState();
      }
    }
  }

private:
  Application& mApplication;

  Toolkit::ImageView mImageView;

  Animation mAnimation;

  TextLabel mOnLabel;
  TextLabel mOffLabel;

  GaussianBlurView mGaussianBlurView;

  float mExcessWidth;
  float mStrength;

  bool mActivate;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  GaussianBlurViewExample test(application);

  application.MainLoop();

  return 0;
}
