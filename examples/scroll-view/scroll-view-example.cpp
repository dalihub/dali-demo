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

// EXTERNAL INCLUDES
#include <sstream>

// INTERNAL INCLUDES
#include "../shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * const BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-default.png" );
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char * const APPLICATION_TITLE( "ScrollView" );
const char * const EFFECT_DEPTH_IMAGE( DALI_IMAGE_DIR "icon-scroll-view-depth.png" );
const char * const EFFECT_INNER_CUBE_IMAGE( DALI_IMAGE_DIR "icon-scroll-view-inner-cube.png" );
const char * const EFFECT_CAROUSEL_IMAGE( DALI_IMAGE_DIR "icon-scroll-view-carousel.png" );

const Vector3 ICON_SIZE(100.0f, 100.0f, 0.0f);

const char* EFFECT_MODE_NAME[] = {
    "Depth",
    "Cube",
    "PageCarousel",
    "PageCube",
    "PageSpiral"
};

const char * const IMAGE_PATHS[] = {
    DALI_IMAGE_DIR "gallery-medium-1.jpg",
    DALI_IMAGE_DIR "gallery-medium-2.jpg",
    DALI_IMAGE_DIR "gallery-medium-3.jpg",
    DALI_IMAGE_DIR "gallery-medium-4.jpg",
    DALI_IMAGE_DIR "gallery-medium-5.jpg",
    DALI_IMAGE_DIR "gallery-medium-6.jpg",
    DALI_IMAGE_DIR "gallery-medium-7.jpg",
    DALI_IMAGE_DIR "gallery-medium-8.jpg",
    DALI_IMAGE_DIR "gallery-medium-9.jpg",
    DALI_IMAGE_DIR "gallery-medium-10.jpg",
    DALI_IMAGE_DIR "gallery-medium-11.jpg",
    DALI_IMAGE_DIR "gallery-medium-12.jpg",
    DALI_IMAGE_DIR "gallery-medium-13.jpg",
    DALI_IMAGE_DIR "gallery-medium-14.jpg",
    DALI_IMAGE_DIR "gallery-medium-15.jpg",
    DALI_IMAGE_DIR "gallery-medium-16.jpg",
    DALI_IMAGE_DIR "gallery-medium-17.jpg",
    DALI_IMAGE_DIR "gallery-medium-18.jpg",
    DALI_IMAGE_DIR "gallery-medium-19.jpg",
    DALI_IMAGE_DIR "gallery-medium-20.jpg",
    DALI_IMAGE_DIR "gallery-medium-21.jpg",
    DALI_IMAGE_DIR "gallery-medium-22.jpg",
    DALI_IMAGE_DIR "gallery-medium-23.jpg",
    DALI_IMAGE_DIR "gallery-medium-24.jpg",
    DALI_IMAGE_DIR "gallery-medium-25.jpg",
    DALI_IMAGE_DIR "gallery-medium-26.jpg",
    DALI_IMAGE_DIR "gallery-medium-27.jpg",
    DALI_IMAGE_DIR "gallery-medium-28.jpg",
    DALI_IMAGE_DIR "gallery-medium-29.jpg",
    DALI_IMAGE_DIR "gallery-medium-30.jpg",
    DALI_IMAGE_DIR "gallery-medium-31.jpg",
    DALI_IMAGE_DIR "gallery-medium-32.jpg",
    DALI_IMAGE_DIR "gallery-medium-33.jpg",
    DALI_IMAGE_DIR "gallery-medium-34.jpg",
    DALI_IMAGE_DIR "gallery-medium-35.jpg",
    DALI_IMAGE_DIR "gallery-medium-36.jpg",
    DALI_IMAGE_DIR "gallery-medium-37.jpg",
    DALI_IMAGE_DIR "gallery-medium-38.jpg",
    DALI_IMAGE_DIR "gallery-medium-39.jpg",
    DALI_IMAGE_DIR "gallery-medium-40.jpg",
    DALI_IMAGE_DIR "gallery-medium-41.jpg",
    DALI_IMAGE_DIR "gallery-medium-42.jpg",
    DALI_IMAGE_DIR "gallery-medium-43.jpg",
    DALI_IMAGE_DIR "gallery-medium-44.jpg",
    DALI_IMAGE_DIR "gallery-medium-45.jpg",
    DALI_IMAGE_DIR "gallery-medium-46.jpg",
    DALI_IMAGE_DIR "gallery-medium-47.jpg",
    DALI_IMAGE_DIR "gallery-medium-48.jpg",
    DALI_IMAGE_DIR "gallery-medium-49.jpg",
    DALI_IMAGE_DIR "gallery-medium-50.jpg",
    DALI_IMAGE_DIR "gallery-medium-51.jpg",
    DALI_IMAGE_DIR "gallery-medium-52.jpg",
    DALI_IMAGE_DIR "gallery-medium-53.jpg",

    NULL
};

