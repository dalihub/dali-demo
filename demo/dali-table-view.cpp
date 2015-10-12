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

// CLASS HEADER
#include "dali-table-view.h"

// EXTERNAL INCLUDES
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <dali/devel-api/images/distance-field.h>
#include <dali-toolkit/devel-api/shader-effects/alpha-discard-effect.h>
#include <dali-toolkit/devel-api/shader-effects/distance-field-effect.h>

// INTERNAL INCLUDES
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

///////////////////////////////////////////////////////////////////////////////

namespace
{

const std::string LOGO_PATH( DALI_IMAGE_DIR "Logo-for-demo.png" );
const std::string DEFAULT_TOOLBAR_IMAGE_PATH( DALI_IMAGE_DIR "top-bar.png" );
const std::string TILE_BACKGROUND(DALI_IMAGE_DIR "item-background.9.png");
const std::string TILE_BACKGROUND_ALPHA(DALI_IMAGE_DIR "item-background-alpha.9.png");

const char * const DEFAULT_TOOLBAR_TEXT( "TOUCH TO LAUNCH EXAMPLE" );

const float BUTTON_PRESS_ANIMATION_TIME = 0.25f;                ///< Time to perform button scale effect.
const float ROTATE_ANIMATION_TIME = 0.5f;                       ///< Time to perform rotate effect.
const int MAX_PAGES = 256;                                      ///< Maximum pages (arbitrary safety limit)
const int EXAMPLES_PER_ROW = 3;
const int ROWS_PER_PAGE = 3;
const int EXAMPLES_PER_PAGE = EXAMPLES_PER_ROW * ROWS_PER_PAGE;
const float LOGO_MARGIN_RATIO = 0.1f / 0.3f;
const float BOTTOM_PADDING_RATIO = 0.4f / 0.9f;
const Vector3 SCROLLVIEW_RELATIVE_SIZE(0.9f, 1.0f, 0.8f );     ///< ScrollView's relative size to its parent
const Vector3 TABLE_RELATIVE_SIZE(0.95f, 0.9f, 0.8f );          ///< TableView's relative size to the entire stage. The Y value means sum of the logo and table relative heights.
const float STENCIL_RELATIVE_SIZE = 1.0f;

const float EFFECT_SNAP_DURATION = 0.66f;                       ///< Scroll Snap Duration for Effects
const float EFFECT_FLICK_DURATION = 0.5f;                       ///< Scroll Flick Duration for Effects
const Vector3 ANGLE_CUBE_PAGE_ROTATE(Math::PI * 0.5f, Math::PI * 0.5f, 0.0f);

const Vector4 BUBBLE_COLOR[] =
{
  Vector4( 0.3255f, 0.3412f, 0.6353f, 0.38f ),
  Vector4( 0.3647f, 0.7569f, 0.8157f, 0.38f ),
  Vector4( 0.3804f, 0.7412f, 0.6510f, 0.38f ),
  Vector4( 1.f, 1.f, 1.f, 0.2f )
};
const int NUMBER_OF_BUBBLE_COLOR( sizeof(BUBBLE_COLOR) / sizeof(BUBBLE_COLOR[0]) );

const int NUM_BACKGROUND_IMAGES = 18;
const float BACKGROUND_SWIPE_SCALE = 0.025f;
const float BACKGROUND_SPREAD_SCALE = 1.5f;
const float SCALE_MOD = 1000.0f * Math::PI * 2.0f;
const float SCALE_SPEED = 10.0f;
const float SCALE_SPEED_SIN = 0.1f;

const unsigned int BACKGROUND_ANIMATION_DURATION = 15000; // 15 secs

const Vector4 BACKGROUND_COLOR( 0.3569f, 0.5451f, 0.7294f, 1.0f );

const float BUBBLE_MIN_Z = -1.0;
const float BUBBLE_MAX_Z = 0.0f;

/**
 * Creates the background image
 */
Control CreateBackground( std::string stylename )
{
  Control background = Control::New();
  Stage::GetCurrent().Add( background );
  background.SetProperty( Control::Property::STYLE_NAME,stylename);
  background.SetName( "BACKGROUND" );
  background.SetAnchorPoint( AnchorPoint::CENTER );
  background.SetParentOrigin( ParentOrigin::CENTER );
  background.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

  return background;
}

/**
 * Constraint to return a position for a bubble based on the scroll value and vertical wrapping
 */
struct AnimateBubbleConstraint
{
public:
  AnimateBubbleConstraint( const Vector3& initialPos, float scale )
      : mInitialX( initialPos.x ),
        mScale( scale )
  {
  }

