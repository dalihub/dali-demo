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

// INTERNAL INCLUDES
#include "generated/benchmark-frag.h"
#include "generated/benchmark-vert.h"
#include "shared/utility.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "gallery-medium-1.jpg",
  DEMO_IMAGE_DIR "gallery-medium-2.jpg",
  DEMO_IMAGE_DIR "gallery-medium-3.jpg",
  DEMO_IMAGE_DIR "gallery-medium-4.jpg",
  DEMO_IMAGE_DIR "gallery-medium-5.jpg",
  DEMO_IMAGE_DIR "gallery-medium-6.jpg",
  DEMO_IMAGE_DIR "gallery-medium-7.jpg",
  DEMO_IMAGE_DIR "gallery-medium-8.jpg",
  DEMO_IMAGE_DIR "gallery-medium-9.jpg",
  DEMO_IMAGE_DIR "gallery-medium-10.jpg",
  DEMO_IMAGE_DIR "gallery-medium-11.jpg",
  DEMO_IMAGE_DIR "gallery-medium-12.jpg",
  DEMO_IMAGE_DIR "gallery-medium-13.jpg",
  DEMO_IMAGE_DIR "gallery-medium-14.jpg",
  DEMO_IMAGE_DIR "gallery-medium-15.jpg",
  DEMO_IMAGE_DIR "gallery-medium-16.jpg",
  DEMO_IMAGE_DIR "gallery-medium-17.jpg",
  DEMO_IMAGE_DIR "gallery-medium-18.jpg",
  DEMO_IMAGE_DIR "gallery-medium-19.jpg",
  DEMO_IMAGE_DIR "gallery-medium-20.jpg",
  DEMO_IMAGE_DIR "gallery-medium-21.jpg",
  DEMO_IMAGE_DIR "gallery-medium-22.jpg",
  DEMO_IMAGE_DIR "gallery-medium-23.jpg",
  DEMO_IMAGE_DIR "gallery-medium-24.jpg",
  DEMO_IMAGE_DIR "gallery-medium-25.jpg",
  DEMO_IMAGE_DIR "gallery-medium-26.jpg",
  DEMO_IMAGE_DIR "gallery-medium-27.jpg",
  DEMO_IMAGE_DIR "gallery-medium-28.jpg",
  DEMO_IMAGE_DIR "gallery-medium-29.jpg",
  DEMO_IMAGE_DIR "gallery-medium-30.jpg",
  DEMO_IMAGE_DIR "gallery-medium-31.jpg",
  DEMO_IMAGE_DIR "gallery-medium-32.jpg",
  DEMO_IMAGE_DIR "gallery-medium-33.jpg",
  DEMO_IMAGE_DIR "gallery-medium-34.jpg",
  DEMO_IMAGE_DIR "gallery-medium-35.jpg",
  DEMO_IMAGE_DIR "gallery-medium-36.jpg",
  DEMO_IMAGE_DIR "gallery-medium-37.jpg",
  DEMO_IMAGE_DIR "gallery-medium-38.jpg",
  DEMO_IMAGE_DIR "gallery-medium-39.jpg",
  DEMO_IMAGE_DIR "gallery-medium-40.jpg",
  DEMO_IMAGE_DIR "gallery-medium-41.jpg",
  DEMO_IMAGE_DIR "gallery-medium-42.jpg",
  DEMO_IMAGE_DIR "gallery-medium-43.jpg",
  DEMO_IMAGE_DIR "gallery-medium-44.jpg",
  DEMO_IMAGE_DIR "gallery-medium-45.jpg",
  DEMO_IMAGE_DIR "gallery-medium-46.jpg",
  DEMO_IMAGE_DIR "gallery-medium-47.jpg",
  DEMO_IMAGE_DIR "gallery-medium-48.jpg",
  DEMO_IMAGE_DIR "gallery-medium-49.jpg",
  DEMO_IMAGE_DIR "gallery-medium-50.jpg",
  DEMO_IMAGE_DIR "gallery-medium-51.jpg",
  DEMO_IMAGE_DIR "gallery-medium-52.jpg",
  DEMO_IMAGE_DIR "gallery-medium-53.jpg",
};

