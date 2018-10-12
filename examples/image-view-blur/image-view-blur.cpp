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
#include <dali-toolkit/devel-api/controls/gaussian-blur-view/gaussian-blur-view.h>

using namespace Dali;
using namespace Dali::Toolkit;

class ImageViewBlurExample : public ConnectionTracker
{
public:
  ImageViewBlurExample( Application& app )
  : mApp( app )
  {
    app.InitSignal().Connect(this, &ImageViewBlurExample::Create);
  }

  void Create(Application& app)
  {
    auto stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &ImageViewBlurExample::OnKeyEvent);

    CreateBlurView1();
  }

  void CreateBlurView1()
  {
    auto stage = Stage::GetCurrent();
    auto size = stage.GetSize();
    auto image1 = ImageView::New( DEMO_IMAGE_DIR "background-1.jpg" );
    image1.SetSize( size * 0.5f );

    blurView1 = GaussianBlurView::New();
    blurView1.SetSize( size * 0.5f );
    blurView1.SetParentOrigin( ParentOrigin::CENTER );//  Vector3(0.0f, -0.25f, 0.0f) );
    blurView1.SetAnchorPoint( AnchorPoint::CENTER );
    blurView1.Add( image1 );
    //blurView1.SetBackgroundColor( Color::RED );

    image1.SetParentOrigin( ParentOrigin::CENTER );
    image1.SetAnchorPoint( AnchorPoint::CENTER );
    blurView1.Activate();

    blurView1.SetProperty(blurView1.GetBlurStrengthPropertyIndex(), 1.0f);
    auto blurAnim = Animation::New(0.5f);
    blurAnim.AnimateTo( Property( blurView1, blurView1.GetBlurStrengthPropertyIndex() ), 1.0f);
    blurAnim.FinishedSignal().Connect( this, &ImageViewBlurExample::OnBlurAnimFinished );
    blurAnim.Play();
    stage.Add(blurView1);
  }

  void OnBlurAnimFinished(Animation& anim)
  {
    blurView1.Deactivate();
  }

  void OnBlurFinished(GaussianBlurView source)
  {
    printf("Blur finished\n");
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApp.Quit();
      }
    }
  }

private:
  Application& mApp;
  GaussianBlurView blurView1;
  GaussianBlurView blurView2;
};

int main( int argc, char** argv )
{
  Application app = Application::New( &argc, &argv );
  ImageViewBlurExample exampleApp( app );
  app.MainLoop();
  return 0;
}