const char * const GetNextImagePath()
{
  static const char * const * imagePtr = &IMAGE_PATHS[0];

  if ( *(++imagePtr) == NULL )
  {
    imagePtr = &IMAGE_PATHS[0];
  }

  return *imagePtr;
}

const int PAGE_COLUMNS = 10;                                                ///< Number of Pages going across (columns)
const int PAGE_ROWS = 1;                                                    ///< Number of Pages going down (rows)
const int IMAGE_ROWS = 5;                                                   ///< Number of Images going down (rows) with a Page

// 3D Effect constants
const Vector3 ANGLE_CUBE_PAGE_ROTATE(Math::PI * 0.2f, Math::PI * 0.2f, 0.0f); ///< Cube page rotates as if it has ten sides with the camera positioned inside
const Vector2 ANGLE_CUSTOM_CUBE_SWING(-Math::PI * 0.45f, -Math::PI * 0.45f);  ///< outer cube pages swing 90 degrees as they pan offscreen
const Vector2 ANGLE_SPIRAL_SWING_IN(Math::PI * 0.45f, Math::PI * 0.45f);
const Vector2 ANGLE_SPIRAL_SWING_OUT(Math::PI * 0.3f, Math::PI * 0.3f);

// Depth Effect constants
const Vector2 POSITION_EXTENT_DEPTH_EFFECT(0.5f, 2.5f);                     ///< Extent of X & Y position to alter function exponent.
const Vector2 OFFSET_EXTENT_DEPTH_EFFECT(1.0f, 1.0f);                       ///< Function exponent offset constant.
const float POSITION_SCALE_DEPTH_EFFECT(1.5f);                              ///< Position scaling.
const float SCALE_EXTENT_DEPTH_EFFECT(0.5f);                                ///< Maximum scale factor when Actors scrolled one page away (50% size)

// 3D Effect constants
const Vector2 ANGLE_SWING_3DEFFECT(Math::PI_2 * 0.75, Math::PI_2 * 0.75f); ///< Angle Swing in radians
const Vector2 POSITION_SWING_3DEFFECT(0.25f, 0.25f); ///< Position Swing relative to stage size.
const Vector3 ANCHOR_3DEFFECT_STYLE0(-105.0f, 30.0f, -240.0f); ///< Rotation Anchor position for 3D Effect (Style 0)
const Vector3 ANCHOR_3DEFFECT_STYLE1(65.0f, -70.0f, -300.0f); ///< Rotation Anchor position for 3D Effect (Style 1)


const unsigned int IMAGE_THUMBNAIL_WIDTH  = 256;                            ///< Width of Thumbnail Image in texels
const unsigned int IMAGE_THUMBNAIL_HEIGHT = 256;                            ///< Height of Thumbnail Image in texels

const float SPIN_DURATION = 5.0f;                                           ///< Times to spin an Image by upon touching, each spin taking a second.

const float EFFECT_SNAP_DURATION(0.66f);                                    ///< Scroll Snap Duration for Effects
const float EFFECT_FLICK_DURATION(0.5f);                                    ///< Scroll Flick Duration for Effects

} // unnamed namespace

/**
 * This example shows how to do custom Scroll Effects
 */
