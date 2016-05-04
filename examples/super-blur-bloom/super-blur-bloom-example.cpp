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
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/super-blur-view/super-blur-view.h>
#include <dali-toolkit/devel-api/controls/bloom-view/bloom-view.h>
#include "shared/view.h"

using namespace Dali;

namespace
{
const char * const TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char * const TITLE_SUPER_BLUR( "Super Blur" );
const char * const TITLE_BLOOM( "Bloom" );
const char * const CHANGE_BACKGROUND_ICON( DEMO_IMAGE_DIR "icon-change.png" );
const char * const CHANGE_BACKGROUND_ICON_SELECTED( DEMO_IMAGE_DIR "icon-change-selected.png" );
const char * const CHANGE_BLUR_ICON( DEMO_IMAGE_DIR "icon-replace.png" );
const char * const CHANGE_BLUR_ICON_SELECTED( DEMO_IMAGE_DIR "icon-replace-selected.png" );

const char* BACKGROUND_IMAGES[]=
{
  DEMO_IMAGE_DIR "background-1.jpg",
  DEMO_IMAGE_DIR "background-2.jpg",
  DEMO_IMAGE_DIR "background-3.jpg",
  DEMO_IMAGE_DIR "background-4.jpg",
  DEMO_IMAGE_DIR "background-5.jpg",
  DEMO_IMAGE_DIR "background-magnifier.jpg",
};
const unsigned int NUM_BACKGROUND_IMAGES( sizeof( BACKGROUND_IMAGES ) / sizeof( BACKGROUND_IMAGES[0] ) );
}

/**
 * @brief Load an image, scaled-down to no more than the stage dimensions.
 *
 * Uses image scaling mode FittingMode::SCALE_TO_FILL to resize the image at
 * load time to cover the entire stage with pixels with no borders,
 * and filter mode BOX_THEN_LINEAR to sample the image with
 * maximum quality.
 */
ResourceImage LoadStageFillingImage( const char * const imagePath )
{
  Size stageSize = Stage::GetCurrent().GetSize();
  return ResourceImage::New( imagePath, Dali::ImageDimensions( stageSize.x, stageSize.y ), Dali::FittingMode::SCALE_TO_FILL, Dali::SamplingMode::BOX_THEN_LINEAR );
}

class BlurExample : public ConnectionTracker
{
public:
  BlurExample(Application &app)
  : mApp(app),
    mImageIndex( 0 ),
    mIsBlurring( false )
  {
    // Connect to the Application's Init signal
    app.InitSignal().Connect(this, &BlurExample::Create);
  }

