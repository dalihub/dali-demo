/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

/**
 * @file simple-text-renderer-example.cpp
 * @brief Basic usage of Text Renderer utility.
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/text/text-utils-devel.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>

using namespace std;
using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const std::string IMAGE1 = DEMO_IMAGE_DIR "application-icon-1.png";
const std::string IMAGE2 = DEMO_IMAGE_DIR "application-icon-6.png";

#define MAKE_SHADER(A)#A

const std::string VERSION_3_ES = "#version 300 es\n";

const char* VERTEX_SHADER = MAKE_SHADER(
  precision mediump float;

  in vec2 aPosition;
  in vec2 aTexCoord;

  out vec2 vUV;

  uniform vec3 uSize;
  uniform mat4 uMvpMatrix;

  void main()
  {
    vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
    vertexPosition.xyz *= uSize;
    gl_Position = uMvpMatrix * vertexPosition;

    vUV = aTexCoord;
  }
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
  precision mediump float;

  in vec2 vUV;

  out vec4 FragColor;

  uniform sampler2D sAlbedo;
  uniform vec4 uColor;

  void main()
  {
    vec4 color = texture( sAlbedo, vUV );
    FragColor = vec4( color.rgb, uColor.a * color.a );
  }
);

Renderer CreateRenderer()
{
  // Create the geometry.
  struct Vertex
  {
    Dali::Vector2 position;
    Dali::Vector2 texCoord;
  };

  static const Vertex vertices[] = {{ Dali::Vector2( -0.5f, -0.5f ), Dali::Vector2( 0.0f, 0.0f ) },
                                    { Dali::Vector2(  0.5f, -0.5f ), Dali::Vector2( 1.0f, 0.0f ) },
                                    { Dali::Vector2( -0.5f,  0.5f ), Dali::Vector2( 0.0f, 1.0f ) },
                                    { Dali::Vector2(  0.5f,  0.5f ), Dali::Vector2( 1.0f, 1.0f ) }};

  Property::Map property;
  property.Add("aPosition", Property::VECTOR2).Add("aTexCoord", Property::VECTOR2);

  PropertyBuffer vertexBuffer = PropertyBuffer::New(property);

  vertexBuffer.SetData(vertices, sizeof(vertices) / sizeof(Vertex));

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);

  geometry.SetType(Geometry::TRIANGLE_STRIP);

  // Create the shader
  Shader shader = Shader::New( VERSION_3_ES + VERTEX_SHADER, VERSION_3_ES + FRAGMENT_SHADER );

  // Create the renderer

  Renderer renderer = Renderer::New( geometry, shader );

  return renderer;
}

TextureSet CreateTextureSet( const Dali::Toolkit::DevelText::RendererParameters& textParameters, const std::vector<std::string>& embeddedItems )
{

  Dali::Vector<Dali::Toolkit::DevelText::EmbeddedItemInfo> embeddedItemLayout;

  Devel::PixelBuffer pixelBuffer = Toolkit::DevelText::Render( textParameters, embeddedItemLayout );


  const int dstWidth = static_cast<int>( pixelBuffer.GetWidth() );
  const int dstHeight = static_cast<int>( pixelBuffer.GetHeight() );

  unsigned int index = 0u;
  for( const auto& itemLayout : embeddedItemLayout )
  {
    int width = static_cast<int>( itemLayout.size.width );
    int height = static_cast<int>( itemLayout.size.height );
    int x = static_cast<int>( itemLayout.position.x );
    int y = static_cast<int>( itemLayout.position.y );

    Dali::Devel::PixelBuffer itemPixelBuffer = Dali::LoadImageFromFile( embeddedItems[index++] );
    itemPixelBuffer.Resize( width, height );
    itemPixelBuffer.Rotate( itemLayout.angle );

    width = static_cast<int>( itemPixelBuffer.GetWidth() );
    height = static_cast<int>( itemPixelBuffer.GetHeight() );

    Dali::Pixel::Format itemPixelFormat = itemPixelBuffer.GetPixelFormat();

    // Check if the item is out of the buffer.

    if( ( x + width < 0 ) ||
        ( x > dstWidth ) ||
        ( y < 0 ) ||
        ( y - height > dstHeight ) )
    {
      // The embedded item is completely out of the buffer.
      continue;
    }

    // Crop if it exceeds the boundaries of the destination buffer.
    int layoutX = 0;
    int layoutY = 0;
    int cropX = 0;
    int cropY = 0;
    int newWidth = width;
    int newHeight = height;

    bool crop = false;

    if( 0 > x )
    {
      newWidth += x;
      cropX = std::abs( x );
      crop = true;
    }
    else
    {
      layoutX = x;
    }

    if( cropX + newWidth > dstWidth )
    {
      crop = true;
      newWidth -= ( ( cropX + newWidth ) - dstWidth );
    }

    layoutY = y;
    if( 0.f > layoutY )
    {
      newHeight += layoutY;
      cropY = std::abs(layoutY);
      crop = true;
    }

    if( cropY + newHeight > dstHeight )
    {
      crop = true;
      newHeight -= ( ( cropY + newHeight ) - dstHeight );
    }

    uint16_t uiCropX = static_cast<uint16_t>(cropX);
    uint16_t uiCropY = static_cast<uint16_t>(cropY);
    uint16_t uiNewWidth = static_cast<uint16_t>(newWidth);
    uint16_t uiNewHeight = static_cast<uint16_t>(newHeight);

    if( crop )
    {
      itemPixelBuffer.Crop( uiCropX, uiCropY, uiNewWidth, uiNewHeight );
    }

    // Blend the item pixel buffer with the text's color according its blending mode.
    if( Dali::TextAbstraction::ColorBlendingMode::MULTIPLY == itemLayout.colorBlendingMode )
    {
      Dali::Devel::PixelBuffer buffer = Dali::Devel::PixelBuffer::New( uiNewWidth,
                                                                       uiNewHeight,
                                                                       itemPixelFormat );

      unsigned char* bufferPtr = buffer.GetBuffer();
      const unsigned char* itemBufferPtr = itemPixelBuffer.GetBuffer();
      const unsigned int bytesPerPixel = Dali::Pixel::GetBytesPerPixel(itemPixelFormat);
      const unsigned int size = uiNewWidth * uiNewHeight * bytesPerPixel;

      for (unsigned int i = 0u; i < size; i += bytesPerPixel)
      {
        *(bufferPtr + 0u) = static_cast<unsigned char>( static_cast<float>( *(itemBufferPtr + 0u) ) * textParameters.textColor.r );
        *(bufferPtr + 1u) = static_cast<unsigned char>( static_cast<float>( *(itemBufferPtr + 1u) ) * textParameters.textColor.g );
        *(bufferPtr + 2u) = static_cast<unsigned char>( static_cast<float>( *(itemBufferPtr + 2u) ) * textParameters.textColor.b );
        *(bufferPtr + 3u) = static_cast<unsigned char>( static_cast<float>( *(itemBufferPtr + 3u) ) * textParameters.textColor.a );

        itemBufferPtr += bytesPerPixel;
        bufferPtr += bytesPerPixel;
      }

      itemPixelBuffer = buffer;
    }

    Dali::Toolkit::DevelText::UpdateBuffer(itemPixelBuffer, pixelBuffer, layoutX, layoutY, true);
  }

  PixelData pixelData = Devel::PixelBuffer::Convert( pixelBuffer );

  Texture texture = Texture::New( TextureType::TEXTURE_2D,
                                  pixelData.GetPixelFormat(),
                                  pixelData.GetWidth(),
                                  pixelData.GetHeight() );
  texture.Upload(pixelData);

  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture( 0u, texture );

  return textureSet;
}

} // namespace


/**
 * @brief The main class of the demo.
 */
