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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/animated-vector-image-visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>
#include <dali-toolkit/public-api/image-loader/image.h>
#include <dali/dali.h>
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>
#include <string>

#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* BACKGROUND_IMAGE(DEMO_IMAGE_DIR "background-gradient.jpg");
const char* TOOLBAR_IMAGE(DEMO_IMAGE_DIR "top-bar.png");
const char* APPLICATION_TITLE("Image view with encoded image buffer");

const char* IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "gallery-small-23.jpg",
  DEMO_IMAGE_DIR "woodEffect.jpg",
  DEMO_IMAGE_DIR "wood.png", // 32bits image
  // DEMO_IMAGE_DIR "heartsframe.9.png", ///< Not implemented yet.
  DEMO_IMAGE_DIR "dog-anim.webp",
  DEMO_IMAGE_DIR "World.svg",        // svg image
  DEMO_IMAGE_DIR "insta_camera.json" // lottie image
};

const unsigned int NUMBER_OF_RESOURCES = sizeof(IMAGE_PATH) / sizeof(char*);

const unsigned int VECTOR_IMAGE_INDEX          = NUMBER_OF_RESOURCES - 2u;
const unsigned int ANIMATED_VECTOR_IMAGE_INDEX = NUMBER_OF_RESOURCES - 1u;

const unsigned int NUMBER_OF_IMAGES_ROW    = 3;
const unsigned int NUMBER_OF_IMAGES_COLUMN = 2;
const unsigned int NUMBER_OF_IMAGES        = NUMBER_OF_IMAGES_ROW * NUMBER_OF_IMAGES_COLUMN;

constexpr Vector2 IMAGE_VIEW_SIZE(200.0f, 200.0f);

const unsigned int TIMER_INTERVAL = 1000; // ms

EncodedImageBuffer ConvertFileToEncodedImageBuffer(const char* url)
{
  EncodedImageBuffer buffer;
  FILE*              fp;
  fp = fopen(url, "rb");
  if(fp != NULL)
  {
    fseek(fp, 0, SEEK_END);
    size_t                size = ftell(fp);
    Dali::Vector<uint8_t> data;
    data.Resize(size);
    fseek(fp, 0, SEEK_SET);
    size_t realSize = fread(data.Begin(), sizeof(uint8_t), size, fp);
    fclose(fp);
    data.Resize(realSize);
    buffer = EncodedImageBuffer::New(data);
  }
  return buffer;
}

} // namespace

class ImageViewEncodedImageBufferApp : public ConnectionTracker
{
public:
  ImageViewEncodedImageBufferApp(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ImageViewEncodedImageBufferApp::Create);
  }

  ~ImageViewEncodedImageBufferApp()
  {
    // Nothing to do here
  }

  void Create(Application& application)
  {
    // The Init signal is received once (only) during the Application lifetime

    ImageUrl backgroundImageUrl = Image::GenerateUrl(ConvertFileToEncodedImageBuffer(BACKGROUND_IMAGE));
    ImageUrl toolbarImageUrl    = Image::GenerateUrl(ConvertFileToEncodedImageBuffer(TOOLBAR_IMAGE));

    // Creates a default view with a default tool bar.
    // The view is added to the window.
    mContentLayer = DemoHelper::CreateView(application,
                                           mView,
                                           mToolBar,
                                           backgroundImageUrl.GetUrl(),
                                           toolbarImageUrl.GetUrl(),
                                           APPLICATION_TITLE);

    // Initialize
    mState      = 0;
    mImageIndex = 0;
    UpdateImageUrl();
    UpdateImageViews();

    // Create automatic unparent and create ticker.
    mTimer = Timer::New(TIMER_INTERVAL);
    mTimer.TickSignal().Connect(this, &ImageViewEncodedImageBufferApp::OnTick);
    mTimer.Start();

    application.GetWindow().GetRootLayer().TouchedSignal().Connect(this, &ImageViewEncodedImageBufferApp::OnTouch);

    application.GetWindow().KeyEventSignal().Connect(this, &ImageViewEncodedImageBufferApp::OnKeyEvent);
  }

