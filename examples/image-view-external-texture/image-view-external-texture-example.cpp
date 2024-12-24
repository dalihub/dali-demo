/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visuals/animated-image-visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali/dali.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/integration-api/debug.h>
#include <string>

#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* BACKGROUND_IMAGE(DEMO_IMAGE_DIR "background-gradient.jpg");
const char* TOOLBAR_IMAGE(DEMO_IMAGE_DIR "top-bar.png");
const char* APPLICATION_TITLE("Image view w. External Textures");

const char* IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "gallery-small-23.jpg",
  DEMO_IMAGE_DIR "gallery-small-14.jpg",
  DEMO_IMAGE_DIR "gallery-small-17.jpg",
  DEMO_IMAGE_DIR "wood.png"};
const unsigned int NUMBER_OF_RESOURCES = sizeof(IMAGE_PATH) / sizeof(char*);
static_assert(NUMBER_OF_RESOURCES > 1);

const char* ANIMATED_IMAGE_PATH = DEMO_IMAGE_DIR "dog-anim.webp";

const char* MASK_IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "mask.png",
  DEMO_IMAGE_DIR "application-icon-2.png",
  DEMO_IMAGE_DIR "cluster-image-shadow.png",
  "invalid.png"};
const unsigned int NUMBER_OF_MASK_RESOURCES = sizeof(MASK_IMAGE_PATH) / sizeof(char*);

std::string EXAMPLE_INSTRUCTIONS =
  "Instructions: Change button cycles through different kinds of image url, and mask image url.";

enum UrlType
{
  REGULAR_IMAGE,
  EXTERNAL_TEXTURE,
  ENCODED_IMAGE_BUFFER,
  NUMBER_OF_URL_TYPE,
};

enum ImageVisualType
{
  NORMAL,
  ANIMATED_IMAGE,
  IMAGE_ARRAY,
  NUMBER_OF_VISUAL_TYPE,
};

enum CellPlacement
{
  TOP_BUTTON,
  MID_BUTTON,
  LOWER_BUTTON,
  IMAGE,
  NUMBER_OF_ROWS
};

const char* BUTTON_LABEL_SUFFIX[NUMBER_OF_URL_TYPE] = {
  "Normal",
  "External",
  "Encoded",
};

void ChangeUrlTypeToNextUrlType(UrlType& urlType)
{
  switch(urlType)
  {
    case UrlType::REGULAR_IMAGE:
    {
      urlType = UrlType::EXTERNAL_TEXTURE;
      break;
    }
    case UrlType::EXTERNAL_TEXTURE:
    {
      urlType = UrlType::ENCODED_IMAGE_BUFFER;
      break;
    }
    case UrlType::ENCODED_IMAGE_BUFFER:
    default:
    {
      urlType = UrlType::REGULAR_IMAGE;
      break;
    }
  }
}

ImageVisualType GetVisualTypeFromButton(Toolkit::Button button)
{
  std::string  buttonName = button.GetProperty<std::string>(Dali::Actor::Property::NAME);
  unsigned int index      = 0;

  if(buttonName != "")
  {
    index = std::stoul(buttonName);
  }

  return static_cast<ImageVisualType>(index);
}

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

std::vector<ImageUrl> gImageUrlHolder;

std::string ConvertFileToImageUrlString(const char* url, UrlType type)
{
  ImageUrl imageUrl;

  bool converted = false;

  switch(type)
  {
    case REGULAR_IMAGE:
    default:
    {
      return std::string(url);
    }
    case EXTERNAL_TEXTURE:
    {
      Devel::PixelBuffer pixelBuffer = LoadImageFromFile(url);
      if(pixelBuffer)
      {
        PixelData pixelData = Devel::PixelBuffer::Convert(pixelBuffer);

        imageUrl = Dali::Toolkit::Image::GenerateUrl(pixelData);
      }
      converted = true;
      break;
    }
    case ENCODED_IMAGE_BUFFER:
    {
      EncodedImageBuffer rawBuffer = ConvertFileToEncodedImageBuffer(url);

      if(rawBuffer)
      {
        imageUrl  = Dali::Toolkit::Image::GenerateUrl(rawBuffer);
        converted = true;
      }
      break;
    }
  }

  if(converted && imageUrl)
  {
    // Keep reference count for image url.
    gImageUrlHolder.push_back(imageUrl);
    return imageUrl.GetUrl();
  }
  else
  {
    switch(type)
    {
      default:
      {
        return "";
      }
      case EXTERNAL_TEXTURE:
      {
        return "dali://invalid";
      }
      case ENCODED_IMAGE_BUFFER:
      {
        return "enbuf://invalid";
      }
    }
  }
}

} // namespace

