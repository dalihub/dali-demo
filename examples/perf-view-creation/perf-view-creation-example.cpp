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

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visuals/color-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>

#include <algorithm> ///< for std::sort
#include <chrono>
#include <iostream>
#include <list>
#include <thread>

// INTERNAL INCLUDES
#include "shared/utility.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

namespace
{
enum class ControlTestType
{
  // clang-format off
  COLOR = 0,            ///< Test with simple color
  IMAGE,                ///< Test with simple image
  TEXT,                 ///< Test with simple text label
  ROUNDED_COLOR,        ///< Test with rounded color
  BORDER_COLOR,         ///< Test with borderline color
  ROUNDED_BORDER_COLOR, ///< Test with rounded borderline color
  BLUR_COLOR,           ///< Test with blur color
  ROUNDED_BLUR_COLOR,   ///< Test with rounded blur color
  TYPE_MAX,
  // clang-format on
};

const char* TestTypeString(ControlTestType type)
{
  // clang-format off
  switch(type)
  {
    case ControlTestType::COLOR:               return "COLOR";
    case ControlTestType::IMAGE:               return "IMAGE";
    case ControlTestType::TEXT:                return "TEXT";
    case ControlTestType::ROUNDED_COLOR:       return "ROUNDED COLOR";
    case ControlTestType::BORDER_COLOR:        return "BORDER COLOR";
    case ControlTestType::ROUNDED_BORDER_COLOR:return "ROUNDED BORDER COLOR";
    case ControlTestType::BLUR_COLOR:          return "BLUR COLOR";
    case ControlTestType::ROUNDED_BLUR_COLOR:  return "ROUNDED BLUR COLOR";
    default:                                   return "UNKNOWN";
  }
  // clang-format on
}

// NOTE : Due to the image load is bottleneck on target, we just use single small image.
const char* IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "gallery-small-1.jpg",
};

constexpr uint32_t NUM_IMAGES = sizeof(IMAGE_PATH) / sizeof(char*);

constexpr uint32_t ROWS_COUNT(40);
constexpr uint32_t COLUMNS_COUNT(40);
constexpr uint32_t TOTAL_COLUMNS_COUNT(80);
constexpr uint32_t DURATION_PER_COLUMNS(50); // miliseconds.
// Increase animation time cause OnTick time can be delayed.
constexpr uint32_t DURATION_OF_ANIMATION(DURATION_PER_COLUMNS*(COLUMNS_COUNT * 4 / 3)); // miliseconds.

// We should render same type of views in some timing.
static_assert(COLUMNS_COUNT * 2 <= TOTAL_COLUMNS_COUNT);

constexpr float VIEW_MARGIN_RATE = 0.2f;

// copy from dali-adaptor time-service.cpp
void GetNanoseconds(uint64_t& timeInNanoseconds)
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch = std::chrono::steady_clock::now().time_since_epoch();

  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);

  timeInNanoseconds = static_cast<uint64_t>(duration.count());
}

Control CreateColor()
{
  Control bgView = Control::New(Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);
  bgView.SetBackgroundColor(Color::YELLOW);
  return bgView;
}

Control CreateImage(uint32_t& imageCount)
{
  Control bgView = ImageView::New(Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS, IMAGE_PATH[imageCount++ % NUM_IMAGES]);
  return bgView;
}

Control CreateTextLabel()
{
  Control bgView = TextLabel::New(Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS, "Hello, World!");
  return bgView;
}

Control CreateRoundedColor()
{
  Control bgView = Control::New(Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);

  Property::Map map;
  map[Visual::Property::TYPE]                      = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR]            = Color::YELLOW;
  map[DevelVisual::Property::CORNER_RADIUS]        = 0.5f;
  map[DevelVisual::Property::CORNER_RADIUS_POLICY] = Visual::Transform::Policy::RELATIVE;

  bgView[Control::Property::BACKGROUND] = map;

  return bgView;
}

Control CreateBorderColor(const float& requiredBorderlineWidth)
{
  Control bgView = Control::New(Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);

  Property::Map map;
  map[Visual::Property::TYPE]                  = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR]        = Color::YELLOW;
  map[DevelVisual::Property::BORDERLINE_WIDTH] = requiredBorderlineWidth;
  map[DevelVisual::Property::BORDERLINE_COLOR] = Color::RED;

  bgView[Control::Property::BACKGROUND] = map;

  return bgView;
}