  void operator()( Vector3& position, const PropertyInputContainer& inputs )
  {
    const Vector3& parentSize = inputs[1]->GetVector3();
    const Vector3& childSize = inputs[2]->GetVector3();

    // Wrap bubbles verically.
    if( position.y + childSize.y * 0.5f < -parentSize.y * 0.5f )
    {
      position.y = parentSize.y * 0.5f + childSize.y * 0.5f;
    }

    // Bubbles X position moves parallax to horizontal
    // panning by a scale factor unique to each bubble.
    position.x = mInitialX + ( inputs[0]->GetVector2().x * mScale );
  }

private:
  float mInitialX;
  float mScale;
  float mShapeSize;
};

bool CompareByTitle( const Example& lhs, const Example& rhs )
{
  return lhs.title < rhs.title;
}

} // namespace

DaliTableView::DaliTableView( Application& application )
: mApplication( application ),
  mBackgroundLayer(),
  mRootActor(),
  mRotateAnimation(),
  mPressedAnimation(),
  mScrollViewLayer(),
  mScrollView(),
  mScrollViewEffect(),
  mScrollRulerX(),
  mScrollRulerY(),
  mPressedActor(),
  mAnimationTimer(),
  mLogoTapDetector(),
  mVersionPopup(),
  mPages(),
  mBackgroundAnimations(),
  mExampleList(),
  mTotalPages(),
  mScrolling( false ),
  mSortAlphabetically( false ),
  mBackgroundAnimsPlaying( false )
{
  application.InitSignal().Connect( this, &DaliTableView::Initialize );
}

DaliTableView::~DaliTableView()
{
}

void DaliTableView::AddExample( Example example )
{
  mExampleList.push_back( example );
}

void DaliTableView::SortAlphabetically( bool sortAlphabetically )
{
  mSortAlphabetically = sortAlphabetically;
}

