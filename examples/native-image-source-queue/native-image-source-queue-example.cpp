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
#include <cstring>

#include <dali/devel-api/adaptor-framework/native-image-source-devel.h>
#include <dali/devel-api/adaptor-framework/native-image-source-queue.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread.h>

#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include "shared/utility.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
class NativeImageRenderThread : public Dali::Thread
{
public:
  NativeImageRenderThread()
  : Thread(),
    mMutex(),
    mDestroyed(false)
  {
  }

  ~NativeImageRenderThread()
  {
  }

  void Finalize()
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    mDestroyed = true;
  }

  void Run()
  {
    while(true)
    {
      {
        Dali::Mutex::ScopedLock lock(mMutex);
        if(mDestroyed)
        {
          break;
        }
      }

      RenderToNative();
    }
  }

protected:
  static void FillRainbowBuffer(uint8_t* buffer, uint32_t width, uint32_t height, uint32_t strideBytes, float frameOffset = 0.0f)
  {
    static const uint32_t bytesPerPixel = 4;

    // Write something to buffer!
    for(uint32_t i = 0; i < height; ++i)
    {
      for(uint32_t j = 0; j < width; ++j)
      {
        const uint32_t index       = i * strideBytes + j * bytesPerPixel;
        const float    widthOffset = ((static_cast<float>(j) + 0.5f) / static_cast<float>(width));

        buffer[index + 0u] = FloatToColor(RainbowOffset(frameOffset + widthOffset + 0.0f / 3.0f));
        buffer[index + 1u] = FloatToColor(RainbowOffset(frameOffset + widthOffset + 1.0f / 3.0f));
        buffer[index + 2u] = FloatToColor(RainbowOffset(frameOffset + widthOffset + 2.0f / 3.0f));
        buffer[index + 3u] = FloatToColor(0.1f + 0.9f * widthOffset);
      }
    }
  }

private:
  inline static uint8_t FloatToColor(float value)
  {
    if(value < 0.0f)
    {
      return 0u;
    }
    uint32_t returnValue = static_cast<uint32_t>(value * 255.0f);
    if(returnValue > 255u)
    {
      return 255u;
    }

    return static_cast<uint8_t>(returnValue);
  }

  inline static float RainbowOffset(float value)
  {
    // Ensure to make value range in [0.0f 1.0f)
    value -= static_cast<int>(value);

    while(value < 0.0f)
    {
      value += 1.0f;
    }
    while(value >= 1.0f)
    {
      value -= 1.0f;
    }

    if(value < 1.0f / 6.0f)
    {
      return value * 6.0f;
    }
    if(value < 3.0f / 6.0f)
    {
      return 1.0f;
    }
    if(value < 4.0f / 6.0f)
    {
      return 4.0f - value * 6.0f;
    }
    return 0.0f;
  }

  virtual void RenderToNative() = 0;

private:
  Dali::Mutex mMutex;

  bool mDestroyed;
};

class NativeImageSourceRenderThread : public NativeImageRenderThread
{
public:
  NativeImageSourceRenderThread(NativeImageSourcePtr nativeSource)
  : NativeImageRenderThread(),
    mNativeSource(nativeSource),
    mOffset(Dali::Random::Range(0.0f, 1.0f))
  {
  }

private:
  void RenderToNative() override
  {
    if(mNativeSource)
    {
      uint32_t width, height, strideBytes;
      uint8_t* buffer = DevelNativeImageSource::AcquireBuffer(*mNativeSource, width, height, strideBytes);

      if(buffer != nullptr)
      {
        FillRainbowBuffer(buffer, width, height, strideBytes, mOffset);

        // Don't forget to call ReleaseBuffer
        DevelNativeImageSource::ReleaseBuffer(*mNativeSource, Rect<uint32_t>());
      }
      else
      {
        DALI_LOG_ERROR("AcquireBuffer not supported! Ignore to render thread\n");
      }

      // DevNote : Since Acquire + Release make render thread block.
      //           let we just finalize NativeImageSource thread now.
      Finalize();
    }
  }

private:
  NativeImageSourcePtr mNativeSource;
  float                mOffset;
};

class NativeImageSourceQueueRenderThread : public NativeImageRenderThread
{
public:
  NativeImageSourceQueueRenderThread(NativeImageSourceQueuePtr nativeQueue)
  : NativeImageRenderThread(),
    mNativeQueue(nativeQueue)
  {
  }

private:
  void RenderToNative() override
  {
    if(mNativeQueue)
    {
      if(!mNativeQueue->CanDequeueBuffer())
      {
        // Ignore last source and try again
        mNativeQueue->IgnoreSourceImage();
        if(!mNativeQueue->CanDequeueBuffer())
        {
          // Invalid to render! Ignore.
          return;
        }
      }
      uint32_t width, height, strideBytes;
      uint8_t* buffer = mNativeQueue->DequeueBuffer(width, height, strideBytes);

      if(buffer != nullptr)
      {
        static uint32_t       frameCount    = 0;
        static const uint32_t frameCountMax = 1000;

        const float frameOffset = ((static_cast<float>(frameCount) + 0.5f) / static_cast<float>(frameCountMax));

        FillRainbowBuffer(buffer, width, height, strideBytes, frameOffset);

        frameCount = (frameCount + 1) % frameCountMax;

        // Don't forget to call EnqueueBuffer
        mNativeQueue->EnqueueBuffer(buffer);
      }
    }
  }

private:
  NativeImageSourceQueuePtr mNativeQueue;
};

} // namespace

