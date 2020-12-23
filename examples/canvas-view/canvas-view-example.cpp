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
#include <dali-toolkit/devel-api/controls/canvas-view/canvas-view.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer-shape.h>

using namespace Dali;

class CanvasViewController : public ConnectionTracker
{
public:
  CanvasViewController(Application& application)
  : mApplication(application),
    mShape(),
    mDummyShape(),
    mTempTimer(),
    mCount(0)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &CanvasViewController::Create);
  }

  ~CanvasViewController()
  {
    // Nothing to do here
  }

  void Create(Application& application)
  {
    // The Init signal is received once (only) during the Application lifetime
    Window  window     = application.GetWindow();
    Vector2 windowSize = window.GetSize();

    Toolkit::CanvasView mCanvasView = Toolkit::CanvasView::New(windowSize);
    mCanvasView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mCanvasView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mCanvasView.SetProperty(Actor::Property::SIZE, windowSize);

    Dali::CanvasRenderer::Shape canvasBackground = Dali::CanvasRenderer::Shape::New();
    canvasBackground.AddRect(Rect<float>(0.0f, 0.0f, windowSize.width, windowSize.height), Vector2(0, 0));
    canvasBackground.SetFillColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    mCanvasView.AddDrawable(canvasBackground);

    Dali::CanvasRenderer::Shape shape1 = Dali::CanvasRenderer::Shape::New();
    shape1.AddRect(Rect<float>(-50.0f, -50.0f, 100.0f, 100.0f), Vector2(0.0f, 0.0f));
    shape1.SetFillColor(Vector4(0.0f, 0.5f, 0.0f, 0.5f));
    shape1.SetStrokeColor(Vector4(0.5f, 0.0f, 0.0f, 0.5f));
    shape1.SetStrokeWidth(10.0f);
    shape1.Scale(1.2f);
    shape1.Rotate(Degree(45.0f));
    shape1.Translate(Vector2(100.0f, 100.0f));

    mCanvasView.AddDrawable(shape1);

    Dali::CanvasRenderer::Shape shape2 = Dali::CanvasRenderer::Shape::New();
    shape2.AddRect(Rect<float>(10.0f, 350.0f, 200.0f, 140.0f), Vector2(40.0f, 40.0f));
    shape2.SetFillColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    shape2.SetOpacity(0.5f);
    shape2.SetStrokeColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    shape2.SetStrokeWidth(10.0f);
    shape2.SetStrokeJoin(Dali::CanvasRenderer::Shape::StrokeJoin::MITER);
    Dali::Vector<float> dashPattern;
    dashPattern.PushBack(15.0f);
    dashPattern.PushBack(30.0f);
    shape2.SetStrokeDash(dashPattern);
    mCanvasView.AddDrawable(shape2);

    Dali::CanvasRenderer::Shape shape3 = Dali::CanvasRenderer::Shape::New();
    shape3.AddMoveTo(Vector2(535.0f, 135.0f));
    shape3.AddLineTo(Vector2(660.0f, 455.0f));
    shape3.AddLineTo(Vector2(355.0f, 250.0f));
    shape3.AddLineTo(Vector2(715.0f, 250.0f));
    shape3.AddLineTo(Vector2(410.0f, 455.0f));
    shape3.Close();
    shape3.SetFillRule(Dali::CanvasRenderer::Shape::FillRule::EVEN_ODD);
    shape3.SetFillColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    shape3.SetOpacity(0.5f);
    shape3.SetStrokeColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
    shape3.SetStrokeWidth(20.0f);
    shape3.SetStrokeJoin(Dali::CanvasRenderer::Shape::StrokeJoin::ROUND);
    shape3.Transform(Matrix3(0.6f, 0.0f, 20.0f, 0.0f, 0.6f, -50.0f, 0.0f, 0.0f, 1.0f));
    mCanvasView.AddDrawable(shape3);

    Dali::CanvasRenderer::Shape shape4 = Dali::CanvasRenderer::Shape::New();
    shape4.AddArc(Vector2(100.0f, 650.0f), 80.0f, 10.0f, 120.0f, true);
    shape4.SetOpacity(0.5f);
    shape4.SetStrokeColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    shape4.SetStrokeWidth(10.0f);
    shape4.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::ROUND);
    mCanvasView.AddDrawable(shape4);

    mShape = Dali::CanvasRenderer::Shape::New();
    mShape.AddMoveTo(Vector2(-1.0f, -165.0f));
    mShape.AddLineTo(Vector2(53.0f, -56.0f));
    mShape.AddLineTo(Vector2(174.0f, -39.0f));
    mShape.AddLineTo(Vector2(87.0f, 45.0f));
    mShape.AddLineTo(Vector2(107.0f, 166.0f));
    mShape.AddLineTo(Vector2(-1.0f, 110.0f));
    mShape.AddLineTo(Vector2(-103.0f, 166.0f));
    mShape.AddLineTo(Vector2(-88.0f, 46.0f));
    mShape.AddLineTo(Vector2(-174.0f, -38.0f));
    mShape.AddLineTo(Vector2(-54.0f, -56.0f));

    mShape.Close();

    mShape.SetFillColor(Vector4(0.0f, 1.0f, 1.0f, 1.0f));
    mShape.SetStrokeColor(Vector4(0.5f, 1.0f, 0.5f, 1.0f));
    mShape.SetStrokeWidth(30.0f);
    mShape.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::ROUND);
    mShape.Scale(0.6f);
    mShape.Translate(Vector2(350.0f, 450.0f));
    mShape.SetOpacity(0.5f);

    mCanvasView.AddDrawable(mShape);

    Dali::CanvasRenderer::Shape dummyShape = Dali::CanvasRenderer::Shape::New();
    dummyShape.AddRect(Rect<float>(10.0f, 350.0f, 200.0f, 140.0f), Vector2(40.0f, 40.0f));
    dummyShape.SetStrokeColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    dummyShape.SetStrokeWidth(10.0f);
    //This dummy shape don't added to CanvasView.
    //mCanvasView.AddDrawable(dummyShape);

    mDummyShape = Dali::CanvasRenderer::Shape::New();
    mDummyShape.AddRect(Rect<float>(10.0f, 350.0f, 200.0f, 140.0f), Vector2(40.0f, 40.0f));
    mDummyShape.SetStrokeColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    mDummyShape.SetStrokeWidth(10.0f);
    //This dummy shape don't added to CanvasView.
    //mCanvasView.AddDrawable(mDummyShape);

    mTempTimer = Timer::New(1000.0f / 32.0f);
    mTempTimer.TickSignal().Connect(this, &CanvasViewController::tick);
    mTempTimer.Start();

    window.Add(mCanvasView);
  }

  bool tick()
  {
    mShape.Rotate(Degree(mCount * 2.0f));
    mShape.Scale(float(mCount % 100) * 0.01f + 0.6f);
    mCount++;
    return true;
  }

private:
  Application&                mApplication;
  Dali::CanvasRenderer::Shape mShape;
  Dali::CanvasRenderer::Shape mDummyShape;
  Timer                       mTempTimer;
  int                         mCount;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application          application = Application::New(&argc, &argv);
  CanvasViewController test(application);
  application.MainLoop();
  return 0;
}
