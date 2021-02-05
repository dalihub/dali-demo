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

// CLASS HEADER
#include "dali-table-view.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/accessibility-manager/accessibility-manager.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali-toolkit/devel-api/shader-effects/alpha-discard-effect.h>
#include <dali-toolkit/devel-api/shader-effects/distance-field-effect.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/images/distance-field.h>
#include <algorithm>

// INTERNAL INCLUDES
#include "shared/execute-process.h"
#include "shared/utility.h"
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

///////////////////////////////////////////////////////////////////////////////

namespace
{
const std::string LOGO_PATH(DEMO_IMAGE_DIR "Logo-for-demo.png");

// Keyboard focus effect constants.
const float   KEYBOARD_FOCUS_ANIMATION_DURATION   = 1.0f;                                                                                                     ///< The total duration of the keyboard focus animation
const int32_t KEYBOARD_FOCUS_ANIMATION_LOOP_COUNT = 5;                                                                                                        ///< The number of loops for the keyboard focus animation
const float   KEYBOARD_FOCUS_FINAL_SCALE_FLOAT    = 1.05f;                                                                                                    ///< The final scale of the focus highlight
const float   KEYBOARD_FOCUS_ANIMATED_SCALE_FLOAT = 1.18f;                                                                                                    ///< The scale of the focus highlight during the animation
const float   KEYBOARD_FOCUS_FINAL_ALPHA          = 0.7f;                                                                                                     ///< The final alpha of the focus highlight
const float   KEYBOARD_FOCUS_ANIMATING_ALPHA      = 0.0f;                                                                                                     ///< The alpha of the focus highlight during the animation
const float   KEYBOARD_FOCUS_FADE_PERCENTAGE      = 0.16f;                                                                                                    ///< The duration of the fade (from translucent to the final-alpha) as a percentage of the overall animation duration
const Vector3 KEYBOARD_FOCUS_FINAL_SCALE(KEYBOARD_FOCUS_FINAL_SCALE_FLOAT, KEYBOARD_FOCUS_FINAL_SCALE_FLOAT, KEYBOARD_FOCUS_FINAL_SCALE_FLOAT);               ///< @see KEYBOARD_FOCUS_START_SCALE_FLOAT
const Vector3 FOCUS_INDICATOR_ANIMATING_SCALE(KEYBOARD_FOCUS_ANIMATED_SCALE_FLOAT, KEYBOARD_FOCUS_ANIMATED_SCALE_FLOAT, KEYBOARD_FOCUS_ANIMATED_SCALE_FLOAT); ///< @see KEYBOARD_FOCUS_END_SCALE_FLOAT
const float   KEYBOARD_FOCUS_MID_KEY_FRAME_TIME = KEYBOARD_FOCUS_ANIMATION_DURATION - (KEYBOARD_FOCUS_ANIMATION_DURATION * KEYBOARD_FOCUS_FADE_PERCENTAGE);   ///< Time of the mid key-frame

const float   TILE_LABEL_PADDING          = 8.0f;  ///< Border between edge of tile and the example text
const float   BUTTON_PRESS_ANIMATION_TIME = 0.35f; ///< Time to perform button scale effect.
const int     EXAMPLES_PER_ROW            = 3;
const int     ROWS_PER_PAGE               = 3;
const int     EXAMPLES_PER_PAGE           = EXAMPLES_PER_ROW * ROWS_PER_PAGE;
const Vector3 TABLE_RELATIVE_SIZE(0.95f, 0.9f, 0.8f); ///< TableView's relative size to the entire stage. The Y value means sum of the logo and table relative heights.

const char* const DEMO_BUILD_DATE = __DATE__ " " __TIME__;

/**
 * Creates the background image
 */
Actor CreateBackground(std::string stylename)
{
  Control background = Control::New();
  background.SetStyleName(stylename);
  background.SetProperty(Actor::Property::NAME, "BACKGROUND");
  background.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  background.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  background.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  return background;
}

/**
 * Constraint to precalculate values from the scroll-view
 * and tile positions to pass to the tile shader.
 */
struct TileShaderPositionConstraint
{
  TileShaderPositionConstraint(float pageWidth, float tileXOffset)
  : mPageWidth(pageWidth),
    mTileXOffset(tileXOffset)
  {
  }