const char* NINEPATCH_IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "selection-popup-bg.1.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.2.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.3.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.4.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.5.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.6.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.7.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.8.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.9.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.10.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.11.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.12.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.13.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.14.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.15.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.16.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.17.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.18.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.19.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.20.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.21.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.22.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.23.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.24.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.25.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.26.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.27.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.28.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.29.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.30.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.31.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.32.9.png",
  DEMO_IMAGE_DIR "selection-popup-bg.33.9.png",
  DEMO_IMAGE_DIR "button-disabled.9.png",
  DEMO_IMAGE_DIR "button-down.9.png",
  DEMO_IMAGE_DIR "button-down-disabled.9.png",
  DEMO_IMAGE_DIR "button-up-1.9.png",
  DEMO_IMAGE_DIR "button-up-2.9.png",
  DEMO_IMAGE_DIR "button-up-3.9.png",
  DEMO_IMAGE_DIR "button-up-4.9.png",
  DEMO_IMAGE_DIR "button-up-5.9.png",
  DEMO_IMAGE_DIR "button-up-6.9.png",
  DEMO_IMAGE_DIR "button-up-7.9.png",
  DEMO_IMAGE_DIR "button-up-8.9.png",
  DEMO_IMAGE_DIR "button-up-9.9.png",
  DEMO_IMAGE_DIR "button-up-10.9.png",
  DEMO_IMAGE_DIR "button-up-11.9.png",
  DEMO_IMAGE_DIR "button-up-12.9.png",
  DEMO_IMAGE_DIR "button-up-13.9.png",
  DEMO_IMAGE_DIR "button-up-14.9.png",
  DEMO_IMAGE_DIR "button-up-15.9.png",
  DEMO_IMAGE_DIR "button-up-16.9.png",
  DEMO_IMAGE_DIR "button-up-17.9.png",
};

const unsigned int NUM_IMAGES           = sizeof(IMAGE_PATH) / sizeof(char*);
const unsigned int NUM_NINEPATCH_IMAGES = sizeof(NINEPATCH_IMAGE_PATH) / sizeof(char*);

const float ANIMATION_TIME(5.0f); // animation length in seconds

struct VertexWithTexture
{
  Vector2 position;
  Vector2 texCoord;
};

bool         gUseMesh(false);
bool         gNinePatch(false);
unsigned int gRowsPerPage(25);
unsigned int gColumnsPerPage(25);
unsigned int gPageCount(13);

Renderer CreateRenderer(unsigned int index, Geometry geometry, Shader shader)
{
  Renderer    renderer   = Renderer::New(geometry, shader);
  const char* imagePath  = !gNinePatch ? IMAGE_PATH[index] : NINEPATCH_IMAGE_PATH[index];
  Texture     texture    = DemoHelper::LoadTexture(imagePath);
  TextureSet  textureSet = TextureSet::New();
  textureSet.SetTexture(0u, texture);
  renderer.SetTextures(textureSet);
  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::OFF);
  return renderer;
}

} // namespace
// Test application to compare performance between using a mesh and ImageView
// By default, the application consist of 10 pages of 25x25 Image views, this can be modified using the following command line arguments:
// -r NumberOfRows  (Modifies the number of rows per page)
// -c NumberOfColumns (Modifies the number of columns per page)
// -p NumberOfPages (Modifies the nimber of pages )
// --use-mesh ( Use new renderer API (as ImageView) but shares renderers between actors when possible )
// --nine-patch ( Use nine patch images )

