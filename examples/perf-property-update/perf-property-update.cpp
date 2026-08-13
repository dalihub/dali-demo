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
#include <dali/integration-api/string-utils.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/update/frame-callback-interface.h>

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Toolkit;

using Dali::Integration::ToPropertyValue;

// Stress test for property updates.
//
// It register many custom properties across many actors and updates them
// every tick, with periodic bursts of extra actors to keep the update volume
// varying throughout the run.
//
// FPS can be read from DALI_FPS_TRACKING in the log, or the on-screen/stdout
// readout (but just as a reference). Let it settle for a few seconds before
// recording.
//
// Command line arguments:
//
// -a NumberOfActors      (default 1500, steady-state load)
// -k PropertiesPerActor  (default 8, max 32)
// -i TickIntervalMs      (default 16, roughly one tick per frame at 60Hz)
// -t DurationSeconds     (default 60, 0 = run until touch/key/quit; gates by
//                         an equivalent TICK COUNT, not wall-clock time)
// -c BurstActors         (default 1500, extra actors per burst; 0 disables churn)
// -b BurstPeriodTicks    (default 30, ticks between the start of each burst)

namespace
{
uint32_t gActors           = 1500;
uint32_t gProperties       = 8;
uint32_t gTickInterval     = 16;
uint32_t gDuration         = 60;
uint32_t gBurstActors      = 1500;
uint32_t gBurstPeriodTicks = 30;

constexpr uint32_t REPORT_INTERVAL_MS       = 1000;
constexpr uint32_t MAX_PROPERTIES_PER_ACTOR = 32;

// How long a burst's extra actors stay live and ticking before being torn
// down again. Long enough for their property sets to actually land in a
// frame's message volume; short enough to cycle many times within a run.
constexpr uint32_t BURST_DURATION_TICKS = 10;

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERF_PROPERTY_UPDATE, true);

// Fixed names rather than generated strings: keeps registration allocation-free
// of std::string churn and makes the uniform names stable for inspection.
const char* const PROPERTY_NAMES[MAX_PROPERTIES_PER_ACTOR] =
  {
    "uParam00", "uParam01", "uParam02", "uParam03", "uParam04", "uParam05", "uParam06", "uParam07", "uParam08", "uParam09", "uParam10", "uParam11", "uParam12", "uParam13", "uParam14", "uParam15", "uParam16", "uParam17", "uParam18", "uParam19", "uParam20", "uParam21", "uParam22", "uParam23", "uParam24", "uParam25", "uParam26", "uParam27", "uParam28", "uParam29", "uParam30", "uParam31"};

} // namespace

/**
 * Counts genuine rendered frames on the update thread.
 *
 * A wall-clock timer on the event thread cannot do this: under heavy load the
 * event thread can fall behind, so timer ticks will stop corresponding to frames.
 * FrameCallbackInterface::Update() is invoked once per update/render cycle, which
 * is the number we actually want.
 */
class FrameCounter : public FrameCallbackInterface
{
public:
  bool Update(UpdateProxy& /* updateProxy */, float /* elapsedSeconds */) override
  {
    mFrames.fetch_add(1u, std::memory_order_relaxed);
    return true; // Keep rendering: this is a continuous benchmark.
  }

  /// Returns the frames counted since the previous call, and resets.
  uint32_t TakeFrameCount()
  {
    return mFrames.exchange(0u, std::memory_order_relaxed);
  }

private:
  std::atomic<uint32_t> mFrames{0u};
};

class PropertyUpdateStressTest : public ConnectionTracker
{
public:
  PropertyUpdateStressTest(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &PropertyUpdateStressTest::Create);
  }

  ~PropertyUpdateStressTest() = default;