  void operator()(Vector3& position, const PropertyInputContainer& inputs)
  {
    // Set up position.x as the tiles X offset (0.0 -> 1.0).
    position.x = mTileXOffset;
    // Set up position.z as the linear scroll-view X offset (0.0 -> 1.0).
    position.z = 1.0f * (-fmod(inputs[0]->GetVector2().x, mPageWidth) / mPageWidth);
    // Set up position.y as a rectified version of the scroll-views X offset.
    // IE. instead of 0.0 -> 1.0, it moves between 0.0 -> 0.5 -> 0.0 within the same span.
    if(position.z > 0.5f)
    {
      position.y = 1.0f - position.z;
    }
    else
    {
      position.y = position.z;
    }
  }

private:
  float mPageWidth;
  float mTileXOffset;
};

/**
 * Creates a popup that shows the version information of the DALi libraries and demo
 */
Dali::Toolkit::Popup CreateVersionPopup(Application& application, ConnectionTrackerInterface& connectionTracker)
{
  std::ostringstream stream;
  stream << "DALi Core: " << CORE_MAJOR_VERSION << "." << CORE_MINOR_VERSION << "." << CORE_MICRO_VERSION << std::endl
         << "(" << CORE_BUILD_DATE << ")\n";
  stream << "DALi Adaptor: " << ADAPTOR_MAJOR_VERSION << "." << ADAPTOR_MINOR_VERSION << "." << ADAPTOR_MICRO_VERSION << std::endl
         << "(" << ADAPTOR_BUILD_DATE << ")\n";
  stream << "DALi Toolkit: " << TOOLKIT_MAJOR_VERSION << "." << TOOLKIT_MINOR_VERSION << "." << TOOLKIT_MICRO_VERSION << std::endl
         << "(" << TOOLKIT_BUILD_DATE << ")\n";
  stream << "DALi Demo:"
         << "\n(" << DEMO_BUILD_DATE << ")\n";

  Dali::Toolkit::Popup popup = Dali::Toolkit::Popup::New();

  Toolkit::TextLabel titleActor = Toolkit::TextLabel::New("Version information");
  titleActor.SetProperty(Actor::Property::NAME, "titleActor");
  titleActor.SetProperty(Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::CENTER);
  titleActor.SetProperty(Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE);

  Toolkit::TextLabel contentActor = Toolkit::TextLabel::New(stream.str());
  contentActor.SetProperty(Actor::Property::NAME, "contentActor");
  contentActor.SetProperty(Toolkit::TextLabel::Property::MULTI_LINE, true);
  contentActor.SetProperty(Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::CENTER);
  contentActor.SetProperty(Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE);
  contentActor.SetProperty(Actor::Property::PADDING, Padding(0.0f, 0.0f, 20.0f, 0.0f));

  popup.SetTitle(titleActor);
  popup.SetContent(contentActor);

  popup.SetResizePolicy(ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::WIDTH);
  popup.SetProperty(Actor::Property::SIZE_MODE_FACTOR, Vector3(0.75f, 1.0f, 1.0f));
  popup.SetResizePolicy(ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT);

  application.GetWindow().Add(popup);

  // Hide the popup when touched outside
  popup.OutsideTouchedSignal().Connect(
    &connectionTracker,
    [popup]() mutable {
      if(popup && (popup.GetDisplayState() == Toolkit::Popup::SHOWN))
      {
        popup.SetDisplayState(Popup::HIDDEN);
      }
    });

  return popup;
}

/// Sets up the inner cube effect
Dali::Toolkit::ScrollViewEffect SetupInnerPageCubeEffect(const Vector2 windowSize, int totalPages)
{
  Dali::Path            path = Dali::Path::New();
  Dali::Property::Array points;
  points.Resize(3);
  points[0] = Vector3(windowSize.x * 0.5, 0.0f, windowSize.x * 0.5f);
  points[1] = Vector3(0.0f, 0.0f, 0.0f);
  points[2] = Vector3(-windowSize.x * 0.5f, 0.0f, windowSize.x * 0.5f);
  path.SetProperty(Path::Property::POINTS, points);

  Dali::Property::Array controlPoints;
  controlPoints.Resize(4);
  controlPoints[0] = Vector3(windowSize.x * 0.5f, 0.0f, windowSize.x * 0.3f);
  controlPoints[1] = Vector3(windowSize.x * 0.3f, 0.0f, 0.0f);
  controlPoints[2] = Vector3(-windowSize.x * 0.3f, 0.0f, 0.0f);
  controlPoints[3] = Vector3(-windowSize.x * 0.5f, 0.0f, windowSize.x * 0.3f);
  path.SetProperty(Path::Property::CONTROL_POINTS, controlPoints);

  return ScrollViewPagePathEffect::New(path,
                                       Vector3(-1.0f, 0.0f, 0.0f),
                                       Toolkit::ScrollView::Property::SCROLL_FINAL_X,
                                       Vector3(windowSize.x * TABLE_RELATIVE_SIZE.x, windowSize.y * TABLE_RELATIVE_SIZE.y, 0.0f),
                                       totalPages);
}

/// Sets up the scroll view rulers
void SetupScrollViewRulers(ScrollView& scrollView, const uint16_t windowWidth, const float pageWidth, const int totalPages)
{
  // Update Ruler info. for the scroll-view
  Dali::Toolkit::RulerPtr rulerX = new FixedRuler(pageWidth);
  Dali::Toolkit::RulerPtr rulerY = new DefaultRuler();
  rulerX->SetDomain(RulerDomain(0.0f, (totalPages + 1) * windowWidth * TABLE_RELATIVE_SIZE.x * 0.5f, true));
  rulerY->Disable();
  scrollView.SetRulerX(rulerX);
  scrollView.SetRulerY(rulerY);
}

} // namespace