  ~BlurExample()
  {
  }
private:
  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& app)
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    stage.KeyEventSignal().Connect(this, &BlurExample::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Layer content = DemoHelper::CreateView( app,
                                            mBackground,
                                            mToolBar,
                                            "",
                                            TOOLBAR_IMAGE,
                                            "" );

    // Add a button to change background. (right of toolbar)
    Toolkit::PushButton changeBackgroundButton = Toolkit::PushButton::New();
    changeBackgroundButton.SetUnselectedImage( CHANGE_BACKGROUND_ICON );
    changeBackgroundButton.SetSelectedImage( CHANGE_BACKGROUND_ICON_SELECTED );
    changeBackgroundButton.ClickedSignal().Connect( this, &BlurExample::OnChangeBackgroundIconClicked );
    mToolBar.AddControl( changeBackgroundButton,
        DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage,
        Toolkit::Alignment::HorizontalRight,
        DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    // Add a button to change the blur view. (left of toolbar)
    Toolkit::PushButton changeBlurButton = Toolkit::PushButton::New();
    changeBlurButton.SetUnselectedImage( CHANGE_BLUR_ICON );
    changeBlurButton.SetSelectedImage( CHANGE_BLUR_ICON_SELECTED );
    changeBlurButton.ClickedSignal().Connect( this, &BlurExample::OnChangeBlurIconClicked );
    mToolBar.AddControl( changeBlurButton,
        DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage,
        Toolkit::Alignment::HorizontalLeft,
        DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    mSuperBlurView = Toolkit::SuperBlurView::New( 5 );
    mSuperBlurView.SetSize( stageSize );
    mSuperBlurView.SetParentOrigin( ParentOrigin::CENTER );
    mSuperBlurView.SetAnchorPoint( AnchorPoint::CENTER );
    mSuperBlurView.BlurFinishedSignal().Connect(this, &BlurExample::OnBlurFinished);
    mCurrentImage = LoadStageFillingImage( BACKGROUND_IMAGES[mImageIndex] );
    mSuperBlurView.SetImage( mCurrentImage );
    mBackground.Add( mSuperBlurView );
    mIsBlurring = true;
    SetTitle( TITLE_SUPER_BLUR );

    mBloomView = Toolkit::BloomView::New();
    mBloomView.SetParentOrigin(ParentOrigin::CENTER);
    mBloomView.SetSize(stageSize);
    mBloomActor = Toolkit::ImageView::New(mCurrentImage);
    mBloomActor.SetParentOrigin( ParentOrigin::CENTER );
    mBloomView.Add( mBloomActor );

    // Connect the callback to the touch signal on the background
    mSuperBlurView.TouchedSignal().Connect( this, &BlurExample::OnTouch );
    mBloomView.TouchedSignal().Connect( this, &BlurExample::OnTouch );
  }

  // Callback function of the touch signal on the background
  bool OnTouch(Dali::Actor actor, const Dali::TouchEvent& event)
  {
    const TouchPoint &point = event.GetPoint(0);
    switch(point.state)
    {
      case TouchPoint::Down:
      {
        if( mAnimation )
        {
          mAnimation.Clear();
        }

        mAnimation = Animation::New( 2.f );
        if( mSuperBlurView.OnStage() )
        {
          mAnimation.AnimateTo( Property( mSuperBlurView, mSuperBlurView.GetBlurStrengthPropertyIndex() ), 1.f );
        }
        else
        {
          mAnimation.AnimateTo( Property( mBloomView, mBloomView.GetBloomIntensityPropertyIndex() ), 3.f );
        }
        mAnimation.Play();
        break;
      }
      case TouchPoint::Up:
      case TouchPoint::Leave:
      case TouchPoint::Interrupted:
      {
        if( mAnimation )
        {
          mAnimation.Clear();
        }

        mAnimation = Animation::New( 2.f );
        if( mSuperBlurView.OnStage() )
        {
          mAnimation.AnimateTo( Property( mSuperBlurView, mSuperBlurView.GetBlurStrengthPropertyIndex() ), 0.f );
        }
        else
        {
          mAnimation.AnimateTo( Property( mBloomView, mBloomView.GetBloomIntensityPropertyIndex() ), 0.f );
        }
        mAnimation.Play();
        break;
      }
      case TouchPoint::Motion:
      case TouchPoint::Stationary:
      case TouchPoint::Last:
      default:
      {
        break;
      }
    }
    return true;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApp.Quit();
      }
    }
  }

  bool OnChangeBackgroundIconClicked( Toolkit::Button button )
  {
    if( mIsBlurring )
    {
      return true;
    }

    if( mAnimation )
    {
      mAnimation.Clear();
    }

    mImageIndex = (mImageIndex+1u)%NUM_BACKGROUND_IMAGES;
    mCurrentImage = LoadStageFillingImage( BACKGROUND_IMAGES[mImageIndex] );

    if( mSuperBlurView.OnStage() )
    {
      mIsBlurring = true;

      mSuperBlurView.SetBlurStrength( 0.f );
      mSuperBlurView.SetImage( mCurrentImage );
    }
    else
    {
      mBloomView.SetProperty( mBloomView.GetBloomIntensityPropertyIndex(), 0.f );
      mBloomActor.SetImage( mCurrentImage );
    }

    return true;
  }

  bool OnChangeBlurIconClicked( Toolkit::Button button )
  {
    if( mSuperBlurView.OnStage() )
    {
      SetTitle( TITLE_BLOOM );
      mBackground.Remove( mSuperBlurView );

      mBloomActor.SetImage( mCurrentImage );
      mBloomView.SetProperty( mBloomView.GetBloomIntensityPropertyIndex(), 0.f );
      mBackground.Add( mBloomView );
      mBloomView.Activate();

    }
    else
    {
      SetTitle( TITLE_SUPER_BLUR );
      mBackground.Remove( mBloomView );
      mBloomView.Deactivate();

      mBackground.Add( mSuperBlurView );
      mSuperBlurView.SetBlurStrength( 0.f );
      mSuperBlurView.SetImage( mCurrentImage );
      mIsBlurring = true;
    }

    return true;
  }

  void OnBlurFinished( Toolkit::SuperBlurView blurView )
  {
    mIsBlurring = false;
  }

  /**
   * Sets/Updates the title of the View
   * @param[in] title The new title for the view.
   */
  void SetTitle(const std::string& title)
  {
    if(!mTitleActor)
    {
      mTitleActor = DemoHelper::CreateToolBarLabel( title );
      // Add title to the tool bar.
      mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Toolkit::Alignment::HorizontalCenter );
    }

    mTitleActor.SetProperty( Toolkit::TextLabel::Property::TEXT, title );
  }

private:

  Application&               mApp;
  Toolkit::ToolBar           mToolBar;
  Toolkit::TextLabel         mTitleActor;             ///< The Toolbar's Title.
  Toolkit::Control           mBackground;
  Toolkit::SuperBlurView     mSuperBlurView;
  Toolkit::BloomView         mBloomView;
  Animation                  mAnimation;
  Toolkit::ImageView         mBloomActor;
  Image                      mCurrentImage;
  unsigned int               mImageIndex;
  bool                       mIsBlurring;
};

/*****************************************************************************/

static void
RunTest(Application& app)
{
  BlurExample theApp(app);
  app.MainLoop();
}

/*****************************************************************************/

int DALI_EXPORT_API main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv, DEMO_THEME_PATH);

  RunTest(app);

  return 0;
}