class ImageViewController : public ConnectionTracker
{
public:
  ImageViewController(Application& application)
  : mApplication(application),
    mCurrentPositionToggle(0, 0),
    mCurrentPositionImage(0, 0)
  {
    for(uint32_t i = 0u; i < NUMBER_OF_VISUAL_TYPE; ++i)
    {
      mImageViewMaskUrlImageIndex[i] = 0u;
      mImageViewUrlType[i]           = UrlType::REGULAR_IMAGE;
      mImageViewMaskUrlType[i]       = UrlType::REGULAR_IMAGE;
    }

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ImageViewController::Create);
  }

  ~ImageViewController()
  {
    // Nothing to do here
  }

  void Create(Application& application)
  {
    // The Init signal is received once (only) during the Application lifetime

    // Creates a default view with a default tool bar.
    // The view is added to the window.
    mContentLayer = DemoHelper::CreateView(application,
                                           mView,
                                           mToolBar,
                                           BACKGROUND_IMAGE,
                                           TOOLBAR_IMAGE,
                                           APPLICATION_TITLE);

    // Create a table view to show a pair of buttons above each image.
    mTable = Toolkit::TableView::New(CellPlacement::NUMBER_OF_ROWS, NUMBER_OF_VISUAL_TYPE);
    mTable.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mTable.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mTable.SetResizePolicy(ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS);
    Vector3 offset(0.9f, 0.70f, 0.0f);
    mTable.SetProperty(Actor::Property::SIZE_MODE_FACTOR, offset);
    mTable.SetFitHeight(CellPlacement::TOP_BUTTON);
    mTable.SetFitHeight(CellPlacement::MID_BUTTON);
    mTable.SetFitHeight(CellPlacement::LOWER_BUTTON);
    mContentLayer.Add(mTable);

    Toolkit::TextLabel instructions = Toolkit::TextLabel::New(EXAMPLE_INSTRUCTIONS);
    instructions.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    instructions.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
    instructions.SetProperty(Actor::Property::POSITION_Y, -50.0f);
    instructions.SetProperty(Toolkit::TextLabel::Property::ENABLE_AUTO_SCROLL, true);
    instructions.SetProperty(Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 10);
    mContentLayer.Add(instructions);

    for(unsigned int x = 0; x < NUMBER_OF_VISUAL_TYPE; x++)
    {
      std::string s = std::to_string(x);

      // Set changeable counter and toggle for each ImageView
      mImageViewMaskUrlImageIndex[x] = 0;
      mImageViewUrlType[x]           = UrlType::REGULAR_IMAGE;
      mImageViewMaskUrlType[x]       = UrlType::REGULAR_IMAGE;

      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetProperty(Toolkit::Button::Property::LABEL, "Mask image change");
      button.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
      button.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
      button.ClickedSignal().Connect(this, &ImageViewController::ChangeMaskImageUrl);
      button.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      button.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
      button.SetProperty(Dali::Actor::Property::NAME, s);
      mTable.AddChild(button, Toolkit::TableView::CellPosition(CellPlacement::TOP_BUTTON, x));

      Toolkit::PushButton button2 = Toolkit::PushButton::New();
      button2.SetProperty(Toolkit::Button::Property::LABEL, std::string("Image-") + std::string(BUTTON_LABEL_SUFFIX[UrlType::REGULAR_IMAGE]));
      button2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
      button2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
      button2.ClickedSignal().Connect(this, &ImageViewController::ChangeImageUrlType);
      button2.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      button2.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
      button2.SetProperty(Dali::Actor::Property::NAME, s);
      mTable.AddChild(button2, Toolkit::TableView::CellPosition(CellPlacement::MID_BUTTON, x));

      Toolkit::PushButton button3 = Toolkit::PushButton::New();
      button3.SetProperty(Toolkit::Button::Property::LABEL, std::string("Mask-") + std::string(BUTTON_LABEL_SUFFIX[UrlType::REGULAR_IMAGE]));
      button3.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
      button3.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
      button3.ClickedSignal().Connect(this, &ImageViewController::ChangeMaskImageUrlType);
      button3.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      button3.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
      button3.SetProperty(Dali::Actor::Property::NAME, s);
      mTable.AddChild(button3, Toolkit::TableView::CellPosition(CellPlacement::LOWER_BUTTON, x));

      mImageViews[x] = Toolkit::ImageView::New();
      mImageViews[x].SetProperty(Toolkit::ImageView::Property::IMAGE, CreateImageVisualPropertyMap(static_cast<ImageVisualType>(x)));
      mImageViews[x].SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      mImageViews[x].SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      mImageViews[x].SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
      mTable.AddChild(mImageViews[x], Toolkit::TableView::CellPosition(CellPlacement::IMAGE, x));
    }

    application.GetWindow().KeyEventSignal().Connect(this, &ImageViewController::OnKeyEvent);
  }

