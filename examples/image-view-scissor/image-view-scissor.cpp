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
 */


#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

class ImageViewScissor : public ConnectionTracker
{
public:
  ImageViewScissor( Application& application )
  : mApplication( application )
  {
    mApplication.InitSignal().Connect( this, &ImageViewScissor::Create );
    mApplication.TerminateSignal().Connect( this, &ImageViewScissor::Terminate );
  }

private:
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &ImageViewScissor::OnKeyEvent);
    stage.SetBackgroundColor( Color::WHITE );
    Vector2 stageSize = stage.GetSize();

    // Top half of screen - layer clipping is on in center
    auto topLayer = Layer::New();
    topLayer.SetName( "Top layer");
    topLayer.SetSize( stageSize.width, stageSize.height * 0.5f );
    topLayer.SetParentOrigin( ParentOrigin::TOP_LEFT );
    topLayer.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    stage.Add( topLayer );

    auto topImage = ImageView::New( DEMO_IMAGE_DIR "gallery-large-7.jpg" );
    topImage.SetName( "top clipped image" );
    topImage.SetSize( stageSize.width, stageSize.height * 0.5f );
    topImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    topLayer.Add( topImage );
    topLayer.SetClipping(true);
    topLayer.SetClippingBox( stageSize.width * 0.25f, stageSize.height * 0.25f, stageSize.width * 0.25f, stageSize.height  * 0.25f);

    // Clip using Actor based Scissor clipping
    Control bottomBg = Control::New();
    bottomBg.SetName( "BottomBG" );
    bottomBg.SetSize( stageSize.width*0.5f, stageSize.height * 0.25f );
    bottomBg.SetPosition( stageSize.width*0.25f, stageSize.height * -0.125f);
    bottomBg.SetBackgroundColor( Color::BLUE );
    bottomBg.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    bottomBg.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    stage.Add(bottomBg);

    auto bottomImage = ImageView::New( DEMO_IMAGE_DIR "gallery-large-7.jpg" );
    bottomImage.SetName( "bottom clipped image" );
    bottomImage.SetSize( stageSize.width, stageSize.height * 0.5f );
    bottomImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    bottomImage.SetPosition( -stageSize.width*0.25f, stageSize.height * -0.125f );
    bottomBg.Add(bottomImage);
    bottomBg.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX );

    auto overlay = Layer::New();
    overlay.SetSize( stageSize.width, stageSize.height );
    overlay.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    stage.Add( overlay );

    auto label1 = TextLabel::New( "Layer Clipping" );
    auto label2 = TextLabel::New( "Actor Clipping" );
    Property::Map outline;
    outline.Add("color", Color::YELLOW).Add("width", 1);
    label1.SetProperty(TextLabel::Property::OUTLINE, outline);
    label2.SetProperty(TextLabel::Property::OUTLINE, outline);
    label1.SetParentOrigin( ParentOrigin::TOP_CENTER );
    label1.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    label2.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    label2.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );

    overlay.Add(label1);
    overlay.Add(label2);
  }

  void Terminate( Application& application )
  {
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
  Application& mApplication;
};

int main( int argc, char** argv )
{
  Application application = Application::New(&argc, &argv);
  ImageViewScissor test( application );
  application.MainLoop();
  return 0;
}