// This example shows how to create and use a NativeImageSourceQueueController as the target of the render task.
//
class NativeImageSourceQueueController : public ConnectionTracker
{
public:
  NativeImageSourceQueueController(Application& application)
  : mApplication(application),
    mNativeSource(),
    mNativeQueue(),
    mSourceRenderThreadPtr(nullptr),
    mQueueRenderThreadPtr(nullptr),
    mNativeSourceSupported(false),
    mNativeSourceQueueSupported(false)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &NativeImageSourceQueueController::Create);
  }

  ~NativeImageSourceQueueController()
  {
    TerminateRenderThread();
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    window.KeyEventSignal().Connect(this, &NativeImageSourceQueueController::OnKeyEvent);

    mNativeSource = NativeImageSource::New(123, 345, NativeImageSource::ColorDepth::COLOR_DEPTH_32);

    // Check whether current platform support NativeImageSource or not.
    if(mNativeSource && !mNativeSource->GetNativeImageSource().Empty())
    {
      mNativeSourceSupported = true;

      mSourceRenderThreadPtr = new NativeImageSourceRenderThread(mNativeSource);
      mSourceRenderThreadPtr->Start();
    }

    mNativeQueue = NativeImageSourceQueue::New(123, 345, NativeImageSourceQueue::ColorFormat::BGRA8888);

    // Check whether current platform support NativeImageSourceQueue or not.
    if(mNativeQueue && !mNativeQueue->GetNativeImageSourceQueue().Empty())
    {
      mNativeSourceQueueSupported = true;

      mQueueRenderThreadPtr = new NativeImageSourceQueueRenderThread(mNativeQueue);
      mQueueRenderThreadPtr->Start();
    }

    TextLabel supportedLabel = TextLabel::New();
    supportedLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    supportedLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    supportedLabel.SetProperty(TextLabel::Property::TEXT,
                               std::string("Support NativeImageSource ") + std::string(mNativeSourceSupported ? "T" : "F") + std::string(" Queue ") + std::string(mNativeSourceQueueSupported ? "T" : "F"));
    window.Add(supportedLabel);

    if(mNativeSource)
    {
      ImageUrl nativeImageUrl = Image::GenerateUrl(mNativeSource, false);

      ImageView nativeImageViewer = ImageView::New(nativeImageUrl.GetUrl());
      nativeImageViewer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      nativeImageViewer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      nativeImageViewer.SetProperty(Actor::Property::SIZE, Vector2(123.0f, 345.0f));
      nativeImageViewer.SetProperty(Actor::Property::POSITION, Vector2(-(123.0f * 0.5f + 1.0f), 0.0f));

      window.Add(nativeImageViewer);

      // Trick to make render native image always.
      Renderer nativeImageRenderer = nativeImageViewer.GetRendererAt(0u);
      nativeImageRenderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::CONTINUOUSLY);
    }
    if(mNativeQueue)
    {
      ImageUrl nativeImageUrl = Image::GenerateUrl(mNativeQueue, false);

      ImageView nativeImageViewer = ImageView::New(nativeImageUrl.GetUrl());
      nativeImageViewer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      nativeImageViewer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      nativeImageViewer.SetProperty(Actor::Property::SIZE, Vector2(123.0f, 345.0f));
      nativeImageViewer.SetProperty(Actor::Property::POSITION, Vector2(123.0f * 0.5f + 1.0f, 0.0f));

      window.Add(nativeImageViewer);

      // Trick to make render native image always.
      Renderer nativeImageRenderer = nativeImageViewer.GetRendererAt(0u);
      nativeImageRenderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::CONTINUOUSLY);
    }

    mBlurView = Control::New();
    mBlurView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mBlurView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mBlurView.SetProperty(Actor::Property::SIZE, Vector2(752.0f, 125.0f));
    mBlurView.SetBackgroundColor(Vector4(0.5f, 0.3f, 0.2f, 0.2f));
    mBlurView.SetRenderEffect(Toolkit::BackgroundBlurEffect::New(40u));
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        TerminateRenderThread();
        mApplication.Quit();
      }
      else if(event.GetKeyName() == "1")
      {
        if(mBlurView)
        {
          if(mBlurView.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
          {
            mBlurView.Unparent();
          }
          else
          {
            Window window = mApplication.GetWindow();
            window.Add(mBlurView);
          }
        }
      }
      else if(event.GetKeyName() == "2")
      {
        if(mNativeSourceSupported)
        {
          DALI_LOG_ERROR("Regenerate TBM surface result\n");
          if(mSourceRenderThreadPtr)
          {
            mSourceRenderThreadPtr->Finalize();
            mSourceRenderThreadPtr->Join();
            delete mSourceRenderThreadPtr;
            mSourceRenderThreadPtr = nullptr;
          }
          mSourceRenderThreadPtr = new NativeImageSourceRenderThread(mNativeSource);
          mSourceRenderThreadPtr->Start();
        }
      }
    }
  }

  void TerminateRenderThread()
  {
    if(mSourceRenderThreadPtr)
    {
      mSourceRenderThreadPtr->Finalize();
      mSourceRenderThreadPtr->Join();
      delete mSourceRenderThreadPtr;
      mSourceRenderThreadPtr = nullptr;
    }
    if(mQueueRenderThreadPtr)
    {
      mQueueRenderThreadPtr->Finalize();
      mQueueRenderThreadPtr->Join();
      delete mQueueRenderThreadPtr;
      mQueueRenderThreadPtr = nullptr;
    }
  }

private:
  Application& mApplication;

  NativeImageSourcePtr      mNativeSource;
  NativeImageSourceQueuePtr mNativeQueue;

  NativeImageSourceRenderThread*      mSourceRenderThreadPtr;
  NativeImageSourceQueueRenderThread* mQueueRenderThreadPtr;

  Control mBlurView;

  bool mNativeSourceSupported;
  bool mNativeSourceQueueSupported;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                      application = Application::New(&argc, &argv);
  NativeImageSourceQueueController test(application);
  application.MainLoop();
  return 0;
}