void DaliTableView::Initialize( Application& application )
{
  Stage::GetCurrent().KeyEventSignal().Connect( this, &DaliTableView::OnKeyEvent );

  const Vector2 stageSize = Stage::GetCurrent().GetSize();

  // Background
  Control background = CreateBackground( "launcherbackground" );
  Stage::GetCurrent().Add( background );

  // Add root actor
  mRootActor = TableView::New( 4, 1 );
  mRootActor.SetAnchorPoint( AnchorPoint::CENTER );
  mRootActor.SetParentOrigin( ParentOrigin::CENTER );
  mRootActor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  Stage::GetCurrent().Add( mRootActor );

  // Toolbar at top
  Dali::Toolkit::ToolBar toolbar;
  Dali::Layer toolBarLayer = DemoHelper::CreateToolbar(toolbar,
                                                       DEFAULT_TOOLBAR_IMAGE_PATH,
                                                       DEFAULT_TOOLBAR_TEXT,
                                                       DemoHelper::DEFAULT_VIEW_STYLE);

  mRootActor.AddChild( toolBarLayer, TableView::CellPosition( 0, 0 ) );
  mRootActor.SetFitHeight( 0 );

  // Add logo
  ImageView logo = CreateLogo( LOGO_PATH );
  logo.SetName( "LOGO_IMAGE" );
  logo.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  const float paddingHeight = ( ( 1.f-TABLE_RELATIVE_SIZE.y ) * stageSize.y );
  const float logoMargin = paddingHeight * LOGO_MARGIN_RATIO;

  // Show version in a popup when log is tapped
  mLogoTapDetector = TapGestureDetector::New();
  mLogoTapDetector.Attach( logo );
  mLogoTapDetector.DetectedSignal().Connect( this, &DaliTableView::OnLogoTapped );

  const float bottomMargin = paddingHeight * BOTTOM_PADDING_RATIO;

  Alignment alignment = Alignment::New();
  alignment.SetName( "LOGO_ALIGNMENT" );
  alignment.Add( logo );
  alignment.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  alignment.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );
  Actor alignmentActor = alignment;
  alignmentActor.SetPadding( Padding( 0.0f, 0.0f, logoMargin, logoMargin ));
  mRootActor.AddChild( alignment, TableView::CellPosition( 1, 0 ) );
  mRootActor.SetFitHeight( 1 );

  // scrollview occupying the majority of the screen
  mScrollView = ScrollView::New();

  mScrollView.SetAnchorPoint( AnchorPoint::CENTER );
  mScrollView.SetParentOrigin( ParentOrigin::CENTER );
  mScrollView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  const float buttonsPageMargin = ( 1.0f - TABLE_RELATIVE_SIZE.x ) * 0.5f * stageSize.width;
  mScrollView.SetPadding( Padding( buttonsPageMargin, buttonsPageMargin, 0.0f, 0.0f ) );

  mScrollView.SetAxisAutoLock( true );
  mScrollView.ScrollCompletedSignal().Connect( this, &DaliTableView::OnScrollComplete );
  mScrollView.ScrollStartedSignal().Connect( this, &DaliTableView::OnScrollStart );
  mScrollView.TouchedSignal().Connect( this, &DaliTableView::OnScrollTouched );

  mScrollViewLayer = Layer::New();

  // Disable the depth test for performance
  mScrollViewLayer.SetDepthTestDisabled( true );
  mScrollViewLayer.SetAnchorPoint( AnchorPoint::CENTER );
  mScrollViewLayer.SetParentOrigin( ParentOrigin::CENTER );
  mScrollViewLayer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

  // Create solid background colour.
  Control backgroundColourActor = Control::New();
  backgroundColourActor.SetBackgroundColor( BACKGROUND_COLOR );
  backgroundColourActor.SetAnchorPoint( AnchorPoint::CENTER );
  backgroundColourActor.SetParentOrigin( ParentOrigin::CENTER );
  backgroundColourActor.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
  backgroundColourActor.SetSizeModeFactor( Vector3( 1.0f, 1.5f, 1.0f ) );

  mScrollViewLayer.Add( backgroundColourActor );

  // Populate background and bubbles - needs to be scrollViewLayer so scroll ends show
  Actor bubbleContainer = Actor::New();
  bubbleContainer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  bubbleContainer.SetAnchorPoint( AnchorPoint::CENTER );
  bubbleContainer.SetParentOrigin( ParentOrigin::CENTER );
  backgroundColourActor.Add( bubbleContainer );

  SetupBackground( bubbleContainer );

  Alignment buttonsAlignment = Alignment::New();
  buttonsAlignment.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  buttonsAlignment.Add( mScrollViewLayer );

  mScrollViewLayer.Add( mScrollView );

  mRootActor.AddChild( buttonsAlignment, TableView::CellPosition( 2, 0 ) );

  mRootActor.SetFixedHeight( 3, bottomMargin );

  // Add scroll view effect and setup constraints on pages
  ApplyScrollViewEffect();

  // Add pages and tiles
  Populate();

  // Remove constraints for inner cube effect
  ApplyCubeEffectToPages();

  Dali::Window winHandle = application.GetWindow();
  winHandle.AddAvailableOrientation( Dali::Window::PORTRAIT );
  winHandle.RemoveAvailableOrientation( Dali::Window::LANDSCAPE );
  winHandle.AddAvailableOrientation( Dali::Window::PORTRAIT_INVERSE );
  winHandle.RemoveAvailableOrientation( Dali::Window::LANDSCAPE_INVERSE );

  // Set initial orientation
  unsigned int degrees = 0;
  Rotate( degrees );

  //orientation.ChangedSignal().Connect( this, &DaliTableView::OrientationChanged );

  winHandle.ShowIndicator( Dali::Window::INVISIBLE );

  // Background animation
  mAnimationTimer = Timer::New( BACKGROUND_ANIMATION_DURATION );
  mAnimationTimer.TickSignal().Connect( this, &DaliTableView::PauseBackgroundAnimation );
  mAnimationTimer.Start();
  mBackgroundAnimsPlaying = true;

  KeyboardFocusManager::Get().PreFocusChangeSignal().Connect( this, &DaliTableView::OnKeyboardPreFocusChange );
  KeyboardFocusManager::Get().FocusedActorEnterKeySignal().Connect( this, &DaliTableView::OnFocusedActorActivated );
  AccessibilityManager::Get().FocusedActorActivatedSignal().Connect( this, &DaliTableView::OnFocusedActorActivated );
}

void DaliTableView::ApplyCubeEffectToPages()
{
  ScrollViewPagePathEffect effect = ScrollViewPagePathEffect::DownCast( mScrollViewEffect );
  unsigned int pageCount(0);
  for( std::vector< Actor >::iterator pageIter = mPages.begin(); pageIter != mPages.end(); ++pageIter )
  {
    Actor page = *pageIter;
    effect.ApplyToPage( page, pageCount++ );
  }
}

void DaliTableView::OnButtonsPageRelayout( const Dali::Actor& actor )
{

}

