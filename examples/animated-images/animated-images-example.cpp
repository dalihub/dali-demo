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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>

#include "shared/utility.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * const PLAY_ICON( DEMO_IMAGE_DIR "icon-play.png" );
const char * const PLAY_ICON_SELECTED( DEMO_IMAGE_DIR "icon-play-selected.png" );

const char* const STATIC_GIF_DOG( DEMO_IMAGE_DIR "dog-static.gif" );
const char* const ANIMATE_GIF_DOG( DEMO_IMAGE_DIR "dog-anim.gif" );

const char* const STATIC_GIF_LOGO( DEMO_IMAGE_DIR "dali-logo-static.gif" );
const char* const ANIMATE_GIF_LOGO( DEMO_IMAGE_DIR "dali-logo-anim.gif" );

const Vector4 DIM_COLOR( 0.85f, 0.85f, 0.85f, 0.85f );
}

/* This example shows how to display a GIF image.
 * First a static GIF image is loaded and then when the user presses on the "Play" icon,
 * the static image is replaced by an animated one
 */

class AnimatedImageController : public ConnectionTracker
{
public:

  AnimatedImageController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &AnimatedImageController::Create );
  }

  ~AnimatedImageController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );
    // Tie-in input event handlers:
    stage.KeyEventSignal().Connect( this, &AnimatedImageController::OnKeyEvent );

    mActorDog = CreateGifViewWithOverlayButton( STATIC_GIF_DOG );
    mActorDog.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mActorDog.SetY( -100.f );
    stage.Add( mActorDog );

    mActorLogo = CreateGifViewWithOverlayButton( STATIC_GIF_LOGO );
    mActorLogo.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mActorLogo.SetY( 100.f );
    stage.Add( mActorLogo );
  }

  /**
   * Create the gif image view with an overlay play button.
   */
  Toolkit::ImageView CreateGifViewWithOverlayButton( const std::string& gifUrl  )
  {
    Toolkit::ImageView imageView = Toolkit::ImageView::New( gifUrl );
    imageView.SetParentOrigin( ParentOrigin::CENTER );

    // Create a push button, and add it as child of the image view
    Toolkit::PushButton animateButton = Toolkit::PushButton::New();
    animateButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, PLAY_ICON );
    animateButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, PLAY_ICON_SELECTED );
    animateButton.SetParentOrigin( ParentOrigin::CENTER );
    animateButton.SetAnchorPoint( AnchorPoint::CENTER );
    animateButton.ClickedSignal().Connect( this, &AnimatedImageController::OnPlayButtonClicked );
    imageView.Add( animateButton );

    // Apply dim color on the gif view and the play button
    imageView.SetColor( DIM_COLOR );

    return imageView;
  }

  bool OnPlayButtonClicked( Toolkit::Button button )
  {
    Stage stage = Stage::GetCurrent();

    // With play button clicked, the static gif is replaced with animated gif.
    if( button.GetParent() ==  mActorDog )
    {
      // remove the static gif view, the play button is also removed as its child.
      stage.Remove( mActorDog );

      mActorDog = Toolkit::ImageView::New( ANIMATE_GIF_DOG );
      mActorDog.SetParentOrigin( ParentOrigin::CENTER );
      mActorDog.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
      mActorDog.SetY( -100.f );
      stage.Add( mActorDog );
    }
    else // button.GetParent() ==  mActorLogo
    {
      // remove the static gif view, the play button is also removed as its child.
      stage.Remove( mActorLogo );

      mActorLogo = Toolkit::ImageView::New( ANIMATE_GIF_LOGO );
      mActorLogo.SetParentOrigin( ParentOrigin::CENTER );
      mActorLogo.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      mActorLogo.SetY( 100.f );
      stage.Add( mActorLogo );
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
  Toolkit::ImageView mActorDog;
  Toolkit::ImageView mActorLogo;
};

// Entry point for Linux & Tizen applications
//
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  AnimatedImageController test( application );

  application.MainLoop();

  return 0;
}
