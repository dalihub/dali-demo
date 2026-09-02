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
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali/integration-api/trace.h>

#include <cstdlib>
#include <random>
#include <vector>

// INTERNAL INCLUDES
#include "shared/utility.h"

using namespace Dali;
using namespace Dali::Toolkit;

// Test application that stresses the Update<->Event message queue.
// It is a load generator only: it prints nothing and exposes no metrics.
// Profile it externally (systrace/Perfetto, DALI_PERFORMANCE_LOG_LEVEL, etc.)
// on-device, once with the mutex-based queue and once with the lockless queue,
// and compare.
//
// It stresses both directions of the queue:
//  - Event -> Update: every tick, every actor gets several SetProperty() calls
//    (position/size/color), which is the dominant source of UpdateMessageQueue
//    traffic in real apps (animations, layouting, touch feedback, etc).
//  - Update -> Event: every actor has a PropertyNotification watching a
//    property that changes every tick, so NotificationManager is kept busy
//    relaying notifications back to the Event thread every frame.
//  - Actor lifecycle churn: a fraction of actors are destroyed and recreated
//    periodically, to exercise the message-based ownership handover paths
//    (OwnerContainer / MessageBase lifetime) rather than just steady-state
//    property writes.
//
// Command line arguments:
// -r NumberOfRows          (default 15)
// -c NumberOfColumns       (default 15)
// -i TickIntervalMs        (default 16, i.e. roughly one tick per frame at 60Hz)
// -t DurationSeconds       (default 60)
// -b BurstMultiplier       (default 1 = disabled; e.g. 8 means burst ticks send
//                           8x the per-actor property writes of a normal tick)
// -p BurstPeriodTicks      (default 30; a burst fires once every this-many ticks)
//
// Burst mode (-b > 1) makes the Event->Update producer bursty instead of
// steady-state: most ticks send the normal message volume, but every
// BurstPeriodTicks-th tick sends BurstMultiplier times as many. Steady traffic
// rarely fills a bounded ring; bursts are what actually drive the lockless ring
// to full and the mutex path into contention, so this is the mode that exercises
// the difference between the two implementations.
//
// Reproducibility: the burst schedule is driven by an integer tick counter (not
// wall-clock time), and the jitter RNG is fixed-seed, so a given set of args
// produces a byte-for-byte identical workload on every run - the whole point
// being that the ONLY thing that changes between an old-build run and a
// new-build run is the queue implementation, never the workload.

namespace
{
uint32_t gRows            = 15;
uint32_t gColumns         = 15;
uint32_t gTickInterval    = 16;
uint32_t gDuration        = 60;
uint32_t gBurstMultiplier = 1;  // 1 = disabled (steady state). >1 enables burst mode.
uint32_t gBurstPeriod     = 30; // A burst fires once every this-many ticks.

constexpr float    VIEW_MARGIN_RATE     = 0.15f;
constexpr uint32_t CHURN_INTERVAL_MS    = 1000;  // Replace a batch of actors this often.
constexpr float    CHURN_RATIO          = 0.05f; // Fraction of actors replaced per churn tick.
constexpr float    NOTIFY_STEP_FRACTION = 0.02f; // PropertyNotification StepCondition, relative to cell size.

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERF_MESSAGE_QUEUE_STRESS, true);

std::mt19937 gRandomEngine(12345u); // Fixed seed: keeps the workload reproducible run-to-run.

float RandomFloat(float low, float high)
{
  std::uniform_real_distribution<float> dist(low, high);
  return dist(gRandomEngine);
}

Vector4 RandomColor()
{
  return Vector4(RandomFloat(0.2f, 1.0f), RandomFloat(0.2f, 1.0f), RandomFloat(0.2f, 1.0f), 1.0f);
}

} // namespace

class MessageQueueStress : public ConnectionTracker
{
public:
  MessageQueueStress(Application& application)
  : mApplication(application),
    mRows(gRows),
    mColumns(gColumns)
  {
    mApplication.InitSignal().Connect(this, &MessageQueueStress::Create);
  }

  ~MessageQueueStress() = default;

  void Create(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Color::WHITE);
    mWindow.GetRootLayer().SetProperty(Layer::Property::DEPTH_TEST, false);

    auto positionSize = mWindow.GetPositionSize();
    mWindowSize       = Vector2(positionSize.width, positionSize.height);
    mCellSize         = Vector3(mWindowSize.x / mColumns, mWindowSize.y / mRows, 0.0f);

    mWindow.TouchEventSignal().Connect(this, &MessageQueueStress::OnTouch);
    mWindow.KeyEventSignal().Connect(this, &MessageQueueStress::OnKeyEvent);