void DaliTableView::Populate()
{
  const Vector2 stageSize = Stage::GetCurrent().GetSize();

  mTotalPages = ( mExampleList.size() + EXAMPLES_PER_PAGE - 1 ) / EXAMPLES_PER_PAGE;

  // Populate ScrollView.
  if( mExampleList.size() > 0 )
  {
    if( mSortAlphabetically )
    {
      sort( mExampleList.begin(), mExampleList.end(), CompareByTitle );
    }

    unsigned int exampleCount = 0;
    ExampleListConstIter iter = mExampleList.begin();

    for( int t = 0; t < mTotalPages; t++ )
    {
      // Create Table
      TableView page = TableView::New( ROWS_PER_PAGE, EXAMPLES_PER_ROW );
      page.SetAnchorPoint( AnchorPoint::CENTER );
      page.SetParentOrigin( ParentOrigin::CENTER );
      page.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mScrollView.Add( page );

      // Calculate the number of images going across (columns) within a page, according to the screen resolution and dpi.
      const float margin = 2.0f;
      const float tileParentMultiplier = 1.0f / EXAMPLES_PER_ROW;

      for(int row = 0; row < ROWS_PER_PAGE; row++)
      {
        for(int column = 0; column < EXAMPLES_PER_ROW; column++)
        {
          const Example& example = ( *iter );

          Actor tile = CreateTile( example.name, example.title, Vector3( tileParentMultiplier, tileParentMultiplier, 1.0f ), true );
          AccessibilityManager accessibilityManager = AccessibilityManager::Get();
          accessibilityManager.SetFocusOrder( tile, ++exampleCount );
          accessibilityManager.SetAccessibilityAttribute( tile, Dali::Toolkit::AccessibilityManager::ACCESSIBILITY_LABEL,
                                                  example.title );
          accessibilityManager.SetAccessibilityAttribute( tile, Dali::Toolkit::AccessibilityManager::ACCESSIBILITY_TRAIT, "Tile" );
          accessibilityManager.SetAccessibilityAttribute( tile, Dali::Toolkit::AccessibilityManager::ACCESSIBILITY_HINT,
                                                  "You can run this example" );

          tile.SetPadding( Padding( margin, margin, margin, margin ) );

          page.AddChild( tile, TableView::CellPosition( row, column ) );

          iter++;

          if( iter == mExampleList.end() )
          {
            break;
          }
        }

        if( iter == mExampleList.end() )
        {
          break;
        }
      }

      // Set tableview position
      Vector3 pagePos( stageSize.width * TABLE_RELATIVE_SIZE.x * t, 0.0f, 0.0f );
      page.SetPosition( pagePos );

      mPages.push_back( page );

      if( iter == mExampleList.end() )
      {
        break;
      }
    }
  }

  // Update Ruler info.
  mScrollRulerX = new FixedRuler( stageSize.width * TABLE_RELATIVE_SIZE.x * 0.5f );
  mScrollRulerY = new DefaultRuler();
  mScrollRulerX->SetDomain( RulerDomain( 0.0f, (mTotalPages+1) * stageSize.width * TABLE_RELATIVE_SIZE.x * 0.5f, true ) );
  mScrollRulerY->Disable();
  mScrollView.SetRulerX( mScrollRulerX );
  mScrollView.SetRulerY( mScrollRulerY );
}

void DaliTableView::OrientationChanged( Orientation orientation )
{
  // TODO: Implement if orientation change required
}

void DaliTableView::Rotate( unsigned int degrees )
{
  // Resize the root actor
  Vector2 stageSize = Stage::GetCurrent().GetSize();
  Vector3 targetSize( stageSize.x, stageSize.y, 1.0f );

  if( degrees == 90 || degrees == 270 )
  {
    targetSize = Vector3( stageSize.y, stageSize.x, 1.0f );
  }

  if( mRotateAnimation )
  {
    mRotateAnimation.Stop();
    mRotateAnimation.Clear();
  }

  mRotateAnimation = Animation::New( ROTATE_ANIMATION_TIME );
  mRotateAnimation.AnimateTo( Property( mRootActor, Actor::Property::ORIENTATION ), Quaternion( Radian( Degree( 360 - degrees ) ), Vector3::ZAXIS ), AlphaFunction::EASE_OUT );
  mRotateAnimation.AnimateTo( Property( mRootActor, Actor::Property::SIZE ), targetSize, AlphaFunction::EASE_OUT );
  mRotateAnimation.Play();
}

Actor DaliTableView::CreateTile( const std::string& name, const std::string& title, const Dali::Vector3& sizeMultiplier, bool addBackground )
{
  Actor content = Actor::New();
  content.SetName( name );
  content.SetAnchorPoint( AnchorPoint::CENTER );
  content.SetParentOrigin( ParentOrigin::CENTER );
  content.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
  content.SetSizeModeFactor( sizeMultiplier );

  // create background image
  if( addBackground )
  {
    ImageView image = ImageView::New( TILE_BACKGROUND );
    image.SetAnchorPoint( AnchorPoint::CENTER );
    image.SetParentOrigin( ParentOrigin::CENTER );
    // make the image 100% of tile
    image.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    content.Add( image );

    // Add stencil
    ImageActor stencil = NewStencilImage();
    stencil.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    image.Add( stencil );
  }

  TextLabel label = TextLabel::New();
  label.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  label.SetProperty( Control::Property::STYLE_NAME, "launcherlabel" );
  label.SetProperty( TextLabel::Property::MULTI_LINE, true );
  label.SetProperty( TextLabel::Property::TEXT, title );
  label.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  label.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
  label.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
  content.Add( label );

  // Set the tile to be keyboard focusable
  content.SetKeyboardFocusable(true);

  // connect to the touch events
  content.TouchedSignal().Connect( this, &DaliTableView::OnTilePressed );
  content.HoveredSignal().Connect( this, &DaliTableView::OnTileHovered );

  return content;
}

