/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>

using namespace Dali;

namespace
{
const char* const IMAGE_PATH ( DEMO_IMAGE_DIR "gallery-large-20.jpg" );
}

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

    auto green0 = Vector4( 0.f, 1.f, 0.f, 0.25f );
    auto green1 = Vector4( 0.f, 0.25f, 0.f, 0.25f );
    auto redGreen0 = CreateTexture( Color::RED, green0 );
    auto redGreen1 = CreateTexture( Color::RED, green1 );
    float imageSize = 512.f;

    Toolkit::ImageView imageView0 = CreateImageView( IMAGE_PATH );
    imageView0.SetSize(imageSize, imageSize);
    imageView0.SetParentOrigin( ParentOrigin::CENTER );
    imageView0.SetY( -imageSize*0.5f );
    stage.Add(imageView0);
    Toolkit::ImageView imageView1 = CreateImageView( redGreen0 );
    imageView1.SetParentOrigin( ParentOrigin::CENTER );
    imageView1.SetSize(imageSize, imageSize);
    imageView0.Add(imageView1);

    Toolkit::ImageView imageView2 = CreateImageView( IMAGE_PATH );
    imageView2.SetSize(imageSize, imageSize);
    imageView2.SetParentOrigin( ParentOrigin::CENTER );
    imageView2.SetY( imageSize*0.5f );
    stage.Add(imageView2);
    Toolkit::ImageView imageView3 = CreateImageView( redGreen1);
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

  std::string CreateTexture( const Vector4& color1, const Vector4& color2 )
  {
    const auto width = 2u;
    const auto height = 1u;
    auto size = width * height * 4;
    auto pixelBuffer = new unsigned char[size];
    pixelBuffer[0] = 0xFF * color1.x;
    pixelBuffer[1] = 0xFF * color1.y;
    pixelBuffer[2] = 0xFF * color1.z;
    pixelBuffer[3] = 0xFF * color1.w;
    pixelBuffer[4] = 0xFF * color2.x;
    pixelBuffer[5] = 0xFF * color2.y;
    pixelBuffer[6] = 0xFF * color2.z;
    pixelBuffer[7] = 0xFF * color2.w;

    auto pixelData = PixelData::New(pixelBuffer, size, width, height, Pixel::RGBA8888, PixelData::ReleaseFunction::DELETE_ARRAY);
    auto texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );
    texture.Upload(pixelData);

    return Toolkit::TextureManager::AddTexture(texture);
  }

  template<typename TextT>
  Toolkit::ImageView CreateImageView(TextT&& filename)
  {
    auto imageView = Toolkit::ImageView::New();

    Property::Map propertyMap;
    propertyMap.Insert(Toolkit::ImageVisual::Property::URL, std::forward<TextT>(filename));
    propertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
    propertyMap.Insert(Toolkit::DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::FILL);
    imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, propertyMap );

    return imageView;
  }

private:
  Application&  mApplication;
  unsigned int mIndex;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  ImageViewAlphaBlendApp test( application );
  application.MainLoop();
  return 0;
}