DaliTableView::DaliTableView(Application& application)
: mApplication(application),
  mRootActor(),
  mPressedAnimation(),
  mScrollView(),
  mScrollViewEffect(),
  mPressedActor(),
  mLogoTapDetector(),
  mVersionPopup(),
  mPages(),
  mExampleList(),
  mPageWidth(0.0f),
  mTotalPages(),
  mScrolling(false),
  mSortAlphabetically(false)
{
  application.InitSignal().Connect(this, &DaliTableView::Initialize);
}

void DaliTableView::AddExample(Example example)
{
  mExampleList.push_back(example);
}

void DaliTableView::SortAlphabetically(bool sortAlphabetically)
{
  mSortAlphabetically = sortAlphabetically;
}

void DaliTableView::Initialize(Application& application)
{
  Window window = application.GetWindow();
  window.KeyEventSignal().Connect(this, &DaliTableView::OnKeyEvent);
  const Window::WindowSize windowSize = window.GetSize();

  // Background
  mRootActor = CreateBackground("LauncherBackground");
  window.Add(mRootActor);

  // Add logo
  ImageView logo = ImageView::New(LOGO_PATH);
  logo.SetProperty(Actor::Property::NAME, "LOGO_IMAGE");
  logo.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
  logo.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.5f, 0.1f, 0.5f));
  logo.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);
  // The logo should appear on top of everything.
  logo.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  mRootActor.Add(logo);

  // Show version in a popup when log is tapped
  mLogoTapDetector = TapGestureDetector::New();
  mLogoTapDetector.Attach(logo);
  mLogoTapDetector.DetectedSignal().Connect(this, &DaliTableView::OnLogoTapped);

  // Scrollview occupying the majority of the screen
  mScrollView = ScrollView::New();
  mScrollView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
  mScrollView.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.5f, 1.0f - 0.05f, 0.5f));
  mScrollView.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
  mScrollView.SetResizePolicy(ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::HEIGHT);
  mScrollView.SetProperty(Actor::Property::SIZE_MODE_FACTOR, Vector3(0.0f, 0.6f, 0.0f));

  const float buttonsPageMargin = (1.0f - TABLE_RELATIVE_SIZE.x) * 0.5f * windowSize.GetWidth();
  mScrollView.SetProperty(Actor::Property::PADDING, Padding(buttonsPageMargin, buttonsPageMargin, 0.0f, 0.0f));

  mScrollView.SetAxisAutoLock(true);
  mScrollView.ScrollCompletedSignal().Connect(this, &DaliTableView::OnScrollComplete);
  mScrollView.ScrollStartedSignal().Connect(this, &DaliTableView::OnScrollStart);
  mScrollView.TouchedSignal().Connect(this, &DaliTableView::OnScrollTouched);

  mPageWidth = windowSize.GetWidth() * TABLE_RELATIVE_SIZE.x * 0.5f;

  mBubbleAnimator.Initialize(mRootActor, mScrollView);

  mRootActor.Add(mScrollView);

  // Add scroll view effect and setup constraints on pages
  ApplyScrollViewEffect();

  // Add pages and tiles
  Populate();

  // Remove constraints for inner cube effect
  ApplyCubeEffectToPages();

  Dali::Window winHandle = application.GetWindow();

  if(windowSize.GetWidth() <= windowSize.GetHeight())
  {
    winHandle.AddAvailableOrientation(Dali::WindowOrientation::PORTRAIT);
    winHandle.RemoveAvailableOrientation(Dali::WindowOrientation::LANDSCAPE);
    winHandle.AddAvailableOrientation(Dali::WindowOrientation::PORTRAIT_INVERSE);
    winHandle.RemoveAvailableOrientation(Dali::WindowOrientation::LANDSCAPE_INVERSE);
  }
  else
  {
    winHandle.AddAvailableOrientation(Dali::WindowOrientation::LANDSCAPE);
    winHandle.RemoveAvailableOrientation(Dali::WindowOrientation::PORTRAIT);
    winHandle.AddAvailableOrientation(Dali::WindowOrientation::LANDSCAPE_INVERSE);
    winHandle.RemoveAvailableOrientation(Dali::WindowOrientation::PORTRAIT_INVERSE);
  }

  CreateFocusEffect();
}