class ExampleController : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  ExampleController( Application& application )
  : mApplication( application ),
    mView(),
    mScrolling(false),
    mEffectMode(CubeEffect)
  {
    // Connect to the Application's Init and orientation changed signal
    mApplication.InitSignal().Connect(this, &ExampleController::OnInit);
  }

  ~ExampleController()
  {
    // Nothing to do here; everything gets deleted automatically
  }

  /**
   * This method gets called once the main loop of application is up and running
   */
  void OnInit(Application& app)
  {
    Stage stage = Dali::Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &ExampleController::OnKeyEvent);

    // Hide the indicator bar
    mApplication.GetWindow().ShowIndicator(Dali::Window::INVISIBLE);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( app,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            "" );

    mEffectIcon[ DepthEffect ]     = ResourceImage::New( EFFECT_DEPTH_IMAGE );
    mEffectIcon[ CubeEffect ]      = ResourceImage::New( EFFECT_INNER_CUBE_IMAGE );
    mEffectIcon[ PageCarouselEffect ] = ResourceImage::New( EFFECT_CAROUSEL_IMAGE );
    mEffectIcon[ PageCubeEffect ]     = ResourceImage::New( EFFECT_CAROUSEL_IMAGE );
    mEffectIcon[ PageSpiralEffect ]   = ResourceImage::New( EFFECT_CAROUSEL_IMAGE );

    // Create a effect change button. (right of toolbar)
    mEffectChangeButton = Toolkit::PushButton::New();
    mEffectChangeButton.ClickedSignal().Connect( this, &ExampleController::OnEffectTouched );
    mToolBar.AddControl( mEffectChangeButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    // Create the content layer.
    AddContentLayer();

    // Hack to force screen refresh.
    Animation animation = Animation::New(1.0f);
    animation.AnimateTo(Property(mContentLayer, Actor::POSITION), Vector3::ZERO );
    animation.Play();
  }

