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
#include <dali-scene3d/public-api/light/light.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/actors/actor-devel.h>

#include <dali/integration-api/debug.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

const std::string imagedir = DEMO_IMAGE_DIR;
const std::string uri_diffuse_texture(imagedir + "meadow_diffuse_rgb16f_cm.ktx");
const std::string uri_specular_texture(imagedir + "meadow_radiance_rgb16f_cm.ktx");
const std::string uri_diffuse_texture2(imagedir + "Studio/Irradiance.ktx");
const std::string uri_specular_texture2(imagedir + "Studio/Radiance.ktx");

// This example shows how to create and display Hello World! using a simple TextActor
//
class Scene3dLightController : public ConnectionTracker
{
public:
  Scene3dLightController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &Scene3dLightController::Create);
  }

  ~Scene3dLightController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    sceneView = Scene3D::SceneView::New();
    sceneView.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    sceneView.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    sceneView.SetProperty(Dali::Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    sceneView.SetProperty(Dali::Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    sceneView.SetBackgroundColor(Color::BEIGE);
    sceneView.UseFramebuffer(true);

    light1 = Scene3D::Light::New();
    light1.SetProperty(Dali::Actor::Property::COLOR, Color::BROWN);
    light1.SetProperty(Dali::Actor::Property::POSITION, Vector3(-2.0f, -0.5f, 0.0f));
    Dali::DevelActor::LookAt(light1, Vector3(0.0f, 0.0f, 0.0f));
    mAnimation                = Dali::Animation::New(5);
    Dali::KeyFrames keyFrames = Dali::KeyFrames::New();
    keyFrames.Add(0.0f, Quaternion(Radian(Degree(0.0f)), Vector3::YAXIS));
    keyFrames.Add(0.25f, Quaternion(Radian(Degree(90.0f)), Vector3::YAXIS));
    keyFrames.Add(0.5f, Quaternion(Radian(Degree(180.0f)), Vector3::YAXIS));
    keyFrames.Add(0.75f, Quaternion(Radian(Degree(270.0f)), Vector3::YAXIS));
    keyFrames.Add(1.0f, Quaternion(Radian(Degree(0.0f)), Vector3::YAXIS));

    Actor dummyActor = Actor::New();
    dummyActor.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    dummyActor.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    dummyActor.Add(light1);
    sceneView.Add(dummyActor);

    mAnimation.AnimateBetween(Dali::Property(dummyActor, Dali::Actor::Property::ORIENTATION), keyFrames);
    mAnimation.Play();
    mAnimation.SetLooping(true);
    light1.Enable(true);
    light1.EnableShadow(true);
    light1.EnableShadowSoftFiltering(true);

    light2 = Scene3D::Light::New();
    light2.SetProperty(Dali::Actor::Property::COLOR, Color::WHITE * 0.4);
    light2.SetProperty(Dali::Actor::Property::POSITION, Vector3(-1.0f, 0.0f, 1.1f));
    Dali::DevelActor::LookAt(light2, Vector3(0.0, 0.0, 0.0f));
    light2.EnableShadow(true);
    sceneView.Add(light2);

    light3 = Scene3D::Light::New();
    light3.SetProperty(Dali::Actor::Property::COLOR, Color::BLUE);
    light3.SetProperty(Dali::Actor::Property::POSITION, Vector3(-2.0, 0.0, -0.6));
    Dali::DevelActor::LookAt(light3, Vector3(0.0f, 0.0f, 0.0f));
    sceneView.Add(light3);

    light4 = Scene3D::Light::New();
    light4.SetProperty(Dali::Actor::Property::COLOR, Color::RED);
    light4.SetProperty(Dali::Actor::Property::POSITION, Vector3(-1.0f, -1.0f, 1.1f));
    Dali::DevelActor::LookAt(light4, Vector3(0.0, 0.0, 0.0f));
    sceneView.Add(light4);

    light5 = Scene3D::Light::New();
    light5.SetProperty(Dali::Actor::Property::COLOR, Color::GREEN);
    light5.SetProperty(Dali::Actor::Property::POSITION, Vector3(-1.0f, 1.0f, 1.1f));
    Dali::DevelActor::LookAt(light5, Vector3(0.0, 0.0, 0.0f));
    sceneView.Add(light5);

    CameraActor camera = sceneView.GetSelectedCamera();
    camera.SetProperty(Dali::Actor::Property::POSITION, Vector3(1.5f, 0.0f, 1.5f));
    Dali::DevelActor::LookAt(camera, Vector3(0.0, 0.0, 0.0f));
    camera.SetProperty(Dali::CameraActor::Property::NEAR_PLANE_DISTANCE, 1.0f);
    camera.SetProperty(Dali::CameraActor::Property::FAR_PLANE_DISTANCE, 4.0f);

    Scene3D::Model model = Scene3D::Model::New(std::string(DEMO_MODEL_DIR) + "DamagedHelmet.gltf");
    model.SetProperty(Dali::Actor::Property::POSITION, Vector3(-0.5f, 0.0f, 0.0f));
    model.SetProperty(Dali::Actor::Property::SIZE, Vector3::ONE);
    sceneView.Add(model);

    Scene3D::Model model2 = Scene3D::Model::New(std::string(DEMO_MODEL_DIR) + "microphone.gltf");
    model2.SetProperty(Dali::Actor::Property::POSITION, Vector3(0.2f, 0.0f, 0.0f));
    model2.SetProperty(Dali::Actor::Property::SIZE, Vector3::ONE);
    sceneView.Add(model2);

    window.Add(sceneView);

    auto text                             = TextLabel::New("Press keys 0-9 to change lighting");
    text[Actor::Property::PARENT_ORIGIN]  = ParentOrigin::BOTTOM_CENTER;
    text[Actor::Property::ANCHOR_POINT]   = AnchorPoint::BOTTOM_CENTER;
    text[TextLabel::Property::TEXT_COLOR] = Color::BLACK;
    window.Add(text);

    // Respond to a touch anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &Scene3dLightController::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &Scene3dLightController::OnKeyEvent);
  }

  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    const PointState::Type state = touch.GetState(0);

    switch(state)
    {
      default:
      {
        break;
      }
    }
    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(event.GetKeyName() == "0")
      {
        sceneView.SetImageBasedLightSource("", "", 1.0f);
        light1.Enable(true);
        light2.Enable(true);
        light3.Enable(true);
        light4.Enable(true);
        light5.Enable(true);
        light1.EnableShadow(true);
        light2.EnableShadow(false);
        mAnimation.Play();
      }
      if(event.GetKeyName() == "1")
      {
        sceneView.SetImageBasedLightSource(uri_diffuse_texture2, uri_specular_texture2, 0.6f);
        light1.Enable(true);
        light2.Enable(true);
        light3.Enable(true);
        light4.Enable(true);
        light5.Enable(true);
        light1.EnableShadow(true);
        light2.EnableShadow(false);
        mAnimation.Play();
      }
      else if(event.GetKeyName() == "2")
      {
        sceneView.SetImageBasedLightSource(uri_diffuse_texture2, uri_specular_texture2, 0.6f);
        light1.Enable(true);
        light2.Enable(true);
        light3.Enable(true);
        light4.Enable(true);
        light5.Enable(true);
        light1.EnableShadow(false);
        light2.EnableShadow(true);
        mAnimation.Play();
      }
      else if(event.GetKeyName() == "3")
      {
        sceneView.SetImageBasedLightSource(uri_diffuse_texture, uri_specular_texture, 0.6f);
        light1.Enable(true);
        light2.Enable(true);
        light3.Enable(true);
        light4.Enable(true);
        light5.Enable(true);
        light1.EnableShadow(true);
        light2.EnableShadow(false);
        mAnimation.Play();
      }
      else if(event.GetKeyName() == "4")
      {
        sceneView.SetImageBasedLightSource(uri_diffuse_texture, uri_specular_texture, 0.3f);
        light1.Enable(true);
        light2.Enable(true);
        light3.Enable(true);
        light4.Enable(true);
        light5.Enable(true);
        light1.EnableShadow(true);
        light2.EnableShadow(false);
        mAnimation.Play();
      }
      else if(event.GetKeyName() == "5")
      {
        sceneView.SetImageBasedLightSource(uri_diffuse_texture, uri_specular_texture, 0.1f);
        light1.Enable(true);
        light2.Enable(true);
        light3.Enable(true);
        light4.Enable(true);
        light5.Enable(true);
        light1.EnableShadow(true);
        light2.EnableShadow(false);
        mAnimation.Play();
      }
      else if(event.GetKeyName() == "6")
      {
        sceneView.SetImageBasedLightSource(uri_diffuse_texture, uri_specular_texture, 0.0f);
        light1.Enable(true);
        light2.Enable(true);
        light3.Enable(true);
        light4.Enable(true);
        light5.Enable(true);
        light1.EnableShadow(true);
        light2.EnableShadow(false);
        mAnimation.Play();
      }
      else if(event.GetKeyName() == "7")
      {
        sceneView.SetImageBasedLightSource(uri_diffuse_texture, uri_specular_texture, 0.6f);
        light1.Enable(true);
        light2.Enable(true);
        light3.Enable(true);
        light4.Enable(true);
        light5.Enable(true);
        light2.EnableShadow(false);
        light1.EnableShadow(!light1.IsShadowEnabled());
        mAnimation.Play();
      }
      else if(event.GetKeyName() == "8")
      {
        light1.EnableShadowSoftFiltering(!light1.IsShadowSoftFilteringEnabled());
        light2.EnableShadowSoftFiltering(!light2.IsShadowSoftFilteringEnabled());
      }
      else if(event.GetKeyName() == "9")
      {
        progress += 0.05;
        if(progress >= 1.0)
        {
          progress = 0.0f;
        }
        mAnimation.Play();
        mAnimation.SetCurrentProgress(progress);
        mAnimation.Pause();
      }
      else if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  float              progress{0.0f};
  Application&       mApplication;
  Scene3D::Light     light1;
  Scene3D::Light     light2;
  Scene3D::Light     light3;
  Scene3D::Light     light4;
  Scene3D::Light     light5;
  Scene3D::Light     light6;
  Scene3D::SceneView sceneView;
  Dali::Animation    mAnimation;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application            application = Application::New(&argc, &argv);
  Scene3dLightController test(application);
  application.MainLoop();
  return 0;
}
