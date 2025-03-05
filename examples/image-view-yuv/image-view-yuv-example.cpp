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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <string.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
struct YuvImageInfo
{
  const char* formatName;
  const char* fileName;
};

static const YuvImageInfo IMAGE_INFOS[] =
  {
    {"YUV444", DEMO_IMAGE_DIR "gallery-small-1.jpg"},
    {"YUV420", DEMO_IMAGE_DIR "gallery-small-1-yuv420.jpg"},
    {"YUV422", DEMO_IMAGE_DIR "woodEffect.jpg"},
    ///< TODO : Need to collect more kinds of YUV!
    /// Required resources : GRAY, YUV_440, YUV_411, YUV_441
};
static const int NUM_IMAGE_INFOS = (sizeof(IMAGE_INFOS) / sizeof(IMAGE_INFOS[0]));

} // unnamed namespace

// This example shows how to display YUV images with ImageView.
//
class ImageYuvController : public ConnectionTracker
{
public:
  ImageYuvController(Application& application)
  : mApplication(application),
    mLastPoint(1.f)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ImageYuvController::Create);
  }

  ~ImageYuvController()
  {
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    mWindow = application.GetWindow();

    mWindow.KeyEventSignal().Connect(this, &ImageYuvController::OnKeyEvent);
    mWindow.SetBackgroundColor(Color::WHITE);
    mWindow.GetRootLayer().TouchedSignal().Connect(this, &ImageYuvController::OnTouch);

    mTableView = Toolkit::TableView::New(NUM_IMAGE_INFOS, 3);
    mTableView.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    mTableView.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
    mTableView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mTableView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mWindow.Add(mTableView);

    for(unsigned int index = 0u; index < NUM_IMAGE_INFOS; ++index)
    {
      TextLabel label = TextLabel::New();
      label.SetProperty(TextLabel::Property::MULTI_LINE, true);
      label.SetProperty(TextLabel::Property::TEXT, IMAGE_INFOS[index].formatName);

      mTableView.SetFitHeight(index);
      mTableView.AddChild(label, Toolkit::TableView::CellPosition(index, 0));

      ImageView image = ImageView::New(IMAGE_INFOS[index].fileName);
      {
        Property::Map map;
        map.Add(Visual::Property::TYPE, Visual::IMAGE);
        map.Add(ImageVisual::Property::URL, IMAGE_INFOS[index].fileName);
        map.Add(ImageVisual::Property::ORIENTATION_CORRECTION, false); ///< Ensure to make we load YUV plane

        image.SetProperty(ImageView::Property::IMAGE, map);
      }
      image.SetProperty(Actor::Property::SIZE, Vector2(128.0f, 128.0f));
      mTableView.AddChild(image, Toolkit::TableView::CellPosition(index, 1));

      ImageView roundedFastTrackImage = ImageView::New();
      roundedFastTrackImage.SetProperty(Actor::Property::SIZE, Vector2(128.0f, 128.0f));
      {
        Property::Map map;
        map.Add(Visual::Property::TYPE, Visual::IMAGE);
        map.Add(ImageVisual::Property::URL, IMAGE_INFOS[index].fileName);
        map.Add(DevelVisual::Property::CORNER_RADIUS, 0.5f);
        map.Add(DevelVisual::Property::CORNER_RADIUS_POLICY, Visual::Transform::Policy::RELATIVE);
        map.Add(ImageVisual::Property::ORIENTATION_CORRECTION, false); ///< Ensure to make we load YUV plane
        map.Add(DevelImageVisual::Property::FAST_TRACK_UPLOADING, true);

        roundedFastTrackImage.SetProperty(ImageView::Property::IMAGE, map);
      }
      mTableView.AddChild(roundedFastTrackImage, Toolkit::TableView::CellPosition(index, 2));
    }

    {
      TextLabel label = TextLabel::New();
      label.SetProperty(TextLabel::Property::TEXT, "Press \'R\' key to Attach / Detach images\nPress \'T\' key to Detach and Attach images immediatly\n");
      label.SetProperty(TextLabel::Property::MULTI_LINE, true);
      label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
      label.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
      mWindow.Add(label);
    }
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
      else if(event.GetKeyName() == "r" || event.GetKeyName() == "R")
      {
        if(mTableView.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
        {
          mTableView.Unparent();
        }
        else
        {
          mWindow.Add(mTableView);
        }
      }
      else if(event.GetKeyName() == "t" || event.GetKeyName() == "T")
      {
        if(mTableView.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
        {
          mTableView.Unparent();
          mWindow.Add(mTableView);
        }
        else
        {
          mWindow.Add(mTableView);
        }
      }
    }
  }

  bool OnTouch(Actor actor, const TouchEvent& event)
  {
    if(1u == event.GetPointCount())
    {
      const PointState::Type state = event.GetState(0);

      const float screenPoint = event.GetScreenPosition(0).y;

      if(PointState::DOWN == state)
      {
        mLastPoint = screenPoint;
      }
      else if(PointState::MOTION == state)
      {
        if(mTableView)
        {
          mTableView.TranslateBy(Vector3(0.0f, screenPoint - mLastPoint, 0.0f));
          mLastPoint = screenPoint;
        }
      }
    }

    return true;
  }

private:
  Application& mApplication;
  Window       mWindow;
  TableView    mTableView;
  float        mLastPoint;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  // Set YUV format load + rendering environment, before application class create.
  setenv("DALI_LOAD_IMAGE_YUV_PLANES", "1", 1);

  // Turn on each format load as YUV or not.
  setenv("DALI_ENABLE_DECODE_JPEG_TO_YUV_444", "1", 1);
  setenv("DALI_ENABLE_DECODE_JPEG_TO_YUV_422", "1", 1);
  setenv("DALI_ENABLE_DECODE_JPEG_TO_YUV_420", "1", 1);
  // TJSAMP_GRAY already supported
  setenv("DALI_ENABLE_DECODE_JPEG_TO_YUV_440", "1", 1);
  setenv("DALI_ENABLE_DECODE_JPEG_TO_YUV_411", "1", 1);
  setenv("DALI_ENABLE_DECODE_JPEG_TO_YUV_441", "1", 1);

  Application        application = Application::New(&argc, &argv);
  ImageYuvController test(application);
  application.MainLoop();
  return 0;
}
