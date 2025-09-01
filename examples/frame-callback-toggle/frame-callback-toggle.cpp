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

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/common/stage-devel.h>
#include <dali/devel-api/update/frame-callback-interface.h>
#include <dali/devel-api/update/update-proxy.h>

using namespace Dali;
using namespace Dali::Toolkit;

// This example shows how to use FrameCallbackInterface to toggle an actor's "ignored" state.
// The actor will appear and disappear periodically, with labels indicating the state.

// FrameCallback to toggle the ignored state of an actor
class IgnoredToggler : public FrameCallbackInterface
{
public:
  /**
   * @brief Constructor.
   * @param[in] actorId The ID of the main actor to control.
   * @param[in] intervalSeconds The interval in seconds to toggle the ignored state.
   * @param[in] visibleLabelId The ID of the "Visible" TextLabel.
   * @param[in] ignoredLabelId The ID of the "Ignored" TextLabel.
   */
  IgnoredToggler(uint32_t actorId, float intervalSeconds, uint32_t visibleLabelId, uint32_t ignoredLabelId)
  : mActorId(actorId),
    mIntervalSeconds(intervalSeconds),
    mElapsedTime(0.0f),
    mCurrentlyIgnored(false),
    mVisibleLabelId(visibleLabelId),
    mIgnoredLabelId(ignoredLabelId)
  {
    // Initial state is set in the main thread (FrameCallbackToggleController::Create)
  }

  // From FrameCallbackInterface
  bool Update(Dali::UpdateProxy& updateProxy, float elapsedSeconds) override
  {
    mElapsedTime += elapsedSeconds;

    if(mElapsedTime >= mIntervalSeconds)
    {
      mCurrentlyIgnored = !mCurrentlyIgnored;
      updateProxy.SetIgnored(mActorId, mCurrentlyIgnored);
      // Control label visibility using UpdateProxy to ensure thread safety
      updateProxy.SetIgnored(mVisibleLabelId, mCurrentlyIgnored); // Hide "Visible" label when main actor is ignored
      updateProxy.SetIgnored(mIgnoredLabelId, !mCurrentlyIgnored); // Show "Ignored" label when main actor is ignored
      mElapsedTime = 0.0f; // Reset timer
    }

    return true; // Keep the callback alive
  }

private:
  uint32_t mActorId;         ///< ID of the actor to control
  float   mIntervalSeconds;  ///< Interval for toggling
  float   mElapsedTime;      ///< Accumulated elapsed time
  bool    mCurrentlyIgnored; ///< Current ignored state
  uint32_t mVisibleLabelId;  ///< ID of the "Visible" TextLabel
  uint32_t mIgnoredLabelId;  ///< ID of the "Ignored" TextLabel
};

/**
 * @brief The main class of the demo.
 */
class FrameCallbackToggleController : public ConnectionTracker
{
public:

  /**
   * @brief Constructor.
   * @param[in] application The application instance
   */
  FrameCallbackToggleController(Application& application)
  : mApplication(application)
  {
    // Connect to the application's init signal
    mApplication.InitSignal().Connect(this, &FrameCallbackToggleController::Create);
  }

  ~FrameCallbackToggleController() = default; // Nothing to do in destructor

private:

  /**
   * @brief The Init signal is received once (only) during the Application lifetime
   */
  void Create(Application& application)
  {
    // Get a handle to the window and stage
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);
    Stage stage = Stage::GetCurrent(); // Get current Stage

    // Create an ImageView to visually demonstrate the ignored state
    mImageView = ImageView::New();
    mImageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mImageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mImageView.SetProperty(Actor::Property::SIZE, Vector2(200, 200));
    mImageView.SetProperty(Dali::Actor::Property::NAME, "demoImageView");

    // Set a visual. Using a solid color rectangle for simplicity.
    // In a real app, you would use an image URL.
    Property::Map map;
    map[Visual::Property::TYPE] = Visual::COLOR;
    map[ColorVisual::Property::MIX_COLOR] = Color::RED; // Corrected property name
    mImageView.SetProperty(ImageView::Property::IMAGE, map);

    window.Add(mImageView);

    // Create TextLabels to display the ignored state
    // "Visible" Label
    mVisibleLabel = TextLabel::New("Visible");
    mVisibleLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mVisibleLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mVisibleLabel.SetProperty(TextLabel::Property::TEXT_COLOR, Color::BLACK);
    mVisibleLabel.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    mVisibleLabel.SetProperty(TextLabel::Property::POINT_SIZE, 12.0f);
    window.Add(mVisibleLabel); // Add as a child so it's ignored with the parent

    // "Ignored" Label
    mIgnoredLabel = TextLabel::New("Ignored");
    mIgnoredLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mIgnoredLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mIgnoredLabel.SetProperty(TextLabel::Property::TEXT_COLOR, Color::RED); // Use red to distinguish
    mIgnoredLabel.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    mIgnoredLabel.SetProperty(TextLabel::Property::POINT_SIZE, 12.0f);
    window.Add(mIgnoredLabel); // Add as a child

    // Set initial visibility for labels using SetIgnored for consistency
    // This ensures the initial state is managed the same way as the toggled state
    // Initially, mIgnoredLabel should be in an 'ignored' state so it's not visible.
    // We call SetIgnored directly here as we are on the main thread.
    // Note: SetIgnored on an actor from the main thread sends a message to the update thread.
    mIgnoredLabel.SetIgnored(true); // Initially ignored

    // Get the actor IDs for the FrameCallback
    uint32_t actorId = mImageView.GetProperty<int>(Actor::Property::ID);
    uint32_t visibleLabelId = mVisibleLabel.GetProperty<int>(Actor::Property::ID);
    uint32_t ignoredLabelId = mIgnoredLabel.GetProperty<int>(Actor::Property::ID);

    // Create and add the frame callback to toggle the ignored state every 2 seconds
    // Pass the label IDs to the IgnoredToggler
    mIgnoredToggler = new IgnoredToggler(actorId, 2.0f, visibleLabelId, ignoredLabelId); // Manage with a unique_ptr or similar if needed
    DevelStage::AddFrameCallback(stage, *mIgnoredToggler, stage.GetRootLayer()); // Dali:: prefix not needed due to 'using namespace Dali;'

    // Respond to a touch anywhere on the window to quit
    window.GetRootLayer().TouchedSignal().Connect(this, &FrameCallbackToggleController::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &FrameCallbackToggleController::OnKeyEvent);
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
  ImageView     mImageView;
  TextLabel     mVisibleLabel;      ///< Label to show when actor is visible
  TextLabel     mIgnoredLabel;      ///< Label to show when actor is ignored
  IgnoredToggler* mIgnoredToggler; // Raw pointer for simplicity, use smart pointer in production code
};

int DALI_EXPORT_API main(int argc, char **argv)
{
  Application application = Application::New(&argc, &argv);
  FrameCallbackToggleController test(application);
  application.MainLoop();
  return 0;
}
