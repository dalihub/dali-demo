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

const char* const ANIMATE_PIXEL_AREA( "Animate PixelArea" );
const char* const ANIMATE_PIXEL_AREA_AND_SCALE( "Animate PixelArea & Scale" );

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

    mActorDog = CreateGifViewWithOverlayPlayButton( STATIC_GIF_DOG );
    mActorDog.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mActorDog.SetY( -100.f );
    stage.Add( mActorDog );

    mActorLogo = CreateGifViewWithOverlayPlayButton( STATIC_GIF_LOGO );
    mActorLogo.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mActorLogo.SetY( 100.f );
    stage.Add( mActorLogo );

    mTapDetector = TapGestureDetector::New();
    mTapDetector.DetectedSignal().Connect( this, &AnimatedImageController::OnTap );
  }

  /**
   * Create the gif image view with an overlay play button.
   */
  Toolkit::ImageView CreateGifViewWithOverlayPlayButton( const std::string& gifUrl  )
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

  Toolkit::ImageView CreateGifViewWithAnimatePixelAreaButton( const std::string& gifUrl, WrapMode::Type wrapModeU, WrapMode::Type wrapModeV, const std::string& buttonLabel )
  {
    Toolkit::ImageView imageView = Toolkit::ImageView::New();
    imageView.SetProperty( Toolkit::ImageView::Property::IMAGE,
                           Property::Map().Add( Toolkit::ImageVisual::Property::URL, gifUrl )
                                          .Add( Toolkit::ImageVisual::Property::WRAP_MODE_U, wrapModeU )
                                          .Add( Toolkit::ImageVisual::Property::WRAP_MODE_V, wrapModeV ));
    imageView.SetParentOrigin( ParentOrigin::CENTER );

    // Create a push button, and add it as child of the image view
    Toolkit::PushButton animateButton = Toolkit::PushButton::New();
    animateButton.SetProperty( Toolkit::Button::Property::LABEL, buttonLabel );
    animateButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    animateButton.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    animateButton.SetY( 20.f );

    animateButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    animateButton.SetProperty( Actor::Property::INHERIT_SCALE, false );
    imageView.Add( animateButton );

    mTapDetector.Attach( animateButton );
    mTapDetector.Attach( imageView );

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

      mActorDog = CreateGifViewWithAnimatePixelAreaButton( ANIMATE_GIF_DOG, WrapMode::REPEAT, WrapMode::DEFAULT, ANIMATE_PIXEL_AREA_AND_SCALE );
      mActorDog.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
      mActorDog.SetY( -100.f );
      stage.Add( mActorDog );
    }
    else // button.GetParent() ==  mActorLogo
    {
      // remove the static gif view, the play button is also removed as its child.
      stage.Remove( mActorLogo );

      mActorLogo = CreateGifViewWithAnimatePixelAreaButton( ANIMATE_GIF_LOGO, WrapMode::DEFAULT, WrapMode::MIRRORED_REPEAT, ANIMATE_PIXEL_AREA );
      mActorLogo.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      mActorLogo.SetY( 100.f );
      stage.Add( mActorLogo );
    }
    return true;
  }

  void OnTap(Dali::Actor actor, const Dali::TapGesture& tap)
  {
    if( actor.GetParent() ==  mActorDog ) // "Animate Pixel Area" button is clicked
    {
      Animation animation = Animation::New( 3.f );
      animation.AnimateTo( Property( mActorDog, ImageView::Property::PIXEL_AREA ), Vector4( -1.0, 0.0, 3.f, 1.f ), AlphaFunction::SIN );
      animation.AnimateTo( Property( mActorDog, Actor::Property::SCALE_X ), 3.f, AlphaFunction::SIN );
      animation.Play();
    }
    else if( actor.GetParent() ==  mActorLogo ) // "Animate Pixel Area" button is clicked
    {
      Animation animation = Animation::New( 3.f );
      animation.AnimateTo( Property( mActorLogo, ImageView::Property::PIXEL_AREA ), Vector4( 0.0, 1.0, 1.f, 1.f ), AlphaFunction::SIN );
      animation.Play();
    }
    else if( actor == mActorDog ) // stop the animated gif, switch to static view
    {
      Stage stage = Stage::GetCurrent();
      stage.Remove( mActorDog );

      mActorDog = CreateGifViewWithOverlayPlayButton( STATIC_GIF_DOG );
      mActorDog.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
      mActorDog.SetY( -100.f );
      stage.Add( mActorDog );
    }
    else if( actor == mActorLogo ) // stop the animated gif, switch to static view
    {
      Stage stage = Stage::GetCurrent();
      stage.Remove( mActorLogo );

      mActorLogo = CreateGifViewWithOverlayPlayButton( STATIC_GIF_LOGO );
      mActorLogo.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      mActorLogo.SetY( 100.f );
      stage.Add( mActorLogo );
    }
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
  TapGestureDetector mTapDetector;
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
