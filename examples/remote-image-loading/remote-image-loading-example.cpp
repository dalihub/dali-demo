/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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


#include <shared/utility.h>
#include <stdlib.h>
#include <iostream>
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/actors/actor-devel.h>


using namespace Dali;
using namespace Dali::Toolkit;

// This example shows the load-time image scaling and filtering features.
//
class MyTester : public ConnectionTracker
{
public:
  MyTester(Application &application) : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &MyTester::Create);
  }

  ~MyTester()
  {
    // Nothing to do here;
  }

  void ConnectEventSignal(Control control)
  {
    control.TouchSignal().Connect(this, &MyTester::OnControlTouch);

    control.SetProperty( Actor::Property::KEYBOARD_FOCUSABLE,true);
    control.KeyEventSignal().Connect(this, &MyTester::OnControlKeyEvent);
    control.KeyInputFocusGainedSignal().Connect(this, &MyTester::OnFocusSet);
    control.KeyInputFocusLostSignal().Connect(this, &MyTester::OnFocusUnSet);
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application &application)
  {
    stage = Stage::GetCurrent();
    stage.SetBackgroundColor(Color::BLACK);
    stage.KeyEventSignal().Connect(this, &MyTester::OnKey);
    stage.TouchSignal().Connect(this, &MyTester::OnTouch);

    TextLabel rubric = TextLabel::New( "You will need a working internet connection to see the images below");
    rubric.SetProperty( TextLabel::Property::MULTI_LINE, true );
    rubric.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE );
    rubric.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    rubric.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
    rubric.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER );
    rubric.SetProperty( Actor::Property::ANCHOR_POINT, ParentOrigin::TOP_CENTER );
    stage.Add( rubric );

    mImageView1 = Toolkit::ImageView::New("http://static.midomi.com/s/s/images/000/000/000/000/293/259/19/520_000000000000293259191500x1500_72dpi_RGB_q70.jpg");

    mImageView1.SetProperty( Dali::Actor::Property::NAME,"mImageView1");
    mImageView1.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
    mImageView1.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_CENTER);
    mImageView1.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
    mImageView1.SetProperty( Actor::Property::POSITION, Vector2(0, 100));
    mImageView1.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    stage.Add(mImageView1);

    mImageView2 = Toolkit::ImageView::New("http://static.midomi.com/s/s/images/000/000/000/000/212/651/88/520_000000000000212651881500x1500_72dpi_RGB_q70.jpg");
    mImageView2.SetProperty( Dali::Actor::Property::NAME,"mImageView2");
    mImageView2.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
    mImageView2.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_CENTER);
    mImageView2.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
    mImageView2.SetProperty( Actor::Property::POSITION, Vector2(400, 100));
    mImageView2.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    stage.Add(mImageView2);

    mImageView3 = Toolkit::ImageView::New("http://static.midomi.com/s/s/images/000/000/000/000/212/353/21/520_000000000000212353211500x1500_72dpi_RGB_q70.jpg");
    mImageView3.SetProperty( Dali::Actor::Property::NAME,"mImageView3");
    mImageView3.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
    mImageView3.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_CENTER);
    mImageView3.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
    mImageView3.SetProperty( Actor::Property::POSITION, Vector2(0, 400));
    mImageView3.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    stage.Add(mImageView3);

    mImageView4 = Toolkit::ImageView::New("http://d2k43l0oslhof9.cloudfront.net/platform/image/contents/vc/20/01/58/20170629100630071189_0bf6b911-a847-cba4-e518-be40fe2f579420170629192203240.jpg");
    mImageView4.SetProperty( Dali::Actor::Property::NAME,"mImageView4");
    mImageView4.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
    mImageView4.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_CENTER);
    mImageView4.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
    mImageView4.SetProperty( Actor::Property::POSITION, Vector2(400, 400));
    mImageView4.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    stage.Add(mImageView4);

    mImageView5 = Toolkit::ImageView::New("http://static.midomi.com/h/images/w/weather_sunny.png");
    mImageView5.SetProperty( Dali::Actor::Property::NAME,"mImageView5");
    mImageView4.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::TOP_LEFT);
    mImageView5.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_CENTER);
    mImageView5.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
    mImageView5.SetProperty( Actor::Property::POSITION, Vector2(800, 100));
    mImageView5.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    stage.Add(mImageView5);

    // Tie-in input event handlers:
    stage.KeyEventSignal().Connect( this, &MyTester::OnKeyEvent );
  }

  void OnAnimationEnd(Animation &source)
  {
    std::cout<<"OnAnimationEnd" << std::endl;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

  void OnKey(const KeyEvent &event)
  {
    if (event.state == KeyEvent::Down)
    {
      std::cout<<"Stage key : " << event.keyCode << std::endl;
    }
  }

  void OnTouch(const TouchData &touch)
  {
    if (touch.GetState(0) == PointState::DOWN)
    {
      std::cout<<"Stage touch" << std::endl;
    }
  }

  bool OnControlKeyEvent(Toolkit::Control control, const KeyEvent &event)
  {
    if (event.state == KeyEvent::Down)
    {
      std::cout<<"Control down key : " << control.GetProperty< std::string >( Dali::Actor::Property::NAME ) << ", keyCode : " << event.keyCode << std::endl;
    }
    else
    {
      std::cout<<"Control up key : " << control.GetProperty< std::string >( Dali::Actor::Property::NAME ) << ", keyCode : " << event.keyCode << std::endl;
    }
    return false;
  }

  bool OnControlTouch(Actor actor, const TouchData &touch)
  {
    if (touch.GetState(0) == PointState::DOWN)
    {

      std::cout<<"Control touch " << actor.GetProperty< std::string >( Dali::Actor::Property::NAME ) << ", parent " << actor.GetParent().GetProperty< std::string >( Dali::Actor::Property::NAME ) << std::endl;
    }

    return false;
  }

  void OnFocusSet(Control control)
  {
    std::cout<<"OnFocusSet " << control.GetProperty< std::string >( Dali::Actor::Property::NAME ) << std::endl;
  }

  void OnFocusUnSet(Control control)
  {
    std::cout<<"OnFocusUnSet " << control.GetProperty< std::string >( Dali::Actor::Property::NAME ) << std::endl;
  }

private:
  Stage stage;
  Application &mApplication;

  Control mControl1;
  Control mControl2;
  ImageView mImageView1;
  ImageView mImageView2;
  ImageView mImageView3;
  ImageView mImageView4;
  ImageView mImageView5;

  TextLabel mTextLabel1;
  TextLabel mTextLabel2;
  TextField mTextField;
  TextEditor mTextEditor;

  Animation mAnimation;
  Timer     mTimer;

};

int DALI_EXPORT_API main(int argc, char **argv)
{
  Application application = Application::New(&argc, &argv, "");
  MyTester test(application);
  application.MainLoop();
  return 0;
}
