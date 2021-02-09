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

#include "generated/radial-progress-basic-frag.h"
#include "generated/radial-progress-basic-vert.h"
#include "generated/radial-progress-textured-frag.h"
#include "generated/radial-progress-textured-vert.h"

using namespace Dali;

namespace // unnamed namespace
{
const char*        TEXTURE_URL    = DEMO_IMAGE_DIR "RadialEffect-280x280.png";
const unsigned int TEXTURE_WIDTH  = 280;
const unsigned int TEXTURE_HEIGHT = 280;

const int   NUMBER_OF_SIDES(64);     // number of sides of the polygon used as a stencil
const float INITIAL_DELAY(2.0f);     // initial delay before showing the circle
const float PROGRESS_DURATION(0.5f); // number of seconds to fully show the circle

} // unnamed namespace

// This example shows how to render a radial progress indicator
//
class RadialProgressController : public ConnectionTracker
{
public:
  RadialProgressController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &RadialProgressController::Create);
  }

  ~RadialProgressController()
  {
    // Nothing to do here
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::BLACK);

    // Connect to the window's key signal to allow Back and Escape to exit.
    window.KeyEventSignal().Connect(this, &RadialProgressController::OnKeyEvent);

    // 1. Create actor to show the effect
    mActor = Actor::New();
    mActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mActor.SetProperty(Actor::Property::SIZE, Vector2(TEXTURE_WIDTH, TEXTURE_HEIGHT));
    mActor.RegisterProperty("uProgress", float(1.0f));
    window.Add(mActor);

    // 1. Create stencil renderer i.e. a triangle fan in the shape of a circle
    Renderer stencilRenderer = CreatePolygon(NUMBER_OF_SIDES);
    mActor.AddRenderer(stencilRenderer);

    // 2. Create textured quad renderer
    Renderer texturedQuad = CreateTexturedQuad(TEXTURE_URL);
    mActor.AddRenderer(texturedQuad);

    // 5. Animate the progress uniform
    Animation animation = Animation::New(PROGRESS_DURATION + INITIAL_DELAY);
    animation.AnimateTo(Property(mActor, "uProgress"), float(NUMBER_OF_SIDES + 1), TimePeriod(INITIAL_DELAY, PROGRESS_DURATION));
    animation.Play();

    // 6. Exit the application when touched
    window.GetRootLayer().TouchedSignal().Connect(this, &RadialProgressController::OnTouch);
  }

  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  /**
   * Generates stencil mask geometry. Geometry is rendered as
   * a triangle fan and occupies square 2.0x2.0.
   * @param[in] numberOfSides The more subdivisions the more smooth mask animation.
   */
  Renderer CreatePolygon(unsigned int numberOfSides)
  {
    float count(numberOfSides);

    // compute radial step in radians
    const float STEP((2.0f * M_PI) / count);
    float       angle(0.0f);

    std::vector<Vector3> vertices;
    vertices.push_back(Vector3::ZERO);

    for(size_t i = 0; i <= numberOfSides; ++i)
    {
      vertices.push_back(Vector3(-0.5f * cos(angle), -0.5f * sin(angle), i + 1));
      angle += STEP;
    }

    Property::Map vertexFormat;
    vertexFormat["aPosition"] = Property::VECTOR3;

    // describe vertex format ( only 2-dimensional positions )
    VertexBuffer vertexBuffer = VertexBuffer::New(vertexFormat);
    vertexBuffer.SetData(vertices.data(), vertices.size());

    // create geometry
    Geometry geometry = Geometry::New();
    geometry.AddVertexBuffer(vertexBuffer);
    geometry.SetType(Geometry::TRIANGLE_FAN);

    Shader   shader   = Shader::New(SHADER_RADIAL_PROGRESS_BASIC_VERT, SHADER_RADIAL_PROGRESS_BASIC_FRAG);
    Renderer renderer = Renderer::New(geometry, shader);

    // Setting stencil data. We don't want to render to the color buffer so
    // with use of RenderMode property we specify that only stencil buffer will
    // be affected.
    renderer.SetProperty(Renderer::Property::RENDER_MODE, RenderMode::STENCIL);

    // Set stencil function
    renderer.SetProperty(Renderer::Property::STENCIL_FUNCTION, StencilFunction::ALWAYS);

    // Stencil function reference
    renderer.SetProperty(Renderer::Property::STENCIL_FUNCTION_REFERENCE, 1);

    // Stencil function mask
    renderer.SetProperty(Renderer::Property::STENCIL_FUNCTION_MASK, 0xFF);

    // Set stencil operations
    renderer.SetProperty(Renderer::Property::STENCIL_OPERATION_ON_FAIL, StencilOperation::KEEP);
    renderer.SetProperty(Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL, StencilOperation::KEEP);
    renderer.SetProperty(Renderer::Property::STENCIL_OPERATION_ON_Z_PASS, StencilOperation::REPLACE);

    // Stencil mask to write
    renderer.SetProperty(Renderer::Property::STENCIL_MASK, 0xFF);

    // Set depth index lower than textured quad renderer, so stencil will render first
    renderer.SetProperty(Renderer::Property::DEPTH_INDEX, 1);

    return renderer;
  }

  /**
   * Creates textured quad renderer
   */
  Renderer CreateTexturedQuad(const char* url)
  {
    // Create shader & geometry needed by Renderer

    Shader shader = Shader::New(SHADER_RADIAL_PROGRESS_TEXTURED_VERT, SHADER_RADIAL_PROGRESS_TEXTURED_FRAG);

    Property::Map vertexFormat;
    vertexFormat["aPosition"] = Property::VECTOR2;
    VertexBuffer vertexBuffer = VertexBuffer::New(vertexFormat);

    const float   P(0.5f);
    const Vector2 vertices[] = {
      Vector2(-P, -P),
      Vector2(+P, -P),
      Vector2(-P, +P),
      Vector2(+P, +P)};

    vertexBuffer.SetData(vertices, 4);

    // Instantiate quad geometry
    Geometry geometry = Geometry::New();
    geometry.AddVertexBuffer(vertexBuffer);
    geometry.SetType(Geometry::TRIANGLE_STRIP);

    // Load texture
    PixelData pixelData = Toolkit::SyncImageLoader::Load(url);
    Texture   texture   = Texture::New(TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight());
    texture.Upload(pixelData);
    texture.GenerateMipmaps();

    // Create texture set
    TextureSet textureSet = TextureSet::New();
    textureSet.SetTexture(0, texture);

    // Create renderer
    Renderer renderer = Renderer::New(geometry, shader);
    renderer.SetTextures(textureSet);

    // Set mode indicating we will use both stencil and color buffers
    renderer.SetProperty(Renderer::Property::RENDER_MODE, RenderMode::COLOR_STENCIL);

    // Stencil function - expecing drawing only when function mask matches exactly
    renderer.SetProperty(Renderer::Property::STENCIL_FUNCTION, StencilFunction::EQUAL);
    renderer.SetProperty(Renderer::Property::STENCIL_FUNCTION_REFERENCE, 1);
    renderer.SetProperty(Renderer::Property::STENCIL_FUNCTION_MASK, 0xFF);

    // We don't want to draw to the stencil, so setting stencil draw mask to 0
    renderer.SetProperty(Renderer::Property::STENCIL_MASK, 0x00);

    // Make sure the quad will render after drawing to stencil buffer
    renderer.SetProperty(Renderer::Property::DEPTH_INDEX, 2);

    return renderer;
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
      if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication;

  Actor mActor;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application              application = Application::New(&argc, &argv);
  RadialProgressController test(application);
  application.MainLoop();
  return 0;
}
