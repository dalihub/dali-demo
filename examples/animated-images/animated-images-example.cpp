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

const char* const STATIC_IMAGE_ARRAY_DOG( DEMO_IMAGE_DIR "dog-anim-001.png" );
const char* const ANIMATE_IMAGE_ARRAY_DOG( DEMO_IMAGE_DIR "dog-anim-%03d.png" );

const char* const STATIC_IMAGE_ARRAY_LOGO( DEMO_IMAGE_DIR "dali-logo-anim-001.png" );
const char* const ANIMATE_IMAGE_ARRAY_LOGO( DEMO_IMAGE_DIR "dali-logo-anim-%03d.png" );


const Vector4 DIM_COLOR( 0.85f, 0.85f, 0.85f, 0.85f );
}

/* This example shows how to display a GIF image.
 * First a static GIF image is loaded and then when the user presses on the "Play" icon,
 * the static image is replaced by an animated one
 */

class AnimatedImageController : public ConnectionTracker
{
public:
  enum ImageType
  {
    GIF,
    IMAGE_ARRAY
  };
  enum StateType
  {
    STATIC,
    ANIMATED
  };

  AnimatedImageController( Application& application )
  : mApplication( application ),
    mImageType(GIF)
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

    CreateStaticImageView( 0 );
    CreateStaticImageView( 1 );

    mGifButton = Toolkit::RadioButton::New("Gif");
    mGifButton.SetProperty( Button::Property::SELECTED, true );
    mArrayButton = Toolkit::RadioButton::New("Array");
    mGifButton.ClickedSignal().Connect( this, &AnimatedImageController::OnTypeButtonClicked );
    mArrayButton.ClickedSignal().Connect( this, &AnimatedImageController::OnTypeButtonClicked );