ImageActor DaliTableView::NewStencilImage()
{
  Image alpha = ResourceImage::New( TILE_BACKGROUND_ALPHA );

  ImageActor stencilActor = ImageActor::New( alpha );

  stencilActor.SetParentOrigin( ParentOrigin::CENTER );
  stencilActor.SetAnchorPoint( AnchorPoint::CENTER );
  stencilActor.SetDrawMode( DrawMode::STENCIL );

  Dali::ShaderEffect shaderEffect = CreateAlphaDiscardEffect();
  stencilActor.SetShaderEffect( shaderEffect );

  return stencilActor;
}

bool DaliTableView::OnTilePressed( Actor actor, const TouchEvent& event )
{
  bool consumed = false;

  const TouchPoint& point = event.GetPoint( 0 );
  if( TouchPoint::Down == point.state )
  {
    mPressedActor = actor;
    consumed = true;
  }

  // A button press is only valid if the Down & Up events
  // both occurred within the button.
  if( ( TouchPoint::Up == point.state ) &&
      ( mPressedActor == actor ) )
  {
    // ignore Example button presses when scrolling or button animating.
    if( ( !mScrolling ) && ( !mPressedAnimation ) )
    {
      std::string name = actor.GetName();
      const ExampleListIter end = mExampleList.end();
      for( ExampleListIter iter = mExampleList.begin(); iter != end; ++iter )
      {
        if( (*iter).name == name )
        {
          // do nothing, until pressed animation finished.
          consumed = true;
          break;
        }
      }
    }

    if( consumed )
    {
      mPressedAnimation = Animation::New( BUTTON_PRESS_ANIMATION_TIME );
      mPressedAnimation.SetEndAction( Animation::Discard );

      // scale the content actor within the Tile, as to not affect the placement within the Table.
      Actor content = actor.GetChildAt(0);
      mPressedAnimation.AnimateTo( Property( content, Actor::Property::SCALE ), Vector3( 0.9f, 0.9f, 1.0f ), AlphaFunction::EASE_IN_OUT,
                                 TimePeriod( 0.0f, BUTTON_PRESS_ANIMATION_TIME * 0.5f ) );
      mPressedAnimation.AnimateTo( Property( content, Actor::Property::SCALE ), Vector3::ONE, AlphaFunction::EASE_IN_OUT,
                                 TimePeriod( BUTTON_PRESS_ANIMATION_TIME * 0.5f, BUTTON_PRESS_ANIMATION_TIME * 0.5f ) );
      mPressedAnimation.Play();
      mPressedAnimation.FinishedSignal().Connect( this, &DaliTableView::OnPressedAnimationFinished );
    }
  }
  return consumed;
}

void DaliTableView::OnPressedAnimationFinished( Dali::Animation& source )
{
  mPressedAnimation.Reset();
  if( mPressedActor )
  {
    std::string name = mPressedActor.GetName();

    std::stringstream stream;
    stream << DALI_EXAMPLE_BIN << name.c_str();
    pid_t pid = fork();
    if( pid == 0)
    {
      execlp( stream.str().c_str(), name.c_str(), NULL );
      DALI_ASSERT_ALWAYS(false && "exec failed!");
    }
    mPressedActor.Reset();
  }
}

void DaliTableView::OnScrollStart( const Dali::Vector2& position )
{
  mScrolling = true;

  PlayAnimation();
}

void DaliTableView::OnScrollComplete( const Dali::Vector2& position )
{
  mScrolling = false;

  // move focus to 1st item of new page
  AccessibilityManager accessibilityManager = AccessibilityManager::Get();
  accessibilityManager.SetCurrentFocusActor(mPages[mScrollView.GetCurrentPage()].GetChildAt(0) );
}

bool DaliTableView::OnScrollTouched( Actor actor, const TouchEvent& event )
{
  const TouchPoint& point = event.GetPoint( 0 );
  if( TouchPoint::Down == point.state )
  {
    mPressedActor = actor;
  }

  return false;
}

void DaliTableView::ApplyScrollViewEffect()
{
  // Remove old effect if exists.

  if( mScrollViewEffect )
  {
    mScrollView.RemoveEffect( mScrollViewEffect );
  }

  // Just one effect for now
  SetupInnerPageCubeEffect();

  mScrollView.ApplyEffect( mScrollViewEffect );
}

