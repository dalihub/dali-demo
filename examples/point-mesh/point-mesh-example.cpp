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

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>

// INTERNAL INCLUDES
#include "shared/utility.h"
#include "shared/view.h"
#include "generated/point-mesh-vert.h"
#include "generated/point-mesh-frag.h"

using namespace Dali;

namespace
{
const char* MATERIAL_SAMPLE(DEMO_IMAGE_DIR "gallery-small-48.jpg");
const char* MATERIAL_SAMPLE2(DEMO_IMAGE_DIR "gallery-medium-19.jpg");

Geometry CreateGeometry()
{
  // Create vertices
  struct Vertex
  {
    Vector2 position;
    float   hue;
  };

  const unsigned int numSides = 20;
  Vertex             polyhedraVertexData[numSides];
  float              angle       = 0;
  float              sectorAngle = 2.0f * Math::PI / (float)numSides;

  for(unsigned int i = 0; i < numSides; ++i)
  {
    polyhedraVertexData[i].position.x = sinf(angle) * 0.5f;
    polyhedraVertexData[i].position.y = cosf(angle) * 0.5f;
    polyhedraVertexData[i].hue        = angle / (2.0f * Math::PI);
    angle += sectorAngle;
  }

  Property::Map polyhedraVertexFormat;
  polyhedraVertexFormat["aPosition"] = Property::VECTOR2;
  polyhedraVertexFormat["aHue"]      = Property::FLOAT;
  VertexBuffer polyhedraVertices     = VertexBuffer::New(polyhedraVertexFormat);
  polyhedraVertices.SetData(polyhedraVertexData, numSides);

  // Create the geometry object
  Geometry polyhedraGeometry = Geometry::New();
  polyhedraGeometry.AddVertexBuffer(polyhedraVertices);
  polyhedraGeometry.SetType(Geometry::POINTS);

  return polyhedraGeometry;
}

} // anonymous namespace

// This example shows how to use a simple mesh
//
class ExampleController : public ConnectionTracker
{
public:
  /**
   * The example controller constructor.
   * @param[in] application The application instance
   */
  ExampleController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ExampleController::Create);
  }

  /**
   * The example controller destructor
   */
  ~ExampleController()
  {
    // Nothing to do here;
  }

  /**
   * Invoked upon creation of application
   * @param[in] application The application instance
   */
  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.KeyEventSignal().Connect(this, &ExampleController::OnKeyEvent);

    mWindowSize = window.GetSize();

    // The Init signal is received once (only) during the Application lifetime

    Texture texture0 = DemoHelper::LoadTexture(MATERIAL_SAMPLE);
    Texture texture1 = DemoHelper::LoadTexture(MATERIAL_SAMPLE2);

    Shader shader = Shader::New(SHADER_POINT_MESH_VERT, SHADER_POINT_MESH_FRAG);

    TextureSet textureSet = TextureSet::New();
    textureSet.SetTexture(0u, texture0);
    textureSet.SetTexture(1u, texture1);

    Geometry geometry = CreateGeometry();

    mRenderer = Renderer::New(geometry, shader);
    mRenderer.SetTextures(textureSet);

    mMeshActor = Actor::New();
    mMeshActor.AddRenderer(mRenderer);
    mMeshActor.SetProperty(Actor::Property::SIZE, Vector2(400, 400));

    mMeshActor.RegisterProperty("uFadeColor", Color::GREEN);

    mRenderer.RegisterProperty("uFadeColor", Color::MAGENTA);
    mRenderer.RegisterProperty("uPointSize", 80.0f);
    mRenderer.SetProperty(Renderer::Property::DEPTH_INDEX, 0);

    mMeshActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mMeshActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    window.Add(mMeshActor);

    Animation animation = Animation::New(15);
    KeyFrames keyFrames = KeyFrames::New();
    keyFrames.Add(0.0f, Vector4::ZERO);
    keyFrames.Add(1.0f, Vector4(1.0f, 0.0f, 1.0f, 1.0f));

    animation.AnimateBy(Property(mMeshActor, Actor::Property::ORIENTATION), Quaternion(Degree(360), Vector3::ZAXIS));

    animation.SetLooping(true);
    animation.Play();

    window.SetBackgroundColor(Vector4(0.0f, 0.2f, 0.2f, 1.0f));
  }

  /**
   * Invoked whenever the quit button is clicked
   * @param[in] button the quit button
   */
  bool OnQuitButtonClicked(Toolkit::Button button)
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
  Application& mApplication; ///< Application instance
  Vector3      mWindowSize;  ///< The size of the window

  Renderer mRenderer;
  Actor    mMeshActor;
  Renderer mRenderer2;
  Actor    mMeshActor2;
  Timer    mChangeImageTimer;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application       application = Application::New(&argc, &argv);
  ExampleController test(application);
  application.MainLoop();
  return 0;
}
