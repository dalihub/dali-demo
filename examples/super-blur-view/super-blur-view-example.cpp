/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/super-blur-view/super-blur-view.h>

using namespace Dali;
using Dali::Toolkit::Button;
using Dali::Toolkit::PushButton;
using Dali::Toolkit::SuperBlurView;

namespace
{

const char* const BACKGROUND_IMAGE( DEMO_IMAGE_DIR "background-4.jpg" );

const unsigned int DEFAULT_BLUR_LEVEL(5u); ///< The default blur level when creating SuperBlurView from the type registry

}

/**
 * This example shows a background image which is "super blurred" while the push-button control is pressed.
 */
class SuperBlurViewExample : public ConnectionTracker
{
public:

  SuperBlurViewExample( Application& application )
  : mApplication( application )
  {
    mApplication.InitSignal().Connect( this, &SuperBlurViewExample::Create );
  }

  ~SuperBlurViewExample() = default;

private:

  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect( this, &SuperBlurViewExample::OnKeyEvent );

    mSuperBlurView = SuperBlurView::New( DEFAULT_BLUR_LEVEL );
    mSuperBlurView.SetParentOrigin( ParentOrigin::CENTER );
    mSuperBlurView.SetAnchorPoint( AnchorPoint::CENTER );
    mSuperBlurView.SetSize( 800, 1280 );
    mSuperBlurView.SetProperty( SuperBlurView::Property::IMAGE_URL, BACKGROUND_IMAGE );
    stage.Add( mSuperBlurView );

    mBlurAnimation = Animation::New(1.0f);
    mBlurAnimation.AnimateTo( Property(mSuperBlurView, mSuperBlurView.GetBlurStrengthPropertyIndex()), 1.0f );

    mClearAnimation = Animation::New(1.0f);
    mClearAnimation.AnimateTo( Property(mSuperBlurView, mSuperBlurView.GetBlurStrengthPropertyIndex()), 0.0f );

    mPushButton = PushButton::New();
    mPushButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mPushButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mPushButton.SetProperty( Button::Property::LABEL, "Blur" );
    mPushButton.PressedSignal().Connect( this, &SuperBlurViewExample::OnButtonPressed );
    mPushButton.ReleasedSignal().Connect( this, &SuperBlurViewExample::OnButtonReleased );
    stage.Add( mPushButton );
  }

  bool OnButtonPressed( Button button )
  {
    mBlurAnimation.Play();
    return true;
  }

  bool OnButtonReleased( Button button )
  {
    mClearAnimation.Play();
    return true;
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if ( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  Application&  mApplication;

  SuperBlurView mSuperBlurView;

  PushButton mPushButton;

  Animation mBlurAnimation;
  Animation mClearAnimation;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  SuperBlurViewExample test( application );

  application.MainLoop();

  return 0;
}