private:
  void UpdateImageUrl()
  {
    if(mImageIndex < NUMBER_OF_RESOURCES)
    {
      mImageBuffer = ConvertFileToEncodedImageBuffer(IMAGE_PATH[mImageIndex]);
    }

    if(mImageIndex == VECTOR_IMAGE_INDEX)
    {
      mImageBuffer.SetImageType(EncodedImageBuffer::ImageType::VECTOR_IMAGE);
    }
    else if(mImageIndex == ANIMATED_VECTOR_IMAGE_INDEX)
    {
      mImageBuffer.SetImageType(EncodedImageBuffer::ImageType::ANIMATED_VECTOR_IMAGE);
    }

    mImageUrl = Image::GenerateUrl(mImageBuffer);
  }

  void UpdateImageViews()
  {
    for(int i = 0; i < static_cast<int>(NUMBER_OF_IMAGES_ROW); i++)
    {
      for(int j = 0; j < static_cast<int>(NUMBER_OF_IMAGES_COLUMN); j++)
      {
        int viewId = i * NUMBER_OF_IMAGES_COLUMN + j;
        // Remove old image and set null
        if(mImageViews[viewId])
        {
          mImageViews[viewId].Unparent();
          mImageViews[viewId] = ImageView(); // Set null image view
        }

        bool needToCreate = true;

        // If current state don't wanna create current view, just skip
        unsigned int currentViewState = 1u << ((i + j) & 1);
        if(mState & currentViewState)
        {
          needToCreate = false;
        }

        if(needToCreate)
        {
          mImageViews[viewId] = CreateImageView(i, j);
          mContentLayer.Add(mImageViews[viewId]);
        }
      }
    }
  }
  ImageView CreateImageView(int offset_y, int offset_x)
  {
    ImageView view = ImageView::New();
    view.SetProperty(Actor::Property::SIZE, IMAGE_VIEW_SIZE);
    view.SetProperty(Actor::Property::POSITION, Vector2(IMAGE_VIEW_SIZE.x * offset_x, IMAGE_VIEW_SIZE.y * offset_y + mToolBar.GetNaturalSize().y));
    view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

    Property::Map imagePropertyMap;
    imagePropertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
    imagePropertyMap.Insert(ImageVisual::Property::URL, mImageUrl.GetUrl());

    view.SetProperty(ImageView::Property::IMAGE, imagePropertyMap);

    if(mImageIndex == ANIMATED_VECTOR_IMAGE_INDEX)
    {
      DevelControl::DoAction(view, ImageView::Property::IMAGE, DevelAnimatedVectorImageVisual::Action::PLAY, Property::Value());
    }

    return view;
  }

  bool OnTick()
  {
    mState = (mState + 1) % 4;
    UpdateImageViews();
    return true; // Keep tick always
  }

  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      // Change resource
      mImageIndex = (mImageIndex + 1) % NUMBER_OF_RESOURCES;
      // Change resource's url
      UpdateImageUrl();
      // Update image views
      UpdateImageViews();
    }
    return true;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
      {
        if(mTimer.IsRunning())
        {
          mTimer.Stop();
          mTimer.TickSignal().Disconnect(this, &ImageViewEncodedImageBufferApp::OnTick);
        }
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication;

  Toolkit::Control mView;         ///< The View instance.
  Toolkit::ToolBar mToolBar;      ///< The View's Toolbar.
  Layer            mContentLayer; ///< Content layer

  Toolkit::ImageView mImageViews[NUMBER_OF_IMAGES];

  unsigned int       mImageIndex{0u};
  EncodedImageBuffer mImageBuffer;
  ImageUrl           mImageUrl;

  // Automatic unparent and create ticker.
  Timer        mTimer;
  unsigned int mState{0u}; ///< 0 : draw all images, 1 : draw half, 2 : draw another half, 3 : don't draw images.
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                    application = Application::New(&argc, &argv, DEMO_THEME_PATH);
  ImageViewEncodedImageBufferApp test(application);
  application.MainLoop();
  return 0;
}