void DaliTableView::SetupInnerPageCubeEffect()
{
  const Vector2 stageSize = Stage::GetCurrent().GetSize();

  Dali::Path path = Dali::Path::New();
  Dali::Property::Array points;
  points.Resize(3);
  points[0] = Vector3( stageSize.x*0.5, 0.0f,  stageSize.x*0.5f);
  points[1] = Vector3( 0.0f, 0.0f, 0.0f );
  points[2] = Vector3( -stageSize.x*0.5f, 0.0f, stageSize.x*0.5f);
  path.SetProperty( Path::Property::POINTS, points );

  Dali::Property::Array controlPoints;
  controlPoints.Resize(4);
  controlPoints[0] = Vector3( stageSize.x*0.5f, 0.0f, stageSize.x*0.3f );
  controlPoints[1] = Vector3( stageSize.x*0.3f, 0.0f, 0.0f );
  controlPoints[2] = Vector3(-stageSize.x*0.3f, 0.0f, 0.0f );
  controlPoints[3] = Vector3(-stageSize.x*0.5f, 0.0f,  stageSize.x*0.3f );
  path.SetProperty( Path::Property::CONTROL_POINTS, controlPoints );


  mScrollViewEffect = ScrollViewPagePathEffect::New(path,
                                                    Vector3(-1.0f,0.0f,0.0f),
                                                    Toolkit::ScrollView::Property::SCROLL_FINAL_X,
                                                    Vector3(stageSize.x*TABLE_RELATIVE_SIZE.x,stageSize.y*TABLE_RELATIVE_SIZE.y,0.0f),mTotalPages);
}

void DaliTableView::OnKeyEvent( const KeyEvent& event )
{
  if( event.state == KeyEvent::Down )
  {
    if ( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
      // If there's a Popup, Hide it if it's contributing to the display in any way (EG. transitioning in or out).
      // Otherwise quit.
      if ( mVersionPopup && ( mVersionPopup.GetDisplayState() != Toolkit::Popup::HIDDEN ) )
      {
        mVersionPopup.SetDisplayState( Popup::HIDDEN );
      }
      else
      {
        mApplication.Quit();
      }
    }
  }
}

void DaliTableView::SetupBackground( Actor bubbleContainer )
{
  // Create distance field shape.
  BufferImage distanceField;
  Size imageSize( 512, 512 );
  CreateShapeImage( CIRCLE, imageSize, distanceField );

  // Add bubbles to the bubbleContainer.
  // Note: The bubbleContainer is parented externally to this function.
  AddBackgroundActors( bubbleContainer, NUM_BACKGROUND_IMAGES, distanceField );
}

void DaliTableView::InitialiseBackgroundActors( Actor actor )
{
  // Delete current animations
  mBackgroundAnimations.clear();

  // Create new animations
  const Vector3 size = actor.GetTargetSize();

  for( unsigned int i = 0, childCount = actor.GetChildCount(); i < childCount; ++i )
  {
    Actor child = actor.GetChildAt( i );

    // Calculate a random position
    Vector3 childPos( Random::Range( -size.x * 0.5f * BACKGROUND_SPREAD_SCALE, size.x * 0.5f * BACKGROUND_SPREAD_SCALE ),
                      Random::Range( -size.y, size.y ),
                      Random::Range( BUBBLE_MIN_Z, BUBBLE_MAX_Z ) );

    child.SetPosition( childPos );

    // Define bubble horizontal parallax and vertical wrapping
    Constraint animConstraint = Constraint::New < Vector3 > ( child, Actor::Property::POSITION, AnimateBubbleConstraint( childPos, Random::Range( -0.85f, 0.25f ) ) );
    animConstraint.AddSource( Source( mScrollView, ScrollView::Property::SCROLL_POSITION ) );
    animConstraint.AddSource( Dali::ParentSource( Dali::Actor::Property::SIZE ) );
    animConstraint.AddSource( Dali::LocalSource( Dali::Actor::Property::SIZE ) );
    animConstraint.Apply();

    // Kickoff animation
    Animation animation = Animation::New( Random::Range( 40.0f, 80.0f ) );
    animation.AnimateBy( Property( child, Actor::Property::POSITION ), Vector3( 0.0f, -1.0f, 0.0f ), AlphaFunction::LINEAR );
    animation.SetLooping( true );
    animation.Play();
    mBackgroundAnimations.push_back( animation );
  }
}

