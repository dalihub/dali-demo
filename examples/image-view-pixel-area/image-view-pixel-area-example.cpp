/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

using namespace Dali;

namespace
{
const char* IMAGE_PATH[] = { DEMO_IMAGE_DIR "gallery-large-7.jpg",
                             DEMO_IMAGE_DIR "gallery-large-12.jpg",
                             DEMO_IMAGE_DIR "gallery-large-18.jpg" };

const unsigned int NUM_IMAGES = sizeof(IMAGE_PATH) / sizeof(char*);
}

class ImageViewPixelAreaApp : public ConnectionTracker
{
public:
  ImageViewPixelAreaApp( Application& application )
  : mApplication( application ),
    mIndex(0u)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageViewPixelAreaApp::Create );
  }

  ~ImageViewPixelAreaApp()
  {
    // Nothing to do here;
  }

private:
  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &ImageViewPixelAreaApp::OnKeyEvent);

    ImageDimensions size( 510, 510 );
    float subSize = 170.f;
    float relativeSubSize = 0.33;
    Animation animation = Animation::New( 10.f );
    for( int i=0; i<3;i++ )
      for( int j=0; j<3; j++ )
      {
        mImageView[i][j] = Toolkit::ImageView::New( IMAGE_PATH[mIndex], size );
        mImageView[i][j].SetParentOrigin( ParentOrigin::CENTER );
        mImageView[i][j].SetAnchorPoint(AnchorPoint::CENTER );
        mImageView[i][j].SetPosition( subSize*(i-1)*1.1f, subSize*(j-1)*1.1f );
        mImageView[i][j].SetSize( subSize, subSize );
        stage.Add(mImageView[i][j]);

        animation.AnimateTo( Property(mImageView[i][j], Toolkit::ImageView::Property::PIXEL_AREA),
                             Vector4( relativeSubSize*i, relativeSubSize*j, relativeSubSize, relativeSubSize ),
                             AlphaFunction::BOUNCE );
      }
    animation.SetLooping( true );
    animation.Play();

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &ImageViewPixelAreaApp::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    const TouchPoint &point = touch.GetPoint(0);
    if(point.state == TouchPoint::Down)
    {
      mIndex++;
      for( int i=0; i<3;i++ )
        for( int j=0; j<3; j++ )
        {
          mImageView[i][j].SetImage( IMAGE_PATH[mIndex%NUM_IMAGES] );
        }
    }
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
  Toolkit::ImageView mImageView[3][3];
  unsigned int mIndex;
};

void RunTest( Application& application )
{
  ImageViewPixelAreaApp test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