class SimpleTextRendererExample : public ConnectionTracker
{
public:

  SimpleTextRendererExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &SimpleTextRendererExample::Create );
  }

  ~SimpleTextRendererExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );
    stage.SetBackgroundColor( Vector4( 0.04f, 0.345f, 0.392f, 1.0f ) );

    stage.KeyEventSignal().Connect(this, &SimpleTextRendererExample::OnKeyEvent);

    const std::string image1 = "<item 'width'=26 'height'=26 'url'='" + IMAGE1 + "'/>";
    const std::string image2 = "<item 'width'=26 'height'=26/>";

    Dali::Toolkit::DevelText::RendererParameters textParameters;
    textParameters.text = "Hello " + image1 + " world " + image2 + " this " + image1 + " is " + image2 + " a " + image1 + " demo " + image2 + " of " + image1 + " circular " + image2 + " text " + image1 + " width " + image2 + " icons.";
    textParameters.horizontalAlignment = "center";
    textParameters.verticalAlignment = "center";
    textParameters.circularAlignment = "center";
    textParameters.fontFamily = "SamsungUI";
    textParameters.fontWeight = "";
    textParameters.fontWidth = "";
    textParameters.fontSlant = "";
    textParameters.layout = "circular";
    textParameters.textColor = Color::BLACK;
    textParameters.fontSize = 25.f;
    textParameters.textWidth = 360u;
    textParameters.textHeight = 360u;
    textParameters.radius = 180u;
    textParameters.beginAngle = 15.f;
    textParameters.incrementAngle = 360.f;
    textParameters.ellipsisEnabled = true;
    textParameters.markupEnabled = true;

    std::vector<std::string> embeddedItems = { IMAGE2, IMAGE2, IMAGE2, IMAGE2, IMAGE2 };

    TextureSet textureSet = CreateTextureSet( textParameters, embeddedItems );

    Renderer renderer = CreateRenderer();
    renderer.SetTextures( textureSet );

    Actor actor = Actor::New();
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetParentOrigin( ParentOrigin::CENTER );
    actor.SetPosition( 0.f, 0.f);
    actor.SetSize( 360.f, 360.f );
    actor.SetColor( Color::WHITE );

    actor.AddRenderer( renderer );

    stage.Add( actor );
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  Application& mApplication;
};

/** Entry point for Linux & Tizen applications */
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  SimpleTextRendererExample test( application );

  application.MainLoop();

  return 0;
}