private:
  void Create(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Color::BLACK);

    mWindow.TouchEventSignal().Connect(this, &PropertyUpdateStressTest::OnTouch);
    mWindow.KeyEventSignal().Connect(this, &PropertyUpdateStressTest::OnKeyEvent);

    CreateReadout();
    CreateLoadActors();

    UiContext::Get().AddFrameCallback(mFrameCounter, mWindow.GetRootLayer());

    mTickTimer = Timer::New(gTickInterval);
    mTickTimer.TickSignal().Connect(this, &PropertyUpdateStressTest::OnTick);
    mTickTimer.Start();

    mReportTimer = Timer::New(REPORT_INTERVAL_MS);
    mReportTimer.TickSignal().Connect(this, &PropertyUpdateStressTest::OnReportTick);
    mReportTimer.Start();

    // Gate the run length by TICK COUNT, not a wall-clock Timer. -t is still
    // expressed in (nominal) seconds for a familiar CLI, but is converted
    // to a tick count up front using the nominal tick interval. This matters
    // because OnTick() and any wall-clock quit Timer share the same event
    // loop: under heavier per-tick load, the event thread can fall behind,
    // which delays a wall-clock quit Timer and reduces how many ticks fit
    // in the same duration - silently making the comparison windows unequal.
    // Quitting after a fixed mTickCount instead means it always execute
    // identical total work - same number of property sets, same number of
    // bursts - even if it takes longer in real time to get there.
    mQuitAfterTicks = (gDuration > 0u) ? std::max<uint32_t>(1u, (gDuration * 1000u) / gTickInterval) : 0u;
  }

  void CreateReadout()
  {
    mLabel                                  = TextLabel::New("Starting...");
    mLabel[Actor::Property::PARENT_ORIGIN]  = ParentOrigin::TOP_LEFT;
    mLabel[Actor::Property::PIVOT]          = Pivot::TOP_LEFT;
    mLabel[Actor::Property::POSITION]       = Vector3(40.0f, 40.0f, 0.0f);
    mLabel[Actor::Property::SIZE]           = Vector2(900.0f, 320.0f);
    mLabel[TextLabel::Property::MULTI_LINE] = true;
    mLabel[TextLabel::Property::TEXT_COLOR] = Color::WHITE;
    mLabel[TextLabel::Property::POINT_SIZE] = 18.0f;
    mWindow.Add(mLabel);
  }

  void CreateLoadActors()
  {
    mLoadActors.reserve(gActors);
    mPropertyIndices.reserve(gProperties);

    for(uint32_t a = 0u; a < gActors; ++a)
    {
      // Plain Actor, no renderer, default (zero) size: costs nothing to draw,
      // but is a fully live scene-graph Node that receives every property
      // message we send below.
      Actor actor = Actor::New();
      mWindow.Add(actor);

      for(uint32_t p = 0u; p < gProperties; ++p)
      {
        Property::Index index = actor.RegisterProperty(PROPERTY_NAMES[p], Vector4::ZERO);

        // Every actor registers the same names in the same order starting from
        // a fresh Object, so the indices are identical across actors; record
        // them once from the first actor.
        if(a == 0u)
        {
          mPropertyIndices.push_back(index);
        }
      }

      mLoadActors.push_back(actor);
    }
  }

  // Spawns gBurstActors extra actors, registering the same properties as the
  // steady-state load actors. Called periodically from OnTick.
  void SpawnBurstActors()
  {
    mBurstActors.reserve(gBurstActors);

    for(uint32_t a = 0u; a < gBurstActors; ++a)
    {
      Actor actor = Actor::New();
      mWindow.Add(actor);

      for(uint32_t p = 0u; p < gProperties; ++p)
      {
        actor.RegisterProperty(PROPERTY_NAMES[p], Vector4::ZERO);
      }

      mBurstActors.push_back(actor);
    }
  }

  // Removes and releases the current burst's actors, ending the churn cycle
  // until the next one starts.
  void DestroyBurstActors()
  {
    for(auto& actor : mBurstActors)
    {
      mWindow.Remove(actor);
    }
    mBurstActors.clear();
  }

  bool OnTick()
  {
    DALI_TRACE_BEGIN(gTraceFilter, "DALI_PROPERTY_UPDATE_TICK");

    ++mTickCount;

    if(gBurstActors > 0u)
    {
      if(mBurstTicksRemaining == 0u && (mTickCount % gBurstPeriodTicks) == 0u)
      {
        SpawnBurstActors();
        mBurstTicksRemaining = BURST_DURATION_TICKS;
      }
      else if(mBurstTicksRemaining > 0u)
      {
        --mBurstTicksRemaining;
        if(mBurstTicksRemaining == 0u)
        {
          DestroyBurstActors();
        }
      }
    }

    // The inner loop walks every actor (steady-state, plus any active burst
    // actors) for a single property, so consecutive messages target different
    // Objects and each Object is revisited only once per outer iteration.
    for(uint32_t p = 0u; p < gProperties; ++p)
    {
      const Property::Index index = mPropertyIndices[p];

      // Cheap, deterministic, and different every tick. Deliberately not an
      // RNG: random generation in the hot loop would add per-set cost to both
      // builds and blur the difference being measured.
      const float   component = static_cast<float>((mTickCount + p) & 0xFFu) * (1.0f / 255.0f);
      const Vector4 value(component, component, component, component);

      for(auto& actor : mLoadActors)
      {
        actor.SetProperty(index, value);
      }
      for(auto& actor : mBurstActors)
      {
        actor.SetProperty(index, value);
      }
    }

    mPropertySets += static_cast<uint64_t>(gActors + mBurstActors.size()) * gProperties;

    DALI_TRACE_END(gTraceFilter, "DALI_PROPERTY_UPDATE_TICK");

    if(mQuitAfterTicks > 0u && mTickCount >= mQuitAfterTicks)
    {
      printf("[perf-property-update] quitting after %llu ticks (~%us nominal)\n",
             static_cast<unsigned long long>(mTickCount),
             gDuration);
      fflush(stdout);
      mApplication.Quit();
      return false; // Reached the tick-count-gated run length.
    }

    return true; // Keep ticking.
  }

  bool OnReportTick()
  {
    const uint32_t frames = mFrameCounter.TakeFrameCount();
    const float    fps    = static_cast<float>(frames) * 1000.0f / static_cast<float>(REPORT_INTERVAL_MS);

    const uint64_t sets    = mPropertySets;
    mPropertySets          = 0u;
    const float setsPerSec = static_cast<float>(sets) * 1000.0f / static_cast<float>(REPORT_INTERVAL_MS);

    std::ostringstream out;
    out.setf(std::ios::fixed);
    out.precision(1);
    out << "Property Update\n\n"
        << "FPS              : " << fps << "\n"
        << "Actors           : " << gActors << "\n"
        << "Props per actor  : " << gProperties << "\n"
        << "Property sets/s  : " << (setsPerSec / 1000.0f) << "k\n"
        << "Burst actors     : " << mBurstActors.size() << " / " << gBurstActors << "\n";

    mLabel[TextLabel::Property::TEXT] = ToPropertyValue(out.str());

    return true; // Keep reporting.
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
  TextLabel    mLabel;

  std::vector<Actor>           mLoadActors;
  std::vector<Actor>           mBurstActors; ///< Live only during an active churn burst
  std::vector<Property::Index> mPropertyIndices;

  FrameCounter mFrameCounter;

  Timer mTickTimer;
  Timer mReportTimer;

  uint64_t mTickCount{0};           ///< Drives the deterministic per-tick property values.
  uint64_t mPropertySets{0};        ///< Property sets issued since the last report.
  uint32_t mBurstTicksRemaining{0}; ///< Ticks left before the active burst is torn down; 0 = no active burst.
  uint32_t mQuitAfterTicks{0};      ///< Tick count that ends the run; 0 = run until touch/key/quit.
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  // Log-based FPS as a cross-check against the on-screen readout. Does not
  // overwrite an existing setting, so it can still be overridden externally.
  setenv("DALI_FPS_TRACKING", "1", 0);

  Application application = Application::New(&argc, &argv);

  for(int i = 1; i < argc; ++i)
  {
    std::string arg(argv[i]);
    if(arg.compare(0, 2, "-a") == 0)
    {
      gActors = static_cast<uint32_t>(atoi(arg.substr(2).c_str()));
    }
    else if(arg.compare(0, 2, "-k") == 0)
    {
      gProperties = static_cast<uint32_t>(atoi(arg.substr(2).c_str()));
    }
    else if(arg.compare(0, 2, "-i") == 0)
    {
      gTickInterval = static_cast<uint32_t>(atoi(arg.substr(2).c_str()));
    }
    else if(arg.compare(0, 2, "-t") == 0)
    {
      gDuration = static_cast<uint32_t>(atoi(arg.substr(2).c_str()));
    }
    else if(arg.compare(0, 2, "-c") == 0)
    {
      gBurstActors = static_cast<uint32_t>(atoi(arg.substr(2).c_str()));
    }
    else if(arg.compare(0, 2, "-b") == 0)
    {
      gBurstPeriodTicks = static_cast<uint32_t>(atoi(arg.substr(2).c_str()));
    }
  }

  if(gActors == 0u)
  {
    gActors = 1u;
  }
  if(gProperties == 0u)
  {
    gProperties = 1u;
  }
  if(gProperties > MAX_PROPERTIES_PER_ACTOR)
  {
    gProperties = MAX_PROPERTIES_PER_ACTOR;
  }
  if(gTickInterval == 0u)
  {
    gTickInterval = 1u;
  }
  if(gBurstPeriodTicks == 0u)
  {
    gBurstPeriodTicks = 1u;
  }

  PropertyUpdateStressTest test(application);
  application.MainLoop();

  return 0;
}
