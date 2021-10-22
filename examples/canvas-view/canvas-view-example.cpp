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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/canvas-view/canvas-view.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer-drawable-group.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer-gradient.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer-linear-gradient.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer-picture.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer-radial-gradient.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer-shape.h>

using namespace Dali;

namespace
{
const char* VIEW_LABEL[] = {
  "BasicShapes",
  "ClipPath/Mask",
  "Gradient",
  "Picture"};

enum
{
  BASIC_SHAPE_VIEW = 0,
  CLIP_PATH_VIEW,
  GRADIENT_VIEW,
  PICTURE_VIEW,
  NUMBER_OF_VIEWS
};

const char* PREV_BUTTON_TEXT("Prev");
const char* LEFT_BUTTON_TEXT("Next");

const char* IMAGES[] =
  {
    DEMO_IMAGE_DIR "Kid1.svg",
};

const float COMMON_STROKE_DASH_PATTERN[] = {15.0f, 30.0f};

/**
 * @brief Create shape that have star path.
 */
Dali::CanvasRenderer::Shape MakeStarShape()
{
  Dali::CanvasRenderer::Shape star = Dali::CanvasRenderer::Shape::New();
  star.AddMoveTo(Vector2(-1.0f, -165.0f));
  star.AddLineTo(Vector2(53.0f, -56.0f));
  star.AddLineTo(Vector2(174.0f, -39.0f));
  star.AddLineTo(Vector2(87.0f, 45.0f));
  star.AddLineTo(Vector2(107.0f, 166.0f));
  star.AddLineTo(Vector2(-1.0f, 110.0f));
  star.AddLineTo(Vector2(-103.0f, 166.0f));
  star.AddLineTo(Vector2(-88.0f, 46.0f));
  star.AddLineTo(Vector2(-174.0f, -38.0f));
  star.AddLineTo(Vector2(-54.0f, -56.0f));
  star.Close();
  return star;
}

/**
 * @brief Create shape that have star path.
 */
Dali::CanvasRenderer::Shape MakeStar2Shape()
{
  Dali::CanvasRenderer::Shape star = Dali::CanvasRenderer::Shape::New();
  star.AddMoveTo(Vector2(535.0f, 135.0f));
  star.AddLineTo(Vector2(660.0f, 455.0f));
  star.AddLineTo(Vector2(355.0f, 250.0f));
  star.AddLineTo(Vector2(715.0f, 250.0f));
  star.AddLineTo(Vector2(410.0f, 455.0f));
  star.Close();
  return star;
}

} // namespace
/**
 * @brief This demonstrates how to display and control vector primitives using CanvasView.
 *
 * - It displays various types of shapes. Rectangle, circle, path, etc.
 * - Each shape can be set to fill color, stroke color, width, etc
 *   and can change the transfomation(rotate, scale, translate)
 */
