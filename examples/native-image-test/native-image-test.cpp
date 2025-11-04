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

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/dali.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/native-image-source-devel.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/pixel-data-integ.h>

#include <tbm_surface.h>
#include <cstring> // memcpy

// INTERNAL INCLUDES
#include "generated/native-image-test-texture-frag.h"
#include "generated/native-image-test-texture-vert.h"
#include "shared/utility.h"

using namespace Dali;

namespace
{
//const std::string IMAGE_FILE = DEMO_IMAGE_DIR "Logo-for-demo.png";
const std::string IMAGE_FILE = DEMO_IMAGE_DIR "background-5.jpg";

/**
 * @brief Creates a shader used to render a native image
 * @param[in] nativeImage The native image
 * @return A shader to render the native image
 */
Shader CreateShader(NativeImageInterface& nativeImage)
{
  std::string fragmentShader;

  // Get custom fragment shader prefix
  fragmentShader = SHADER_NATIVE_IMAGE_TEST_TEXTURE_FRAG.data();
  nativeImage.ApplyNativeFragmentShader(fragmentShader, 1);

  return Shader::New(SHADER_NATIVE_IMAGE_TEST_TEXTURE_VERT, fragmentShader);
}

} // namespace

// This example shows how to render a simple native image
//
class NativeImageTestController : public ConnectionTracker
{
public:
  NativeImageTestController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &NativeImageTestController::Create);
  }

  ~NativeImageTestController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Respond to a touch anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &NativeImageTestController::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &NativeImageTestController::OnKeyEvent);

    // Load image
    ImageDimensions    dimensions(window.GetSize());
    Devel::PixelBuffer pixelBuffer = LoadImageFromFile(IMAGE_FILE, dimensions, FittingMode::SCALE_TO_FILL);

    if(!pixelBuffer)
    {
      DALI_LOG_ERROR("Failed to load image: %s\n", IMAGE_FILE.c_str());
      return; // Exit if image loading fails
    }

    // Ensure width and height are powers of two for NativeImageSource (depending on hardware e.g. the RPI4)
    uint32_t nativeImageWidth  = NextPowerOfTwo(pixelBuffer.GetWidth());
    uint32_t nativeImageHeight = NextPowerOfTwo(pixelBuffer.GetHeight());

    pixelBuffer.Resize(static_cast<uint16_t>(nativeImageWidth), static_cast<uint16_t>(nativeImageHeight));

    PixelData pixelData = Devel::PixelBuffer::Convert(pixelBuffer);

    // Create native image source with power-of-two dimensions
    mNativeImageSrc = Dali::NativeImageSource::New(nativeImageWidth, nativeImageHeight, Dali::NativeImageSource::COLOR_DEPTH_DEFAULT);

    mOutputWidth  = nativeImageWidth;
    mOutputHeight = nativeImageHeight;

    // Create the TBM surface
    mTbmSurface = tbm_surface_create(mOutputWidth, mOutputHeight, TBM_FORMAT_ARGB8888);
    if(!mTbmSurface)
    {
      DALI_LOG_ERROR("Failed to create tbm_surface.\n");
      return;
    }

    Dali::Any source(mTbmSurface);
    mNativeImageSrc->SetSource(source);

    auto pixelDataBuffer = Dali::Integration::GetPixelDataBuffer(pixelData);
    if(!Dali::DevelNativeImageSource::SetPixels(*mNativeImageSrc, pixelDataBuffer.buffer, pixelData.GetPixelFormat()))
    {
      DALI_LOG_ERROR("Failed to set the pixel buffer to NativeImageSource.\n");
      return;
    }

    // Set up an actor to display the texture
    mNativeTexture = Texture::New(*mNativeImageSrc);

    mDisplayActor = Actor::New();
    mDisplayActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mDisplayActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);

    Geometry geometry = DemoHelper::CreateTexturedQuad();

    Shader shader = CreateShader(*mNativeImageSrc);

    Renderer renderer = Renderer::New(geometry, shader);

    TextureSet textureSet = TextureSet::New();
    textureSet.SetTexture(0u, mNativeTexture);
    renderer.SetTextures(textureSet);

    mDisplayActor.AddRenderer(renderer);
    mDisplayActor.SetProperty(Actor::Property::SIZE, Vector2(mNativeTexture.GetWidth(), mNativeTexture.GetHeight()));

    window.Add(mDisplayActor);
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

  Dali::NativeImageSourcePtr mNativeImageSrc;

  size_t   mOutputWidth{0u};
  size_t   mOutputHeight{0u};
  size_t   mOutputStride{0u};
  uint8_t* mOutputBuffer{nullptr};

  tbm_surface_h mTbmSurface{nullptr};

  Texture mNativeTexture;
  Actor   mDisplayActor;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application               application = Application::New(&argc, &argv);
  NativeImageTestController test(application);
  application.MainLoop();
  return 0;
}
