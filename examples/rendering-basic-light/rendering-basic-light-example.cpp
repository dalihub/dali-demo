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
#include <dali/dali.h>

#include "generated/rendering-basic-light-frag.h"
#include "generated/rendering-basic-light-vert.h"

using namespace Dali;
using namespace Toolkit;

#define MATERIALS_MAX 24

namespace
{
const char*       BASIC_LIGHT_THEME(DEMO_STYLE_DIR "basic-light-theme.json");
const char* const CUSTOM_BASIC_LIGHT_THEME = "BasicLightButton";

struct Material
{
  const char* name;
  Vector3     ambient;
  Vector3     diffuse;
  Vector3     specular;
  float       shininess;
};

Material material[] =
  {
    {"Emerald", Vector3(0.0215f, 0.1745f, 0.0215f), Vector3(0.07568f, 0.61424f, 0.07568f), Vector3(0.633, 0.727811f, 0.633f), 0.6f},
    {"Jade", Vector3(0.135f, 0.2225f, 0.1575f), Vector3(0.54f, 0.89f, 0.63f), Vector3(0.316228f, 0.316228f, 0.316228f), 0.1f},
    {"Obsidian", Vector3(0.05375f, 0.05f, 0.06625f), Vector3(0.18275f, 0.17f, 0.22525f), Vector3(0.332741f, 0.328634f, 0.346435f), 0.3f},
    {"Perl", Vector3(0.25f, 0.20725f, 0.20725f), Vector3(1.0f, 0.829f, 0.829f), Vector3(0.296648f, 0.296648f, 0.296648f), 0.088f},
    {"Ruby", Vector3(0.1745f, 0.01175f, 0.01175f), Vector3(0.61424f, 0.04136f, 0.04136f), Vector3(0.727811f, 0.626959f, 0.626959f), 0.6f},
    {"Turquoise", Vector3(0.1f, 0.18725f, 0.1745f), Vector3(0.396f, 0.74151f, 0.69102f), Vector3(0.297254, 0.30829f, 0.306678f), 0.1f},
    {"Brass", Vector3(0.329412f, 0.223529f, 0.027451f), Vector3(0.780392f, 0.568627f, 0.113725f), Vector3(0.992157f, 0.941176f, 0.807843f), 0.21794872f},
    {"Bronze", Vector3(0.2125f, 0.1275f, 0.054f), Vector3(0.714f, 0.4284f, 0.18144f), Vector3(0.393548f, 0.271906f, 0.166721f), 0.2f},
    {"Chrome", Vector3(0.25f, 0.25f, 0.25f), Vector3(0.4f, 0.4f, 0.4f), Vector3(0.774597f, 0.774597f, 0.774597f), 0.6f},
    {"Copper", Vector3(0.19125f, 0.0735f, 0.0225f), Vector3(0.7038f, 0.27048f, 0.0828f), Vector3(0.256777f, 0.137622f, 0.086014f), 0.1f},
    {"Gold", Vector3(0.24725f, 0.1995f, 0.0745f), Vector3(0.75164f, 0.60648f, 0.22648f), Vector3(0.628281f, 0.555802f, 0.366065f), 0.4f},
    {"Silver", Vector3(0.19225f, 0.19225f, 0.19225f), Vector3(0.50754f, 0.50754f, 0.50754f), Vector3(0.508273f, 0.508273f, 0.508273f), 0.4f},
    {"Black plastic", Vector3(0.0f, 0.0f, 0.0f), Vector3(0.01f, 0.01f, 0.01f), Vector3(0.50f, 0.50f, 0.50f), 0.25f},
    {"Cyan plastic", Vector3(0.0f, 0.1f, 0.06f), Vector3(0.0f, 0.50980392f, 0.50980392f), Vector3(0.50196078f, 0.50196078f, 0.50196078f), 0.25f},
    {"Green plastic", Vector3(0.0f, 0.0f, 0.0f), Vector3(0.1f, 0.35f, 0.1f), Vector3(0.45, 0.55, 0.45), 0.25f},
    {"Red plastic", Vector3(0.0f, 0.0f, 0.0f), Vector3(0.5f, 0.0f, 0.0f), Vector3(0.7f, 0.6f, 0.6f), 0.25f},
    {"White plastic", Vector3(0.0f, 0.0f, 0.0f), Vector3(0.55f, 0.55f, 0.55f), Vector3(0.7f, 0.7f, 0.7f), 0.25f},
    {"Yellow plastic", Vector3(0.0f, 0.0f, 0.0f), Vector3(0.5f, 0.5f, 0.0f), Vector3(0.6f, 0.6f, 0.5f), 0.25f},
    {"Black rubber", Vector3(0.02f, 0.02f, 0.02f), Vector3(0.01f, 0.01f, 0.01f), Vector3(0.4f, 0.4f, 0.4f), 0.078125f},
    {"Cyan rubber", Vector3(0.0f, 0.05f, 0.05f), Vector3(0.4f, 0.5f, 0.5f), Vector3(0.04f, 0.7f, 0.7f), 0.078125f},
    {"Green rubber", Vector3(0.0f, 0.05f, 0.0f), Vector3(0.4f, 0.5f, 0.4f), Vector3(0.04f, 0.7f, 0.04f), 0.078125f},
    {"Red rubber", Vector3(0.05f, 0.0f, 0.0f), Vector3(0.5f, 0.4f, 0.4f), Vector3(0.7f, 0.04f, 0.04f), 0.078125f},
    {"White rubber", Vector3(0.05f, 0.05f, 0.05f), Vector3(0.5f, 0.5f, 0.5f), Vector3(0.7f, 0.7f, 0.7f), 0.078125f},
    {"Yellow rubber", Vector3(0.05f, 0.05f, 0.0f), Vector3(0.5f, 0.5f, 0.4f), Vector3(0.7f, 0.7f, 0.04f), 0.078125f}};

int MaterialID = 0;

} // namespace