void DaliTableView::CreateFocusEffect()
{
  // Hook the required signals to manage the focus.
  auto keyboardFocusManager = KeyboardFocusManager::Get();
  keyboardFocusManager.PreFocusChangeSignal().Connect(this, &DaliTableView::OnKeyboardPreFocusChange);
  keyboardFocusManager.FocusedActorEnterKeySignal().Connect(this, &DaliTableView::OnFocusedActorActivated);

  // Loop to create both actors for the focus highlight effect.
  for(unsigned int i = 0; i < FOCUS_ANIMATION_ACTOR_NUMBER; ++i)
  {
    mFocusEffect[i].actor = ImageView::New();
    mFocusEffect[i].actor.SetStyleName("FocusActor");
    mFocusEffect[i].actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mFocusEffect[i].actor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    mFocusEffect[i].actor.SetProperty(Actor::Property::INHERIT_SCALE, false);
    mFocusEffect[i].actor.SetProperty(Actor::Property::COLOR_MODE, USE_OWN_COLOR);

    KeyFrames alphaKeyFrames = KeyFrames::New();
    alphaKeyFrames.Add(0.0f, KEYBOARD_FOCUS_FINAL_ALPHA);
    alphaKeyFrames.Add(KEYBOARD_FOCUS_MID_KEY_FRAME_TIME, KEYBOARD_FOCUS_ANIMATING_ALPHA);
    alphaKeyFrames.Add(KEYBOARD_FOCUS_ANIMATION_DURATION, KEYBOARD_FOCUS_FINAL_ALPHA);

    KeyFrames scaleKeyFrames = KeyFrames::New();
    scaleKeyFrames.Add(0.0f, KEYBOARD_FOCUS_FINAL_SCALE);
    scaleKeyFrames.Add(KEYBOARD_FOCUS_MID_KEY_FRAME_TIME, FOCUS_INDICATOR_ANIMATING_SCALE);
    scaleKeyFrames.Add(KEYBOARD_FOCUS_ANIMATION_DURATION, KEYBOARD_FOCUS_FINAL_SCALE);

    mFocusEffect[i].animation = Animation::New(KEYBOARD_FOCUS_ANIMATION_DURATION);
    mFocusEffect[i].animation.AnimateBetween(Property(mFocusEffect[i].actor, Actor::Property::COLOR_ALPHA), alphaKeyFrames);
    mFocusEffect[i].animation.AnimateBetween(Property(mFocusEffect[i].actor, Actor::Property::SCALE), scaleKeyFrames);

    mFocusEffect[i].animation.SetLoopCount(KEYBOARD_FOCUS_ANIMATION_LOOP_COUNT);
  }

  // Parent the secondary effect from the primary.
  mFocusEffect[0].actor.Add(mFocusEffect[1].actor);

  keyboardFocusManager.SetFocusIndicatorActor(mFocusEffect[0].actor);

  // Connect to the on & off scene signals of the indicator which represents when it is enabled & disabled respectively
  mFocusEffect[0].actor.OnSceneSignal().Connect(this, &DaliTableView::OnFocusIndicatorEnabled);
  mFocusEffect[0].actor.OffSceneSignal().Connect(this, &DaliTableView::OnFocusIndicatorDisabled);
}