    Toolkit::TableView radioButtonLayout = Toolkit::TableView::New(1, 2);
    radioButtonLayout.SetName("RadioButtonsLayout");
    radioButtonLayout.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );
    radioButtonLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    radioButtonLayout.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    radioButtonLayout.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    radioButtonLayout.SetFitHeight(0);
    radioButtonLayout.AddChild( mGifButton, TableView::CellPosition(0,0) );
    radioButtonLayout.AddChild( mArrayButton, TableView::CellPosition(0,1) );
    radioButtonLayout.SetCellAlignment( TableView::CellPosition( 0, 0 ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );
    radioButtonLayout.SetCellAlignment( TableView::CellPosition( 0, 1 ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );
    radioButtonLayout.SetY( -10.0f );

    stage.Add( radioButtonLayout );

    mTapDetector = TapGestureDetector::New();
    mTapDetector.DetectedSignal().Connect( this, &AnimatedImageController::OnTap );
  }

  void CreateStaticImageView( int index )
  {
    Actor& actor = (index==0) ? mActorDog : mActorLogo;

    Stage stage = Stage::GetCurrent();
    if( actor )
    {
      stage.Remove( actor );
    }

    Property::Value viewSetup = SetupViewProperties( mImageType, STATIC, index, false );
    actor = CreateImageViewWithPlayButton( viewSetup );
    SetLayout(actor, index);
    stage.Add( actor );
  }


  void CreateAnimImageView( int index )
  {
    Actor& actor = (index==0) ? mActorDog : mActorLogo;

    Stage stage = Stage::GetCurrent();
    if( actor )
    {
      stage.Remove( actor );
    }

    const char* label = (index==0) ? ANIMATE_PIXEL_AREA_AND_SCALE : ANIMATE_PIXEL_AREA;

    Property::Value viewSetup = SetupViewProperties( mImageType, ANIMATED, index, true );
    actor = CreateImageViewWithAnimatePixelAreaButton( viewSetup, label);
    SetLayout(actor, index);

    stage.Add( actor );
  }

  void SetLayout( Actor actor, int index )
  {
    if( index == 0 )
    {
      actor.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
      actor.SetY( -80.f );
    }
    else
    {
      actor.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      actor.SetY( 80.f );
    }
  }

  /**
   * Create the gif image view with an overlay play button.
   */
  Toolkit::ImageView CreateImageViewWithPlayButton( Property::Value& viewSetup )
  {
    Toolkit::ImageView imageView = Toolkit::ImageView::New();
    imageView.SetProperty( ImageView::Property::IMAGE, viewSetup );
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

  Toolkit::ImageView CreateImageViewWithAnimatePixelAreaButton( Property::Value& viewSetup, const std::string& buttonLabel )
  {
    Toolkit::ImageView imageView = Toolkit::ImageView::New();
    imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, viewSetup );
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
      CreateAnimImageView( 0 );
    }
    else // button.GetParent() ==  mActorLogo
    {
      // remove the static gif view, the play button is also removed as its child.
      CreateAnimImageView( 1 );
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
      CreateStaticImageView( 0 );
    }
    else if( actor == mActorLogo ) // stop the animated gif, switch to static view
    {
      CreateStaticImageView( 1 );
    }
  }

  bool OnTypeButtonClicked( Toolkit::Button button )
  {
    if( button == mGifButton )
    {
      mImageType = GIF;
    }
    else
    {
      mImageType = IMAGE_ARRAY;
    }
    Stage stage = Stage::GetCurrent();
    CreateStaticImageView( 0 );
    CreateStaticImageView( 1 );
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

  Property::Value SetupViewProperties( ImageType type, StateType state, int index, bool wrap )
  {
    Property::Map map;

    AddUrl( map, type, state, index );
    AddWrap( map, wrap && state != 0, index );
    AddCache( map, type, index );
    return Property::Value(map);
  }

  void AddUrl( Property::Map& map, ImageType type, StateType state, int index )
  {
    const char* urls[2][2] =
      { { STATIC_GIF_DOG, STATIC_GIF_LOGO },
        { ANIMATE_GIF_DOG, ANIMATE_GIF_LOGO }
      };
    const char* urlFormats[2][2] =
      { { STATIC_IMAGE_ARRAY_DOG, STATIC_IMAGE_ARRAY_LOGO } ,
        { ANIMATE_IMAGE_ARRAY_DOG, ANIMATE_IMAGE_ARRAY_LOGO } };

    int numFrames[2] = { 8, 15 };

    if( type == GIF )
    {
      map.Add( Toolkit::ImageVisual::Property::URL, Property::Value( urls[state][index] ) );
    }
    else
    {
      if( state == STATIC )
      {
        Property::Array frameUrls;
        frameUrls.Add(Property::Value( urlFormats[0][index] ));
        map.Add( Toolkit::ImageVisual::Property::URL, frameUrls );
      }
      else
      {
        Property::Array frameUrls;
        for( int i=1; i<= numFrames[index]; ++i )
        {
          char* buffer;
          int len = asprintf( &buffer, urlFormats[1][index], i);
          if( len > 0 )
          {
            std::string frameUrl(buffer);
            free(buffer);
            frameUrls.Add( Property::Value( frameUrl ) );
          }
        }
        map.Add( Toolkit::ImageVisual::Property::URL, Property::Value( frameUrls ) );
      }
    }
  }

  void AddWrap( Property::Map& map, bool wrap, int index )
  {
    WrapMode::Type wrapModes[2][2] = {
      { WrapMode::REPEAT, WrapMode::DEFAULT  },
      { WrapMode::DEFAULT, WrapMode::MIRRORED_REPEAT } };

    if( wrap )
    {
      map
        .Add( Toolkit::ImageVisual::Property::WRAP_MODE_U, wrapModes[index][0] )
        .Add( Toolkit::ImageVisual::Property::WRAP_MODE_V, wrapModes[index][1] );
    }
    else
    {
      map
        .Add( Toolkit::ImageVisual::Property::WRAP_MODE_U, WrapMode::DEFAULT )
        .Add( Toolkit::ImageVisual::Property::WRAP_MODE_V, WrapMode::DEFAULT );
    }
  }

  void AddCache( Property::Map& map, ImageType type, int index )
  {
    if( type == IMAGE_ARRAY )
    {
      map
        .Add( Toolkit::ImageVisual::Property::BATCH_SIZE, 4 )
        .Add( Toolkit::ImageVisual::Property::CACHE_SIZE, 10 )
        .Add( Toolkit::ImageVisual::Property::FRAME_DELAY, 150 );
    }
  }

private:
  Application&  mApplication;
  Toolkit::ImageView mActorDog;
  Toolkit::ImageView mActorLogo;
  Toolkit::RadioButton mGifButton;
  Toolkit::RadioButton mArrayButton;
  TapGestureDetector mTapDetector;
  ImageType mImageType;
};

//
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  AnimatedImageController test( application );

  application.MainLoop();

  return 0;
}
