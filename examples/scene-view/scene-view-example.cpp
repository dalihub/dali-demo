/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali-scene3d/dali-scene3d.h>
#include <dali-toolkit/dali-toolkit.h>

// INTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/file-stream.h>

using namespace Dali;
using namespace Toolkit;
using namespace Scene3D;

namespace
{
const char* CUBEMAP_SKY_BOX_URL = DEMO_IMAGE_DIR "veste_oberhaus_cubemap.png";
const char* CAMERA_NAME("MyCamera");

const char*       MODEL_URL = DEMO_MODEL_DIR "SphereMetallic.gltf";
constexpr Vector2 MODEL_SIZE(300.0f, 300.0f);

const Quaternion MODEL_ROTATION(Degree(360.f), Vector3(0.1f, 1.0f, -0.1f));
} // namespace

class SceneViewExample : public ConnectionTracker
{
public:
  SceneViewExample(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &SceneViewExample::Create);
  }

  ~SceneViewExample() = default;

private:
  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window        window     = application.GetWindow();
    const Vector2 windowSize = window.GetSize();

    // Create a SceneView and set the Skybox
    SceneView sceneView = Handle::New<SceneView>({{Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER},
                                                  {Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER},
                                                  {Actor::Property::SIZE, windowSize}});
    sceneView.SetSkybox(CUBEMAP_SKY_BOX_URL);
    window.Add(sceneView);

    // Load the model and set IBL
    Model model = Model::New(MODEL_URL);
    model.SetProperties({
      {Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER},
      {Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER},
      {Actor::Property::SIZE, MODEL_SIZE},
    });
    model.SetImageBasedLightSource(CUBEMAP_SKY_BOX_URL, CUBEMAP_SKY_BOX_URL);
    sceneView.Add(model);

    // Create a new camera and reparent as we want to rotate the camera around the origin
    CameraActor cameraActor = Handle::New<CameraActor>({{Actor::Property::NAME, CAMERA_NAME}});
    sceneView.AddCamera(cameraActor);
    sceneView.SelectCamera(CAMERA_NAME);
    cameraActor.SetType(Camera::LOOK_AT_TARGET);
    cameraActor.SetTargetPosition(Vector3::ZERO);
    cameraActor.Unparent();
    Actor rotatingActor = Handle::New<Actor>({{Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER},
                                              {Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER}});
    rotatingActor.Add(cameraActor);
    sceneView.Add(rotatingActor);

    Animation animation = Animation::New(10.0f);
    animation.SetLooping(true);
    animation.AnimateBy(Property(rotatingActor, Actor::Property::ORIENTATION), MODEL_ROTATION);
    animation.Play();

    // Respond to key events
    window.KeyEventSignal().Connect(this, &SceneViewExample::OnKeyEvent);
  }

  /**
   * @brief Called when any key event is received
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
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application      application = Application::New(&argc, &argv);
  SceneViewExample test(application);
  application.MainLoop();
  return 0;
}
