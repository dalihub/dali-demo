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

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali/dali.h>

// INTERNAL INCLUDES
#include "generated/compressed-texture-formats-example-frag.h"
#include "generated/compressed-texture-formats-example-vert.h"
#include "shared/utility.h"

using namespace Dali;
using Dali::Toolkit::TextLabel;

namespace
{
const char* IMAGE_FILENAME_ETC                = DEMO_IMAGE_DIR "tx-etc1.ktx";
const char* IMAGE_FILENAME_ASTC_LINEAR        = DEMO_IMAGE_DIR "tx-astc-4x4-linear.ktx";
const char* IMAGE_FILENAME_ASTC_LINEAR_NATIVE = DEMO_IMAGE_DIR "tx-astc-4x4-linear-native.astc";

/**
 * @brief Create a renderer to render an image and adds it to an actor
 * @param[in] imagePath The path where the image file is located
 * @param[in] actor The actor that will be used to render the image
 * @param[in[ geometry The geometry to use
 * @param[in] shader The shader to use
 */
void AddImage(const char* imagePath, Actor& actor, Geometry& geometry, Shader& shader)
{
  //Load the texture
  Texture    texture    = DemoHelper::LoadTexture(imagePath);
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, texture);

  //Create the renderer
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  //Set actor size and add the renderer
  actor.SetProperty(Actor::Property::SIZE, Vector2(texture.GetWidth(), texture.GetHeight()));
  actor.AddRenderer(renderer);
}

} // namespace
/**
 * @brief This example shows 3 images, each of a different compressed texture type.
 * If built and run on a OpenGL ES 3.1 compatable target, then all 3 images will display.
 * Otherwise, the top image will display and the other 2 will appear as black squares.
 */
class CompressedTextureFormatsController : public ConnectionTracker
{
public:
  CompressedTextureFormatsController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &CompressedTextureFormatsController::Create);
  }

  ~CompressedTextureFormatsController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Setup a TableView to hold a grid of images and labels.
    Toolkit::TableView table = Toolkit::TableView::New(3u, 2u);
    table.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    table.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    table.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    table.SetRelativeWidth(0u, 0.5f);
    table.SetRelativeWidth(1u, 0.5f);
    table.SetRelativeHeight(0u, 1.0f / 3.0f);
    table.SetRelativeHeight(1u, 1.0f / 3.0f);
    table.SetRelativeHeight(2u, 1.0f / 3.0f);

    // Add text labels.
    TextLabel textLabel = TextLabel::New("ETC1 (KTX):");
    textLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    textLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    textLabel.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    textLabel.SetProperty(Toolkit::TextLabel::Property::MULTI_LINE, true);
    table.AddChild(textLabel, Toolkit::TableView::CellPosition(0u, 0u));
    table.SetCellAlignment(Toolkit::TableView::CellPosition(0u, 0u), HorizontalAlignment::LEFT, VerticalAlignment::CENTER);

    textLabel = TextLabel::New("ASTC (KTX) 4x4 linear:");
    textLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    textLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    textLabel.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    textLabel.SetProperty(Toolkit::TextLabel::Property::MULTI_LINE, true);
    table.AddChild(textLabel, Toolkit::TableView::CellPosition(1u, 0u));
    table.SetCellAlignment(Toolkit::TableView::CellPosition(1u, 0u), HorizontalAlignment::LEFT, VerticalAlignment::CENTER);

    textLabel = TextLabel::New("ASTC (Native) 4x4 linear:");
    textLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    textLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    textLabel.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    textLabel.SetProperty(Toolkit::TextLabel::Property::MULTI_LINE, true);
    table.AddChild(textLabel, Toolkit::TableView::CellPosition(2u, 0u));
    table.SetCellAlignment(Toolkit::TableView::CellPosition(2u, 0u), HorizontalAlignment::LEFT, VerticalAlignment::CENTER);

    //Create the geometry and the shader renderers will use
    Geometry geometry = DemoHelper::CreateTexturedQuad();
    Shader   shader   = Shader::New(SHADER_COMPRESSED_TEXTURE_FORMATS_EXAMPLE_VERT, SHADER_COMPRESSED_TEXTURE_FORMATS_EXAMPLE_FRAG);

    // Add images.
    Actor actor = Actor::New();
    actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    AddImage(IMAGE_FILENAME_ETC, actor, geometry, shader);
    table.AddChild(actor, Toolkit::TableView::CellPosition(0u, 1u));
    table.SetCellAlignment(Toolkit::TableView::CellPosition(0u, 1u), HorizontalAlignment::CENTER, VerticalAlignment::CENTER);

    actor = Actor::New();
    actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    AddImage(IMAGE_FILENAME_ASTC_LINEAR, actor, geometry, shader);
    table.AddChild(actor, Toolkit::TableView::CellPosition(1u, 1u));
    table.SetCellAlignment(Toolkit::TableView::CellPosition(1u, 1u), HorizontalAlignment::CENTER, VerticalAlignment::CENTER);

    actor = Actor::New();
    actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    AddImage(IMAGE_FILENAME_ASTC_LINEAR_NATIVE, actor, geometry, shader);
    table.AddChild(actor, Toolkit::TableView::CellPosition(2u, 1u));
    table.SetCellAlignment(Toolkit::TableView::CellPosition(2u, 1u), HorizontalAlignment::CENTER, VerticalAlignment::CENTER);

    window.Add(table);

    // Respond to touch and key signals
    window.GetRootLayer().TouchedSignal().Connect(this, &CompressedTextureFormatsController::OnTouch);
    window.KeyEventSignal().Connect(this, &CompressedTextureFormatsController::OnKeyEvent);
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
  Application                        application = Application::New(&argc, &argv);
  CompressedTextureFormatsController test(application);
  application.MainLoop();
  return 0;
}
