/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/rendering/shader-devel.h>

// INTERNAL INCLUDES
#include "shared/utility.h"

using namespace Dali;
using Dali::Toolkit::TextLabel;

namespace
{

const char* IMAGE_FILENAME_ETC         =        DEMO_IMAGE_DIR "tx-etc1.ktx";
const char* IMAGE_FILENAME_ASTC_LINEAR =        DEMO_IMAGE_DIR "tx-astc-4x4-linear.ktx";
const char* IMAGE_FILENAME_ASTC_LINEAR_NATIVE = DEMO_IMAGE_DIR "tx-astc-4x4-linear-native.astc";

/*
//VERT
#version 430
layout( location = 0 ) in vec2 aPosition;
layout( location = 1 ) in vec2 aTexCoord;

layout ( set = 0, binding = 0, std140) uniform VertexData
{
  mat4 uMvpMatrix;
  vec3 uSize;
};

layout ( location = 0) out vec2 vTexCoord;

void main()
{
  vec4 position = vec4(aPosition,0.0,1.0)*vec4(uSize,1.0);
  gl_Position = uMvpMatrix * position;
  vTexCoord = aTexCoord;
}

//FRAG
#version 430
layout( location = 0 ) in vec2 vTexCoord;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
  vec4 uColor;
};

layout( set = 0, binding = 2 ) uniform sampler2D sTexture;

layout( location = 0 ) out vec4 fragColor;

void main()
{
  fragColor = texture( sTexture, vTexCoord ) * uColor;
}
*/

std::vector<uint32_t> VERTEX_SHADER_TEXTURE = {
	0x07230203,0x00010000,0x00080007,0x00000034,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0009000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000c,0x00000027,0x00000031,
	0x00000032,0x00030003,0x00000002,0x000001ae,0x00040005,0x00000004,0x6e69616d,0x00000000,
	0x00050005,0x00000009,0x69736f70,0x6e6f6974,0x00000000,0x00050005,0x0000000c,0x736f5061,
	0x6f697469,0x0000006e,0x00050005,0x00000015,0x74726556,0x61447865,0x00006174,0x00060006,
	0x00000015,0x00000000,0x70764d75,0x7274614d,0x00007869,0x00050006,0x00000015,0x00000001,
	0x7a695375,0x00000065,0x00030005,0x00000017,0x00000000,0x00060005,0x00000025,0x505f6c67,
	0x65567265,0x78657472,0x00000000,0x00060006,0x00000025,0x00000000,0x505f6c67,0x7469736f,
	0x006e6f69,0x00070006,0x00000025,0x00000001,0x505f6c67,0x746e696f,0x657a6953,0x00000000,
	0x00070006,0x00000025,0x00000002,0x435f6c67,0x4470696c,0x61747369,0x0065636e,0x00030005,
	0x00000027,0x00000000,0x00050005,0x00000031,0x78655476,0x726f6f43,0x00000064,0x00050005,
	0x00000032,0x78655461,0x726f6f43,0x00000064,0x00040047,0x0000000c,0x0000001e,0x00000000,
	0x00040048,0x00000015,0x00000000,0x00000005,0x00050048,0x00000015,0x00000000,0x00000023,
	0x00000000,0x00050048,0x00000015,0x00000000,0x00000007,0x00000010,0x00050048,0x00000015,
	0x00000001,0x00000023,0x00000040,0x00030047,0x00000015,0x00000002,0x00040047,0x00000017,
	0x00000022,0x00000000,0x00040047,0x00000017,0x00000021,0x00000000,0x00050048,0x00000025,
	0x00000000,0x0000000b,0x00000000,0x00050048,0x00000025,0x00000001,0x0000000b,0x00000001,
	0x00050048,0x00000025,0x00000002,0x0000000b,0x00000003,0x00030047,0x00000025,0x00000002,
	0x00040047,0x00000031,0x0000001e,0x00000000,0x00040047,0x00000032,0x0000001e,0x00000001,
	0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,0x00000020,
	0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000007,0x00000007,
	0x00040017,0x0000000a,0x00000006,0x00000002,0x00040020,0x0000000b,0x00000001,0x0000000a,
	0x0004003b,0x0000000b,0x0000000c,0x00000001,0x0004002b,0x00000006,0x0000000e,0x00000000,
	0x0004002b,0x00000006,0x0000000f,0x3f800000,0x00040018,0x00000013,0x00000007,0x00000004,
	0x00040017,0x00000014,0x00000006,0x00000003,0x0004001e,0x00000015,0x00000013,0x00000014,
	0x00040020,0x00000016,0x00000002,0x00000015,0x0004003b,0x00000016,0x00000017,0x00000002,
	0x00040015,0x00000018,0x00000020,0x00000001,0x0004002b,0x00000018,0x00000019,0x00000001,
	0x00040020,0x0000001a,0x00000002,0x00000014,0x00040015,0x00000022,0x00000020,0x00000000,
	0x0004002b,0x00000022,0x00000023,0x00000001,0x0004001c,0x00000024,0x00000006,0x00000023,
	0x0005001e,0x00000025,0x00000007,0x00000006,0x00000024,0x00040020,0x00000026,0x00000003,
	0x00000025,0x0004003b,0x00000026,0x00000027,0x00000003,0x0004002b,0x00000018,0x00000028,
	0x00000000,0x00040020,0x00000029,0x00000002,0x00000013,0x00040020,0x0000002e,0x00000003,
	0x00000007,0x00040020,0x00000030,0x00000003,0x0000000a,0x0004003b,0x00000030,0x00000031,
	0x00000003,0x0004003b,0x0000000b,0x00000032,0x00000001,0x00050036,0x00000002,0x00000004,
	0x00000000,0x00000003,0x000200f8,0x00000005,0x0004003b,0x00000008,0x00000009,0x00000007,
	0x0004003d,0x0000000a,0x0000000d,0x0000000c,0x00050051,0x00000006,0x00000010,0x0000000d,
	0x00000000,0x00050051,0x00000006,0x00000011,0x0000000d,0x00000001,0x00070050,0x00000007,
	0x00000012,0x00000010,0x00000011,0x0000000e,0x0000000f,0x00050041,0x0000001a,0x0000001b,
	0x00000017,0x00000019,0x0004003d,0x00000014,0x0000001c,0x0000001b,0x00050051,0x00000006,
	0x0000001d,0x0000001c,0x00000000,0x00050051,0x00000006,0x0000001e,0x0000001c,0x00000001,
	0x00050051,0x00000006,0x0000001f,0x0000001c,0x00000002,0x00070050,0x00000007,0x00000020,
	0x0000001d,0x0000001e,0x0000001f,0x0000000f,0x00050085,0x00000007,0x00000021,0x00000012,
	0x00000020,0x0003003e,0x00000009,0x00000021,0x00050041,0x00000029,0x0000002a,0x00000017,
	0x00000028,0x0004003d,0x00000013,0x0000002b,0x0000002a,0x0004003d,0x00000007,0x0000002c,
	0x00000009,0x00050091,0x00000007,0x0000002d,0x0000002b,0x0000002c,0x00050041,0x0000002e,
	0x0000002f,0x00000027,0x00000028,0x0003003e,0x0000002f,0x0000002d,0x0004003d,0x0000000a,
	0x00000033,0x00000032,0x0003003e,0x00000031,0x00000033,0x000100fd,0x00010038
};

std::vector<uint32_t> FRAGMENT_SHADER_TEXTURE = {
	0x07230203,0x00010000,0x00080007,0x0000001d,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x00000011,0x00030010,
	0x00000004,0x00000007,0x00030003,0x00000002,0x000001ae,0x00040005,0x00000004,0x6e69616d,
	0x00000000,0x00050005,0x00000009,0x67617266,0x6f6c6f43,0x00000072,0x00050005,0x0000000d,
	0x78655473,0x65727574,0x00000000,0x00050005,0x00000011,0x78655476,0x726f6f43,0x00000064,
	0x00050005,0x00000014,0x67617246,0x61746144,0x00000000,0x00050006,0x00000014,0x00000000,
	0x6c6f4375,0x0000726f,0x00030005,0x00000016,0x00000000,0x00040047,0x00000009,0x0000001e,
	0x00000000,0x00040047,0x0000000d,0x00000022,0x00000000,0x00040047,0x0000000d,0x00000021,
	0x00000002,0x00040047,0x00000011,0x0000001e,0x00000000,0x00050048,0x00000014,0x00000000,
	0x00000023,0x00000000,0x00030047,0x00000014,0x00000002,0x00040047,0x00000016,0x00000022,
	0x00000000,0x00040047,0x00000016,0x00000021,0x00000001,0x00020013,0x00000002,0x00030021,
	0x00000003,0x00000002,0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,
	0x00000004,0x00040020,0x00000008,0x00000003,0x00000007,0x0004003b,0x00000008,0x00000009,
	0x00000003,0x00090019,0x0000000a,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
	0x00000001,0x00000000,0x0003001b,0x0000000b,0x0000000a,0x00040020,0x0000000c,0x00000000,
	0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000000,0x00040017,0x0000000f,0x00000006,
	0x00000002,0x00040020,0x00000010,0x00000001,0x0000000f,0x0004003b,0x00000010,0x00000011,
	0x00000001,0x0003001e,0x00000014,0x00000007,0x00040020,0x00000015,0x00000002,0x00000014,
	0x0004003b,0x00000015,0x00000016,0x00000002,0x00040015,0x00000017,0x00000020,0x00000001,
	0x0004002b,0x00000017,0x00000018,0x00000000,0x00040020,0x00000019,0x00000002,0x00000007,
	0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,0x00000005,0x0004003d,
	0x0000000b,0x0000000e,0x0000000d,0x0004003d,0x0000000f,0x00000012,0x00000011,0x00050057,
	0x00000007,0x00000013,0x0000000e,0x00000012,0x00050041,0x00000019,0x0000001a,0x00000016,
	0x00000018,0x0004003d,0x00000007,0x0000001b,0x0000001a,0x00050085,0x00000007,0x0000001c,
	0x00000013,0x0000001b,0x0003003e,0x00000009,0x0000001c,0x000100fd,0x00010038
};

/**
 * @brief Create a renderer to render an image and adds it to an actor
 * @param[in] imagePath The path where the image file is located
 * @param[in] actor The actor that will be used to render the image
 * @param[in[ geometry The geometry to use
 * @param[in] shader The shader to use
 */
void AddImage( const char*imagePath, Actor& actor, Geometry& geometry, Shader& shader )
{
  //Load the texture
  Texture texture = DemoHelper::LoadTexture( imagePath );
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture( 0u, texture );

  //Create the renderer
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  //Set actor size and add the renderer
  actor.SetSize( texture.GetWidth(), texture.GetHeight() );
  actor.AddRenderer( renderer );
}

}
/**
 * @brief This example shows 3 images, each of a different compressed texture type.
 * If built and run on a OpenGL ES 3.1 compatable target, then all 3 images will display.
 * Otherwise, the top image will display and the other 2 will appear as black squares.
 */