void DaliTableView::OnFocusIndicatorEnabled(Actor /* actor */)
{
  // Play the animation on the 1st glow object.
  mFocusEffect[0].animation.Play();

  // Stagger the animation on the 2nd glow object half way through.
  mFocusEffect[1].animation.PlayFrom(KEYBOARD_FOCUS_ANIMATION_DURATION / 2.0f);
}

void DaliTableView::OnFocusIndicatorDisabled(Dali::Actor /* actor */)
{
  // Stop the focus effect animations
  for(auto i = 0u; i < FOCUS_ANIMATION_ACTOR_NUMBER; ++i)
  {
    mFocusEffect[i].animation.Stop();
  }
}

void DaliTableView::ApplyCubeEffectToPages()
{
  ScrollViewPagePathEffect effect = ScrollViewPagePathEffect::DownCast(mScrollViewEffect);
  unsigned int             pageCount(0);
  for(std::vector<Actor>::iterator pageIter = mPages.begin(); pageIter != mPages.end(); ++pageIter)
  {
    Actor page = *pageIter;
    effect.ApplyToPage(page, pageCount++);
  }
}

void DaliTableView::Populate()
{
  const Window::WindowSize windowSize = mApplication.GetWindow().GetSize();

  mTotalPages = (mExampleList.size() + EXAMPLES_PER_PAGE - 1) / EXAMPLES_PER_PAGE;

  // Populate ScrollView.
  if(mExampleList.size() > 0)
  {
    if(mSortAlphabetically)
    {
      sort(mExampleList.begin(), mExampleList.end(), [](auto& lhs, auto& rhs) -> bool { return lhs.title < rhs.title; });
    }

    unsigned int         exampleCount = 0;
    ExampleListConstIter iter         = mExampleList.begin();

    for(int t = 0; t < mTotalPages && iter != mExampleList.end(); t++)
    {
      // Create Table
      TableView page = TableView::New(ROWS_PER_PAGE, EXAMPLES_PER_ROW);
      page.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      page.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      page.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
      mScrollView.Add(page);

      // Calculate the number of images going across (columns) within a page, according to the screen resolution and dpi.
      const float margin               = 2.0f;
      const float tileParentMultiplier = 1.0f / EXAMPLES_PER_ROW;

      for(int row = 0; row < ROWS_PER_PAGE && iter != mExampleList.end(); row++)
      {
        for(int column = 0; column < EXAMPLES_PER_ROW && iter != mExampleList.end(); column++)
        {
          const Example& example = (*iter);

          // Calculate the tiles relative position on the page (between 0 & 1 in each dimension).
          Vector2              position(static_cast<float>(column) / (EXAMPLES_PER_ROW - 1.0f), static_cast<float>(row) / (EXAMPLES_PER_ROW - 1.0f));
          Actor                tile                 = CreateTile(example.name, example.title, Vector3(tileParentMultiplier, tileParentMultiplier, 1.0f), position);
          AccessibilityManager accessibilityManager = AccessibilityManager::Get();
          accessibilityManager.SetFocusOrder(tile, ++exampleCount);
          accessibilityManager.SetAccessibilityAttribute(tile, Dali::Toolkit::AccessibilityManager::ACCESSIBILITY_LABEL, example.title);
          accessibilityManager.SetAccessibilityAttribute(tile, Dali::Toolkit::AccessibilityManager::ACCESSIBILITY_TRAIT, "Tile");
          accessibilityManager.SetAccessibilityAttribute(tile, Dali::Toolkit::AccessibilityManager::ACCESSIBILITY_HINT, "You can run this example");

          tile.SetProperty(Actor::Property::PADDING, Padding(margin, margin, margin, margin));
          page.AddChild(tile, TableView::CellPosition(row, column));

          iter++;
        }
      }

      mPages.push_back(page);
    }
  }

  SetupScrollViewRulers(mScrollView, windowSize.GetWidth(), mPageWidth, mTotalPages);
}

