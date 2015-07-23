/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/bubble-effect/bubble-emitter.h>
#include "shared/view.h"

using namespace Dali;

namespace
{
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char * const APPLICATION_TITLE( "Bubble Effect" );
const char * const CHANGE_BACKGROUND_ICON( DALI_IMAGE_DIR "icon-change.png" );
const char * const CHANGE_BACKGROUND_ICON_SELECTED( DALI_IMAGE_DIR "icon-change-selected.png" );
const char * const CHANGE_BUBBLE_SHAPE_ICON( DALI_IMAGE_DIR "icon-replace.png" );
const char * const CHANGE_BUBBLE_SHAPE_ICON_SELECTED( DALI_IMAGE_DIR "icon-replace-selected.png" );

const char* BACKGROUND_IMAGES[]=
{
  DALI_IMAGE_DIR "background-1.jpg",
  DALI_IMAGE_DIR "background-2.jpg",
  DALI_IMAGE_DIR "background-3.jpg",
  DALI_IMAGE_DIR "background-4.jpg",
  DALI_IMAGE_DIR "background-5.jpg",
};
const unsigned int NUM_BACKGROUND_IMAGES( sizeof( BACKGROUND_IMAGES ) / sizeof( BACKGROUND_IMAGES[0] ) );

const char* BUBBLE_SHAPE_IMAGES[] =
{
  DALI_IMAGE_DIR "bubble-ball.png",
  DALI_IMAGE_DIR "icon-effect-cross.png",
  DALI_IMAGE_DIR "icon-item-view-layout-spiral.png",
  DALI_IMAGE_DIR "icon-replace.png"
};
const unsigned int NUM_BUBBLE_SHAPE_IMAGES( sizeof( BUBBLE_SHAPE_IMAGES ) / sizeof( BUBBLE_SHAPE_IMAGES[0] ) );

const Vector2 DEFAULT_BUBBLE_SIZE( 10.f, 30.f );
const unsigned int DEFAULT_NUMBER_OF_BUBBLES( 1000 );

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

}// end LOCAL_STUFF

// This example shows the usage of BubbleEmitter which displays lots of moving bubbles on the stage.
class BubbleEffectExample : public ConnectionTracker
{
public:
  BubbleEffectExample(Application &app)
  : mApp(app),
    mHSVDelta( Vector3( 0.f, 0.f, 0.5f ) ),
    mTimerInterval( 16 ),
    mCurrentBackgroundImageId( 0 ),
    mCurrentBubbleShapeImageId( 0 ),
    mNeedNewAnimation( true )
  {
    // Connect to the Application's Init signal
    app.InitSignal().Connect(this, &BubbleEffectExample::Create);
  }

