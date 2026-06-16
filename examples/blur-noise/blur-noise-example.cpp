/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/key-event.h>

#include <algorithm>
#include <array>
#include <iomanip>
#include <sstream>

using namespace Dali;

namespace
{
constexpr uint32_t BLUR_RADIUS       = 40u;
constexpr float    BLUR_DOWNSCALE   = 0.25f;
constexpr float    IMAGE_SIZE       = 200.0f;
constexpr float    IMAGE_GAP_X      = 120.0f;
constexpr float    IMAGE_GAP_Y      = 90.0f;
constexpr float    BOARD_MOVE_X     = 180.0f;
constexpr float    BOARD_MOVE_Y     = 140.0f;
constexpr float    BOARD_MOVE_TIME  = 6.0f;
constexpr float    DITHER_STEP      = 5.0f / 255.0f;
constexpr float    DITHER_SCALE     = 0.2f;
constexpr float    INITIAL_DITHER   = 0.1f;
constexpr int      IMAGE_COLUMNS    = 6;
constexpr int      IMAGE_ROWS       = 4;

const std::array<Vector4, 4> BACKGROUND_COLORS =
  {{
    Vector4(0.05f, 0.07f, 0.09f, 1.0f),
    Vector4(0.08f, 0.10f, 0.14f, 1.0f),
    Vector4(0.04f, 0.06f, 0.10f, 1.0f),
    Vector4(0.10f, 0.08f, 0.06f, 1.0f),
  }};

const char* const IMAGE_PATHS[] = {
  DEMO_IMAGE_DIR "gallery-medium-1.jpg",
  DEMO_IMAGE_DIR "gallery-medium-2.jpg",
  DEMO_IMAGE_DIR "gallery-medium-3.jpg",
  DEMO_IMAGE_DIR "gallery-medium-4.jpg",
  DEMO_IMAGE_DIR "gallery-medium-5.jpg",
  DEMO_IMAGE_DIR "gallery-medium-6.jpg",
  DEMO_IMAGE_DIR "gallery-medium-7.jpg",
  DEMO_IMAGE_DIR "gallery-medium-8.jpg",
  DEMO_IMAGE_DIR "gallery-medium-9.jpg",
  DEMO_IMAGE_DIR "gallery-medium-10.jpg",
  DEMO_IMAGE_DIR "gallery-medium-11.jpg",
  DEMO_IMAGE_DIR "gallery-medium-12.jpg",
  DEMO_IMAGE_DIR "gallery-medium-13.jpg",
  DEMO_IMAGE_DIR "gallery-medium-14.jpg",
  DEMO_IMAGE_DIR "gallery-medium-15.jpg",
  DEMO_IMAGE_DIR "gallery-medium-16.jpg",
  DEMO_IMAGE_DIR "gallery-medium-17.jpg",
  DEMO_IMAGE_DIR "gallery-medium-18.jpg",
  DEMO_IMAGE_DIR "gallery-medium-19.jpg",
  DEMO_IMAGE_DIR "gallery-medium-20.jpg",
  DEMO_IMAGE_DIR "gallery-medium-21.jpg",
  DEMO_IMAGE_DIR "gallery-medium-22.jpg",
  DEMO_IMAGE_DIR "gallery-medium-23.jpg",
  DEMO_IMAGE_DIR "gallery-medium-24.jpg",
};

constexpr unsigned int NUMBER_OF_IMAGES = sizeof(IMAGE_PATHS) / sizeof(IMAGE_PATHS[0]);
} // namespace

class BlurNoiseController : public ConnectionTracker
{
public:
  explicit BlurNoiseController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &BlurNoiseController::Create);
  }

  void Create(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(BACKGROUND_COLORS[0]);

    mImageLayer = Layer::New();
    mImageLayer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mImageLayer.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    mImageLayer.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    mWindow.Add(mImageLayer);

    mBlurLayer = Layer::New();
    mBlurLayer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mBlurLayer.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    mBlurLayer.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    mWindow.Add(mBlurLayer);

    CreateMovingImageBoard();
    CreateFullscreenBlurPane();
    CreateStatusLabel();

    mWindow.KeyEventSignal().Connect(this, &BlurNoiseController::OnKeyEvent);
    mWindow.ResizeSignal().Connect(this, &BlurNoiseController::OnWindowResized);
  }