Actor DaliTableView::CreateTile(const std::string& name, const std::string& title, const Dali::Vector3& sizeMultiplier, Vector2& position)
{
  Toolkit::ImageView focusableTile = ImageView::New();

  focusableTile.SetStyleName("DemoTile");
  focusableTile.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  focusableTile.SetResizePolicy(ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS);
  focusableTile.SetProperty(Actor::Property::SIZE_MODE_FACTOR, sizeMultiplier);
  focusableTile.SetProperty(Actor::Property::NAME, name);

  // Set the tile to be keyboard focusable
  focusableTile.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  // Register a property with the ImageView. This allows us to inject the scroll-view position into the shader.
  Property::Value value         = Vector3(0.0f, 0.0f, 0.0f);
  Property::Index propertyIndex = focusableTile.RegisterProperty("uCustomPosition", value);

  // We create a constraint to perform a precalculation on the scroll-view X offset
  // and pass it to the shader uniform, along with the tile's position.
  Constraint shaderPosition = Constraint::New<Vector3>(focusableTile, propertyIndex, TileShaderPositionConstraint(mPageWidth, position.x));
  shaderPosition.AddSource(Source(mScrollView, ScrollView::Property::SCROLL_POSITION));
  shaderPosition.SetRemoveAction(Constraint::DISCARD);
  shaderPosition.Apply();
  //focusableTile.Add( tileContent );

  // Create an ImageView for the 9-patch border around the tile.
  ImageView borderImage = ImageView::New();
  borderImage.SetStyleName("DemoTileBorder");
  borderImage.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  borderImage.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  borderImage.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  borderImage.SetProperty(Actor::Property::OPACITY, 0.8f);
  DevelControl::AppendAccessibilityRelation(borderImage, focusableTile, Accessibility::RelationType::CONTROLLED_BY);
  focusableTile.Add(borderImage);

  TextLabel label = TextLabel::New();
  label.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  label.SetStyleName("LauncherLabel");
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  label.SetProperty(TextLabel::Property::TEXT, title);
  label.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
  label.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
  label.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT);

  // Pad around the label as its size is the same as the 9-patch border. It will overlap it without padding.
  label.SetProperty(Actor::Property::PADDING, Padding(TILE_LABEL_PADDING, TILE_LABEL_PADDING, TILE_LABEL_PADDING, TILE_LABEL_PADDING));
  DevelControl::AppendAccessibilityRelation(label, focusableTile, Accessibility::RelationType::CONTROLLED_BY);
  focusableTile.Add(label);

  // Connect to the touch events
  focusableTile.TouchedSignal().Connect(this, &DaliTableView::OnTilePressed);
  focusableTile.HoveredSignal().Connect(this, &DaliTableView::OnTileHovered);
  focusableTile.SetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::PUSH_BUTTON);
  DevelControl::AccessibilityActivateSignal(focusableTile).Connect(this, [=]() {
    DoTilePress(focusableTile, PointState::DOWN);
    DoTilePress(focusableTile, PointState::UP);
  });

  return focusableTile;
}