// This example shows per-pixel lighting of materials with different ambient, diffuse, specular and shininess parameters
//
class BasicLightController : public ConnectionTracker
{
public:
  BasicLightController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &BasicLightController::Create);
  }

  ~BasicLightController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::BLACK);
    mLabel = TextLabel::New(material[MaterialID].name);
    mLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    mLabel.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.5f, 0.0f, 0.5f));
    mLabel.SetProperty(Actor::Property::SIZE, Vector2(window.GetSize().GetWidth() * 0.5f, window.GetSize().GetHeight() * 0.083f));
    mLabel.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    mLabel.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
    mLabel.SetProperty(TextLabel::Property::TEXT_COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    window.Add(mLabel);
    mButton = PushButton::New();
    mButton.SetProperty(Button::Property::LABEL, "Exit");
    mButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mButton.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mButton.ClickedSignal().Connect(this, &BasicLightController::OnExit);
    mButton.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.5f, 0.1f, 0.5f));
    mButton.SetStyleName(CUSTOM_BASIC_LIGHT_THEME);
    mButton.SetProperty(Actor::Property::COLOR, Vector4(material[MaterialID].diffuse) + Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    window.Add(mButton);

    // Step 1. Create shader
    CreateCubeShader();

    // Step 2. Prepare geometry
    CreateCubeGeometry();

    // Step 3. Create a renderer
    CreateRenderer();

    // Step 4. Create an Actor
    CreateActor();

    // Step 5. Play animation to rotate the cube
    PlayAnimation();

    // Respond to a click anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &BasicLightController::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &BasicLightController::OnKeyEvent);
  }

  /**
   * This function will change the material of the cube when touched
   */
  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      MaterialID++;
      MaterialID %= MATERIALS_MAX;

      mShader.SetProperty(mShader.GetPropertyIndex("material.ambient"), material[MaterialID].ambient);
      mShader.SetProperty(mShader.GetPropertyIndex("material.diffuse"), material[MaterialID].diffuse);
      mShader.SetProperty(mShader.GetPropertyIndex("material.specular"), material[MaterialID].specular);
      mShader.SetProperty(mShader.GetPropertyIndex("material.shininess"), material[MaterialID].shininess * 128.0f);
      mLabel.SetProperty(TextLabel::Property::TEXT, material[MaterialID].name);
      mButton.SetProperty(Actor::Property::COLOR, Vector4(material[MaterialID].diffuse) + Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    }
    return true;
  }

  /**
   * This function will the terminate the application when the exit button is pressed
   */
  bool OnExit(Button button)
  {
    mApplication.Quit();
    return true;
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

  /**
   * This function creates a cube geometry including texture coordinates.
   * Also it demonstrates using the indexed draw feature by setting an index array.
   */
  void CreateCubeGeometry()
  {
    struct Vertex
    {
      Vector3 aPosition;
      Vector3 aNormal;
    };

    const Vector3 NORMAL0(-1.0f, 0.0f, 0.0f);
    const Vector3 NORMAL1(0.0f, 1.0f, 0.0f);
    const Vector3 NORMAL2(0.0f, -1.0f, 0.0f);
    const Vector3 NORMAL3(0.0f, 0.0f, 1.0f);
    const Vector3 NORMAL4(1.0f, 0.0f, 0.0f);
    const Vector3 NORMAL5(0.0f, 0.0f, -1.0f);

    Vertex vertices[] = {
      {Vector3(1.0f, -1.0f, -1.0f), NORMAL5},
      {Vector3(-1.0f, 1.0f, -1.0f), NORMAL5},
      {Vector3(1.0f, 1.0f, -1.0f), NORMAL5},
      {Vector3(-1.0f, 1.0f, 1.0f), NORMAL3},
      {Vector3(1.0f, -1.0f, 1.0f), NORMAL3},
      {Vector3(1.0f, 1.0f, 1.0f), NORMAL3},
      {Vector3(1.0f, 1.0f, 1.0f), NORMAL4},
      {Vector3(1.0f, -1.0f, -1.0f), NORMAL4},
      {Vector3(1.0f, 1.0f, -1.0f), NORMAL4},
      {Vector3(1.0f, -1.0f, 1.0f), NORMAL1},
      {Vector3(-1.0f, -1.0f, -1.0f), NORMAL1},
      {Vector3(1.0f, -1.0f, -1.0f), NORMAL1},
      {Vector3(-1.0f, -1.0f, -1.0f), NORMAL0},
      {Vector3(-1.0f, 1.0f, 1.0f), NORMAL0},
      {Vector3(-1.0f, 1.0f, -1.0f), NORMAL0},
      {Vector3(1.0f, 1.0f, -1.0f), NORMAL2},
      {Vector3(-1.0f, 1.0f, 1.0f), NORMAL2},
      {Vector3(1.0f, 1.0f, 1.0f), NORMAL2},
      {Vector3(1.0f, -1.0f, -1.0f), NORMAL5},
      {Vector3(-1.0f, -1.0f, -1.0f), NORMAL5},
      {Vector3(-1.0f, 1.0f, -1.0f), NORMAL5},
      {Vector3(-1.0f, 1.0f, 1.0f), NORMAL3},
      {Vector3(-1.0f, -1.0f, 1.0f), NORMAL3},
      {Vector3(1.0f, -1.0f, 1.0f), NORMAL3},
      {Vector3(1.0f, 1.0f, 1.0f), NORMAL4},
      {Vector3(1.0f, -1.0f, 1.0f), NORMAL4},
      {Vector3(1.0f, -1.0f, -1.0f), NORMAL4},
      {Vector3(1.0f, -1.0f, 1.0f), NORMAL1},
      {Vector3(-1.0f, -1.0f, 1.0f), NORMAL1},
      {Vector3(-1.0f, -1.0f, -1.0f), NORMAL1},
      {Vector3(-1.0f, -1.0f, -1.0f), NORMAL0},
      {Vector3(-1.0f, -1.0f, 1.0f), NORMAL0},
      {Vector3(-1.0f, 1.0f, 1.0f), NORMAL0},
      {Vector3(1.0f, 1.0f, -1.0f), NORMAL2},
      {Vector3(-1.0f, 1.0f, -1.0f), NORMAL2},
      {Vector3(-1.0f, 1.0f, 1.0f), NORMAL2},
    };

    Property::Map property;
    property.Insert("aPosition", Property::VECTOR3);
    property.Insert("aNormal", Property::VECTOR3);

    VertexBuffer vertexBuffer = VertexBuffer::New(property);

    vertexBuffer.SetData(vertices, sizeof(vertices) / sizeof(Vertex));

    // create indices
    const unsigned short INDEX_CUBE[] = {
      2, 1, 0, 5, 4, 3, 8, 7, 6, 11, 10, 9, 14, 13, 12, 17, 16, 15, 20, 19, 18, 23, 22, 21, 26, 25, 24, 29, 28, 27, 32, 31, 30, 35, 34, 33};
    mGeometry = Geometry::New();
    mGeometry.AddVertexBuffer(vertexBuffer);
    mGeometry.SetIndexBuffer(INDEX_CUBE,
                             sizeof(INDEX_CUBE) / sizeof(INDEX_CUBE[0]));
    mGeometry.SetType(Geometry::TRIANGLES);
  }

  /**
   * Creates a shader using inlined variable SHADER_RENDERING_BASIC_LIGHT_VERT and SHADER_RENDERING_BASIC_LIGHT_FRAG
   *
   * Shaders are very basic and all they do is transforming vertices and interpolating
   * input per-vertex color.
   */
  void CreateCubeShader()
  {
    mShader = Shader::New(SHADER_RENDERING_BASIC_LIGHT_VERT, SHADER_RENDERING_BASIC_LIGHT_FRAG);

    float scale = 120.0f;
    mShader.RegisterProperty("light.position", Vector3(1.2 * scale, scale, 2.0 * scale));
    mShader.RegisterProperty("light.color", Vector3(1.0f, 1.0f, 1.0f));
    mShader.RegisterProperty("viewPos", Vector3(0, 0, 3.0 * scale));

    mShader.RegisterProperty("material.ambient", material[MaterialID].ambient);
    mShader.RegisterProperty("material.diffuse", material[MaterialID].diffuse);
    mShader.RegisterProperty("material.specular", material[MaterialID].specular);
    mShader.RegisterProperty("material.shininess", material[MaterialID].shininess * 128.0f);
  }

  /**
   * Function creates renderer. It turns on depth test and depth write.
   */
  void CreateRenderer()
  {
    mRenderer = Renderer::New(mGeometry, mShader);

    // Face culling is enabled to hide the backwards facing sides of the cube
    // This is sufficient to render a single object; for more complex scenes depth-testing might be required
    mRenderer.SetProperty(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK);
  }

  /**
   * Creates new actor and attaches renderer.
   */
  void CreateActor()
  {
    Window window = mApplication.GetWindow();

    float quarterWindowWidth = window.GetSize().GetWidth() * 0.25f;
    mActor                   = Actor::New();
    mActor.SetProperty(Actor::Property::COLOR, Vector4(1.0f, 1.0f, 0.6f, 1.0f));
    mActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mActor.SetProperty(Actor::Property::SIZE, Vector3(quarterWindowWidth, quarterWindowWidth, quarterWindowWidth));
    mActor.AddRenderer(mRenderer);
    window.Add(mActor);
  }

  /**
   * Plays animation
   */
  void PlayAnimation()
  {
    mAnimation = Animation::New(15.0f);
    mAnimation.SetLooping(true);
    mAnimation.AnimateBy(Property(mActor, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(360)), Vector3::ZAXIS));
    mAnimation.AnimateBy(Property(mActor, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(360)), Vector3::YAXIS));
    mAnimation.AnimateBy(Property(mActor, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(360)), Vector3::XAXIS));
    mAnimation.Play();
  }

private:
  Application& mApplication;
  TextLabel    mLabel;
  PushButton   mButton;
  Renderer     mRenderer;
  Shader       mShader;
  Geometry     mGeometry;
  Actor        mActor;
  Animation    mAnimation;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application          application = Application::New(&argc, &argv, BASIC_LIGHT_THEME);
  BasicLightController test(application);
  application.MainLoop();
  return 0;
}