private:
  void CreateMovingImageBoard()
  {
    mImageBoard = Actor::New();
    mImageBoard.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mImageBoard.SetProperty(Actor::Property::POSITION, Vector3::ZERO);
    mImageLayer.Add(mImageBoard);

    const float strideX = IMAGE_SIZE + IMAGE_GAP_X;
    const float strideY = IMAGE_SIZE + IMAGE_GAP_Y;
    const float startX  = -strideX * static_cast<float>(IMAGE_COLUMNS - 1) * 0.5f;
    const float startY  = -strideY * static_cast<float>(IMAGE_ROWS - 1) * 0.5f;

    unsigned int imageIndex = 0u;
    for(int row = 0; row < IMAGE_ROWS; ++row)
    {
      for(int column = 0; column < IMAGE_COLUMNS; ++column)
      {
        Toolkit::ImageView image = Toolkit::ImageView::New(IMAGE_PATHS[imageIndex]);
        image.SetProperty(Actor::Property::SIZE, Vector2(IMAGE_SIZE, IMAGE_SIZE));
        image.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
        image.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);
        image.SetProperty(Actor::Property::POSITION, Vector3(startX + strideX * column, startY + strideY * row, 0.0f));
        mImageBoard.Add(image);

        imageIndex = (imageIndex + 1u) % NUMBER_OF_IMAGES;
      }
    }
  }

  void CreateFullscreenBlurPane()
  {
    Vector2 windowSize = mWindow.GetSize();

    mBlurPane = Toolkit::Control::New();
    mBlurPane.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mBlurPane.SetProperty(Actor::Property::SIZE, windowSize);
    mBlurPane.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    mBlurPane.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    mBlurLayer.Add(mBlurPane);

    mBackgroundBlur = Toolkit::BackgroundBlurEffect::New(BLUR_RADIUS);
    mBackgroundBlur.SetBlurDownscaleFactor(BLUR_DOWNSCALE);
    mBackgroundBlur.SetDitherNoiseStrength(mDitherNoiseStrength);
    mBackgroundBlur.SetBlurOnce(false);
    mBlurPane.SetRenderEffect(mBackgroundBlur);
  }

  void CreateStatusLabel()
  {
    mStatusLabel = Toolkit::TextLabel::New();
    mStatusLabel.SetProperty(Actor::Property::SIZE, Vector2(260.0f, 40.0f));
    mStatusLabel.SetProperty(Toolkit::TextLabel::Property::POINT_SIZE, 14.0f);
    mStatusLabel.SetProperty(Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE);
    mStatusLabel.SetProperty(Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "END");
    mStatusLabel.SetProperty(Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
    mStatusLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_RIGHT);
    mStatusLabel.SetProperty(Actor::Property::PIVOT, Pivot::TOP_RIGHT);
    mStatusLabel.SetProperty(Actor::Property::POSITION, Vector3(-24.0f, 24.0f, 0.0f));
    mBlurLayer.Add(mStatusLabel);

    UpdateStatusLabel();
  }

  void UpdateStatusLabel()
  {
    std::ostringstream oss;
    oss << "Noise " << static_cast<int>(mDitherNoiseStrength * 255.0f + 0.5f)
        << "/255  " << std::fixed << std::setprecision(3) << mDitherNoiseStrength
        << " -> " << std::setprecision(4) << mDitherNoiseStrength * DITHER_SCALE;
    mStatusLabel.SetProperty(Toolkit::TextLabel::Property::TEXT, oss.str().c_str());
  }

  void StartBoardAnimation()
  {
    if(mBoardAnimation)
    {
      mBoardAnimation.Stop();
      mBoardAnimation.Clear();
    }

    KeyFrames positions = KeyFrames::New();
    positions.Add(0.00f, Vector3(-BOARD_MOVE_X, -BOARD_MOVE_Y, 0.0f));
    positions.Add(0.25f, Vector3(BOARD_MOVE_X, -BOARD_MOVE_Y, 0.0f));
    positions.Add(0.50f, Vector3(BOARD_MOVE_X, BOARD_MOVE_Y, 0.0f));
    positions.Add(0.75f, Vector3(-BOARD_MOVE_X, BOARD_MOVE_Y, 0.0f));
    positions.Add(1.00f, Vector3(-BOARD_MOVE_X, -BOARD_MOVE_Y, 0.0f));

    mBoardAnimation = Animation::New(BOARD_MOVE_TIME / mSpeedMultiplier);
    mBoardAnimation.AnimateBetween(Property(mImageBoard, Actor::Property::POSITION), positions, AlphaFunction::LINEAR);
    mBoardAnimation.SetLooping(true);
    mBoardAnimation.Play();
  }

  void OnWindowResized(Window windowHandle, Window::WindowSize windowSize)
  {
    mBlurPane.SetProperty(Actor::Property::SIZE, Vector2(windowSize.GetWidth(), windowSize.GetHeight()));
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(event.GetKeyName() == "1")
    {
      mBlurVisible = !mBlurVisible;
      mBlurPane.SetProperty(Actor::Property::VISIBLE, mBlurVisible);
    }
    else if(event.GetKeyName() == "2")
    {
      mSpeedMultiplier = mSpeedMultiplier < 4.0f ? mSpeedMultiplier * 2.0f : 0.5f;
      if(mAnimationEnabled)
      {
        StartBoardAnimation();
      }
    }
    else if(event.GetKeyName() == "3")
    {
      if(mCurrentBlurRadius > 10u)
      {
        mCurrentBlurRadius -= 10u;
        mBackgroundBlur.SetBlurRadius(mCurrentBlurRadius);
      }
    }
    else if(event.GetKeyName() == "4")
    {
      mCurrentBlurRadius += 10u;
      mBackgroundBlur.SetBlurRadius(mCurrentBlurRadius);
    }
    else if(event.GetKeyName() == "5")
    {
      mBackgroundColorIndex = (mBackgroundColorIndex + 1u) % BACKGROUND_COLORS.size();
      mWindow.SetBackgroundColor(BACKGROUND_COLORS[mBackgroundColorIndex]);
    }
    else if(event.GetKeyName() == "6")
    {
      mAnimationEnabled = !mAnimationEnabled;
      if(mAnimationEnabled)
      {
        StartBoardAnimation();
      }
      else if(mBoardAnimation)
      {
        mBoardAnimation.Stop();
      }
    }
    else if(event.GetKeyName() == "7")
    {
      mDitherNoiseStrength = std::max(0.0f, mDitherNoiseStrength - DITHER_STEP);
      mBackgroundBlur.SetDitherNoiseStrength(mDitherNoiseStrength);
      UpdateStatusLabel();
    }
    else if(event.GetKeyName() == "8")
    {
      mDitherNoiseStrength = std::min(1.0f, mDitherNoiseStrength + DITHER_STEP);
      mBackgroundBlur.SetDitherNoiseStrength(mDitherNoiseStrength);
      UpdateStatusLabel();
    }
  }

private:
  Application& mApplication;

  Window mWindow;
  Layer  mImageLayer;
  Layer  mBlurLayer;
  Actor  mImageBoard;

  Toolkit::Control              mBlurPane;
  Toolkit::TextLabel            mStatusLabel;
  Toolkit::BackgroundBlurEffect mBackgroundBlur;
  Animation                     mBoardAnimation;

  uint32_t mCurrentBlurRadius{BLUR_RADIUS};
  uint32_t mBackgroundColorIndex{0u};
  float    mSpeedMultiplier{1.0f};
  float    mDitherNoiseStrength{INITIAL_DITHER};
  bool     mBlurVisible{true};
  bool     mAnimationEnabled{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  BlurNoiseController controller(application);
  application.MainLoop();
  return 0;
}