bool DaliTableView::OnTilePressed(Actor actor, const TouchEvent& event)
{
  return DoTilePress(actor, event.GetState(0));
}

bool DaliTableView::DoTilePress(Actor actor, PointState::Type pointState)
{
  bool consumed = false;

  if(PointState::DOWN == pointState)
  {
    mPressedActor = actor;
    consumed      = true;
  }

  // A button press is only valid if the Down & Up events
  // both occurred within the button.
  if((PointState::UP == pointState) &&
     (mPressedActor == actor))
  {
    // ignore Example button presses when scrolling or button animating.
    if((!mScrolling) && (!mPressedAnimation))
    {
      std::string name = actor.GetProperty<std::string>(Dali::Actor::Property::NAME);
      for(Example& example : mExampleList)
      {
        if(example.name == name)
        {
          // do nothing, until pressed animation finished.
          consumed = true;
          break;
        }
      }
    }

    if(consumed)
    {
      mPressedAnimation = Animation::New(BUTTON_PRESS_ANIMATION_TIME);
      mPressedAnimation.SetEndAction(Animation::DISCARD);

      // scale the content actor within the Tile, as to not affect the placement within the Table.
      Actor content = actor.GetChildAt(0);
      mPressedAnimation.AnimateTo(Property(content, Actor::Property::SCALE), Vector3(0.7f, 0.7f, 1.0f), AlphaFunction::EASE_IN_OUT, TimePeriod(0.0f, BUTTON_PRESS_ANIMATION_TIME * 0.5f));
      mPressedAnimation.AnimateTo(Property(content, Actor::Property::SCALE), Vector3::ONE, AlphaFunction::EASE_IN_OUT, TimePeriod(BUTTON_PRESS_ANIMATION_TIME * 0.5f, BUTTON_PRESS_ANIMATION_TIME * 0.5f));

      // Rotate button on the Y axis when pressed.
      mPressedAnimation.AnimateBy(Property(content, Actor::Property::ORIENTATION), Quaternion(Degree(0.0f), Degree(180.0f), Degree(0.0f)));

      mPressedAnimation.Play();
      mPressedAnimation.FinishedSignal().Connect(this, &DaliTableView::OnPressedAnimationFinished);
    }
  }
  return consumed;
}

void DaliTableView::OnPressedAnimationFinished(Dali::Animation& source)
{
  mPressedAnimation.Reset();
  if(mPressedActor)
  {
    std::string name = mPressedActor.GetProperty<std::string>(Dali::Actor::Property::NAME);

    ExecuteProcess(name, mApplication);

    mPressedActor.Reset();
  }
}

void DaliTableView::OnScrollStart(const Dali::Vector2& position)
{
  mScrolling = true;

  mBubbleAnimator.PlayAnimation();
}

void DaliTableView::OnScrollComplete(const Dali::Vector2& position)
{
  mScrolling = false;

  // move focus to 1st item of new page
  AccessibilityManager accessibilityManager = AccessibilityManager::Get();
  accessibilityManager.SetCurrentFocusActor(mPages[mScrollView.GetCurrentPage()].GetChildAt(0));
}

bool DaliTableView::OnScrollTouched(Actor actor, const TouchEvent& event)
{
  if(PointState::DOWN == event.GetState(0))
  {
    mPressedActor = actor;
  }

  return false;
}

void DaliTableView::ApplyScrollViewEffect()
{
  // Remove old effect if exists.

  if(mScrollViewEffect)
  {
    mScrollView.RemoveEffect(mScrollViewEffect);
  }

  // Just one effect for now
  mScrollViewEffect = SetupInnerPageCubeEffect(mApplication.GetWindow().GetSize(), mTotalPages);

  mScrollView.ApplyEffect(mScrollViewEffect);
}