  ~BubbleEffectExample()
  {
  }

private:

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& app)
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    stage.KeyEventSignal().Connect(this, &BubbleEffectExample::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Toolkit::ToolBar toolBar;
    Layer content = DemoHelper::CreateView( app,
                                            mBackground,
                                            toolBar,
                                            "",
                                            TOOLBAR_IMAGE,
                                            APPLICATION_TITLE );

    // Add a button to change background. (right of toolbar)
    mChangeBackgroundButton = Toolkit::PushButton::New();
    mChangeBackgroundButton.SetUnselectedImage( CHANGE_BACKGROUND_ICON );
    mChangeBackgroundButton.SetSelectedImage( CHANGE_BACKGROUND_ICON_SELECTED );
    mChangeBackgroundButton.ClickedSignal().Connect( this, &BubbleEffectExample::OnChangeIconClicked );
    toolBar.AddControl( mChangeBackgroundButton,
                        DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage,
                        Toolkit::Alignment::HorizontalRight,
                        DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );
    // Add a button to change bubble shape. ( left of bar )
    mChangeBubbleShapeButton = Toolkit::PushButton::New();
    mChangeBubbleShapeButton.SetUnselectedImage( CHANGE_BUBBLE_SHAPE_ICON );
    mChangeBubbleShapeButton.SetSelectedImage( CHANGE_BUBBLE_SHAPE_ICON_SELECTED );
    mChangeBubbleShapeButton.ClickedSignal().Connect( this, &BubbleEffectExample::OnChangeIconClicked );
    toolBar.AddControl( mChangeBubbleShapeButton,
                        DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage,
                        Toolkit::Alignment::HorizontalLeft,
                        DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    // Create and initialize the BubbleEmitter object
    mBubbleEmitter = Toolkit::BubbleEmitter::New( stageSize,
                                                  ResourceImage::New( BUBBLE_SHAPE_IMAGES[mCurrentBubbleShapeImageId] ),
                                                  DEFAULT_NUMBER_OF_BUBBLES,
                                                  DEFAULT_BUBBLE_SIZE);
    mBackgroundImage = LoadStageFillingImage( BACKGROUND_IMAGES[mCurrentBackgroundImageId] );
    mBubbleEmitter.SetBackground( mBackgroundImage, mHSVDelta );

    // Get the root actor of all bubbles, and add it to stage.
    Actor bubbleRoot = mBubbleEmitter.GetRootActor();
    bubbleRoot.SetParentOrigin(ParentOrigin::CENTER);
    bubbleRoot.SetZ(0.1f); // Make sure the bubbles displayed on top og the background.
    content.Add( bubbleRoot );

    // Add the background image actor to stage
    mBackground.SetBackgroundImage( mBackgroundImage );

    // Set up the timer to emit bubble regularly when the finger is touched down but not moved
    mTimerForBubbleEmission = Timer::New( mTimerInterval );
    mTimerForBubbleEmission.TickSignal().Connect(this, &BubbleEffectExample::OnTimerTick);

    // Connect the callback to the touch signal on the background
    mBackground.TouchedSignal().Connect( this, &BubbleEffectExample::OnTouch );
  }


/***********
 * Emit bubbles
 *****************/

  // Set up the animation of emitting bubbles, to be efficient, every animation controls multiple emission ( 4 here )
  void SetUpAnimation( Vector2 emitPosition, Vector2 direction )
  {
    if( mNeedNewAnimation )
    {
      float duration = Random::Range(1.f, 1.5f);
      mEmitAnimation = Animation::New( duration );
      mNeedNewAnimation = false;
      mAnimateComponentCount = 0;
    }

    mBubbleEmitter.EmitBubble( mEmitAnimation, emitPosition, direction + Vector2(0.f, 30.f) /* upwards */, Vector2(300, 600) );

    mAnimateComponentCount++;

    if( mAnimateComponentCount % 4 ==0 )
    {
      mEmitAnimation.Play();
      mNeedNewAnimation = true;
    }
  }

  // Emit bubbles when the finger touches down but keep stationary.
  // And stops emitting new bubble after being stationary for 2 seconds
  bool OnTimerTick()
  {
    if(mEmitPosition == mCurrentTouchPosition) // finger is not moving
    {
      mNonMovementCount++;
      if(mNonMovementCount < (1000 / mTimerInterval)) // 1 seconds
      {
        for(int i = 0; i < 4; i++) // emit 4 bubbles every timer tick
        {
          SetUpAnimation( mCurrentTouchPosition+Vector2(rand()%5, rand()%5), Vector2(rand()%60-30, rand()%100-50) );
        }
      }
    }
    else
    {
      mNonMovementCount = 0;
      mEmitPosition = mCurrentTouchPosition;
    }

    return true;
  }

  // Callback function of the touch signal on the background
  bool OnTouch(Dali::Actor actor, const Dali::TouchEvent& event)
  {
    const TouchPoint &point = event.GetPoint(0);
    switch(point.state)
    {
      case TouchPoint::Down:
      {
        mCurrentTouchPosition = point.screen;
        mEmitPosition = point.screen;
        mTimerForBubbleEmission.Start();
        mNonMovementCount = 0;

        break;
      }
      case TouchPoint::Motion:
      {
        Vector2 displacement = point.screen - mCurrentTouchPosition;
        mCurrentTouchPosition = point.screen;
        //emit multiple bubbles along the moving direction when the finger moves quickly
        float step = std::min(5.f, displacement.Length());
        for( float i=0.25f; i<step; i=i+1.f)
        {
          SetUpAnimation( mCurrentTouchPosition+displacement*(i/step), displacement );
        }
        break;
      }
      case TouchPoint::Up:
      case TouchPoint::Leave:
      case TouchPoint::Interrupted:
      {
        mTimerForBubbleEmission.Stop();
        mEmitAnimation.Play();
        mNeedNewAnimation = true;
        mAnimateComponentCount = 0;
        break;
      }
      case TouchPoint::Stationary:
      case TouchPoint::Last:
      default:
      {
        break;
      }

    }
    return true;
  }

  bool OnChangeIconClicked( Toolkit::Button button )
  {
    if(button == mChangeBackgroundButton)
    {
      mBackgroundImage = LoadStageFillingImage( BACKGROUND_IMAGES[ ++mCurrentBackgroundImageId % NUM_BACKGROUND_IMAGES  ] );

      mBubbleEmitter.SetBackground( mBackgroundImage, mHSVDelta );

      mBackground.SetBackgroundImage( mBackgroundImage );
    }
    else if( button == mChangeBubbleShapeButton )
    {
      mBubbleEmitter.SetShapeImage( ResourceImage::New( BUBBLE_SHAPE_IMAGES[ ++mCurrentBubbleShapeImageId % NUM_BUBBLE_SHAPE_IMAGES ] ) );
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

private:

  Application&               mApp;
  Image                      mBackgroundImage;
  Dali::Toolkit::Control     mBackground;

  Toolkit::BubbleEmitter     mBubbleEmitter;
  Animation                  mEmitAnimation;
  Toolkit::PushButton        mChangeBackgroundButton;
  Toolkit::PushButton        mChangeBubbleShapeButton;
  Timer                      mTimerForBubbleEmission;

  Vector3                    mHSVDelta;
  Vector2                    mCurrentTouchPosition;
  Vector2                    mEmitPosition;

  unsigned int               mAnimateComponentCount;
  unsigned int               mNonMovementCount;
  unsigned int               mTimerInterval;
  unsigned int               mCurrentBackgroundImageId;
  unsigned int               mCurrentBubbleShapeImageId;

  bool                       mNeedNewAnimation;
};

/*****************************************************************************/

static void
RunTest(Application& app)
{
  BubbleEffectExample theApp(app);
  app.MainLoop();
}

/*****************************************************************************/

int
main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv, DALI_DEMO_THEME_PATH);

  RunTest(app);

  return 0;
}