private:

  /**
   * Adds content to the ContentLayer. This is everything we see
   * excluding the toolbar at the top.
   */
  void AddContentLayer()
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    mScrollView = ScrollView::New();
    mScrollView.SetAnchorPoint(AnchorPoint::CENTER);
    mScrollView.SetParentOrigin(ParentOrigin::CENTER);
    mContentLayer.Add( mScrollView );
    mScrollView.SetSize( stageSize );
    mScrollView.SetAxisAutoLock( true );
    mScrollView.SetAxisAutoLockGradient( 1.0f );

    mScrollView.ScrollStartedSignal().Connect( this, &ExampleController::OnScrollStarted );
    mScrollView.ScrollCompletedSignal().Connect( this, &ExampleController::OnScrollCompleted );

    for(int row = 0;row<PAGE_ROWS;row++)
    {
      for(int column = 0;column<PAGE_COLUMNS;column++)
      {
        Actor page = CreatePage();

        page.SetPosition( column * stageSize.x, row * stageSize.y );
        mScrollView.Add( page );

        mPages.push_back(page);
      }
    }

    Update();
  }

  /**
   * Updates the ScrollView and it's children based
   * on the current effect.
   */
  void Update()
  {
    std::stringstream ss(APPLICATION_TITLE);
    ss << APPLICATION_TITLE << ": " << EFFECT_MODE_NAME[mEffectMode];
    SetTitle(ss.str());

    mEffectChangeButton.SetBackgroundImage( mEffectIcon[ mEffectMode ] );

    // remove old Effect if exists.
    if(mScrollViewEffect)
    {
      mScrollView.RemoveEffect(mScrollViewEffect);
    }

    // apply new Effect to ScrollView
    ApplyEffectToScrollView();

    for(ActorIter pageIter = mPages.begin(); pageIter != mPages.end(); ++pageIter)
    {
      Actor page = *pageIter;
      ApplyEffectToPage( page );

      unsigned int numChildren = (*pageIter).GetChildCount();
      for(unsigned int i=0; i<numChildren; ++i)
      {
        Actor image = (*pageIter).GetChildAt(i);

        // Remove old effect's manual constraints.
        image.RemoveConstraints();

        // Apply new effect's manual constraints.
        ApplyEffectToActor( image, page );
      }
    }
  }

  /**
   * Creates a page using a source of images.
   */
  Actor CreatePage()
  {
    Actor page = Actor::New();
    page.SetSizeMode( SIZE_EQUAL_TO_PARENT );
    page.SetParentOrigin( ParentOrigin::CENTER );
    page.SetAnchorPoint( AnchorPoint::CENTER );

    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    const float margin = 10.0f;

    // Calculate the number of images going across (columns) within a page, according to the screen resolution and dpi.
    int imageColumns = round(IMAGE_ROWS * (stageSize.x / stage.GetDpi().x) / (stageSize.y / stage.GetDpi().y));
    const Vector3 imageSize((stageSize.x / imageColumns) - margin, (stageSize.y / IMAGE_ROWS) - margin, 0.0f);

    for(int row = 0;row<IMAGE_ROWS;row++)
    {
      for(int column = 0;column<imageColumns;column++)
      {
        ImageActor image = CreateImage( GetNextImagePath() );

        image.SetParentOrigin( ParentOrigin::CENTER );
        image.SetAnchorPoint( AnchorPoint::CENTER );

        Vector3 position( margin * 0.5f + (imageSize.x + margin) * column - stageSize.width * 0.5f,
                         margin * 0.5f + (imageSize.y + margin) * row - stageSize.height * 0.5f,
                          0.0f);
        image.SetPosition( position + imageSize * 0.5f );
        image.SetSize( imageSize );
        page.Add(image);
      }
    }

    return page;
  }

  /**
   * [ScrollView]
   * Applies effect to scrollView
   */
  void ApplyEffectToScrollView()
  {
    bool wrap(true);
    bool snap(true);

    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    switch( mEffectMode )
    {
      case DepthEffect:
      {
        mScrollViewEffect = ScrollViewDepthEffect::New();
        mScrollView.SetScrollSnapDuration(EFFECT_SNAP_DURATION);
        mScrollView.SetScrollFlickDuration(EFFECT_FLICK_DURATION);
        mScrollView.SetScrollSnapAlphaFunction(AlphaFunctions::EaseOut);
        mScrollView.SetScrollFlickAlphaFunction(AlphaFunctions::EaseOut);
        mScrollView.RemoveConstraintsFromChildren();
        break;
      }

      case CubeEffect:
      {
        mScrollViewEffect = ScrollViewCubeEffect::New();
        mScrollView.SetScrollSnapDuration(EFFECT_SNAP_DURATION);
        mScrollView.SetScrollFlickDuration(EFFECT_FLICK_DURATION);
        mScrollView.SetScrollSnapAlphaFunction(AlphaFunctions::EaseOutBack);
        mScrollView.SetScrollFlickAlphaFunction(AlphaFunctions::EaseOutBack);
        mScrollView.RemoveConstraintsFromChildren();
        break;
      }

      case PageCarouselEffect:
      {
        mScrollViewEffect = ScrollViewPageCarouselEffect::New();
        mScrollView.SetScrollSnapDuration(EFFECT_SNAP_DURATION);
        mScrollView.SetScrollFlickDuration(EFFECT_FLICK_DURATION);
        mScrollView.SetScrollSnapAlphaFunction(AlphaFunctions::EaseOut);
        mScrollView.SetScrollFlickAlphaFunction(AlphaFunctions::EaseOut);
        mScrollView.RemoveConstraintsFromChildren();
        break;
      }

      case PageCubeEffect:
      {
        mScrollViewEffect = ScrollViewPageCubeEffect::New();
        mScrollView.SetScrollSnapDuration(EFFECT_SNAP_DURATION);
        mScrollView.SetScrollFlickDuration(EFFECT_FLICK_DURATION);
        mScrollView.SetScrollSnapAlphaFunction(AlphaFunctions::EaseOut);
        mScrollView.SetScrollFlickAlphaFunction(AlphaFunctions::EaseOut);
        mScrollView.RemoveConstraintsFromChildren();
        break;
      }

      case PageSpiralEffect:
      {
        mScrollViewEffect = ScrollViewPageSpiralEffect::New();
        mScrollView.SetScrollSnapDuration(EFFECT_SNAP_DURATION);
        mScrollView.SetScrollFlickDuration(EFFECT_FLICK_DURATION);
        mScrollView.SetScrollSnapAlphaFunction(AlphaFunctions::EaseOut);
        mScrollView.SetScrollFlickAlphaFunction(AlphaFunctions::EaseOut);
        mScrollView.RemoveConstraintsFromChildren();
        break;
      }

      default:
      {
        break;
      }
    }

    if( mScrollViewEffect )
    {
      mScrollView.ApplyEffect(mScrollViewEffect);
    }

    mScrollView.SetWrapMode(wrap);

    RulerPtr rulerX = CreateRuler(snap ? stageSize.width : 0.0f);
    RulerPtr rulerY = new DefaultRuler;
    rulerX->SetDomain(RulerDomain(0.0f, stageSize.x * PAGE_COLUMNS, !wrap));
    rulerY->Disable();

    mScrollView.SetRulerX( rulerX );
    mScrollView.SetRulerY( rulerY );
  }

  /**
   * Creates a Ruler that snaps to a specified grid size.
   * If that grid size is 0.0 then this ruler does not
   * snap.
   *
   * @param[in] gridSize (optional) The grid size for the ruler,
   * (Default = 0.0 i.e. no snapping)
   * @return The ruler is returned.
   */
  RulerPtr CreateRuler(float gridSize = 0.0f)
  {
    if(gridSize <= Math::MACHINE_EPSILON_0)
    {
        return new DefaultRuler();
    }
    return new FixedRuler(gridSize);
  }
  // end switch
  /**
    * [Page]
    * Applies effect to the pages within scroll view.
    *
    * @param[in] page The page Actor to apply effect to.
    */
   void ApplyEffectToPage(Actor page)
   {
     page.RemoveConstraints();
     page.SetSizeMode( SIZE_EQUAL_TO_PARENT );

     switch( mEffectMode )
     {
       case PageCarouselEffect:
       {
         ScrollViewPageCarouselEffect effect = ScrollViewPageCarouselEffect::DownCast( mScrollViewEffect );
         effect.ApplyToPage( page );
         break;
       }

       case PageCubeEffect:
       {
         ScrollViewPageCubeEffect effect = ScrollViewPageCubeEffect::DownCast( mScrollViewEffect );
         effect.ApplyToPage( page, ANGLE_SWING_3DEFFECT );
         break;
       }

       case PageSpiralEffect:
       {
         ScrollViewPageSpiralEffect effect = ScrollViewPageSpiralEffect::DownCast( mScrollViewEffect );
         effect.ApplyToPage( page, ANGLE_SWING_3DEFFECT );
         break;
       }

       default:
       {
         break;
       }
     }
   }

  /**
   * [Actor]
   * Applies effect to child which resides in page (which in turn resides in scrollview)
   *
   * @note Page is typically the Parent of child, although in
   * some scenarios Page is simply a container which has a child as
   * a descendent.
   *
   * @param[in] child The child actor to apply effect to
   * @param[in] page The page which this child is inside
   */
  void ApplyEffectToActor( Actor child, Actor page )
  {
    switch( mEffectMode )
    {
      case DepthEffect:
      {
        ApplyDepthEffectToActor( child );
        break;
      }

      case CubeEffect:
      {
        ApplyCubeEffectToActor( child );
        break;
      }

      default:
      {
        break;
      }
    }
  }

  /**
   * Applies depth effect to the child which resides in page (which in turn resides in scrollview)
   *
   * @param[in] child The child actor to apply depth effect to
   */
  void ApplyDepthEffectToActor( Actor child )
  {
    ScrollViewDepthEffect depthEffect = ScrollViewDepthEffect::DownCast(mScrollViewEffect);
    depthEffect.ApplyToActor( child,
                              POSITION_EXTENT_DEPTH_EFFECT,
                              OFFSET_EXTENT_DEPTH_EFFECT,
                              POSITION_SCALE_DEPTH_EFFECT,
                              SCALE_EXTENT_DEPTH_EFFECT );
  }

  void ApplyCubeEffectToActor( Actor child )
  {
    Vector3 anchor;
    if(rand()&1)
    {
      anchor = ANCHOR_3DEFFECT_STYLE0;
    }
    else
    {
      anchor = ANCHOR_3DEFFECT_STYLE1;
    }

    ScrollViewCubeEffect cubeEffect = ScrollViewCubeEffect::DownCast(mScrollViewEffect);
    cubeEffect.ApplyToActor( child,
                             anchor,
                             ANGLE_SWING_3DEFFECT,
                             POSITION_SWING_3DEFFECT * Vector2(Stage::GetCurrent().GetSize()));
  }

  /**
   * Creates an Image (Helper)
   *
   * @param[in] filename the path of the image.
   * @param[in] width the width of the image in texels
   * @param[in] height the height of the image in texels.
   */
  ImageActor CreateImage( const std::string& filename, unsigned int width = IMAGE_THUMBNAIL_WIDTH, unsigned int height = IMAGE_THUMBNAIL_HEIGHT )
  {
    ImageAttributes attributes;

    attributes.SetSize(width, height);
    attributes.SetScalingMode(ImageAttributes::ShrinkToFit);
    Image img = ResourceImage::New(filename, attributes);
    ImageActor actor = ImageActor::New(img);
    actor.SetName( filename );
    actor.SetParentOrigin(ParentOrigin::CENTER);
    actor.SetAnchorPoint(AnchorPoint::CENTER);

    actor.TouchedSignal().Connect( this, &ExampleController::OnTouchImage );
    return actor;
  }

  /**
   * When scroll starts (i.e. user starts to drag scrollview),
   * note this state (mScrolling = true)
   * @param[in] position Current Scroll Position
   */
  void OnScrollStarted( const Vector3& position )
  {
    mScrolling = true;
  }

  /**
   * When scroll starts (i.e. user stops dragging scrollview, and scrollview has snapped to destination),
   * note this state (mScrolling = false)
   * @param[in] position Current Scroll Position
   */
  void OnScrollCompleted( const Vector3& position )
  {
    mScrolling = false;
  }

  /**
   * Upon Touching an image (Release), make it spin
   * (provided we're not scrolling).
   * @param[in] actor The actor touched
   * @param[in] event The TouchEvent.
   */
  bool OnTouchImage( Actor actor, const TouchEvent& event )
  {
    if( (event.points.size() > 0) && (!mScrolling) )
    {
      TouchPoint point = event.points[0];
      if(point.state == TouchPoint::Up)
      {
        // Spin the Image a few times.
        Animation animation = Animation::New(SPIN_DURATION);
        animation.RotateBy( actor, Degree(360.0f * SPIN_DURATION), Vector3::XAXIS, AlphaFunctions::EaseOut);
        animation.Play();
      }
    }
    return false;
  }

  /**
   * Signal handler, called when the 'Effect' button has been touched.
   *
   * @param[in] button The button that was pressed.
   */
  bool OnEffectTouched(Button button)
  {
    mEffectMode = static_cast<EffectMode>((static_cast<int>(mEffectMode) + 1) % static_cast<int>(Total));
    Update();
    return true;
  }

  /**
   * Sets/Updates the title of the View
   * @param[in] title The new title for the view.
   */
  void SetTitle(const std::string& title)
  {
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
        mApplication.Quit();
      }
    }
  }

