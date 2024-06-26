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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-scene3d/dali-scene3d.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

namespace
{
  Vector3 BackFaceColors[3] =
  {
    Vector3(1.0f, 0.0f, 0.0f),
    Vector3(0.0f, 1.0f, 0.0f),
    Vector3(0.0f, 0.0f, 1.0f),
  };
}

// This example shows how to create and display Hello World! using a simple TextActor
//
class Scene3DPanelController : public ConnectionTracker
{
public:
  Scene3DPanelController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &Scene3DPanelController::Create);
  }

  ~Scene3DPanelController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    Vector2 windowSize(window.GetSize().GetWidth(), window.GetSize().GetHeight());

    Dali::Scene3D::SceneView sceneView = Scene3D::SceneView::New();
    sceneView.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    sceneView.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    sceneView.SetProperty(Dali::Actor::Property::SIZE, windowSize);
    sceneView.SetBackgroundColor(Color::BEIGE);
    sceneView.UseFramebuffer(true);
    window.Add(sceneView);

    Dali::Toolkit::Control parent = Dali::Toolkit::Control::New();
    parent.SetBackgroundColor(Color::BLUE);
    parent.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    parent.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    parent.SetProperty(Dali::Actor::Property::SIZE, Vector2(100, 100));

    Dali::Toolkit::Control child = Dali::Toolkit::Control::New();
    child.SetBackgroundColor(Color::RED);
    child.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    child.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    child.SetProperty(Dali::Actor::Property::SIZE, Vector2(100, 100));
    child.SetProperty(Dali::Actor::Property::POSITION, Vector2(25, 100));
    parent.Add(child);

    mPanel = Dali::Scene3D::Panel::New();
    mPanel.SetProperty(Dali::Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.5f));
    mPanel.SetProperty(Dali::Actor::Property::SIZE, Vector2(2.0f, 1.0f));
    mPanel.SetPanelResolution(Vector2(400, 500));
    mPanel.SetContent(parent);
    mPanel.SetProperty(Dali::Scene3D::Panel::Property::BACK_FACE_PLANE_COLOR, BackFaceColors[mColorIndex]);
    sceneView.Add(mPanel);

    SetGuide();

    Animation animation = Animation::New(15.0f);
    animation.AnimateTo(Dali::Property(mPanel, Dali::Actor::Property::ORIENTATION), Quaternion(Dali::Radian(M_PI), Vector3::YAXIS));
    animation.AnimateTo(Dali::Property(mPanel, Dali::Actor::Property::SIZE), Vector3(1.0f, 2.0f, 0.0f));
    animation.SetLooping(true);
    animation.SetLoopingMode(Dali::Animation::LoopingMode::AUTO_REVERSE);
    animation.Play();

    // Respond to a touch anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &Scene3DPanelController::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &Scene3DPanelController::OnKeyEvent);
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
      if(event.GetKeyName() == "1")
      {
        bool isTransparent = mPanel.GetProperty<bool>(Dali::Scene3D::Panel::Property::TRANSPARENT);
        mPanel.SetProperty(Dali::Scene3D::Panel::Property::TRANSPARENT, !isTransparent);
      }
      else if(event.GetKeyName() == "2")
      {
        bool isDoubleSided = mPanel.GetProperty<bool>(Dali::Scene3D::Panel::Property::DOUBLE_SIDED);
        mPanel.SetProperty(Dali::Scene3D::Panel::Property::DOUBLE_SIDED, !isDoubleSided);
      }
      else if(event.GetKeyName() == "3")
      {
        bool isUsingBackFacePlane = mPanel.GetProperty<bool>(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE);
        mPanel.SetProperty(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE, !isUsingBackFacePlane);
      }
      else if(event.GetKeyName() == "4")
      {
        mColorIndex = ((mColorIndex + 1) % 3);
        mPanel.SetProperty(Dali::Scene3D::Panel::Property::BACK_FACE_PLANE_COLOR, BackFaceColors[mColorIndex]);
      }
      SetGuide();
    }
  }

  void SetGuide()
  {
    if(!mGuideText)
    {
      mGuideText = TextLabel::New();
      mGuideText.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_LEFT);
      mGuideText.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
      mGuideText.SetProperty(Dali::Toolkit::TextLabel::Property::POINT_SIZE, 30.0f);
      mGuideText.SetProperty(Dali::Toolkit::TextLabel::Property::MULTI_LINE, true);
    }
    mApplication.GetWindow().Add(mGuideText);

    std::string    isTransparent        = mPanel.GetProperty<bool>(Dali::Scene3D::Panel::Property::TRANSPARENT) ? "true" : "false";
    std::string    isDoubleSided        = mPanel.GetProperty<bool>(Dali::Scene3D::Panel::Property::DOUBLE_SIDED) ? "true" : "false";
    std::string    isUsingBackFacePlane = mPanel.GetProperty<bool>(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE) ? "true" : "false";
    std::string    backFaceColor;
    backFaceColor.append("(" + std::to_string(BackFaceColors[mColorIndex].r));
    backFaceColor.append(", " + std::to_string(BackFaceColors[mColorIndex].g));
    backFaceColor.append(", " + std::to_string(BackFaceColors[mColorIndex].b) + ")");

    std::string guideText;
    guideText += "Press 1 to convert transparency - Current value : " + isTransparent + "\n";
    guideText += "Press 2 to convert double sided - Current value : " + isDoubleSided + "\n";
    guideText += "Press 3 to convert back face - Current value : " + isUsingBackFacePlane + "\n";
    guideText += "Press 4 to convert back face color - Current value : " + backFaceColor + "\n";
    mGuideText.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, guideText);
  }

private:
  TextLabel            mGuideText;
  uint32_t             mColorIndex{0};
  Dali::Scene3D::Panel mPanel;
  Application&         mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application          application = Application::New(&argc, &argv);
  Scene3DPanelController test(application);
  application.MainLoop();
  return 0;
}