    CreateGrid();

    mTickTimer = Timer::New(gTickInterval);
    mTickTimer.TickSignal().Connect(this, &MessageQueueStress::OnTick);
    mTickTimer.Start();

    mChurnTimer = Timer::New(CHURN_INTERVAL_MS);
    mChurnTimer.TickSignal().Connect(this, &MessageQueueStress::OnChurnTick);
    mChurnTimer.Start();

    if(gDuration > 0)
    {
      mQuitTimer = Timer::New(gDuration * 1000u);
      mQuitTimer.TickSignal().Connect(this, &MessageQueueStress::OnQuitTick);
      mQuitTimer.Start();
    }
  }

private:
  struct Cell
  {
    Control              view;
    PropertyNotification notification;
    uint32_t             row;
    uint32_t             column;
  };

  Vector3 CellPosition(uint32_t row, uint32_t column) const
  {
    return Vector3(mCellSize.x * column + mCellSize.x * VIEW_MARGIN_RATE * 0.5f,
                   mCellSize.y * row + mCellSize.y * VIEW_MARGIN_RATE * 0.5f,
                   0.0f);
  }

  Control CreateCellView(uint32_t row, uint32_t column)
  {
    Control view                         = Control::New(Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);
    view[Actor::Property::PARENT_ORIGIN] = ParentOrigin::TOP_LEFT;
    view[Actor::Property::PIVOT]         = Pivot::TOP_LEFT;
    view[Actor::Property::SIZE]          = Vector2(mCellSize.x * (1.0f - VIEW_MARGIN_RATE), mCellSize.y * (1.0f - VIEW_MARGIN_RATE));
    view[Actor::Property::POSITION]      = CellPosition(row, column);
    view[Control::Property::BACKGROUND]  = Property::Map().Add(Visual::Property::TYPE, (int)Visual::Type::COLOR).Add(ColorVisual::Property::MIX_COLOR, RandomColor());
    return view;
  }

  void CreateGrid()
  {
    mCells.reserve(mRows * mColumns);
    for(uint32_t row = 0; row < mRows; ++row)
    {
      for(uint32_t column = 0; column < mColumns; ++column)
      {
        Cell cell;
        cell.row    = row;
        cell.column = column;
        cell.view   = CreateCellView(row, column);
        mWindow.Add(cell.view);

        // Watch POSITION_X: it changes every tick in OnTick(), so this keeps
        // the Update -> Event notification path continuously busy.
        cell.notification = cell.view.AddPropertyNotification(Actor::Property::POSITION_X, StepCondition(mCellSize.x * NOTIFY_STEP_FRACTION, 0.0f));
        cell.notification.NotifySignal().Connect(this, &MessageQueueStress::OnPropertyNotify);

        mCells.push_back(cell);
      }
    }
  }

  bool OnTick()
  {
    DALI_TRACE_BEGIN(gTraceFilter, "DALI_MESSAGE_QUEUE_STRESS_TICK");

    // Deterministic burst schedule: every gBurstPeriod-th tick sends
    // gBurstMultiplier times the normal message volume. Counted in integer
    // ticks (not wall-clock) so the workload is identical run-to-run - the
    // only variable across an old-build vs new-build comparison must be the
    // queue implementation, never the traffic pattern.
    ++mTickCount;
    const bool     isBurstTick = (gBurstMultiplier > 1) && (mTickCount % gBurstPeriod == 0);
    const uint32_t repeats     = isBurstTick ? gBurstMultiplier : 1u;

    if(isBurstTick)
    {
      DALI_TRACE_BEGIN(gTraceFilter, "DALI_MESSAGE_QUEUE_STRESS_BURST");
    }

    // Every actor gets several SetProperty() calls per tick. Each one sends
    // a message from the Event thread into the update message queue, which
    // is the traffic this demo is meant to stress. On a burst tick the whole
    // set is repeated gBurstMultiplier times, so a single tick dumps a large
    // batch of messages into the queue at once - this is what drives the ring
    // toward full (new build) or the mutex into contention (old build).
    for(uint32_t r = 0; r < repeats; ++r)
    {
      for(auto& cell : mCells)
      {
        Vector3 basePosition = CellPosition(cell.row, cell.column);
        float   jitterX      = RandomFloat(-mCellSize.x * 0.1f, mCellSize.x * 0.1f);
        float   jitterY      = RandomFloat(-mCellSize.y * 0.1f, mCellSize.y * 0.1f);

        cell.view[Actor::Property::POSITION] = basePosition + Vector3(jitterX, jitterY, 0.0f);
        cell.view[Actor::Property::COLOR_MULTIPLIER]    = RandomColor();
        cell.view[Actor::Property::SIZE]     = Vector2(mCellSize.x, mCellSize.y) * (1.0f - VIEW_MARGIN_RATE) * RandomFloat(0.85f, 1.0f);
      }
    }

    if(isBurstTick)
    {
      DALI_TRACE_END(gTraceFilter, "DALI_MESSAGE_QUEUE_STRESS_BURST");
    }

    DALI_TRACE_END(gTraceFilter, "DALI_MESSAGE_QUEUE_STRESS_TICK");

    return true; // Keep ticking.
  }

  bool OnChurnTick()
  {
    DALI_TRACE_BEGIN(gTraceFilter, "DALI_MESSAGE_QUEUE_STRESS_CHURN");

    // Destroy and recreate a fraction of the grid each time. This exercises
    // the actor/message lifecycle (creation, destruction, ownership handover
    // through the queue) rather than just steady-state property writes.
    uint32_t churnCount = static_cast<uint32_t>(mCells.size() * CHURN_RATIO);
    for(uint32_t i = 0; i < churnCount && !mCells.empty(); ++i)
    {
      std::uniform_int_distribution<size_t> indexDist(0, mCells.size() - 1);
      size_t                                index = indexDist(gRandomEngine);

      Cell& cell = mCells[index];
      cell.notification.NotifySignal().Disconnect(this, &MessageQueueStress::OnPropertyNotify);
      cell.view.Unparent();

      cell.view = CreateCellView(cell.row, cell.column);
      mWindow.Add(cell.view);
      cell.notification = cell.view.AddPropertyNotification(Actor::Property::POSITION_X, StepCondition(mCellSize.x * NOTIFY_STEP_FRACTION, 0.0f));
      cell.notification.NotifySignal().Connect(this, &MessageQueueStress::OnPropertyNotify);
    }

    DALI_TRACE_END(gTraceFilter, "DALI_MESSAGE_QUEUE_STRESS_CHURN");

    return true; // Keep churning.
  }

  void OnPropertyNotify(PropertyNotification source)
  {
    // Deliberately cheap: the point is the round trip through
    // NotificationManager, not the work done in the handler. A no-op handler
    // risks being optimised away in spirit (if not literally), so touch a
    // property to make the callback observably do something.
    Actor target = Actor::DownCast(source.GetTarget());
    if(target)
    {
      Vector4 color                  = target[Actor::Property::COLOR_MULTIPLIER];
      color.a                        = RandomFloat(0.6f, 1.0f);
      target[Actor::Property::COLOR_MULTIPLIER] = color;
    }
  }

  bool OnQuitTick()
  {
    mApplication.Quit();
    return false;
  }

  void OnTouch(Window window, TouchEvent touch)
  {
    mApplication.Quit();
  }

  void OnKeyEvent(Window window, KeyEvent event)
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
  Window       mWindow;
  Vector2      mWindowSize;
  Vector3      mCellSize;

  const uint32_t mRows;
  const uint32_t mColumns;

  std::vector<Cell> mCells;

  Timer mTickTimer;
  Timer mChurnTimer;
  Timer mQuitTimer;

  uint64_t mTickCount{0}; ///< monotonic tick counter driving the deterministic burst schedule
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  for(int i = 1; i < argc; ++i)
  {
    std::string arg(argv[i]);
    if(arg.compare(0, 2, "-r") == 0)
    {
      gRows = static_cast<uint32_t>(atoi(arg.substr(2).c_str()));
    }
    else if(arg.compare(0, 2, "-c") == 0)
    {
      gColumns = static_cast<uint32_t>(atoi(arg.substr(2).c_str()));
    }
    else if(arg.compare(0, 2, "-i") == 0)
    {
      gTickInterval = static_cast<uint32_t>(atoi(arg.substr(2).c_str()));
    }
    else if(arg.compare(0, 2, "-t") == 0)
    {
      gDuration = static_cast<uint32_t>(atoi(arg.substr(2).c_str()));
    }
    else if(arg.compare(0, 2, "-b") == 0)
    {
      gBurstMultiplier = static_cast<uint32_t>(atoi(arg.substr(2).c_str()));
    }
    else if(arg.compare(0, 2, "-p") == 0)
    {
      gBurstPeriod = static_cast<uint32_t>(atoi(arg.substr(2).c_str()));
    }
  }

  if(gBurstPeriod == 0)
  {
    gBurstPeriod = 1; // avoid modulo-by-zero; period 1 means every tick bursts
  }

  MessageQueueStress test(application);
  application.MainLoop();

  return 0;
}
