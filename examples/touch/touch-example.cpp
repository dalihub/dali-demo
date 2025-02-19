/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <dali-scene3d/dali-scene3d.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/window-system-devel.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

// This example shows how does the DALi touch algorithm work.
//
class TouchController : public ConnectionTracker
{
public:
  TouchController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &TouchController::Create);
  }

  ~TouchController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    basicColorMap[Toolkit::Visual::Property::TYPE]                  = Toolkit::Visual::COLOR;
    basicColorMap[Toolkit::ColorVisual::Property::MIX_COLOR]        = Color::RED;
    basicColorMap[Toolkit::DevelVisual::Property::BORDERLINE_WIDTH] = 3;

    notTouchableColorMap[Toolkit::Visual::Property::TYPE]                  = Toolkit::Visual::COLOR;
    notTouchableColorMap[Toolkit::ColorVisual::Property::MIX_COLOR]        = Color::GRAY;
    notTouchableColorMap[Toolkit::DevelVisual::Property::BORDERLINE_WIDTH] = 3;

    touchedColorMap[Toolkit::Visual::Property::TYPE]                  = Toolkit::Visual::COLOR;
    touchedColorMap[Toolkit::ColorVisual::Property::MIX_COLOR]        = Color::BLUE;
    touchedColorMap[Toolkit::DevelVisual::Property::BORDERLINE_WIDTH] = 3;

    mKeyDescription = Dali::Toolkit::TextLabel::New("C : Major Scenario Type Change\n1 ~ 0 : Scenario Detail Change\nG : PropagationType::GEOMETRY\nP : PropagationType::PARENT");
    mKeyDescription.SetProperty(Dali::Toolkit::TextLabel::Property::MULTI_LINE, true);
    mKeyDescription.SetProperty(Dali::Actor::Property::NAME, "mKeyDescription");
    mKeyDescription.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_LEFT);
    mKeyDescription.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    window.Add(mKeyDescription);

    mScenarioTypeLabelString    = "Single RenderTask";
    mScenarioDetailLabelString  = "Single Object";
    mPropagationTypeLabelString = "Propagation::PARENT";

    mScenarioTypeLabel = Dali::Toolkit::TextLabel::New();
    mScenarioTypeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, mScenarioTypeLabelString);
    mScenarioTypeLabel.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mScenarioTypeLabel.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    window.Add(mScenarioTypeLabel);

    mScenarioDetailLabel = Dali::Toolkit::TextLabel::New();
    mScenarioDetailLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, mScenarioDetailLabelString);
    mScenarioDetailLabel.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mScenarioDetailLabel.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    mScenarioTypeLabel.Add(mScenarioDetailLabel);

    mPropagationTypeLabel = Dali::Toolkit::TextLabel::New();
    mPropagationTypeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, mPropagationTypeLabelString);
    mPropagationTypeLabel.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mPropagationTypeLabel.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    mScenarioDetailLabel.Add(mPropagationTypeLabel);

    Toolkit::Control touchableControl = Toolkit::Control::New();
    touchableControl.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, basicColorMap);
    touchableControl.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    touchableControl.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    touchableControl.SetProperty(Dali::Actor::Property::SIZE, 30);
    mPropagationTypeLabel.Add(touchableControl);

    Dali::Toolkit::TextLabel touchableExampleLabel = Dali::Toolkit::TextLabel::New();
    touchableExampleLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "Touchable Control");
    touchableExampleLabel.SetProperty(Dali::Actor::Property::SIZE_WIDTH, 1500.0f);
    touchableExampleLabel.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);
    touchableExampleLabel.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_RIGHT);
    touchableControl.Add(touchableExampleLabel);

    Toolkit::Control notTouchableControl = Toolkit::Control::New();
    notTouchableControl.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, notTouchableColorMap);
    notTouchableControl.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    notTouchableControl.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    notTouchableControl.SetProperty(Dali::Actor::Property::SIZE, 30);
    touchableControl.Add(notTouchableControl);

    Dali::Toolkit::TextLabel notTouchableExampleLabel = Dali::Toolkit::TextLabel::New();
    notTouchableExampleLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "Not Touchable Control (Not Consume, No Touch Event etc.)");
    notTouchableExampleLabel.SetProperty(Dali::Actor::Property::SIZE_WIDTH, 1500.0f);
    notTouchableExampleLabel.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);
    notTouchableExampleLabel.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_RIGHT);
    notTouchableControl.Add(notTouchableExampleLabel);

    Toolkit::Control touchedControl = Toolkit::Control::New();
    touchedControl.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, touchedColorMap);
    touchedControl.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    touchedControl.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    touchedControl.SetProperty(Dali::Actor::Property::SIZE, 30);
    notTouchableControl.Add(touchedControl);

    Dali::Toolkit::TextLabel touchedExampleLabel = Dali::Toolkit::TextLabel::New();
    touchedExampleLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, "Touched Control");
    touchedExampleLabel.SetProperty(Dali::Actor::Property::SIZE_WIDTH, 1500.0f);
    touchedExampleLabel.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);
    touchedExampleLabel.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_RIGHT);
    touchedControl.Add(touchedExampleLabel);

    GenScene1();

    // Respond to a touch anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &TouchController::OnTouchActor);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &TouchController::OnKeyEvent);
  }

  bool OnTouchActor(Actor actor, const TouchEvent& touch)
  {
    // quit the application
    DALI_LOG_ERROR("touched : name : %s, id : %d\n", actor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str(), actor.GetProperty<int>(Dali::Actor::Property::ID));
    Toolkit::Control control = Toolkit::Control::DownCast(actor);
    if(control)
    {
      control.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, touchedColorMap);
    }
    return true;
  }

  bool OnTouch3DActor(Actor actor, const TouchEvent& touch)
  {
    // quit the application
    DALI_LOG_ERROR("3D touched : name : %s, id : %d\n", actor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str(), actor.GetProperty<int>(Dali::Actor::Property::ID));
    Toolkit::Control control = Toolkit::Control::DownCast(actor);
    if(control)
    {
      control.SetBackgroundColor(Color::BLUE);
    }
    return true;
  }

  bool OnTouchActorNotConsume(Actor actor, const TouchEvent& touch)
  {
    // quit the application
    DALI_LOG_ERROR("touched But not consume : %d\n", actor.GetProperty<int>(Dali::Actor::Property::ID));
    return false;
  }

  Dali::Toolkit::Control CreateControl(Dali::Property::Map map, Vector2 position, Vector2 size, std::string name, bool hit = true)
  {
    Toolkit::Control control = Toolkit::Control::New();
    control.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, map);
    control.SetProperty(Dali::Actor::Property::NAME, name);
    control.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    control.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    control.SetProperty(Dali::Actor::Property::POSITION, position);
    control.SetProperty(Dali::Actor::Property::SIZE, size);
    mActorList.push_back(control);
    if(hit)
    {
      control.TouchedSignal().Connect(this, &TouchController::OnTouchActor);
    }

    Dali::Toolkit::TextLabel tag = Dali::Toolkit::TextLabel::New(name);
    tag.SetProperty(Dali::Actor::Property::SIZE_WIDTH, 1500.0f);
    tag.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    tag.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
    control.Add(tag);
    mActorList.push_back(tag);

    return control;
  }

  void GenScene1()
  {
    mScenarioDetailLabelString = "Single Object";

    // Simple Touch
    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control1");
    window.Add(control1);
  }

  void GenScene2()
  {
    mScenarioDetailLabelString = "Simple 2 Object";

    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control1");
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control2");
    window.Add(control2);
  }

  void GenScene3()
  {
    mScenarioDetailLabelString = "Consume Layer between Control1 and Control2";

    // Consuming
    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control1");
    window.Add(control1);

    Layer layer = Layer::New();
    layer.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    layer.SetProperty(Dali::Actor::Property::NAME, "layer");
    layer.SetProperty(Dali::Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    layer.SetProperty(Dali::Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    layer.SetProperty(Dali::Layer::Property::CONSUMES_TOUCH, true);
    window.Add(layer);
    mActorList.push_back(layer);

    Toolkit::Control controlNotHit = CreateControl(notTouchableColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "Consume Layer", false);
    controlNotHit.SetProperty(Dali::Actor::Property::OPACITY, 0.5f);
    controlNotHit.SetProperty(Dali::Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    controlNotHit.SetProperty(Dali::Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    layer.Add(controlNotHit);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control2");
    layer.Add(control2);
  }

  void GenScene4()
  {
    mScenarioDetailLabelString = "Control2 don't consume touch";

    // Propagation
    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 230.0f, "control1");
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(80.0f, 50.0f), Vector2::ONE * 100.0f, "control2 (parent : Control1)");
    control1.Add(control2);

    Toolkit::Control control3 = CreateControl(notTouchableColorMap, Vector2(100.0f, 100.0f), Vector2::ONE * 100.0f, "control3 Not Consume (parent : Control1)", false);
    control3.TouchedSignal().Connect(this, &TouchController::OnTouchActorNotConsume);
    control1.Add(control3);
  }

  void GenScene5()
  {
    mScenarioDetailLabelString = "Clipping on Control1";

    // clipping
    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 180.0f, "control1 with clipping");
    control1.SetProperty(Dali::Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control2 (parent : Control1)");
    control1.Add(control2);

    Toolkit::Control control3 = CreateControl(basicColorMap, Vector2(100.0f, 100.0f), Vector2::ONE * 100.0f, "control3 (parent : Control1)");
    control1.Add(control3);
  }

  void GenScene6()
  {
    mScenarioDetailLabelString = "Control2 in Overlay";

    // overlay
    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2(230.0f, 180.0f), "control1");
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2(100.0f, 150.0f), "overlay control2 (parent : Control1)");
    control2.SetProperty(Dali::Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
    control1.Add(control2);

    Toolkit::Control control3 = CreateControl(basicColorMap, Vector2(100.0f, 100.0f), Vector2::ONE * 100.0f, "control3 (parent : Control1)");
    control1.Add(control3);
  }

  void GenScene7()
  {
    mScenarioDetailLabelString = "CLIP_TO_BOUNDING_BOX Parent and Child Overlay";

    // overlay in clipping
    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 250.0f, "control1 with clipping");
    control1.SetProperty(Dali::Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(100.0f, 50.0f), Vector2::ONE * 150.0f, "overlay control2 (parent : Control1)");
    control2.SetProperty(Dali::Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
    control1.Add(control2);

    Toolkit::Control control3 = CreateControl(basicColorMap, Vector2(20.0f, 100.0f), Vector2::ONE * 100.0f, "control3 (parent : Control1)");
    control1.Add(control3);

    Toolkit::Control control4 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control4 (parent : Control2)");
    control2.Add(control4);
  }

  void GenScene8()
  {
    mScenarioDetailLabelString = "CLIP_CHILDREN Parent and Child Overlay";

    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 250.0f, "control1 with clipping");
    control1.SetProperty(Dali::Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(100.0f, 50.0f), Vector2::ONE * 150.0f, "overlay control2 (parent : Control1)");
    control2.SetProperty(Dali::Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
    control1.Add(control2);

    Toolkit::Control control3 = CreateControl(basicColorMap, Vector2(20.0f, 100.0f), Vector2::ONE * 100.0f, "control3 (parent : Control1)");
    control1.Add(control3);

    Toolkit::Control control4 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control4 (parent : Control2)");
    control2.Add(control4);
  }

  void GenScene9()
  {
    mScenarioDetailLabelString = "Clipping on the Overlay";

    // clipping in overlay
    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 180.0f, "control1");
    control1.SetProperty(Dali::Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "overlay clipping control2 (parent : Control1)");
    control2.SetProperty(Dali::Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
    control2.SetProperty(Dali::Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
    control1.Add(control2);

    Toolkit::Control control3 = CreateControl(basicColorMap, Vector2(20.0f, 100.0f), Vector2::ONE * 100.0f, "control3 (parent : Control1)");
    control1.Add(control3);

    Toolkit::Control control4 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control4 (parent : Control2)");
    control2.Add(control4);
  }

  void GenScene0()
  {
    mScenarioDetailLabelString = "Multiple Overlays control2 and control3 is separated Overlay sibling";

    // multiple overlay
    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 180.0f, "control1 with clipping");
    control1.SetProperty(Dali::Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "overlay control2 (parent : Control1)");
    control2.SetProperty(Dali::Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
    control1.Add(control2);

    Toolkit::Control control3 = CreateControl(basicColorMap, Vector2(20.0f, 120.0f), Vector2::ONE * 100.0f, "overlay control3 (parent : Control1)");
    control3.SetProperty(Dali::Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
    control1.Add(control3);

    Toolkit::Control control4 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "overlay control4 (parent : Control2)");
    control4.SetProperty(Dali::Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
    control2.Add(control4);
  }

  Toolkit::Control AddNewRenderTask(Dali::Actor sourceActor, Dali::Vector2 size, bool useFBO, bool isExclusive, Vector4 clearColor)
  {
    Dali::CameraActor cameraActor            = Dali::CameraActor::New(size);
    float             cameraDefaultZPosition = cameraActor.GetProperty<float>(Dali::Actor::Property::POSITION_Z);
    cameraActor.SetProperty(Dali::Actor::Property::POSITION, Vector3(0.0f, 0.0f, cameraDefaultZPosition));
    cameraActor.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    cameraActor.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mActorList.push_back(cameraActor);
    window.Add(cameraActor);

    RenderTask renderTask = window.GetRenderTaskList().CreateTask();
    renderTask.SetRefreshRate(Dali::RenderTask::REFRESH_ALWAYS);
    renderTask.SetSourceActor(sourceActor);
    renderTask.SetCameraActor(cameraActor);
    renderTask.SetClearColor(clearColor);
    renderTask.SetClearEnabled(true);
    renderTask.SetExclusive(isExclusive);

    Toolkit::Control positionControl;
    if(useFBO)
    {
      positionControl = Toolkit::Control::New();
      positionControl.SetProperty(Dali::Actor::Property::NAME, "Maping Actor");
      positionControl.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      positionControl.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      mActorList.push_back(positionControl);

      positionControl.SetProperty(Dali::Actor::Property::ORIENTATION, Quaternion(Dali::Radian(Dali::Degree(45.0f)), Vector3::YAXIS));

      Dali::Texture     texture     = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, unsigned(size.x), unsigned(size.y));
      Dali::FrameBuffer frameBuffer = Dali::FrameBuffer::New(texture.GetWidth(), texture.GetHeight(), Dali::FrameBuffer::Attachment::DEPTH);
      frameBuffer.AttachColorTexture(texture);
      mFrameBufferList.push_back(frameBuffer);
      mTextureList.push_back(texture);

      renderTask.SetFrameBuffer(frameBuffer);
      renderTask.SetScreenToFrameBufferMappingActor(positionControl);
      renderTask.GetCameraActor().SetInvertYAxis(true);

      Dali::Toolkit::ImageUrl imageUrl = Dali::Toolkit::Image::GenerateUrl(frameBuffer, 0u);
      Property::Map           imagePropertyMap;
      imagePropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
      imagePropertyMap.Insert(Toolkit::ImageVisual::Property::URL, imageUrl.GetUrl());
      imagePropertyMap.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, 30.0f);
      imagePropertyMap.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, 3.0f);
      positionControl.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, imagePropertyMap);
    }
    else
    {
      positionControl = Toolkit::Control::New();
      positionControl.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, basicColorMap);
      positionControl.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      positionControl.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      mActorList.push_back(positionControl);

      renderTask.SetViewportGuideActor(positionControl);
      renderTask.GetCameraActor().SetInvertYAxis(false);
    }

    mRenderTaskList.push_back(renderTask);

    return positionControl;
  }

  void GenSceneV1()
  {
    mScenarioDetailLabelString = "Simple copied Scene with Viewport";

    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control1");
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control2 (parent : Control1)");
    control1.Add(control2);

    Dali::CameraActor cameraActor            = Dali::CameraActor::New(Dali::Size(400.0f, 400.0f));
    float             cameraDefaultZPosition = cameraActor.GetProperty<float>(Dali::Actor::Property::POSITION_Z);
    cameraActor.SetProperty(Dali::Actor::Property::POSITION, Vector3(0.0f, 0.0f, cameraDefaultZPosition));
    cameraActor.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    cameraActor.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mActorList.push_back(cameraActor);
    window.Add(cameraActor);

    RenderTask renderTask = window.GetRenderTaskList().CreateTask();
    renderTask.SetRefreshRate(Dali::RenderTask::REFRESH_ALWAYS);
    renderTask.SetSourceActor(control1);
    renderTask.SetCameraActor(cameraActor);
    renderTask.SetClearColor(Color::LIGHT_SALMON);
    renderTask.SetClearEnabled(true);
    renderTask.GetCameraActor().SetInvertYAxis(false);

    Vector2 windowSize = window.GetSize();
    renderTask.SetViewport(Dali::Viewport(Vector4(windowSize.x / 2.0f + 200.0f, windowSize.y / 2.0f - 200.0f, 400, 400)));

    mRenderTaskList.push_back(renderTask);
  }

  void GenSceneV2()
  {
    mScenarioDetailLabelString = "Simple copied Scene with Viewport Guide Actor";

    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control1");
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control2 (parent : Control1)");
    control1.Add(control2);

    Toolkit::Control positionControl = AddNewRenderTask(control1, Vector2(400.0f, 400.0f), false, false, Color::LIGHT_CORAL);
    positionControl.SetProperty(Dali::Actor::Property::POSITION, Vector2(400.0f, 0.0f));
    positionControl.SetProperty(Dali::Actor::Property::SIZE, Vector2(400.0f, 400.0f));
    window.Add(positionControl);
  }

  void GenSceneV3()
  {
    mScenarioDetailLabelString = "Simple copied Scene with Viewport Guide Actor with invertYAxis true\n - Hit direction is following rendered result. (Engine knows how the render result is drawn.)";

    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control1");
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control2 (parent : Control1)");
    control1.Add(control2);

    Dali::CameraActor cameraActor            = Dali::CameraActor::New(Vector2(400.0f, 400.0f));
    float             cameraDefaultZPosition = cameraActor.GetProperty<float>(Dali::Actor::Property::POSITION_Z);
    cameraActor.SetProperty(Dali::Actor::Property::POSITION, Vector3(0.0f, 0.0f, cameraDefaultZPosition));
    cameraActor.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    cameraActor.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mActorList.push_back(cameraActor);
    window.Add(cameraActor);

    RenderTask renderTask = window.GetRenderTaskList().CreateTask();
    renderTask.SetRefreshRate(Dali::RenderTask::REFRESH_ALWAYS);
    renderTask.SetSourceActor(control1);
    renderTask.SetCameraActor(cameraActor);
    renderTask.SetClearColor(Color::LIGHT_CORAL);
    renderTask.SetClearEnabled(true);
    renderTask.SetExclusive(false);

    Toolkit::Control positionControl;
    positionControl = Toolkit::Control::New();
    positionControl.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, basicColorMap);
    positionControl.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    positionControl.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mActorList.push_back(positionControl);

    renderTask.SetViewportGuideActor(positionControl);
    renderTask.GetCameraActor().SetInvertYAxis(true);

    mRenderTaskList.push_back(renderTask);

    positionControl.SetProperty(Dali::Actor::Property::POSITION, Vector2(400.0f, 0.0f));
    positionControl.SetProperty(Dali::Actor::Property::SIZE, Vector2(400.0f, 400.0f));
    window.Add(positionControl);
  }

  void GenSceneV4()
  {
    mScenarioDetailLabelString = "Simple copied Scene with MappingActor with invertYAxis false (with flip by using PixelArea)\n - Hit direction cannot be following rendered result. (Engine cannot know how the FBO is textured.)\n - To use MappingActor with invertYAxis false, need to flip the result by app.";

    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control1");
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control2 (parent : Control1)");
    control1.Add(control2);

    Dali::CameraActor cameraActor            = Dali::CameraActor::New(Vector2(400.0f, 400.0f));
    float             cameraDefaultZPosition = cameraActor.GetProperty<float>(Dali::Actor::Property::POSITION_Z);
    cameraActor.SetProperty(Dali::Actor::Property::POSITION, Vector3(0.0f, 0.0f, cameraDefaultZPosition));
    cameraActor.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    cameraActor.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mActorList.push_back(cameraActor);
    window.Add(cameraActor);

    RenderTask renderTask = window.GetRenderTaskList().CreateTask();
    renderTask.SetRefreshRate(Dali::RenderTask::REFRESH_ALWAYS);
    renderTask.SetSourceActor(control1);
    renderTask.SetCameraActor(cameraActor);
    renderTask.SetClearColor(Color::LIGHT_CORAL);
    renderTask.SetClearEnabled(true);
    renderTask.SetExclusive(false);

    Toolkit::Control positionControl;
    positionControl = Toolkit::Control::New();
    positionControl.SetProperty(Dali::Actor::Property::NAME, "Maping Actor");
    positionControl.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    positionControl.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mActorList.push_back(positionControl);

    positionControl.SetProperty(Dali::Actor::Property::ORIENTATION, Quaternion(Dali::Radian(Dali::Degree(45.0f)), Vector3::YAXIS));

    Dali::Texture     texture     = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, unsigned(400), unsigned(400));
    Dali::FrameBuffer frameBuffer = Dali::FrameBuffer::New(texture.GetWidth(), texture.GetHeight(), Dali::FrameBuffer::Attachment::DEPTH);
    frameBuffer.AttachColorTexture(texture);
    mFrameBufferList.push_back(frameBuffer);
    mTextureList.push_back(texture);

    renderTask.SetFrameBuffer(frameBuffer);
    renderTask.SetScreenToFrameBufferMappingActor(positionControl);
    renderTask.GetCameraActor().SetInvertYAxis(false);

    Dali::Toolkit::ImageUrl imageUrl = Dali::Toolkit::Image::GenerateUrl(frameBuffer, 0u);
    Property::Map           imagePropertyMap;
    imagePropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
    imagePropertyMap.Insert(Toolkit::ImageVisual::Property::URL, imageUrl.GetUrl());
    imagePropertyMap.Insert(Toolkit::ImageVisual::Property::PIXEL_AREA, Vector4(0, 1, 1, -1));
    imagePropertyMap.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, 30.0f);
    imagePropertyMap.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, 3.0f);
    positionControl.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, imagePropertyMap);

    mRenderTaskList.push_back(renderTask);

    positionControl.SetProperty(Dali::Actor::Property::POSITION, Vector2(400.0f, 0.0f));
    positionControl.SetProperty(Dali::Actor::Property::SIZE, Vector2(400.0f, 400.0f));
    window.Add(positionControl);
  }

  void GenSceneV5()
  {
    mScenarioDetailLabelString = "Simple separated Scenes with Viewport Guide Actor";

    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control1");
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control2 (parent : Control1)");
    control1.Add(control2);

    Toolkit::Control control3 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control3");
    window.Add(control3);

    Toolkit::Control control4 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control4 (parent : Control3)");
    control3.Add(control4);

    Toolkit::Control positionControl = AddNewRenderTask(control3, Vector2(400.0f, 400.0f), false, true, Color::LIGHT_CYAN);
    positionControl.SetProperty(Dali::Actor::Property::POSITION, Vector2(400.0f, 0.0f));
    positionControl.SetProperty(Dali::Actor::Property::SIZE, Vector2(400.0f, 400.0f));
    window.Add(positionControl);
  }

  void GenSceneV6()
  {
    mScenarioDetailLabelString = "Simple copied Scene with FBO";

    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control1");
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control2 (parent : Control1)");
    control1.Add(control2);

    Toolkit::Control positionControl = AddNewRenderTask(control1, Vector2(400.0f, 400.0f), true, false, Color::LIGHT_GOLDEN_ROD_YELLOW);
    positionControl.SetProperty(Dali::Actor::Property::POSITION, Vector2(400.0f, 0.0f));
    positionControl.SetProperty(Dali::Actor::Property::SIZE, Vector2(400.0f, 400.0f));
    window.Add(positionControl);
  }

  void GenSceneV7()
  {
    mScenarioDetailLabelString = "Simple separated Scenes Scene with FBO";

    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control1");
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control2 (parent : Control1)");
    control1.Add(control2);

    Toolkit::Control control3 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control3");
    window.Add(control3);

    Toolkit::Control control4 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control4 (parent : Control3)");
    control3.Add(control4);

    Toolkit::Control positionControl = AddNewRenderTask(control3, Vector2(400.0f, 400.0f), true, true, Color::LIGHT_GRAY);
    positionControl.SetProperty(Dali::Actor::Property::POSITION, Vector2(400.0f, 0.0f));
    positionControl.SetProperty(Dali::Actor::Property::SIZE, Vector2(400.0f, 400.0f));
    window.Add(positionControl);
  }

  void GenSceneV8()
  {
    mScenarioDetailLabelString = "Multiple separated Scenes Scene with FBO";

    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control1");
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control2 (parent : Control1)");
    control1.Add(control2);

    Toolkit::Control control3 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control3");
    window.Add(control3);

    Toolkit::Control control4 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control4 (parent : Control3)");
    control3.Add(control4);

    Toolkit::Control positionControl1 = AddNewRenderTask(control3, Vector2(400.0f, 400.0f), true, true, Color::LIGHT_GREEN);
    positionControl1.SetProperty(Dali::Actor::Property::POSITION, Vector2(400.0f, 0.0f));
    positionControl1.SetProperty(Dali::Actor::Property::SIZE, Vector2(400.0f, 400.0f));
    window.Add(positionControl1);

    Toolkit::Control control5 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control5");
    window.Add(control5);

    Toolkit::Control control6 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control6 (parent : Control5)");
    control5.Add(control6);

    Toolkit::Control positionControl2 = AddNewRenderTask(control5, Vector2(400.0f, 400.0f), true, true, Color::LIGHT_GREY);
    positionControl2.SetProperty(Dali::Actor::Property::POSITION, Vector2(-400.0f, 0.0f));
    positionControl2.SetProperty(Dali::Actor::Property::SIZE, Vector2(400.0f, 400.0f));
    window.Add(positionControl2);

    Toolkit::Control control7 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 100.0f, "control7");
    window.Add(control7);

    Toolkit::Control control8 = CreateControl(basicColorMap, Vector2(50.0f, 50.0f), Vector2::ONE * 100.0f, "control8 (parent : Control7)");
    control7.Add(control8);

    Toolkit::Control positionControl3 = AddNewRenderTask(control7, Vector2(400.0f, 300.0f), true, true, Color::LIGHT_PINK);
    positionControl3.SetProperty(Dali::Actor::Property::POSITION, Vector2(0.0f, -300.0f));
    positionControl3.SetProperty(Dali::Actor::Property::SIZE, Vector2(400.0f, 300.0f));
    window.Add(positionControl3);

    Toolkit::Control positionControl4 = AddNewRenderTask(control1, Vector2(300.0f, 200.0f), true, false, Color::LIGHT_SALMON);
    positionControl4.SetProperty(Dali::Actor::Property::POSITION, Vector2(0.0f, 300.0f));
    positionControl4.SetProperty(Dali::Actor::Property::SIZE, Vector2(300.0f, 200.0f));
    window.Add(positionControl4);
  }

  void GenSceneV9()
  {
    mScenarioDetailLabelString = "Multiple separated Scenes Scene with FBO";

    Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 500.0f, "control1");
    window.Add(control1);

    Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 500.0f, "control2");
    window.Add(control2);

    Toolkit::Control positionControl1 = AddNewRenderTask(control2, Vector2(700.0f, 700.0f), true, true, Color::LIGHT_GREEN);
    control1.Add(positionControl1);
    positionControl1.SetProperty(Dali::Actor::Property::POSITION, Vector2(50.0f, 150.0f));
    positionControl1.SetProperty(Dali::Actor::Property::SIZE, Vector2(500.0f, 500.0f));

    Toolkit::Control control3 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 500.0f, "control3");
    window.Add(control3);

    Toolkit::Control positionControl2 = AddNewRenderTask(control3, Vector2(700.0f, 700.0f), true, true, Color::LIGHT_GREEN);
    control2.Add(positionControl2);
    positionControl2.SetProperty(Dali::Actor::Property::POSITION, Vector2(50.0f, 150.0f));
    positionControl2.SetProperty(Dali::Actor::Property::SIZE, Vector2(500.0f, 500.0f));
  }

  void GenScene3D1()
  {
    mScenarioDetailLabelString = "3D Scene With Simple Object";

    Dali::Scene3D::SceneView sceneView = Scene3D::SceneView::New();
    sceneView.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    sceneView.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    sceneView.SetProperty(Dali::Actor::Property::NAME, "SCENEVIEW");
    //    sceneView.SetProperty(Dali::Actor::Property::POSITION, Vector2(-200.0f, -200.0f));
    sceneView.SetProperty(Dali::Actor::Property::SIZE, Vector2(1000.0f, 800.0f));
    sceneView.SetBackgroundColor(Color::BEIGE);
    sceneView.UseFramebuffer(true);
    mActorList.push_back(sceneView);
    window.Add(sceneView);

    CameraActor camera = sceneView.GetSelectedCamera();
    camera.SetProperty(Dali::Actor::Property::POSITION, Vector3(1.5f, 1.0f, 1.5f));
    Dali::DevelActor::LookAt(camera, Vector3(0.0, 0.0, 0.0f));
    camera.SetProperty(Dali::CameraActor::Property::NEAR_PLANE_DISTANCE, 1.0f);
    camera.SetProperty(Dali::CameraActor::Property::FAR_PLANE_DISTANCE, 4.0f);

    //    Scene3D::Model model = Scene3D::Model::New(std::string(DEMO_MODEL_DIR) + "BoxAnimated.gltf");
    Scene3D::Model model = Scene3D::Model::New(std::string(DEMO_MODEL_DIR) + "DamagedHelmet.gltf");
    model.SetProperty(Dali::Actor::Property::POSITION, Vector3(-0.5f, 0.0f, 0.0f));
    model.SetProperty(Dali::Actor::Property::SIZE, Vector3::ONE);
    model.SetProperty(Dali::Actor::Property::SCALE, Vector3::ONE * 0.5f);
    model.SetProperty(Dali::Actor::Property::NAME, "HELMET");
    model.SetBackgroundColor(Color::RED);
    mActorList.push_back(model);
    sceneView.Add(model);
    model.TouchedSignal().Connect(this, &TouchController::OnTouch3DActor);
  }

  void GenScene3D2()
  {
    mScenarioDetailLabelString = "3D Scene with Panel";

    Dali::Scene3D::SceneView sceneView = Scene3D::SceneView::New();
    sceneView.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    sceneView.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    sceneView.SetProperty(Dali::Actor::Property::NAME, "SCENEVIEW");
    sceneView.SetProperty(Dali::Actor::Property::SIZE, Vector2(1000.0f, 800.0f));
    sceneView.SetBackgroundColor(Color::BEIGE);
    sceneView.UseFramebuffer(true);
    mActorList.push_back(sceneView);
    window.Add(sceneView);

    Dali::Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 150.0f, "control1");

    Dali::Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50, 50), Vector2::ONE * 150.0f, "control2");
    control1.Add(control2);

    Dali::Scene3D::Panel mPanel = Dali::Scene3D::Panel::New();
    mPanel.SetProperty(Dali::Actor::Property::POSITION, Vector3(2.0f, 0.0f, 0.0f));
    mPanel.SetProperty(Dali::Actor::Property::SIZE, Vector2(4.0f, 2.0f));
    mPanel.SetPanelResolution(Vector2(400, 500));
    mPanel.SetProperty(Dali::Actor::Property::NAME, "mPanel");
    mPanel.SetContent(control1);
    mPanel.SetProperty(Dali::Scene3D::Panel::Property::BACK_FACE_PLANE_COLOR, Color::RED);
    sceneView.Add(mPanel);
    mActorList.push_back(mPanel);

    Vector3 axis = Vector3::ONE;
    axis.Normalize();
    mPanel.SetProperty(Dali::Actor::Property::ORIENTATION, Quaternion(Radian(Degree(45.0f)), axis));

    Scene3D::Model model = Scene3D::Model::New(std::string(DEMO_MODEL_DIR) + "DamagedHelmet.gltf");
    model.SetProperty(Dali::Actor::Property::POSITION, Vector3(-0.5f, 0.0f, 0.0f));
    model.SetProperty(Dali::Actor::Property::SIZE, Vector3::ONE * 0.7f);
    model.SetProperty(Dali::Actor::Property::SCALE, Vector3::ONE * 1.5f);
    model.SetProperty(Dali::Actor::Property::NAME, "HELMET");
    model.SetBackgroundColor(Color::RED);
    mActorList.push_back(model);
    sceneView.Add(model);
    model.TouchedSignal().Connect(this, &TouchController::OnTouch3DActor);
    model.SetProperty(Dali::Actor::Property::ORIENTATION, Quaternion(Radian(Degree(-30.0f)), axis));
  }

  void GenScene3D3()
  {
    mScenarioDetailLabelString = "3D Scene with Multi Overlapped Models";

    Dali::Scene3D::SceneView sceneView = Scene3D::SceneView::New();
    sceneView.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    sceneView.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    sceneView.SetProperty(Dali::Actor::Property::NAME, "SCENEVIEW");
    sceneView.SetProperty(Dali::Actor::Property::SIZE, Vector2(1000.0f, 800.0f));
    sceneView.SetBackgroundColor(Color::BEIGE);
    sceneView.UseFramebuffer(true);
    mActorList.push_back(sceneView);
    window.Add(sceneView);

    Dali::Toolkit::Control control1 = CreateControl(basicColorMap, Vector2(0.0f, 0.0f), Vector2::ONE * 150.0f, "control1");

    Dali::Toolkit::Control control2 = CreateControl(basicColorMap, Vector2(50, 50), Vector2::ONE * 150.0f, "control2");
    control1.Add(control2);

    Dali::Scene3D::Panel mPanel = Dali::Scene3D::Panel::New();
    mPanel.SetProperty(Dali::Actor::Property::POSITION, Vector3(2.0f, 0.0f, 0.0f));
    mPanel.SetProperty(Dali::Actor::Property::SIZE, Vector2(4.0f, 5.0f));
    mPanel.SetProperty(Dali::Actor::Property::SCALE, Vector3::ONE * 0.25);
    mPanel.SetPanelResolution(Vector2(400, 500));
    mPanel.SetProperty(Dali::Actor::Property::NAME, "mPanel");
    mPanel.SetContent(control1);
    mPanel.SetProperty(Dali::Scene3D::Panel::Property::BACK_FACE_PLANE_COLOR, Color::RED);
    sceneView.Add(mPanel);
    mActorList.push_back(mPanel);
    Vector3 axis = Vector3::ONE;
    axis.Normalize();
    mPanel.SetProperty(Dali::Actor::Property::ORIENTATION, Quaternion(Radian(Degree(45.0f)), axis));
    mPanel.SetBackgroundColor(Color::BLUE);
    mPanel.TouchedSignal().Connect(this, &TouchController::OnTouch3DActor);

    Scene3D::Model model = Scene3D::Model::New(std::string(DEMO_MODEL_DIR) + "DamagedHelmet.gltf");
    model.SetProperty(Dali::Actor::Property::POSITION, Vector3(-0.5f, 0.0f, 2.0f));
    model.SetProperty(Dali::Actor::Property::SIZE, Vector3::ONE * 0.7f);
    model.SetProperty(Dali::Actor::Property::SCALE, Vector3::ONE * 1.5f);
    model.SetProperty(Dali::Actor::Property::NAME, "HELMET 1");
    model.SetBackgroundColor(Color::RED);
    mActorList.push_back(model);
    sceneView.Add(model);
    model.TouchedSignal().Connect(this, &TouchController::OnTouch3DActor);

    Scene3D::Model model3 = Scene3D::Model::New(std::string(DEMO_MODEL_DIR) + "DamagedHelmet.gltf");
    model3.SetProperty(Dali::Actor::Property::POSITION, Vector3(0.0f, 0.5f, 1.0f));
    model3.SetProperty(Dali::Actor::Property::SIZE, Vector3::ONE * 0.7f);
    model3.SetProperty(Dali::Actor::Property::SCALE, Vector3::ONE * 1.5f);
    model3.SetProperty(Dali::Actor::Property::NAME, "HELMET 3");
    model3.SetBackgroundColor(Color::RED);
    mActorList.push_back(model3);
    sceneView.Add(model3);
    model3.TouchedSignal().Connect(this, &TouchController::OnTouch3DActor);

    Scene3D::Model model2 = Scene3D::Model::New(std::string(DEMO_MODEL_DIR) + "DamagedHelmet.gltf");
    model2.SetProperty(Dali::Actor::Property::POSITION, Vector3(0.5f, 0.0f, 0.0f));
    model2.SetProperty(Dali::Actor::Property::SIZE, Vector3::ONE * 0.7f);
    model2.SetProperty(Dali::Actor::Property::SCALE, Vector3::ONE * 1.5f);
    model2.SetProperty(Dali::Actor::Property::NAME, "HELMET 2");
    model2.SetBackgroundColor(Color::RED);
    mActorList.push_back(model2);
    sceneView.Add(model2);
    model2.TouchedSignal().Connect(this, &TouchController::OnTouch3DActor);
  }

  void ClearScene()
  {
    for(auto&& actor : mActorList)
    {
      actor.Unparent();
      actor.Reset();
    }
    mActorList.clear();

    for(auto&& renderTask : mRenderTaskList)
    {
      window.GetRenderTaskList().RemoveTask(renderTask);
    }
    mRenderTaskList.clear();

    for(auto&& frameBuffer : mFrameBufferList)
    {
      frameBuffer.Reset();
    }
    mFrameBufferList.clear();

    for(auto&& texture : mTextureList)
    {
      texture.Reset();
    }
    mTextureList.clear();
  }

  void GenSingleScene(int scenarioDetailKey)
  {
    mSceneDetailType = scenarioDetailKey;
    switch(scenarioDetailKey)
    {
      case 1:
      {
        GenScene1();
        break;
      }
      case 2:
      {
        GenScene2();
        break;
      }
      case 3:
      {
        GenScene3();
        break;
      }
      case 4:
      {
        GenScene4();
        break;
      }
      case 5:
      {
        GenScene5();
        break;
      }
      case 6:
      {
        GenScene6();
        break;
      }
      case 7:
      {
        GenScene7();
        break;
      }
      case 8:
      {
        GenScene8();
        break;
      }
      case 9:
      {
        GenScene9();
        break;
      }
      case 0:
      {
        GenScene0();
        break;
      }
      default:
      {
        mSceneDetailType = 1;
        GenScene1();
        break;
      }
    }
  }

  void GenViewportScene(int scenarioDetailKey)
  {
    mSceneDetailType = scenarioDetailKey;
    switch(scenarioDetailKey)
    {
      case 1:
      {
        GenSceneV1();
        break;
      }
      case 2:
      {
        GenSceneV2();
        break;
      }
      case 3:
      {
        GenSceneV3();
        break;
      }
      case 4:
      {
        GenSceneV4();
        break;
      }
      case 5:
      {
        GenSceneV5();
        break;
      }
      case 6:
      {
        GenSceneV6();
        break;
      }
      case 7:
      {
        GenSceneV7();
        break;
      }
      case 8:
      {
        GenSceneV8();
        break;
      }
      case 9:
      {
        GenSceneV9();
        break;
      }
      default:
      {
        mSceneDetailType = 1;
        GenSceneV1();
        break;
      }
    }
  }

  void Gen3DScene(int scenarioDetailKey)
  {
    mSceneDetailType = scenarioDetailKey;
    switch(scenarioDetailKey)
    {
      case 1:
      {
        GenScene3D1();
        break;
      }
      case 2:
      {
        GenScene3D2();
        break;
      }
      case 3:
      {
        GenScene3D3();
        break;
      }
      default:
      {
        mSceneDetailType = 1;
        GenScene3D1();
        break;
      }
    }
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(event.GetKeyName() == "c" || event.GetKeyName() == "C")
      {
        ClearScene();
        int sceneTypeInteger = (int)mSceneType;
        sceneTypeInteger     = (sceneTypeInteger + 1) % SceneTypeCount;
        mSceneType           = (SceneType)(sceneTypeInteger);
        switch(mSceneType)
        {
          case SceneType::DEFAULT:
          {
            mScenarioTypeLabelString = "Single RenderTask";
            GenSingleScene(mSceneDetailType);
            break;
          }
          case SceneType::VIEWPORT:
          {
            mScenarioTypeLabelString = "Use Viewport or FBO";
            GenViewportScene(mSceneDetailType);
            break;
          }
          case SceneType::THREE_DIMENSION:
          {
            mScenarioTypeLabelString = "3D Scene";
            Gen3DScene(mSceneDetailType);
            break;
          }
          default:
          {
            mScenarioTypeLabelString = "Single RenderTask";
            GenSingleScene(mSceneDetailType);
            break;
          }
        }
      }
      else if(event.GetKeyName() >= "0" && event.GetKeyName() <= "9")
      {
        ClearScene();
        switch(mSceneType)
        {
          case SceneType::DEFAULT:
          {
            GenSingleScene(std::stoi(event.GetKeyName()));
            break;
          }
          case SceneType::VIEWPORT:
          {
            GenViewportScene(std::stoi(event.GetKeyName()));
            break;
          }
          case SceneType::THREE_DIMENSION:
          {
            Gen3DScene(std::stoi(event.GetKeyName()));
            break;
          }
          default:
          {
            GenSingleScene(std::stoi(event.GetKeyName()));
            break;
          }
        }
      }
      else if(event.GetKeyName() == "g" || event.GetKeyName() == "G")
      {
        DevelWindowSystem::SetGeometryHittestEnabled(true);
        mPropagationTypeLabelString = "PropagationType::GEOMETRY";
      }
      else if(event.GetKeyName() == "p" || event.GetKeyName() == "P")
      {
        DevelWindowSystem::SetGeometryHittestEnabled(false);
        mPropagationTypeLabelString = "PropagationType::PARENT";
      }

      mScenarioTypeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, mScenarioTypeLabelString);
      mScenarioDetailLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, mScenarioDetailLabelString);
      mPropagationTypeLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, mPropagationTypeLabelString);

      for(auto&& actor : mActorList)
      {
        DALI_LOG_ERROR("%s id : %d\n", actor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str(), actor.GetProperty<int>(Dali::Actor::Property::ID));
      }
    }
  }

  enum SceneType
  {
    DEFAULT,
    VIEWPORT,
    THREE_DIMENSION
  };

private:
  int SceneTypeCount{3};

private:
  Dali::Toolkit::TextLabel mKeyDescription;
  Dali::Toolkit::TextLabel mScenarioDetailLabel;
  std::string              mScenarioDetailLabelString;
  Dali::Toolkit::TextLabel mPropagationTypeLabel;
  std::string              mPropagationTypeLabelString;
  Dali::Toolkit::TextLabel mScenarioTypeLabel;
  std::string              mScenarioTypeLabelString;
  SceneType                mSceneType{SceneType::DEFAULT};
  int                      mSceneDetailType{1};
  Property::Map            basicColorMap;
  Property::Map            notTouchableColorMap;
  Property::Map            touchedColorMap;
  std::vector<Actor>       mActorList;
  std::vector<RenderTask>  mRenderTaskList;
  std::vector<FrameBuffer> mFrameBufferList;
  std::vector<Texture>     mTextureList;
  Application&             mApplication;
  Window                   window;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application     application = Application::New(&argc, &argv);
  TouchController test(application);
  application.MainLoop();
  return 0;
}