//
class Benchmark : public ConnectionTracker
{
public:
  Benchmark(Application& application)
  : mApplication(application),
    mRowsPerPage(gRowsPerPage),
    mColumnsPerPage(gColumnsPerPage),
    mPageCount(gPageCount)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &Benchmark::Create);
  }

  ~Benchmark() = default;

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);
    Vector2 windowSize = window.GetSize();

    window.GetRootLayer().SetProperty(Layer::Property::DEPTH_TEST, false);

    mSize = Vector3(windowSize.x / mColumnsPerPage, windowSize.y / mRowsPerPage, 0.0f);

    // Respond to a click anywhere on the window
    window.GetRootLayer().TouchedSignal().Connect(this, &Benchmark::OnTouch);

    // Respond to key events
    window.KeyEventSignal().Connect(this, &Benchmark::OnKeyEvent);

    if(gUseMesh)
    {
      CreateMeshActors();
    }
    else
    {
      CreateImageViews();
    }

    ShowAnimation();
  }

  bool OnTouch(Actor actor, const TouchEvent& touch)
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  const char* ImagePath(int i)
  {
    return !gNinePatch ? IMAGE_PATH[i % NUM_IMAGES] : NINEPATCH_IMAGE_PATH[i % NUM_NINEPATCH_IMAGES];
  }

  void CreateImageViews()
  {
    Window       window = mApplication.GetWindow();
    unsigned int actorCount(mRowsPerPage * mColumnsPerPage * mPageCount);
    mImageView.resize(actorCount);

    for(size_t i(0); i < actorCount; ++i)
    {
      mImageView[i] = ImageView::New(ImagePath(i));
      mImageView[i].SetProperty(Actor::Property::SIZE, Vector3(0.0f, 0.0f, 0.0f));
      mImageView[i].SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
      window.Add(mImageView[i]);
    }
  }

  void CreateMeshActors()
  {
    unsigned int numImages = !gNinePatch ? NUM_IMAGES : NUM_NINEPATCH_IMAGES;

    //Create all the renderers
    std::vector<Renderer> renderers(numImages);
    Shader                shader   = Shader::New(SHADER_BENCHMARK_VERT, SHADER_BENCHMARK_FRAG);
    Geometry              geometry = DemoHelper::CreateTexturedQuad();
    for(unsigned int i(0); i < numImages; ++i)
    {
      renderers[i] = CreateRenderer(i, geometry, shader);
    }

    //Create the actors
    Window       window = mApplication.GetWindow();
    unsigned int actorCount(mRowsPerPage * mColumnsPerPage * mPageCount);
    mActor.resize(actorCount);
    for(size_t i(0); i < actorCount; ++i)
    {
      mActor[i] = Actor::New();
      mActor[i].AddRenderer(renderers[i % numImages]);
      mActor[i].SetProperty(Actor::Property::SIZE, Vector3(0.0f, 0.0f, 0.0f));
      window.Add(mActor[i]);
    }
  }

  void OnAnimationEnd(Animation& source)
  {
    if(source == mShow)
    {
      ScrollAnimation();
    }
    else if(source == mScroll)
    {
      HideAnimation();
    }
    else
    {
      mApplication.Quit();
    }
  }

  void ShowAnimation()
  {
    Window        window = mApplication.GetWindow();
    const Vector2 windowSize(window.GetSize());
    Vector3       initialPosition(windowSize.width * 0.5f, windowSize.height * 0.5f, 1000.0f);

    unsigned int totalColumns = mColumnsPerPage * mPageCount;

    size_t count(0);
    float  xpos, ypos;

    float totalDuration(10.0f);
    float durationPerActor(0.5f);
    float delayBetweenActors = (totalDuration - durationPerActor) / (mRowsPerPage * mColumnsPerPage);

    mShow = Animation::New(totalDuration);

    for(size_t i(0); i < totalColumns; ++i)
    {
      xpos = mSize.x * i;

      for(size_t j(0); j < mRowsPerPage; ++j)
      {
        ypos = mSize.y * j;

        float delay    = 0.0f;
        float duration = 0.0f;
        if(count < (static_cast<size_t>(mRowsPerPage) * mColumnsPerPage))
        {
          duration = durationPerActor;
          delay    = delayBetweenActors * count;
        }
        if(gUseMesh)
        {
          mActor[count].SetProperty(Actor::Property::POSITION, initialPosition);
          mActor[count].SetProperty(Actor::Property::SIZE, Vector3(0.0f, 0.0f, 0.0f));
          mActor[count].SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(0.0f), Vector3::XAXIS));
          mShow.AnimateTo(Property(mActor[count], Actor::Property::POSITION), Vector3(xpos + mSize.x * 0.5f, ypos + mSize.y * 0.5f, 0.0f), AlphaFunction::EASE_OUT_BACK, TimePeriod(delay, duration));
          mShow.AnimateTo(Property(mActor[count], Actor::Property::SIZE), mSize, AlphaFunction::EASE_OUT_BACK, TimePeriod(delay, duration));
        }
        else
        {
          mImageView[count].SetProperty(Actor::Property::POSITION, initialPosition);
          mImageView[count].SetProperty(Actor::Property::SIZE, Vector3(0.0f, 0.0f, 0.0f));
          mImageView[count].SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(0.0f), Vector3::XAXIS));
          mShow.AnimateTo(Property(mImageView[count], Actor::Property::POSITION), Vector3(xpos + mSize.x * 0.5f, ypos + mSize.y * 0.5f, 0.0f), AlphaFunction::EASE_OUT_BACK, TimePeriod(delay, duration));
          mShow.AnimateTo(Property(mImageView[count], Actor::Property::SIZE), mSize, AlphaFunction::EASE_OUT_BACK, TimePeriod(delay, duration));
        }
        ++count;
      }
    }
    mShow.Play();
    mShow.FinishedSignal().Connect(this, &Benchmark::OnAnimationEnd);
  }

  void ScrollAnimation()
  {
    Window  window = mApplication.GetWindow();
    Vector3 windowSize(window.GetSize());

    mScroll = Animation::New(10.0f);
    size_t actorCount(static_cast<size_t>(mRowsPerPage) * mColumnsPerPage * mPageCount);
    for(size_t i(0); i < actorCount; ++i)
    {
      if(gUseMesh)
      {
        mScroll.AnimateBy(Property(mActor[i], Actor::Property::POSITION), Vector3(-4.0f * windowSize.x, 0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(0.0f, 3.0f));
        mScroll.AnimateBy(Property(mActor[i], Actor::Property::POSITION), Vector3(-4.0f * windowSize.x, 0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(3.0f, 3.0f));
        mScroll.AnimateBy(Property(mActor[i], Actor::Property::POSITION), Vector3(-4.0f * windowSize.x, 0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(6.0f, 2.0f));
        mScroll.AnimateBy(Property(mActor[i], Actor::Property::POSITION), Vector3(12.0f * windowSize.x, 0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(8.0f, 2.0f));
      }
      else
      {
        mScroll.AnimateBy(Property(mImageView[i], Actor::Property::POSITION), Vector3(-4.0f * windowSize.x, 0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(0.0f, 3.0f));
        mScroll.AnimateBy(Property(mImageView[i], Actor::Property::POSITION), Vector3(-4.0f * windowSize.x, 0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(3.0f, 3.0f));
        mScroll.AnimateBy(Property(mImageView[i], Actor::Property::POSITION), Vector3(-4.0f * windowSize.x, 0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(6.0f, 2.0f));
        mScroll.AnimateBy(Property(mImageView[i], Actor::Property::POSITION), Vector3(12.0f * windowSize.x, 0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(8.0f, 2.0f));
      }
    }
    mScroll.Play();
    mScroll.FinishedSignal().Connect(this, &Benchmark::OnAnimationEnd);
  }

  void HideAnimation()
  {
    size_t       count(0);
    unsigned int actorsPerPage(mRowsPerPage * mColumnsPerPage);

    unsigned int totalColumns = mColumnsPerPage * mPageCount;

    float finalZ = mApplication.GetWindow().GetRenderTaskList().GetTask(0).GetCameraActor().GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION).z;
    float totalDuration(5.0f);
    float durationPerActor(0.5f);
    float delayBetweenActors = (totalDuration - durationPerActor) / (mRowsPerPage * mColumnsPerPage);

    mHide = Animation::New(totalDuration * 2.0f);

    for(size_t i(0); i < mRowsPerPage; ++i)
    {
      for(size_t j(0); j < totalColumns; ++j)
      {
        float delay    = 0.0f;
        float duration = 0.0f;
        if(count < actorsPerPage)
        {
          duration = durationPerActor;
          delay    = delayBetweenActors * count;
        }

        if(gUseMesh)
        {
          mHide.AnimateTo(Property(mActor[count], Actor::Property::ORIENTATION), Quaternion(Radian(Degree(70.0f)), Vector3::XAXIS), AlphaFunction::EASE_OUT, TimePeriod(delay, duration));
          mHide.AnimateBy(Property(mActor[count], Actor::Property::POSITION_Z), finalZ, AlphaFunction::EASE_OUT_BACK, TimePeriod(delay + delayBetweenActors * actorsPerPage + duration, duration));
        }
        else
        {
          mHide.AnimateTo(Property(mImageView[count], Actor::Property::ORIENTATION), Quaternion(Radian(Degree(70.0f)), Vector3::XAXIS), AlphaFunction::EASE_OUT, TimePeriod(delay, duration));
          mHide.AnimateBy(Property(mImageView[count], Actor::Property::POSITION_Z), finalZ, AlphaFunction::EASE_OUT_BACK, TimePeriod(delay + delayBetweenActors * actorsPerPage + duration, duration));
        }
        ++count;
      }
    }

    mHide.Play();
    mHide.FinishedSignal().Connect(this, &Benchmark::OnAnimationEnd);
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

  std::vector<Actor>     mActor;
  std::vector<ImageView> mImageView;

  Vector3      mSize;
  unsigned int mRowsPerPage;
  unsigned int mColumnsPerPage;
  unsigned int mPageCount;

  Animation mShow;
  Animation mScroll;
  Animation mHide;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  for(int i(1); i < argc; ++i)
  {
    std::string arg(argv[i]);
    if(arg.compare("--use-mesh") == 0)
    {
      gUseMesh = true;
    }
    else if(arg.compare("--nine-patch") == 0)
    {
      gNinePatch = true;
    }
    else if(arg.compare(0, 2, "-r") == 0)
    {
      gRowsPerPage = atoi(arg.substr(2, arg.size()).c_str());
    }
    else if(arg.compare(0, 2, "-c") == 0)
    {
      gColumnsPerPage = atoi(arg.substr(2, arg.size()).c_str());
    }
    else if(arg.compare(0, 2, "-p") == 0)
    {
      gPageCount = atoi(arg.substr(2, arg.size()).c_str());
    }
  }

  Benchmark test(application);
  application.MainLoop();

  return 0;
}