private:

  Application& mApplication;                            ///< Application instance
  Toolkit::View mView;                                  ///< The View instance.
  Toolkit::ToolBar mToolBar;                            ///< The View's Toolbar.
  Layer mContentLayer;                                  ///< The content layer (contains game actors)
  ScrollView mScrollView;                               ///< ScrollView UI Component
  bool mScrolling;                                      ///< ScrollView scrolling state (true = scrolling, false = stationary)
  ScrollViewEffect mScrollViewEffect;                   ///< ScrollView Effect instance.
  ActorContainer mPages;                                ///< Keeps track of all the pages for applying effects.

  /**
   * Enumeration of different effects this scrollview can operate under.
   */
  enum EffectMode
  {
    DepthEffect,                                        ///< Depth Effect
    CubeEffect,                                         ///< Cube effect
    PageCarouselEffect,                                 ///< Page carousel effect
    PageCubeEffect,                                     ///< Page cube effect
    PageSpiralEffect,                                   ///< Page spiral effect

    Total
  };

  EffectMode mEffectMode;                               ///< Current Effect mode

  Image mEffectIcon[Total];                             ///< Icons for the effect button
  Toolkit::PushButton mEffectChangeButton;              ///< Effect Change Button
};

int main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv);
  ExampleController test(app);
  app.MainLoop();
  return 0;
}