class CanvasViewController : public ConnectionTracker
{
public:
  /**
   * @brief Constructor.
   * @param[in]  application  A reference to the Application class
   */
  CanvasViewController(Application& application)
  : mApplication(application),
    mCanvasView(),
    mRoundedRect(),
    mArc(),
    mStar(),
    mTimer(),
    mCount(0),
    mViewCount(BASIC_SHAPE_VIEW),
    mLinearGradient()
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &CanvasViewController::Create);
  }

  ~CanvasViewController() = default;

  /**
   * @brief Create a view for basic shape example.
   */
  void CreateBasicShape()
  {
    Window  window     = mApplication.GetWindow();
    Vector2 windowSize = window.GetSize();

    mTitle.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, VIEW_LABEL[BASIC_SHAPE_VIEW]);

    Dali::CanvasRenderer::Shape canvasBackground = Dali::CanvasRenderer::Shape::New();
    canvasBackground.AddRect(Rect<float>(0.0f, 0.0f, windowSize.width, windowSize.height), Vector2::ZERO);
    canvasBackground.SetFillColor(Color::WHITE);
    mCanvasView.AddDrawable(canvasBackground);

    Dali::CanvasRenderer::Shape shape1 = Dali::CanvasRenderer::Shape::New();
    shape1.AddRect(Rect<float>(-50.0f, -50.0f, 100.0f, 100.0f), Vector2::ZERO);
    shape1.SetFillColor(Vector4(0.0f, 0.5f, 0.0f, 0.5f));
    shape1.SetStrokeColor(Vector4(0.5f, 0.0f, 0.0f, 0.5f));
    shape1.SetStrokeWidth(10.0f);
    shape1.Scale(1.2f);
    shape1.Rotate(Degree(45.0f));
    shape1.Translate(Vector2(100.0f, 200.0f));

    mRoundedRect = Dali::CanvasRenderer::Shape::New();
    mRoundedRect.AddRect(Rect<float>(10.0f, 350.0f, 200.0f, 140.0f), Vector2(40.0f, 40.0f));
    mRoundedRect.SetFillColor(Color::BLUE);
    mRoundedRect.SetStrokeColor(Color::YELLOW);
    mRoundedRect.SetStrokeWidth(10.0f);
    mRoundedRect.SetStrokeJoin(Dali::CanvasRenderer::Shape::StrokeJoin::MITER);
    mRoundedRect.SetOpacity(0.5f);
    Dali::Vector<float> dashPattern;
    for(auto& pattern : COMMON_STROKE_DASH_PATTERN)
    {
      dashPattern.PushBack(pattern);
    }
    mRoundedRect.SetStrokeDash(dashPattern);

    Dali::CanvasRenderer::Shape shape2 = MakeStar2Shape();
    shape2.SetFillColor(Color::RED);
    shape2.SetOpacity(0.5f);
    shape2.SetStrokeColor(Color::MAGENTA);
    shape2.SetStrokeWidth(10.0f);
    shape2.SetFillRule(Dali::CanvasRenderer::Shape::FillRule::EVEN_ODD);
    shape2.SetStrokeJoin(Dali::CanvasRenderer::Shape::StrokeJoin::ROUND);
    shape2.Transform(Matrix3(0.6f, 0.0f, 20.0f, 0.0f, 0.6f, 50.0f, 0.0f, 0.0f, 1.0f));

    mArc = Dali::CanvasRenderer::Shape::New();
    mArc.AddArc(Vector2(100.0f, 650.0f), 80.0f, 10.0f, 0.0f, true);
    mArc.AddArc(Vector2(100.0f, 650.0f), 80.0f, 10.0f, 0.0f, true);
    mArc.SetOpacity(0.5f);
    mArc.SetStrokeColor(Color::LIME);
    mArc.SetStrokeWidth(10.0f);
    mArc.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::ROUND);
    mCanvasView.AddDrawable(mArc);

    mStar = MakeStarShape();
    mStar.SetFillColor(Color::CYAN);
    mStar.SetStrokeColor(Color::DARK_GRAY);
    mStar.SetStrokeWidth(30.0f);
    mStar.Scale(0.6f);
    mStar.Translate(Vector2(350.0f, 500.0f));

    mCanvasView.AddDrawable(mStar);

    mGroup1 = Dali::CanvasRenderer::DrawableGroup::New();

    mGroup1.AddDrawable(mRoundedRect);
    mGroup1.AddDrawable(shape1);

    mGroup2 = Dali::CanvasRenderer::DrawableGroup::New();

    mGroup2.AddDrawable(mGroup1);
    mGroup2.AddDrawable(shape2);

    mCanvasView.AddDrawable(mGroup2);

    mTimer.Start();

    window.Add(mCanvasView);
  }

  /**
   * @brief Create a view for clip-path and mask example.
   */
  void CreateClipPathAndMask()
  {
    Window  window     = mApplication.GetWindow();
    Vector2 windowSize = window.GetSize();

    mTitle.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, VIEW_LABEL[CLIP_PATH_VIEW]);

    Dali::CanvasRenderer::Shape canvasBackground = Dali::CanvasRenderer::Shape::New();
    canvasBackground.AddRect(Rect<float>(0.0f, 0.0f, windowSize.width, windowSize.height), Vector2::ZERO);
    canvasBackground.SetFillColor(Color::WHITE);
    mCanvasView.AddDrawable(canvasBackground);

    Dali::CanvasRenderer::Shape shape1 = Dali::CanvasRenderer::Shape::New();
    shape1.AddRect(Rect<float>(-50.0f, -50.0f, 100.0f, 100.0f), Vector2::ZERO);
    shape1.SetFillColor(Vector4(0.0f, 0.5f, 0.0f, 0.5f));
    shape1.SetStrokeColor(Vector4(0.5f, 0.0f, 0.0f, 0.5f));
    shape1.SetStrokeWidth(10.0f);
    shape1.Scale(1.2f);
    shape1.Rotate(Degree(45.0f));
    shape1.Translate(Vector2(100.0f, 200.0f));

    Dali::CanvasRenderer::Shape shape2 = MakeStar2Shape();
    shape2.SetFillRule(Dali::CanvasRenderer::Shape::FillRule::EVEN_ODD);
    shape2.SetFillColor(Color::RED);
    shape2.SetOpacity(0.5f);
    shape2.SetStrokeColor(Color::MAGENTA);
    shape2.SetStrokeWidth(10.0f);
    shape2.SetStrokeJoin(Dali::CanvasRenderer::Shape::StrokeJoin::ROUND);
    shape2.Transform(Matrix3(0.6f, 0.0f, 20.0f, 0.0f, 0.6f, 50.0f, 0.0f, 0.0f, 1.0f));

    mStar = MakeStarShape();
    mStar.Scale(0.6f);
    mStar.SetFillColor(Color::CYAN);
    mStar.Translate(Vector2(240.0f, 240.0f));

    Dali::CanvasRenderer::DrawableGroup group1;
    group1 = Dali::CanvasRenderer::DrawableGroup::New();
    group1.AddDrawable(shape1);
    group1.AddDrawable(shape2);
    group1.SetClipPath(mStar);
    mCanvasView.AddDrawable(group1);

    Dali::CanvasRenderer::Shape shape3 = Dali::CanvasRenderer::Shape::New();
    shape3.AddRect(Rect<float>(140.0f, 500.0f, 200.0f, 140.0f), Vector2(40.0f, 40.0f));
    shape3.SetFillColor(Color::BLUE);
    shape3.SetOpacity(0.5f);
    shape3.SetStrokeColor(Color::YELLOW);
    shape3.SetStrokeWidth(10.0f);
    shape3.SetStrokeJoin(Dali::CanvasRenderer::Shape::StrokeJoin::MITER);
    Dali::Vector<float> dashPattern;
    for(auto& pattern : COMMON_STROKE_DASH_PATTERN)
    {
      dashPattern.PushBack(pattern);
    }
    shape3.SetStrokeDash(dashPattern);

    mArc = Dali::CanvasRenderer::Shape::New();
    mArc.AddArc(Vector2(240.0f, 550.0f), 100.0f, 10.0f, 0.0f, true);
    mArc.AddArc(Vector2(240.0f, 550.0f), 100.0f, 10.0f, 0.0f, true);
    mArc.SetOpacity(0.5f);
    mArc.SetFillColor(Color::GREEN);

    shape3.SetMask(mArc, Dali::CanvasRenderer::Drawable::MaskType::ALPHA);
    mCanvasView.AddDrawable(shape3);

    mTimer.Start();

    window.Add(mCanvasView);
  }

  /**
   * @brief Create a view for gradient example.
   */
  void CreateGradient()
  {
    Window  window     = mApplication.GetWindow();
    Vector2 windowSize = window.GetSize();
    mTitle.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, VIEW_LABEL[GRADIENT_VIEW]);

    Dali::CanvasRenderer::Shape canvasBackground = Dali::CanvasRenderer::Shape::New();
    canvasBackground.AddRect(Rect<float>(0.0f, 0.0f, windowSize.width, windowSize.height), Vector2::ZERO);
    canvasBackground.SetFillColor(Color::WHITE);
    mCanvasView.AddDrawable(canvasBackground);

    Dali::Vector<float> dashPattern;
    for(auto& pattern : COMMON_STROKE_DASH_PATTERN)
    {
      dashPattern.PushBack(pattern);
    }

    Dali::Vector<Dali::CanvasRenderer::Gradient::ColorStop> radialGradientStops;
    radialGradientStops.PushBack({0.0f, Color::LIGHT_SEA_GREEN});
    radialGradientStops.PushBack({1.0f, Color::BLUE_VIOLET});

    Dali::CanvasRenderer::RadialGradient radialGradient = Dali::CanvasRenderer::RadialGradient::New();
    radialGradient.SetBounds(Vector2(240, 170), 70);
    radialGradient.SetColorStops(radialGradientStops);
    radialGradient.SetSpread(Dali::CanvasRenderer::Gradient::Spread::REFLECT);

    Dali::CanvasRenderer::Shape roundedRect = Dali::CanvasRenderer::Shape::New();
    roundedRect.AddRect(Rect<float>(50.0f, 100.0f, 380.0f, 140.0f), Vector2(40.0f, 40.0f));
    roundedRect.SetStrokeColor(Color::YELLOW);
    roundedRect.SetStrokeWidth(10.0f);
    roundedRect.SetStrokeJoin(Dali::CanvasRenderer::Shape::StrokeJoin::MITER);
    roundedRect.SetStrokeDash(dashPattern);
    roundedRect.SetFillGradient(radialGradient);
    mCanvasView.AddDrawable(roundedRect);

    Dali::CanvasRenderer::RadialGradient radialGradient2 = Dali::CanvasRenderer::RadialGradient::New();
    radialGradient2.SetBounds(Vector2(240, 320), 70);
    radialGradient2.SetColorStops(radialGradientStops);
    radialGradient2.SetSpread(Dali::CanvasRenderer::Gradient::Spread::REPEAT);

    Dali::CanvasRenderer::Shape roundedRect2 = Dali::CanvasRenderer::Shape::New();
    roundedRect2.AddRect(Rect<float>(50.0f, 250.0f, 380.0f, 140.0f), Vector2(40.0f, 40.0f));
    roundedRect2.SetStrokeColor(Color::YELLOW);
    roundedRect2.SetStrokeWidth(10.0f);
    roundedRect2.SetStrokeJoin(Dali::CanvasRenderer::Shape::StrokeJoin::MITER);
    roundedRect2.SetStrokeDash(dashPattern);
    roundedRect2.SetFillGradient(radialGradient2);
    mCanvasView.AddDrawable(roundedRect2);

    Dali::Vector<Dali::CanvasRenderer::Gradient::ColorStop> stops;
    stops.PushBack({0.0f, Color::RED});
    stops.PushBack({0.5f, Color::BLUE});
    stops.PushBack({1.0f, Color::GREEN});

    mLinearGradient = Dali::CanvasRenderer::LinearGradient::New();
    mLinearGradient.SetBounds(Vector2(-100, -100), Vector2(100, 100));
    mLinearGradient.SetColorStops(stops);

    Dali::Vector<Dali::CanvasRenderer::Gradient::ColorStop> strokeLinearGradientStops;
    strokeLinearGradientStops.PushBack({0.0f, Color::SLATE_BLUE});
    strokeLinearGradientStops.PushBack({1.0f, Color::MAROON});

    Dali::CanvasRenderer::LinearGradient strokeLinearGradient = Dali::CanvasRenderer::LinearGradient::New();
    strokeLinearGradient.SetBounds(Vector2(-100, -100), Vector2(100, 100));
    strokeLinearGradient.SetColorStops(strokeLinearGradientStops);

    Dali::CanvasRenderer::Shape star = MakeStarShape();
    star.SetStrokeWidth(30.0f);
    star.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::ROUND);
    star.Translate(Vector2(240.0f, 600.0f));
    star.SetFillGradient(mLinearGradient);
    star.SetStrokeGradient(strokeLinearGradient);
    mCanvasView.AddDrawable(star);

    window.Add(mCanvasView);

    mTimer.Start();
  }

  /**
   * @brief Create a view for picture example.
   */
  void CreatePicture()
  {
    Window  window     = mApplication.GetWindow();
    Vector2 windowSize = window.GetSize();
    mTitle.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, VIEW_LABEL[PICTURE_VIEW]);

    Dali::CanvasRenderer::Shape canvasBackground = Dali::CanvasRenderer::Shape::New();
    canvasBackground.AddRect(Rect<float>(0.0f, 0.0f, windowSize.width, windowSize.height), Vector2::ZERO);
    canvasBackground.SetFillColor(Color::WHITE);
    mCanvasView.AddDrawable(canvasBackground);

    Dali::CanvasRenderer::Picture picture = Dali::CanvasRenderer::Picture::New();
    picture.Load(IMAGES[0]);
    picture.Translate(Vector2(100, 100));
    picture.SetSize(Vector2(200, 200));
    mCanvasView.AddDrawable(picture);

    window.Add(mCanvasView);
  }

  /**
   * @brief Clean up the specified number of view.
   * @param[in] viewCnt The number of view.
   */
  void ResetView(int viewCnt)
  {
    Window window = mApplication.GetWindow();
    mCanvasView.RemoveAllDrawables();
    window.Remove(mCanvasView);
    mTimer.Stop();
  }

  /**
   * @brief Change to the specified number of view.
   * @param[in] viewCnt The number of view.
   */
  void ChangeView(int viewCnt)
  {
    switch(viewCnt)
    {
      case BASIC_SHAPE_VIEW:
      {
        CreateBasicShape();
        break;
      }
      case CLIP_PATH_VIEW:
      {
        CreateClipPathAndMask();
        break;
      }
      case GRADIENT_VIEW:
      {
        CreateGradient();
        break;
      }
      case PICTURE_VIEW:
      {
        CreatePicture();
        break;
      }
    }
    mPrevButton.RaiseToTop();
    mNextButton.RaiseToTop();
    mTitle.RaiseToTop();
  }

  /**
   * @brief Create a view and set up the button and timer, and title.
   */
  void Create(Application& application)
  {
    // The Init signal is received once (only) during the Application lifetime
    Window  window     = application.GetWindow();
    Vector2 windowSize = window.GetSize();
    window.KeyEventSignal().Connect(this, &CanvasViewController::OnKeyEvent);

    mPrevButton = Toolkit::PushButton::New();
    mPrevButton.SetProperty(Toolkit::Button::Property::LABEL, PREV_BUTTON_TEXT);
    mPrevButton.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mPrevButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    window.Add(mPrevButton);
    mPrevButton.ClickedSignal().Connect(this, &CanvasViewController::OnPrevButtonClicked);

    mNextButton = Toolkit::PushButton::New();
    mNextButton.SetProperty(Toolkit::Button::Property::LABEL, LEFT_BUTTON_TEXT);
    mNextButton.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_RIGHT);
    mNextButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_RIGHT);
    window.Add(mNextButton);
    mNextButton.ClickedSignal().Connect(this, &CanvasViewController::OnNextButtonClicked);

    mCanvasView = Toolkit::CanvasView::New();
    mCanvasView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mCanvasView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mCanvasView.SetProperty(Actor::Property::SIZE, windowSize);

    mTitle                                                     = Toolkit::TextLabel::New(VIEW_LABEL[BASIC_SHAPE_VIEW]);
    mTitle[Actor::Property::ANCHOR_POINT]                      = AnchorPoint::TOP_CENTER;
    mTitle[Actor::Property::PARENT_ORIGIN]                     = Vector3(0.5f, 0.0f, 0.5f);
    mTitle[Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT] = "CENTER";
    mTitle[Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT]   = "CENTER";
    mTitle[Toolkit::TextLabel::Property::TEXT_COLOR]           = Color::BLACK;
    window.Add(mTitle);

    mTimer = Timer::New(1000.0f / 32.0f);
    mTimer.TickSignal().Connect(this, &CanvasViewController::tick);

    ChangeView(mViewCount);

    mPrevButton.RaiseToTop();
    mNextButton.RaiseToTop();
    mTitle.RaiseToTop();
  }

  /**
   * @brief Called when prev button clicked.
   *
   * Change to previous view.
   */
  bool OnPrevButtonClicked(Toolkit::Button button)
  {
    ResetView(mViewCount);
    mViewCount--;
    if(mViewCount < BASIC_SHAPE_VIEW)
    {
      mViewCount = NUMBER_OF_VIEWS - 1;
    }
    ChangeView(mViewCount);
    return true;
  }

  /**
   * @brief Called when prev button clicked.
   *
   * Change to next view.
   */
  bool OnNextButtonClicked(Toolkit::Button button)
  {
    ResetView(mViewCount);
    mViewCount++;
    if(mViewCount >= NUMBER_OF_VIEWS)
    {
      mViewCount = 0;
    }
    ChangeView(mViewCount);
    return true;
  }

  /**
   * @brief Called when set time.
   *
   * Change transformation every call to make it look like vector animation.
   */
  bool tick()
  {
    if(mViewCount == BASIC_SHAPE_VIEW)
    {
      mRoundedRect.ResetPath();
      mRoundedRect.AddRect(Rect<float>(10.0f, 350.0f, 200.0f, 140.0f), Vector2(float(mCount % 80), float(mCount % 80)));

      mArc.ResetPath();
      mArc.AddArc(Vector2(100.0f, 650.0f), 80.0f, 10.0f, float(mCount % 180), true);
      mArc.AddArc(Vector2(100.0f, 650.0f), 80.0f, 10.0f + float(mCount % 180), float(mCount % 180) / 2.0f, true);

      mStar.Rotate(Degree(mCount * 2.0f));
      mStar.Scale(0.6f + (!((mCount / 100) & 0x1) ? float(mCount % 100) * 0.01f : 1 - (float(mCount % 100) * 0.01f)));

      mGroup1.Scale(float(mCount % 50) * 0.005f + 0.8f);

      mGroup2.SetOpacity(1.0f - float(mCount % 50) * 0.015f);
    }
    else if(mViewCount == CLIP_PATH_VIEW)
    {
      mArc.ResetPath();
      mArc.AddArc(Vector2(240.0f, 550.0f), 100.0f, 10.0f, float(mCount % 180), true);
      mArc.AddArc(Vector2(240.0f, 550.0f), 100.0f, 10.0f + float(mCount % 180), float(mCount % 180) / 2.0f, true);

      mStar.Rotate(Degree(mCount * 2.0f));
      mStar.Scale(0.6f + (!((mCount / 100) & 0x1) ? float(mCount % 100) * 0.01f : 1 - (float(mCount % 100) * 0.01f)));
    }
    else if(mViewCount == GRADIENT_VIEW)
    {
      Dali::Vector<Dali::CanvasRenderer::Gradient::ColorStop> stops;
      stops.PushBack({0.0f, Vector4(1.0f, 0.0f, 0.0f, 1.0f)});
      stops.PushBack({!((mCount / 100) & 0x1) ? float(mCount % 100) / 100.0f : 1 - (float(mCount % 100) / 100.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)});
      stops.PushBack({1.0f, Vector4(0.0f, 1.0f, 1.0f, 1.0f)});

      mLinearGradient.SetColorStops(stops);
    }
    mCount++;
    return true;
  }

  /**
   * @brief Called when any key event is received.
   *
   * @param[in] event The key event information
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
      else if(IsKey(event, Dali::DALI_KEY_CURSOR_LEFT))
      {
        ResetView(mViewCount);
        mViewCount--;
        if(mViewCount < BASIC_SHAPE_VIEW)
        {
          mViewCount = NUMBER_OF_VIEWS - 1;
        }
        ChangeView(mViewCount);
      }
      else if(IsKey(event, Dali::DALI_KEY_CURSOR_RIGHT))
      {
        ResetView(mViewCount);
        mViewCount++;
        if(mViewCount >= NUMBER_OF_VIEWS)
        {
          mViewCount = BASIC_SHAPE_VIEW;
        }
        ChangeView(mViewCount);
      }
    }
  }

private:
  Application&                         mApplication;
  Toolkit::CanvasView                  mCanvasView;
  Dali::CanvasRenderer::Shape          mRoundedRect;
  Dali::CanvasRenderer::Shape          mArc;
  Dali::CanvasRenderer::Shape          mStar;
  Dali::CanvasRenderer::DrawableGroup  mGroup1;
  Dali::CanvasRenderer::DrawableGroup  mGroup2;
  Timer                                mTimer;
  int                                  mCount;
  int                                  mViewCount;
  Dali::CanvasRenderer::LinearGradient mLinearGradient;
  Toolkit::PushButton                  mPrevButton;
  Toolkit::PushButton                  mNextButton;
  Toolkit::TextLabel                   mTitle;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application          application = Application::New(&argc, &argv);
  CanvasViewController test(application);
  application.MainLoop();
  return 0;
}
