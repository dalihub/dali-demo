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

class ImageViewAlphaBlendApp : public ConnectionTracker
{
public:
  ImageViewAlphaBlendApp( Application& application )
  : mApplication( application ),
    mIndex(0u)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageViewAlphaBlendApp::Create );
  }

  ~ImageViewAlphaBlendApp()
  {
    // Nothing to do here;
  }

private:
  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &ImageViewAlphaBlendApp::OnKeyEvent);

    Vector4 green0 = Vector4( 0.f, 1.f, 0.f, 0.25f );
    Vector4 green1 = Vector4( 0.f, 0.25f, 0.f, 0.25f );
    BufferImage redGreen0 = CreateBufferImage( Color::RED, green0 );
    BufferImage redGreen1 = CreateBufferImage( Color::RED, green1 );
    ResourceImage testImage = ResourceImage::New( DEMO_IMAGE_DIR "gallery-large-20.jpg" );
    float imageSize = 512.f;

    Toolkit::ImageView imageView0 = Toolkit::ImageView::New( testImage );
    imageView0.SetSize(imageSize, imageSize);
    imageView0.SetParentOrigin( ParentOrigin::CENTER );
    imageView0.SetY( -imageSize*0.5f );
    stage.Add(imageView0);
    Toolkit::ImageView imageView1 = Toolkit::ImageView::New( redGreen0 );
    imageView1.SetParentOrigin( ParentOrigin::CENTER );
    imageView1.SetSize(imageSize, imageSize);
    imageView0.Add(imageView1);

    Toolkit::ImageView imageView2 = Toolkit::ImageView::New( testImage);
    imageView2.SetSize(imageSize, imageSize);
    imageView2.SetParentOrigin( ParentOrigin::CENTER );
    imageView2.SetY( imageSize*0.5f );
    stage.Add(imageView2);
    Toolkit::ImageView imageView3 = Toolkit::ImageView::New( redGreen1);
    imageView3.SetParentOrigin( ParentOrigin::CENTER );
    imageView3.SetSize(imageSize, imageSize);
    imageView2.Add(imageView3);

    imageView2.SetProperty( Toolkit::ImageView::Property::PRE_MULTIPLIED_ALPHA, true );
    imageView3.SetProperty( Toolkit::ImageView::Property::PRE_MULTIPLIED_ALPHA, true );

    Animation animation = Animation::New( 10.f );
    animation.AnimateTo(Property(imageView0, Actor::Property::COLOR), Vector4(1.0f, 1.0f, 1.0f, 0.0f), AlphaFunction::BOUNCE, TimePeriod( 2.f, 8.f ));
    animation.AnimateTo(Property(imageView2, Actor::Property::COLOR), Vector4(1.0f, 1.0f, 1.0f, 0.0f), AlphaFunction::BOUNCE, TimePeriod( 2.f, 8.f ));
    animation.SetLooping( true );
    animation.Play();
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

  BufferImage CreateBufferImage( const Vector4& color1, const Vector4& color2 )
  {
    BufferImage image = BufferImage::New( 2, 1, Pixel::RGBA8888 );
    PixelBuffer* pixelBuffer = image.GetBuffer();
    pixelBuffer[0]=0xFF * color1.x;
    pixelBuffer[1]=0xFF * color1.y;
    pixelBuffer[2]=0xFF * color1.z;
    pixelBuffer[3]=0xFF * color1.w;
    pixelBuffer[4]=0xFF * color2.x;
    pixelBuffer[5]=0xFF * color2.y;
    pixelBuffer[6]=0xFF * color2.z;
    pixelBuffer[7]=0xFF * color2.w;
    image.Update();
    return image;
  }


private:
  Application&  mApplication;
  unsigned int mIndex;
};

void RunTest( Application& application )
{
  ImageViewAlphaBlendApp test( application );

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