private:
  Property::Map CreateImageVisualPropertyMap(ImageVisualType visualType)
  {
    Property::Map imagePropertyMap;
    uint32_t      buttonIndex = static_cast<uint32_t>(visualType);

    switch(visualType)
    {
      case ImageVisualType::NORMAL:
      default:
      {
        imagePropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
        imagePropertyMap.Insert(Toolkit::ImageVisual::Property::URL, ConvertFileToImageUrlString(IMAGE_PATH[0], mImageViewUrlType[buttonIndex]));
        break;
      }
      case ImageVisualType::ANIMATED_IMAGE:
      {
        imagePropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::ANIMATED_IMAGE);
        imagePropertyMap.Insert(Toolkit::ImageVisual::Property::URL, ConvertFileToImageUrlString(ANIMATED_IMAGE_PATH, mImageViewUrlType[buttonIndex]));
        imagePropertyMap.Insert(Toolkit::DevelImageVisual::Property::LOOP_COUNT, 3);
        imagePropertyMap.Insert(Toolkit::DevelImageVisual::Property::STOP_BEHAVIOR, Toolkit::DevelImageVisual::StopBehavior::FIRST_FRAME);
        break;
      }
      case ImageVisualType::IMAGE_ARRAY:
      {
        imagePropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::ANIMATED_IMAGE);
        Property::Array array;
        for(uint32_t i = 0; i < NUMBER_OF_RESOURCES; ++i)
        {
          array.PushBack(ConvertFileToImageUrlString(IMAGE_PATH[i], mImageViewUrlType[buttonIndex]));
        }
        imagePropertyMap.Insert(Toolkit::ImageVisual::Property::URL, array);
        imagePropertyMap.Insert(Toolkit::ImageVisual::Property::FRAME_DELAY, 300.0f);
        imagePropertyMap.Insert(Toolkit::DevelImageVisual::Property::LOOP_COUNT, 3);
        imagePropertyMap.Insert(Toolkit::DevelImageVisual::Property::STOP_BEHAVIOR, Toolkit::DevelImageVisual::StopBehavior::FIRST_FRAME);
        break;
      }
    }

    if(mImageViewMaskUrlImageIndex[buttonIndex] < NUMBER_OF_MASK_RESOURCES)
    {
      imagePropertyMap.Insert(Toolkit::ImageVisual::Property::ALPHA_MASK_URL, ConvertFileToImageUrlString(MASK_IMAGE_PATH[mImageViewMaskUrlImageIndex[buttonIndex]], mImageViewMaskUrlType[buttonIndex]));
      imagePropertyMap.Insert(Toolkit::ImageVisual::Property::CROP_TO_MASK, false);
    }
    return imagePropertyMap;
  }

  bool ChangeMaskImageUrl(Toolkit::Button button)
  {
    ImageVisualType visualType  = GetVisualTypeFromButton(button);
    uint32_t        buttonIndex = static_cast<uint32_t>(visualType);

    ++mImageViewMaskUrlImageIndex[buttonIndex];
    if(mImageViewMaskUrlImageIndex[buttonIndex] >= NUMBER_OF_MASK_RESOURCES + 1)
    {
      mImageViewMaskUrlImageIndex[buttonIndex] = 0;
    }

    DALI_LOG_RELEASE_INFO("Change mask image url for [%d]'s image, mask url : %s\n", buttonIndex, mImageViewMaskUrlImageIndex[buttonIndex] < NUMBER_OF_MASK_RESOURCES ? MASK_IMAGE_PATH[mImageViewMaskUrlImageIndex[buttonIndex]] : "(none)");

    Toolkit::ImageView imageView = mImageViews[buttonIndex];
    imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, CreateImageVisualPropertyMap(visualType));

    gImageUrlHolder.clear();

    return true;
  }

  bool ChangeImageUrlType(Toolkit::Button button)
  {
    ImageVisualType visualType  = GetVisualTypeFromButton(button);
    uint32_t        buttonIndex = static_cast<uint32_t>(visualType);

    ChangeUrlTypeToNextUrlType(mImageViewUrlType[buttonIndex]);

    button.SetProperty(Toolkit::Button::Property::LABEL, std::string("Image-") + std::string(BUTTON_LABEL_SUFFIX[mImageViewUrlType[buttonIndex]]));

    DALI_LOG_RELEASE_INFO("Change image url type for [%d]'s image, url type : %s\n", buttonIndex, BUTTON_LABEL_SUFFIX[mImageViewUrlType[buttonIndex]]);

    Toolkit::ImageView imageView = mImageViews[buttonIndex];
    imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, CreateImageVisualPropertyMap(visualType));

    gImageUrlHolder.clear();

    return true;
  }

  bool ChangeMaskImageUrlType(Toolkit::Button button)
  {
    ImageVisualType visualType  = GetVisualTypeFromButton(button);
    uint32_t        buttonIndex = static_cast<uint32_t>(visualType);

    ChangeUrlTypeToNextUrlType(mImageViewMaskUrlType[buttonIndex]);

    button.SetProperty(Toolkit::Button::Property::LABEL, std::string("Mask-") + std::string(BUTTON_LABEL_SUFFIX[mImageViewMaskUrlType[buttonIndex]]));

    DALI_LOG_RELEASE_INFO("Change mask image url type for [%d]'s image, mask url type : %s\n", buttonIndex, BUTTON_LABEL_SUFFIX[mImageViewMaskUrlType[buttonIndex]]);

    Toolkit::ImageView imageView = mImageViews[buttonIndex];
    imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, CreateImageVisualPropertyMap(visualType));

    gImageUrlHolder.clear();

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
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication;

  Toolkit::Control mView;         ///< The View instance.
  Toolkit::ToolBar mToolBar;      ///< The View's Toolbar.
  Layer            mContentLayer; ///< Content layer

  Toolkit::TableView mTable;
  Toolkit::ImageView mImageViews[NUMBER_OF_VISUAL_TYPE];

  uint32_t mImageViewMaskUrlImageIndex[NUMBER_OF_VISUAL_TYPE];
  UrlType  mImageViewUrlType[NUMBER_OF_VISUAL_TYPE];
  UrlType  mImageViewMaskUrlType[NUMBER_OF_VISUAL_TYPE];

  Toolkit::TableView::CellPosition mCurrentPositionToggle;
  Toolkit::TableView::CellPosition mCurrentPositionImage;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application         application = Application::New(&argc, &argv, DEMO_THEME_PATH);
  ImageViewController test(application);
  application.MainLoop();
  return 0;
}