Control CreateRoundedBorderColor(const float& requiredBorderlineWidth)
{
  Control bgView = Control::New(Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);

  Property::Map map;
  map[Visual::Property::TYPE]                      = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR]            = Color::YELLOW;
  map[DevelVisual::Property::CORNER_RADIUS]        = 0.5f;
  map[DevelVisual::Property::CORNER_RADIUS_POLICY] = Visual::Transform::Policy::RELATIVE;
  map[DevelVisual::Property::BORDERLINE_WIDTH]     = requiredBorderlineWidth;
  map[DevelVisual::Property::BORDERLINE_COLOR]     = Color::RED;

  bgView[Control::Property::BACKGROUND] = map;

  return bgView;
}

Control CreateBlurColor(const float& requiredBlurRadius)
{
  Control bgView = Control::New(Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);

  Property::Map map;
  map[Visual::Property::TYPE]                  = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR]        = Color::YELLOW;
  map[DevelColorVisual::Property::BLUR_RADIUS] = requiredBlurRadius;

  bgView[Control::Property::BACKGROUND] = map;

  return bgView;
}

Control CreateRoundedBlurColor(const float& requiredBlurRadius)
{
  Control bgView = Control::New(Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);

  Property::Map map;
  map[Visual::Property::TYPE]                      = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR]            = Color::YELLOW;
  map[DevelVisual::Property::CORNER_RADIUS]        = 0.5f;
  map[DevelVisual::Property::CORNER_RADIUS_POLICY] = Visual::Transform::Policy::RELATIVE;
  map[DevelColorVisual::Property::BLUR_RADIUS]     = requiredBlurRadius;

  bgView[Control::Property::BACKGROUND] = map;

  return bgView;
}

/**
 * @brief Statistic container that we can get average / sum / min/ max.
 *
 * @tparam T Type of data. T should define add, compare, div-by-int, numeric_limits<T>::min and max
 */
template<typename T>
struct Statistic
{
  static constexpr double trimRate = 0.34;

  std::vector<T> v;
  int            vcnt;
  T              vsum;
  T              vmax;
  T              vmin;
  Statistic()
  {
    Clear();
  }

  void Clear()
  {
    v.clear();
    vcnt = 0;
    vsum = 0;
    vmax = std::numeric_limits<T>::min();
    vmin = std::numeric_limits<T>::max();
  }

  void Add(T x)
  {
    v.emplace_back(x);
    vsum += x;
    vcnt++;
    vmax = std::max(vmax, x);
    vmin = std::min(vmin, x);
  }

  double GetAverage()
  {
    if(vcnt == 0) return 0.0;
    return static_cast<double>(vsum) / vcnt;
  }

  double GetTrimedAverage()
  {
    if(vcnt == 0) return 0.0;
    std::sort(v.begin(), v.end());
    T   trimVsum   = 0;
    int removedCnt = static_cast<int>(vcnt * trimRate * 0.5); // floor
    int trimVcnt   = vcnt - removedCnt * 2;
    if(trimVcnt == 0)
    {
      trimVcnt += 2;
      removedCnt--;
    }
    for(int i = removedCnt; i < vcnt - removedCnt; i++)
    {
      trimVsum += v[i];
    }

    return static_cast<double>(trimVsum) / trimVcnt;
  }
};

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERF_VIEW_CREATION_SAMPLE, true);

} // namespace

/**
 * Test application to compare performance between various type of Views creation time & manually created Renderers
 */