void DaliTableView::OnKeyEvent(const KeyEvent& event)
{
  if(event.GetState() == KeyEvent::DOWN)
  {
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      // If there's a Popup, Hide it if it's contributing to the display in any way (EG. transitioning in or out).
      // Otherwise quit.
      if(mVersionPopup && (mVersionPopup.GetDisplayState() != Toolkit::Popup::HIDDEN))
      {
        mVersionPopup.SetDisplayState(Popup::HIDDEN);
      }
      else
      {
        mApplication.Quit();
      }
    }
  }
}

Dali::Actor DaliTableView::OnKeyboardPreFocusChange(Dali::Actor current, Dali::Actor proposed, Dali::Toolkit::Control::KeyboardFocus::Direction direction)
{
  Actor nextFocusActor = proposed;

  if(!current && !proposed)
  {
    // Set the initial focus to the first tile in the current page should be focused.
    nextFocusActor = mPages[mScrollView.GetCurrentPage()].GetChildAt(0);
  }
  else if(!proposed)
  {
    // ScrollView is being focused but nothing in the current page can be focused further
    // in the given direction. We should work out which page to scroll to next.
    int currentPage = mScrollView.GetCurrentPage();
    int newPage     = currentPage;
    if(direction == Dali::Toolkit::Control::KeyboardFocus::LEFT)
    {
      newPage--;
    }
    else if(direction == Dali::Toolkit::Control::KeyboardFocus::RIGHT)
    {
      newPage++;
    }

    newPage = std::max(0, std::min(mTotalPages - 1, newPage));
    if(newPage == currentPage)
    {
      if(direction == Dali::Toolkit::Control::KeyboardFocus::LEFT)
      {
        newPage = mTotalPages - 1;
      }
      else if(direction == Dali::Toolkit::Control::KeyboardFocus::RIGHT)
      {
        newPage = 0;
      }
    }

    // Scroll to the page in the given direction
    mScrollView.ScrollTo(newPage);

    if(direction == Dali::Toolkit::Control::KeyboardFocus::LEFT)
    {
      // Work out the cell position for the last tile
      int remainingExamples = mExampleList.size() - newPage * EXAMPLES_PER_PAGE;
      int rowPos            = (remainingExamples >= EXAMPLES_PER_PAGE) ? ROWS_PER_PAGE - 1 : ((remainingExamples % EXAMPLES_PER_PAGE + EXAMPLES_PER_ROW) / EXAMPLES_PER_ROW - 1);
      int colPos            = remainingExamples >= EXAMPLES_PER_PAGE ? EXAMPLES_PER_ROW - 1 : (remainingExamples % EXAMPLES_PER_PAGE - rowPos * EXAMPLES_PER_ROW - 1);

      // Move the focus to the last tile in the new page.
      nextFocusActor = mPages[newPage].GetChildAt(rowPos * EXAMPLES_PER_ROW + colPos);
    }
    else
    {
      // Move the focus to the first tile in the new page.
      nextFocusActor = mPages[newPage].GetChildAt(0);
    }
  }

  return nextFocusActor;
}

void DaliTableView::OnFocusedActorActivated(Dali::Actor activatedActor)
{
  if(activatedActor)
  {
    mPressedActor = activatedActor;

    // Activate the current focused actor;
    DoTilePress(mPressedActor, PointState::UP);
  }
}

bool DaliTableView::OnTileHovered(Actor actor, const HoverEvent& event)
{
  KeyboardFocusManager::Get().SetCurrentFocusActor(actor);
  return true;
}

void DaliTableView::OnLogoTapped(Dali::Actor actor, const Dali::TapGesture& tap)
{
  // Only show if currently fully hidden. If transitioning-out, the transition will not be interrupted.
  if(!mVersionPopup || (mVersionPopup.GetDisplayState() == Toolkit::Popup::HIDDEN))
  {
    if(!mVersionPopup)
    {
      mVersionPopup = CreateVersionPopup(mApplication, *this);
    }

    mVersionPopup.SetDisplayState(Popup::SHOWN);
  }
}