void DaliTableView::AddBackgroundActors( Actor layer, int count, BufferImage distanceField )
{
  for( int i = 0; i < count; ++i )
  {
    float randSize = Random::Range( 10.0f, 400.0f );
    ImageView dfActor = ImageView::New( distanceField );
    dfActor.SetSize( Vector2( randSize, randSize ) );
    dfActor.SetParentOrigin( ParentOrigin::CENTER );

    Dali::Property::Map effect = Toolkit::CreateDistanceFieldEffect();
    dfActor.SetProperty( Toolkit::ImageView::Property::IMAGE, effect );
    dfActor.SetColor( BUBBLE_COLOR[ i%NUMBER_OF_BUBBLE_COLOR ] );
    layer.Add( dfActor );
  }

  // Positioning will occur when the layer is relaid out
  layer.OnRelayoutSignal().Connect( this, &DaliTableView::InitialiseBackgroundActors );
}

void DaliTableView::CreateShapeImage( ShapeType shapeType, const Size& size, BufferImage& distanceFieldOut )
{
  // this bitmap will hold the alpha map for the distance field shader
  distanceFieldOut = BufferImage::New( size.width, size.height, Pixel::A8 );

  // Generate bit pattern
  std::vector< unsigned char > imageDataA8;
  imageDataA8.reserve( size.width * size.height ); // A8

  switch( shapeType )
  {
    case CIRCLE:
      GenerateCircle( size, imageDataA8 );
      break;
    case SQUARE:
      GenerateSquare( size, imageDataA8 );
      break;
    default:
      break;
  }

  PixelBuffer* buffer = distanceFieldOut.GetBuffer();
  if( buffer )
  {
    GenerateDistanceFieldMap( &imageDataA8[ 0 ], size, buffer, size, 8.0f, size );
    distanceFieldOut.Update();
  }
}

void DaliTableView::GenerateSquare( const Size& size, std::vector< unsigned char >& distanceFieldOut )
{
  for( int h = 0; h < size.height; ++h )
  {
    for( int w = 0; w < size.width; ++w )
    {
      distanceFieldOut.push_back( 0xFF );
    }
  }
}

void DaliTableView::GenerateCircle( const Size& size, std::vector< unsigned char >& distanceFieldOut )
{
  const float radius = size.width * 0.5f * size.width * 0.5f;
  Vector2 center( size.width / 2, size.height / 2 );

  for( int h = 0; h < size.height; ++h )
  {
    for( int w = 0; w < size.width; ++w )
    {
      Vector2 pos( w, h );
      Vector2 dist = pos - center;

      if( dist.x * dist.x + dist.y * dist.y > radius )
      {
        distanceFieldOut.push_back( 0x00 );
      }
      else
      {
        distanceFieldOut.push_back( 0xFF );
      }
    }
  }
}

ImageView DaliTableView::CreateLogo( std::string imagePath )
{
  Image image = ResourceImage::New( imagePath );
  ImageView logo = ImageView::New( image );

  logo.SetAnchorPoint( AnchorPoint::CENTER );
  logo.SetParentOrigin( ParentOrigin::CENTER );

  return logo;
}

bool DaliTableView::PauseBackgroundAnimation()
{
  PauseAnimation();

  return false;
}

void DaliTableView::PauseAnimation()
{
  if( mBackgroundAnimsPlaying )
  {
    for( AnimationListIter animIter = mBackgroundAnimations.begin(); animIter != mBackgroundAnimations.end(); ++animIter )
    {
      Animation anim = *animIter;

      anim.Stop();
    }

    mBackgroundAnimsPlaying = false;
  }
}

void DaliTableView::PlayAnimation()
{
  if ( !mBackgroundAnimsPlaying )
  {
    for( AnimationListIter animIter = mBackgroundAnimations.begin(); animIter != mBackgroundAnimations.end(); ++animIter )
    {
      Animation anim = *animIter;

      anim.Play();
    }

    mBackgroundAnimsPlaying = true;
  }

  mAnimationTimer.SetInterval( BACKGROUND_ANIMATION_DURATION );
}