class CompressedTextureFormatsController : public ConnectionTracker
{
public:

  CompressedTextureFormatsController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &CompressedTextureFormatsController::Create );
  }

  ~CompressedTextureFormatsController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    // Setup a TableView to hold a grid of images and labels.
    Toolkit::TableView table = Toolkit::TableView::New( 3u, 2u );
    table.SetAnchorPoint( AnchorPoint::CENTER );
    table.SetParentOrigin( ParentOrigin::CENTER );
    table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    table.SetRelativeWidth( 0u, 0.5f );
    table.SetRelativeWidth( 1u, 0.5f );
    table.SetRelativeHeight( 0u, 1.0f / 3.0f );
    table.SetRelativeHeight( 1u, 1.0f / 3.0f );
    table.SetRelativeHeight( 2u, 1.0f / 3.0f );


    // Add text labels.
    TextLabel textLabel = TextLabel::New( "ETC1 (KTX):" );
    textLabel.SetAnchorPoint( AnchorPoint::CENTER );
    textLabel.SetParentOrigin( ParentOrigin::CENTER );
    textLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    textLabel.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, true );
    table.AddChild( textLabel, Toolkit::TableView::CellPosition( 0u, 0u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 0u, 0u ), HorizontalAlignment::LEFT, VerticalAlignment::CENTER );

    textLabel = TextLabel::New( "ASTC (KTX) 4x4 linear:" );
    textLabel.SetAnchorPoint( AnchorPoint::CENTER );
    textLabel.SetParentOrigin( ParentOrigin::CENTER );
    textLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    textLabel.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, true );
    table.AddChild( textLabel, Toolkit::TableView::CellPosition( 1u, 0u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 1u, 0u ), HorizontalAlignment::LEFT, VerticalAlignment::CENTER );

    textLabel = TextLabel::New( "ASTC (Native) 4x4 linear:" );
    textLabel.SetAnchorPoint( AnchorPoint::CENTER );
    textLabel.SetParentOrigin( ParentOrigin::CENTER );
    textLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    textLabel.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, true );
    table.AddChild( textLabel, Toolkit::TableView::CellPosition( 2u, 0u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 2u, 0u ), HorizontalAlignment::LEFT, VerticalAlignment::CENTER );

    //Create the geometry and the shader renderers will use
    Geometry geometry = DemoHelper::CreateTexturedQuad();
    Shader shader = DevelShader::New<uint32_t>( VERTEX_SHADER_TEXTURE,
                                                FRAGMENT_SHADER_TEXTURE,
                                                DevelShader::ShaderLanguage::SPIRV_1_0, Property::Map() );

    // Add images.
    Actor actor = Actor::New();
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetParentOrigin( ParentOrigin::CENTER );
    AddImage( IMAGE_FILENAME_ETC, actor, geometry, shader  );
    table.AddChild( actor, Toolkit::TableView::CellPosition( 0u, 1u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 0u, 1u ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );

    actor = Actor::New();
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetParentOrigin( ParentOrigin::CENTER );
    AddImage( IMAGE_FILENAME_ASTC_LINEAR, actor, geometry, shader );
    table.AddChild( actor, Toolkit::TableView::CellPosition( 1u, 1u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 1u, 1u ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );

    actor = Actor::New();
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetParentOrigin( ParentOrigin::CENTER );
    AddImage( IMAGE_FILENAME_ASTC_LINEAR_NATIVE, actor, geometry, shader );
    table.AddChild( actor, Toolkit::TableView::CellPosition( 2u, 1u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 2u, 1u ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );

    stage.Add( table );

    // Respond to touch and key signals
    stage.GetRootLayer().TouchSignal().Connect( this, &CompressedTextureFormatsController::OnTouch );
    stage.KeyEventSignal().Connect(this, &CompressedTextureFormatsController::OnKeyEvent);
  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  CompressedTextureFormatsController test( application );
  application.MainLoop();
  return 0;
}
