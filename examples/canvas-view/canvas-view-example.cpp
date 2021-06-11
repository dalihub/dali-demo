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
#include <dali/devel-api/adaptor-framework/canvas-renderer-shape.h>

using namespace Dali;

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
    mRoundedRect(),
    mArc(),
    mStar(),
    mTimer(),
    mCount(0)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &CanvasViewController::Create);
  }

  ~CanvasViewController() = default;

  void Create(Application& application)
  {
    // The Init signal is received once (only) during the Application lifetime
    Window  window     = application.GetWindow();
    Vector2 windowSize = window.GetSize();
    window.KeyEventSignal().Connect(this, &CanvasViewController::OnKeyEvent);

    Toolkit::CanvasView mCanvasView = Toolkit::CanvasView::New(windowSize);
    mCanvasView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mCanvasView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mCanvasView.SetProperty(Actor::Property::SIZE, windowSize);

    Dali::CanvasRenderer::Shape canvasBackground = Dali::CanvasRenderer::Shape::New();
    canvasBackground.AddRect(Rect<float>(0.0f, 0.0f, windowSize.width, windowSize.height), Vector2::ZERO);
    canvasBackground.SetFillColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    mCanvasView.AddDrawable(canvasBackground);

    Dali::CanvasRenderer::Shape shape1 = Dali::CanvasRenderer::Shape::New();
    shape1.AddRect(Rect<float>(-50.0f, -50.0f, 100.0f, 100.0f), Vector2::ZERO);
    shape1.SetFillColor(Vector4(0.0f, 0.5f, 0.0f, 0.5f));
    shape1.SetStrokeColor(Vector4(0.5f, 0.0f, 0.0f, 0.5f));
    shape1.SetStrokeWidth(10.0f);
    shape1.Scale(1.2f);
    shape1.Rotate(Degree(45.0f));
    shape1.Translate(Vector2(100.0f, 100.0f));

    mRoundedRect = Dali::CanvasRenderer::Shape::New();
    mRoundedRect.AddRect(Rect<float>(10.0f, 350.0f, 200.0f, 140.0f), Vector2(40.0f, 40.0f));
    mRoundedRect.SetFillColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    mRoundedRect.SetOpacity(0.5f);
    mRoundedRect.SetStrokeColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    mRoundedRect.SetStrokeWidth(10.0f);
    mRoundedRect.SetStrokeJoin(Dali::CanvasRenderer::Shape::StrokeJoin::MITER);
    Dali::Vector<float> dashPattern;
    dashPattern.PushBack(15.0f);
    dashPattern.PushBack(30.0f);
    mRoundedRect.SetStrokeDash(dashPattern);

    Dali::CanvasRenderer::Shape shape2 = Dali::CanvasRenderer::Shape::New();
    shape2.AddMoveTo(Vector2(535.0f, 135.0f));
    shape2.AddLineTo(Vector2(660.0f, 455.0f));
    shape2.AddLineTo(Vector2(355.0f, 250.0f));
    shape2.AddLineTo(Vector2(715.0f, 250.0f));
    shape2.AddLineTo(Vector2(410.0f, 455.0f));
    shape2.Close();
    shape2.SetFillRule(Dali::CanvasRenderer::Shape::FillRule::EVEN_ODD);
    shape2.SetFillColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    shape2.SetOpacity(0.5f);
    shape2.SetStrokeColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
    shape2.SetStrokeWidth(20.0f);
    shape2.SetStrokeJoin(Dali::CanvasRenderer::Shape::StrokeJoin::ROUND);
    shape2.Transform(Matrix3(0.6f, 0.0f, 20.0f, 0.0f, 0.6f, -50.0f, 0.0f, 0.0f, 1.0f));

    mArc = Dali::CanvasRenderer::Shape::New();
    mArc.AddArc(Vector2(100.0f, 650.0f), 80.0f, 10.0f, 0.0f, true);
    mArc.AddArc(Vector2(100.0f, 650.0f), 80.0f, 10.0f, 0.0f, true);
    mArc.SetOpacity(0.5f);
    mArc.SetStrokeColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    mArc.SetStrokeWidth(10.0f);
    mArc.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::ROUND);
    mCanvasView.AddDrawable(mArc);

    mStar = Dali::CanvasRenderer::Shape::New();
    mStar.AddMoveTo(Vector2(-1.0f, -165.0f));
    mStar.AddLineTo(Vector2(53.0f, -56.0f));
    mStar.AddLineTo(Vector2(174.0f, -39.0f));
    mStar.AddLineTo(Vector2(87.0f, 45.0f));
    mStar.AddLineTo(Vector2(107.0f, 166.0f));
    mStar.AddLineTo(Vector2(-1.0f, 110.0f));
    mStar.AddLineTo(Vector2(-103.0f, 166.0f));
    mStar.AddLineTo(Vector2(-88.0f, 46.0f));
    mStar.AddLineTo(Vector2(-174.0f, -38.0f));
    mStar.AddLineTo(Vector2(-54.0f, -56.0f));

    mStar.Close();

    mStar.SetFillColor(Vector4(0.0f, 1.0f, 1.0f, 1.0f));
    mStar.SetStrokeColor(Vector4(0.5f, 1.0f, 0.5f, 1.0f));
    mStar.SetStrokeWidth(30.0f);
    mStar.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::ROUND);
    mStar.Scale(0.6f);
    mStar.Translate(Vector2(350.0f, 450.0f));
    mStar.SetOpacity(0.5f);

    mCanvasView.AddDrawable(mStar);

    mGroup1 = Dali::CanvasRenderer::DrawableGroup::New();

    mGroup1.AddDrawable(mRoundedRect);
    mGroup1.AddDrawable(shape1);

    mGroup2 = Dali::CanvasRenderer::DrawableGroup::New();

    mGroup2.AddDrawable(mGroup1);
    mGroup2.AddDrawable(shape2);

    mCanvasView.AddDrawable(mGroup2);

    mTimer = Timer::New(1000.0f / 32.0f);
    mTimer.TickSignal().Connect(this, &CanvasViewController::tick);
    mTimer.Start();

    window.Add(mCanvasView);
  }

  /**
   * @brief Called when set time.
   *
   * Change transformation every call to make it look like vector animation.
   */
  bool tick()
  {
    mRoundedRect.ResetPath();
    mRoundedRect.AddRect(Rect<float>(10.0f, 350.0f, 200.0f, 140.0f), Vector2(float(mCount % 80), float(mCount % 80)));

    mArc.ResetPath();
    mArc.AddArc(Vector2(100.0f, 650.0f), 80.0f, 10.0f, float(mCount % 180), true);
    mArc.AddArc(Vector2(100.0f, 650.0f), 80.0f, 10.0f + float(mCount % 180), float(mCount % 180) / 2.0f, true);

    mStar.Rotate(Degree(mCount * 2.0f));
    mStar.Scale(float(mCount % 100) * 0.01f + 0.6f);

    mGroup1.Scale(float(mCount % 50) * 0.005f + 0.8f);

    mGroup2.SetOpacity(1.0f - float(mCount % 50) * 0.015f);

    mCount++;
    return true;
  }

  /**
   * @brief Called when any key event is received.
   *
   * Will use this to quit the application if Back or the Escape key is received
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
    }
  }

private:
  Application&                        mApplication;
  Dali::CanvasRenderer::Shape         mRoundedRect;
  Dali::CanvasRenderer::Shape         mArc;
  Dali::CanvasRenderer::Shape         mStar;
  Dali::CanvasRenderer::DrawableGroup mGroup1;
  Dali::CanvasRenderer::DrawableGroup mGroup2;
  Timer                               mTimer;
  int                                 mCount;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application          application = Application::New(&argc, &argv);
  CanvasViewController test(application);
  application.MainLoop();
  return 0;
}