Dali::Actor DaliTableView::OnKeyboardPreFocusChange( Dali::Actor current, Dali::Actor proposed, Dali::Toolkit::Control::KeyboardFocus::Direction direction )
{
  Actor nextFocusActor = proposed;

  if ( !current && !proposed  )
  {
    // Set the initial focus to the first tile in the current page should be focused.
    nextFocusActor = mPages[mScrollView.GetCurrentPage()].GetChildAt(0);
  }
  else if( !proposed || (proposed && proposed == mScrollViewLayer) )
  {
    // ScrollView is being focused but nothing in the current page can be focused further
    // in the given direction. We should work out which page to scroll to next.
    int currentPage = mScrollView.GetCurrentPage();
    int newPage = currentPage;
    if( direction == Dali::Toolkit::Control::KeyboardFocus::LEFT )
    {
      newPage--;
    }
    else if( direction == Dali::Toolkit::Control::KeyboardFocus::RIGHT )
    {
      newPage++;
    }

    newPage = std::max(0, std::min(mTotalPages - 1, newPage));
    if( newPage == currentPage )
    {
      if( direction == Dali::Toolkit::Control::KeyboardFocus::LEFT )
      {
        newPage = mTotalPages - 1;
      } else if( direction == Dali::Toolkit::Control::KeyboardFocus::RIGHT )
      {
        newPage = 0;
      }
    }

    // Scroll to the page in the given direction
    mScrollView.ScrollTo(newPage);

    if( direction == Dali::Toolkit::Control::KeyboardFocus::LEFT )
    {
      // Work out the cell position for the last tile
      int remainingExamples = mExampleList.size() - newPage * EXAMPLES_PER_PAGE;
      int rowPos = (remainingExamples >= EXAMPLES_PER_PAGE) ? ROWS_PER_PAGE - 1 : ( (remainingExamples % EXAMPLES_PER_PAGE + EXAMPLES_PER_ROW) / EXAMPLES_PER_ROW - 1 );
      int colPos = remainingExamples >= EXAMPLES_PER_PAGE ? EXAMPLES_PER_ROW - 1 : ( remainingExamples % EXAMPLES_PER_PAGE - rowPos * EXAMPLES_PER_ROW - 1 );

      // Move the focus to the last tile in the new page.
      nextFocusActor = mPages[newPage].GetChildAt(rowPos * EXAMPLES_PER_ROW + colPos);
    }
    else
    {
      // Move the focus to the first tile in the new page.
      nextFocusActor = mPages[newPage].GetChildAt(0);
    }
  }

  return nextFocusActor;
}

void DaliTableView::OnFocusedActorActivated( Dali::Actor activatedActor )
{
  if(activatedActor)
  {
    mPressedActor = activatedActor;

    // Activate the current focused actor;
    TouchEvent touchEventUp;
    touchEventUp.points.push_back( TouchPoint ( 0, TouchPoint::Up, 0.0f, 0.0f ) );
    OnTilePressed(mPressedActor, touchEventUp);
  }
}

bool DaliTableView::OnTileHovered( Actor actor, const HoverEvent& event )
{
  KeyboardFocusManager::Get().SetCurrentFocusActor( actor );
  return true;
}

void DaliTableView::OnLogoTapped( Dali::Actor actor, const Dali::TapGesture& tap )
{
  // Only show if currently fully hidden. If transitioning-out, the transition will not be interrupted.
  if ( !mVersionPopup || ( mVersionPopup.GetDisplayState() == Toolkit::Popup::HIDDEN ) )
  {
    if ( !mVersionPopup )
    {
      std::ostringstream stream;
      stream << "DALi Core: "    << CORE_MAJOR_VERSION << "." << CORE_MINOR_VERSION << "." << CORE_MICRO_VERSION << std::endl << "(" << CORE_BUILD_DATE << ")\n";
      stream << "DALi Adaptor: " << ADAPTOR_MAJOR_VERSION << "." << ADAPTOR_MINOR_VERSION << "." << ADAPTOR_MICRO_VERSION << std::endl << "(" << ADAPTOR_BUILD_DATE << ")\n";
      stream << "DALi Toolkit: " << TOOLKIT_MAJOR_VERSION << "." << TOOLKIT_MINOR_VERSION << "." << TOOLKIT_MICRO_VERSION << std::endl << "(" << TOOLKIT_BUILD_DATE << ")\n";

      mVersionPopup = Dali::Toolkit::Popup::New();

      Toolkit::TextLabel titleActor = Toolkit::TextLabel::New( "Version information" );
      titleActor.SetName( "title-actor" );
      titleActor.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

      Toolkit::TextLabel contentActor = Toolkit::TextLabel::New( stream.str() );
      contentActor.SetName( "content-actor" );
      contentActor.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, true );
      contentActor.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
      contentActor.SetPadding( Padding( 0.0f, 0.0f, 20.0f, 0.0f ) );

      mVersionPopup.SetTitle( titleActor );
      mVersionPopup.SetContent( contentActor );

      mVersionPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::WIDTH );
      mVersionPopup.SetSizeModeFactor( Vector3( 0.75f, 1.0f, 1.0f ) );
      mVersionPopup.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );

      mVersionPopup.OutsideTouchedSignal().Connect( this, &DaliTableView::HideVersionPopup );
      Stage::GetCurrent().Add( mVersionPopup );
    }

    mVersionPopup.SetDisplayState( Popup::SHOWN );
  }
}

void DaliTableView::HideVersionPopup()
{
  // Only hide if currently fully shown. If transitioning-in, the transition will not be interrupted.
  if ( mVersionPopup && ( mVersionPopup.GetDisplayState() == Toolkit::Popup::SHOWN ) )
  {
    mVersionPopup.SetDisplayState( Popup::HIDDEN );
  }
}