class PerfViewCreation : public ConnectionTracker
{
public:
  PerfViewCreation(Application& application)
  : mApplication(application),
    mRowsCount(ROWS_COUNT),
    mColumnsCount(COLUMNS_COUNT),
    mTotalColumnsCount(TOTAL_COLUMNS_COUNT),
    mDurationPerColumns(DURATION_PER_COLUMNS),
    mDurationOfAnimation(DURATION_OF_ANIMATION),
    mTestType(ControlTestType::COLOR)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &PerfViewCreation::Create);
  }

  ~PerfViewCreation() = default;

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    GetNanoseconds(mAppStartTime);

    // Get a handle to the window
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Color::WHITE);
    mWindowSize = mWindow.GetSize();

    mWindow.GetRootLayer().SetProperty(Layer::Property::DEPTH_TEST, false);

    mSize = Vector3(mWindowSize.x / mColumnsCount, mWindowSize.y / mRowsCount, 0.0f);

    Timer timer = Timer::New(mDurationPerColumns);
    timer.TickSignal().Connect(this, &PerfViewCreation::OnTick);
    mTimerList.push_back(timer);

    mCreationStatistic.Clear();

    mCreateCount = 0;
    mDeleteCount = 0;
    mImageCount  = 0;

    timer.Start();

    // Respond to key events
    mWindow.KeyEventSignal().Connect(this, &PerfViewCreation::OnKeyEvent);
  }

  bool OnTick()
  {
    CreateColumnView();
    if(mCreateCount < mColumnsCount)
    {
      // Start next phase.
      Timer timer = Timer::New(mDurationPerColumns);
      timer.TickSignal().Connect(this, &PerfViewCreation::OnTick);
      mTimerList.push_back(timer);

      timer.Start();
    }
    return false;
  }
  void CreateColumnView()
  {
    uint64_t startTime;
    uint64_t endTime;

    GetNanoseconds(startTime);
    DALI_TRACE_BEGIN(gTraceFilter, "DALI_SAMPLE_PERF_VIEW_CREATION");

    Control columnView = Control::New(Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);
    columnView.SetBackgroundColor(Color::BLUE);
    columnView[Actor::Property::PARENT_ORIGIN] = ParentOrigin::TOP_LEFT;
    columnView[Actor::Property::ANCHOR_POINT]  = AnchorPoint::TOP_LEFT;
    columnView[Actor::Property::SIZE]          = Vector2(mSize.x, (float)mWindowSize.y);
    columnView[Actor::Property::POSITION]      = Vector2(mSize.x * (mCreateCount % mColumnsCount), -(float)mWindowSize.y);
    for(uint32_t i = 0; i < mRowsCount; ++i)
    {
      Control bgView;
      switch(mTestType)
      {
        case ControlTestType::COLOR:
        default:
        {
          bgView = CreateColor();
          break;
        }
        case ControlTestType::IMAGE:
        {
          bgView = CreateImage(mImageCount);
          break;
        }
        case ControlTestType::TEXT:
        {
          bgView = CreateTextLabel();
          break;
        }
        case ControlTestType::ROUNDED_COLOR:
        {
          bgView = CreateRoundedColor();
          break;
        }
        case ControlTestType::BORDER_COLOR:
        {
          bgView = CreateBorderColor(std::min(mSize.x, mSize.y) * VIEW_MARGIN_RATE);
          break;
        }
        case ControlTestType::ROUNDED_BORDER_COLOR:
        {
          bgView = CreateRoundedBorderColor(std::min(mSize.x, mSize.y) * VIEW_MARGIN_RATE);
          break;
        }
        case ControlTestType::BLUR_COLOR:
        {
          bgView = CreateBlurColor(std::min(mSize.x, mSize.y) * VIEW_MARGIN_RATE * 0.5f);
          break;
        }
        case ControlTestType::ROUNDED_BLUR_COLOR:
        {
          bgView = CreateRoundedBlurColor(std::min(mSize.x, mSize.y) * VIEW_MARGIN_RATE * 0.5f);
          break;
        }
      }

      bgView[Actor::Property::PARENT_ORIGIN] = ParentOrigin::TOP_LEFT;
      bgView[Actor::Property::ANCHOR_POINT]  = AnchorPoint::TOP_LEFT;
      bgView[Actor::Property::SIZE]          = Vector2(mSize.x * (1.0f - VIEW_MARGIN_RATE), mSize.y * (1.0f - VIEW_MARGIN_RATE));
      bgView[Actor::Property::POSITION]      = Vector2(mSize.x * VIEW_MARGIN_RATE * 0.5f, mSize.y * VIEW_MARGIN_RATE * 0.5f + mSize.y * i);
      columnView.Add(bgView);
    }

    mWindow.GetRootLayer().Add(columnView);
    mCreatingControlList.push_back(columnView);

    // Add appearing animation
    Animation appearingAnimation = Animation::New(mDurationOfAnimation * 0.001f);
    appearingAnimation.AnimateTo(Property(columnView, Actor::Property::POSITION_Y), 0.0f);
    appearingAnimation.FinishedSignal().Connect(this, &PerfViewCreation::OnAppearAnimationFinished);
    appearingAnimation.Play();

    mCreatingAnimationList.push_back(appearingAnimation);

    GetNanoseconds(endTime);

    DALI_TRACE_END(gTraceFilter, "DALI_SAMPLE_PERF_VIEW_CREATION");

    // Append duration of creation time.
    mCreationStatistic.Add((endTime - startTime) / 1000000.0);

    mCreateCount++;

    if(mCreateCount % mTotalColumnsCount == 0)
    {
      DALI_LOG_ERROR("Average of creation %d DALI(%s) : %.6lf ms\n", mRowsCount, TestTypeString(mTestType), mCreationStatistic.GetTrimedAverage());
      mCreationStatistic.Clear();
      mTestType = static_cast<ControlTestType>((static_cast<int>(mTestType) + 1) % static_cast<int>(ControlTestType::TYPE_MAX));
    }
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

  void OnAppearAnimationFinished(Animation& animation)
  {
    // We can assume that front of mControlList must be disappearing.
    auto currentControl = mCreatingControlList.front();
    mCreatingControlList.pop_front();

    // Dereference timer safety
    if(!mTimerList.empty())
    {
      mTimerList.pop_front();
    }

    // Dereference animation safety
    mCreatingAnimationList.pop_back();

    mRemovingControlList.push_back(currentControl);

    if(mCreateCount < mTotalColumnsCount * (static_cast<int>(ControlTestType::TYPE_MAX)))
    {
      CreateColumnView();
    }

    // Add disappearing animation
    Animation disappearingAnimation = Animation::New(mDurationOfAnimation * 0.001f);
    disappearingAnimation.AnimateTo(Property(currentControl, Actor::Property::POSITION_Y), (float)mWindowSize.y);
    disappearingAnimation.FinishedSignal().Connect(this, &PerfViewCreation::OnDisappearAnimationFinished);
    disappearingAnimation.Play();

    mRemovingAnimationList.push_back(disappearingAnimation);
  }
  void OnDisappearAnimationFinished(Animation& animation)
  {
    // We can assume that front of mControlList must be deleted.
    mRemovingControlList.front().Unparent();
    mRemovingControlList.pop_front();

    // Dereference animation safety
    mRemovingAnimationList.pop_back();

    mDeleteCount++;

    // If all controls are deleted, quit this application. byebye~
    if(mDeleteCount == mTotalColumnsCount * (static_cast<int>(ControlTestType::TYPE_MAX)))
    {
      GetNanoseconds(mAppEndTime);

      DALI_LOG_ERROR("Duration of all app running time : %.6lf ms\n", (mAppEndTime - mAppStartTime) / 1000000.0);
      mApplication.Quit();
    }
  }

private:
  Application& mApplication;
  Window       mWindow;
  Vector2      mWindowSize;

  std::list<Control>   mCreatingControlList;
  std::list<Control>   mRemovingControlList;
  std::list<Animation> mCreatingAnimationList;
  std::list<Animation> mRemovingAnimationList;
  std::list<Timer>     mTimerList;

  Vector3 mSize;

  const uint32_t mRowsCount;
  const uint32_t mColumnsCount;
  const uint32_t mTotalColumnsCount;

  const uint32_t mDurationPerColumns;
  const uint32_t mDurationOfAnimation;

  ControlTestType mTestType;

  uint32_t mCreateCount = 0;
  uint32_t mDeleteCount = 0;
  uint32_t mImageCount  = 0;

  uint64_t mAppStartTime = 0;
  uint64_t mAppEndTime   = 0;

  Statistic<double> mCreationStatistic;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  PerfViewCreation test(application);
  application.MainLoop();

  return 0;
}
