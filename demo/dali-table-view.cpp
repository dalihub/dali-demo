/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>

// INTERNAL INCLUDES
#include "shared/view.h"
#include "shared/utility.h"

using namespace Dali;
using namespace Dali::Toolkit;

///////////////////////////////////////////////////////////////////////////////

namespace
{

const std::string LOGO_PATH( DEMO_IMAGE_DIR "Logo-for-demo.png" );
const std::string TILE_BACKGROUND(DEMO_IMAGE_DIR "item-background.9.png");
const std::string TILE_BACKGROUND_ALPHA( DEMO_IMAGE_DIR "demo-tile-texture.9.png" );

const float TILE_LABEL_PADDING = 8.0f;                          ///< Border between edge of tile and the example text
const float BUTTON_PRESS_ANIMATION_TIME = 0.35f;                ///< Time to perform button scale effect.
const float ROTATE_ANIMATION_TIME = 0.5f;                       ///< Time to perform rotate effect.
const int MAX_PAGES = 256;                                      ///< Maximum pages (arbitrary safety limit)
const int EXAMPLES_PER_ROW = 3;
const int ROWS_PER_PAGE = 3;
const int EXAMPLES_PER_PAGE = EXAMPLES_PER_ROW * ROWS_PER_PAGE;
const float LOGO_MARGIN_RATIO = 0.1f / 0.3f;
const float BOTTOM_PADDING_RATIO = 0.4f / 0.9f;
const Vector3 SCROLLVIEW_RELATIVE_SIZE(0.9f, 1.0f, 0.8f );      ///< ScrollView's relative size to its parent
const Vector3 TABLE_RELATIVE_SIZE(0.95f, 0.9f, 0.8f );          ///< TableView's relative size to the entire stage. The Y value means sum of the logo and table relative heights.
const float STENCIL_RELATIVE_SIZE = 1.0f;

const float EFFECT_SNAP_DURATION = 0.66f;                       ///< Scroll Snap Duration for Effects
const float EFFECT_FLICK_DURATION = 0.5f;                       ///< Scroll Flick Duration for Effects
const Vector3 ANGLE_CUBE_PAGE_ROTATE(Math::PI * 0.5f, Math::PI * 0.5f, 0.0f);
const Vector4 TILE_COLOR( 0.4f, 0.6f, 0.9f, 0.6f );

const Vector4 BUBBLE_COLOR[] =
{
  Vector4( 0.3255f, 0.3412f, 0.6353f, 0.32f ),
  Vector4( 0.3647f, 0.7569f, 0.8157f, 0.32f ),
  Vector4( 0.3804f, 0.7412f, 0.6510f, 0.32f ),
  Vector4( 1.f, 1.f, 1.f, 0.13f )
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

// This shader takes a texture.
// An alpha discard is performed.
// The shader uses the tiles position within the scroll-view page and the scroll-views rotation position to create a parallax effect.
const char* FRAGMENT_SHADER_TEXTURED = DALI_COMPOSE_SHADER(
  varying mediump vec2  vTexCoord;
  varying mediump vec3  vIllumination;
  uniform lowp    vec4  uColor;
  uniform sampler2D     sTexture;
  uniform mediump vec3  uCustomPosition;

  void main()
  {
    if( texture2D( sTexture, vTexCoord ).a <= 0.0001 )
    {
      discard;
    }

    mediump vec2 wrapTexCoord = vec2( ( vTexCoord.x / 4.0 ) + ( uCustomPosition.x / 4.0 ) + ( uCustomPosition.z / 2.0 ), vTexCoord.y / 4.0 );
    mediump vec4 color = texture2D( sTexture, wrapTexCoord );
    mediump float positionWeight = ( uCustomPosition.y + 0.3 ) * color.r * 2.0;

    gl_FragColor = vec4( positionWeight, positionWeight, positionWeight, 0.9 ) * uColor + vec4( uColor.xyz, 0.0 );
  }
);

/**
 * Creates the background image
 */
Control CreateBackground( std::string stylename )
{
  Control background = Control::New();
  Stage::GetCurrent().Add( background );
  background.SetStyleName( stylename );
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

    // Wrap bubbles vertically.
    float range = parentSize.y + childSize.y;
    // This performs a float mod (we don't use fmod as we want the arithmetic modulus as opposed to the remainder).
    position.y -= range * ( floor( position.y / range ) + 0.5f );

    // Bubbles X position moves parallax to horizontal
    // panning by a scale factor unique to each bubble.
    position.x = mInitialX + ( inputs[0]->GetVector2().x * mScale );
  }

private:
  float mInitialX;
  float mScale;
};

/**
 * Constraint to precalculate values from the scroll-view
 * and tile positions to pass to the tile shader.
 */
struct TileShaderPositionConstraint
{
  TileShaderPositionConstraint( float pageWidth, float tileXOffset )
  : mPageWidth( pageWidth ),
    mTileXOffset( tileXOffset )
  {
  }

  void operator()( Vector3& position, const PropertyInputContainer& inputs )
  {
    // Set up position.x as the tiles X offset (0.0 -> 1.0).
    position.x = mTileXOffset;
    // Set up position.z as the linear scroll-view X offset (0.0 -> 1.0).
    position.z = 1.0f * ( -fmod( inputs[0]->GetVector2().x, mPageWidth ) / mPageWidth );
    // Set up position.y as a rectified version of the scroll-views X offset.
    // IE. instead of 0.0 -> 1.0, it moves between 0.0 -> 0.5 -> 0.0 within the same span.
    if( position.z > 0.5f )
    {
      position.y = 1.0f - position.z;
    }
    else
    {
      position.y = position.z;
    }
  }

private:
  float mPageWidth;
  float mTileXOffset;
};

bool CompareByTitle( const Example& lhs, const Example& rhs )
{
  return lhs.title < rhs.title;
}

} // namespace

DaliTableView::DaliTableView( Application& application )
: mApplication( application ),
  mRootActor(),
  mRotateAnimation(),
  mPressedAnimation(),
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
  mRootActor = CreateBackground( "LauncherBackground" );
  Stage::GetCurrent().Add( mRootActor );

  // Add logo
  ImageView logo = CreateLogo( LOGO_PATH );
  logo.SetName( "LOGO_IMAGE" );
  logo.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  logo.SetParentOrigin( Vector3( 0.5f, 0.1f, 0.5f ) );
  logo.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

  // Show version in a popup when log is tapped
  mLogoTapDetector = TapGestureDetector::New();
  mLogoTapDetector.Attach( logo );
  mLogoTapDetector.DetectedSignal().Connect( this, &DaliTableView::OnLogoTapped );

  // Scrollview occupying the majority of the screen
  mScrollView = ScrollView::New();
  mScrollView.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mScrollView.SetParentOrigin( Vector3( 0.5f, 1.0f - 0.05f, 0.5f ) );
  mScrollView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  mScrollView.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::HEIGHT );
  mScrollView.SetSizeModeFactor( Vector3( 0.0f, 0.6f, 0.0f ) );

  const float buttonsPageMargin = ( 1.0f - TABLE_RELATIVE_SIZE.x ) * 0.5f * stageSize.width;
  mScrollView.SetPadding( Padding( buttonsPageMargin, buttonsPageMargin, 0.0f, 0.0f ) );

  mScrollView.SetAxisAutoLock( true );
  mScrollView.ScrollCompletedSignal().Connect( this, &DaliTableView::OnScrollComplete );
  mScrollView.ScrollStartedSignal().Connect( this, &DaliTableView::OnScrollStart );
  mScrollView.TouchSignal().Connect( this, &DaliTableView::OnScrollTouched );

  mPageWidth = stageSize.width * TABLE_RELATIVE_SIZE.x * 0.5f;

  // Populate background and bubbles - needs to be scrollViewLayer so scroll ends show
  Actor bubbleContainer = Actor::New();
  bubbleContainer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  bubbleContainer.SetAnchorPoint( AnchorPoint::CENTER );
  bubbleContainer.SetParentOrigin( ParentOrigin::CENTER );
  SetupBackground( bubbleContainer );

  mRootActor.Add( logo );
  // We use depth index to bring the logo above the bubbles (as an alternative to creating actors).
  logo.GetRendererAt( 0 ).SetProperty( Renderer::Property::DEPTH_INDEX, 30000 );
  mRootActor.Add( bubbleContainer );
  mRootActor.Add( mScrollView );

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

          // Calculate the tiles relative position on the page (between 0 & 1 in each dimension).
          Vector2 position( static_cast<float>( column ) / ( EXAMPLES_PER_ROW - 1.0f ), static_cast<float>( row ) / ( EXAMPLES_PER_ROW - 1.0f ) );
          Actor tile = CreateTile( example.name, example.title, Vector3( tileParentMultiplier, tileParentMultiplier, 1.0f ), position );
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
  mScrollRulerX = new FixedRuler( mPageWidth );
  mScrollRulerY = new DefaultRuler();
  mScrollRulerX->SetDomain( RulerDomain( 0.0f, (mTotalPages+1) * stageSize.width * TABLE_RELATIVE_SIZE.x * 0.5f, true ) );
  mScrollRulerY->Disable();
  mScrollView.SetRulerX( mScrollRulerX );
  mScrollView.SetRulerY( mScrollRulerY );
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

Actor DaliTableView::CreateTile( const std::string& name, const std::string& title, const Dali::Vector3& sizeMultiplier, Vector2& position )
{
  Actor content = Actor::New();
  content.SetName( name );
  content.SetAnchorPoint( AnchorPoint::CENTER );
  content.SetParentOrigin( ParentOrigin::CENTER );
  content.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
  content.SetSizeModeFactor( sizeMultiplier );

  Toolkit::ImageView tileContent = ImageView::New();
  tileContent.SetParentOrigin( ParentOrigin::CENTER );
  tileContent.SetAnchorPoint( AnchorPoint::CENTER );
  tileContent.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

  // Add the image via the property first.
  tileContent.SetProperty( Toolkit::ImageView::Property::IMAGE, TILE_BACKGROUND_ALPHA );
  // Register a property with the ImageView. This allows us to inject the scroll-view position into the shader.
  Property::Value value = Vector3( 0.0f, 0.0f, 0.0f );
  Property::Index propertyIndex = tileContent.RegisterProperty( "uCustomPosition", value );

  // Add a shader to the image (details in shader source).
  Property::Map map;
  Property::Map customShader;
  customShader[ Visual::Shader::Property::FRAGMENT_SHADER ] = FRAGMENT_SHADER_TEXTURED;
  map[ Visual::Property::SHADER ] = customShader;
  tileContent.SetProperty( Toolkit::ImageView::Property::IMAGE, map );
  tileContent.SetColor( TILE_COLOR );

  // We create a constraint to perform a precalculation on the scroll-view X offset
  // and pass it to the shader uniform, along with the tile's position.
  Constraint shaderPosition = Constraint::New < Vector3 > ( tileContent, propertyIndex, TileShaderPositionConstraint( mPageWidth, position.x ) );
  shaderPosition.AddSource( Source( mScrollView, ScrollView::Property::SCROLL_POSITION ) );
  shaderPosition.SetRemoveAction( Constraint::Discard );
  shaderPosition.Apply();
  content.Add( tileContent );

  // Create an ImageView for the 9-patch border around the tile.
  ImageView image = ImageView::New( TILE_BACKGROUND );
  image.SetAnchorPoint( AnchorPoint::CENTER );
  image.SetParentOrigin( ParentOrigin::CENTER );
  image.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  image.SetOpacity( 0.8f );
  tileContent.Add( image );

  TextLabel label = TextLabel::New();
  label.SetAnchorPoint( AnchorPoint::CENTER );
  label.SetParentOrigin( ParentOrigin::CENTER );
  label.SetStyleName( "LauncherLabel" );
  label.SetProperty( TextLabel::Property::MULTI_LINE, true );
  label.SetProperty( TextLabel::Property::TEXT, title );
  label.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  label.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
  label.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );

  // Pad around the label as its size is the same as the 9-patch border. It will overlap it without padding.
  label.SetPadding( Padding( TILE_LABEL_PADDING, TILE_LABEL_PADDING, TILE_LABEL_PADDING, TILE_LABEL_PADDING ) );
  content.Add( label );

  // Set the tile to be keyboard focusable
  content.SetKeyboardFocusable(true);

  // connect to the touch events
  content.TouchSignal().Connect( this, &DaliTableView::OnTilePressed );
  content.HoveredSignal().Connect( this, &DaliTableView::OnTileHovered );

  return content;
}

bool DaliTableView::OnTilePressed( Actor actor, const TouchData& event )
{
  return DoTilePress( actor, event.GetState( 0 ) );
}

bool DaliTableView::DoTilePress( Actor actor, PointState::Type pointState )
{
  bool consumed = false;

  if( PointState::DOWN == pointState )
  {
    mPressedActor = actor;
    consumed = true;
  }

  // A button press is only valid if the Down & Up events
  // both occurred within the button.
  if( ( PointState::UP == pointState ) &&
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
      mPressedAnimation.AnimateTo( Property( content, Actor::Property::SCALE ), Vector3( 0.7f, 0.7f, 1.0f ), AlphaFunction::EASE_IN_OUT,
                                 TimePeriod( 0.0f, BUTTON_PRESS_ANIMATION_TIME * 0.5f ) );
      mPressedAnimation.AnimateTo( Property( content, Actor::Property::SCALE ), Vector3::ONE, AlphaFunction::EASE_IN_OUT,
                                 TimePeriod( BUTTON_PRESS_ANIMATION_TIME * 0.5f, BUTTON_PRESS_ANIMATION_TIME * 0.5f ) );

      // Rotate button on the Y axis when pressed.
      mPressedAnimation.AnimateBy( Property( content, Actor::Property::ORIENTATION ), Quaternion( Degree( 0.0f ), Degree( 180.0f ), Degree( 0.0f ) ) );

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
    stream << DEMO_EXAMPLE_BIN << name.c_str();
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

bool DaliTableView::OnScrollTouched( Actor actor, const TouchData& event )
{
  if( PointState::DOWN == event.GetState( 0 ) )
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
  // Create distance field shapes.
  BufferImage distanceFields[2];
  Size imageSize( 512, 512 );

  CreateShapeImage( CIRCLE, imageSize, distanceFields[0] );
  CreateShapeImage( BUBBLE, imageSize, distanceFields[1] );

  // Add bubbles to the bubbleContainer.
  // Note: The bubbleContainer is parented externally to this function.
  AddBackgroundActors( bubbleContainer, NUM_BACKGROUND_IMAGES, distanceFields );
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
    Vector3 childPos( Random::Range( -size.x * 0.5f * BACKGROUND_SPREAD_SCALE, size.x * 0.85f * BACKGROUND_SPREAD_SCALE ),
                      Random::Range( -size.y, size.y ),
                      Random::Range( BUBBLE_MIN_Z, BUBBLE_MAX_Z ) );

    child.SetPosition( childPos );

    // Define bubble horizontal parallax and vertical wrapping
    Constraint animConstraint = Constraint::New < Vector3 > ( child, Actor::Property::POSITION, AnimateBubbleConstraint( childPos, Random::Range( -0.85f, 0.25f ) ) );
    animConstraint.AddSource( Source( mScrollView, ScrollView::Property::SCROLL_POSITION ) );
    animConstraint.AddSource( Dali::ParentSource( Dali::Actor::Property::SIZE ) );
    animConstraint.AddSource( Dali::LocalSource( Dali::Actor::Property::SIZE ) );
    animConstraint.SetRemoveAction( Constraint::Discard );
    animConstraint.Apply();

    // Kickoff animation
    Animation animation = Animation::New( Random::Range( 30.0f, 160.0f ) );
    animation.AnimateBy( Property( child, Actor::Property::POSITION ), Vector3( 0.0f, -2000.0f, 0.0f ), AlphaFunction::LINEAR );
    animation.SetLooping( true );
    animation.Play();
    mBackgroundAnimations.push_back( animation );
  }
}

void DaliTableView::AddBackgroundActors( Actor layer, int count, BufferImage* distanceField )
{
  for( int i = 0; i < count; ++i )
  {
    float randSize = Random::Range( 10.0f, 400.0f );
    int distanceFieldType = static_cast<int>( Random::Range( 0.0f, 1.0f ) + 0.5f );
    ImageView dfActor = ImageView::New( distanceField[ distanceFieldType ] );
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
    case BUBBLE:
      GenerateCircle( size, imageDataA8, true );
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

void DaliTableView::GenerateCircle( const Size& size, std::vector< unsigned char >& distanceFieldOut, bool hollow )
{
  const float radius = size.width * 0.5f * size.width * 0.5f;
  Vector2 center( size.width / 2, size.height / 2 );

  for( int h = 0; h < size.height; ++h )
  {
    for( int w = 0; w < size.width; ++w )
    {
      Vector2 pos( w, h );
      Vector2 dist = pos - center;

      float distance = ( dist.x * dist.x ) + ( dist.y * dist.y );

      // If hollow, check the distance against a min & max value, otherwise just use the max value.
      unsigned char fillByte = ( hollow ? ( ( distance <= radius ) && ( distance > ( radius * 0.7f ) ) ) : ( distance <= radius ) ) ? 0xFF : 0x00;

      distanceFieldOut.push_back( fillByte );
    }
  }
}

ImageView DaliTableView::CreateLogo( std::string imagePath )
{
  ImageView logo = ImageView::New( imagePath );

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

  if( !current && !proposed  )
  {
    // Set the initial focus to the first tile in the current page should be focused.
    nextFocusActor = mPages[mScrollView.GetCurrentPage()].GetChildAt(0);
  }
  else if( !proposed )
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
    DoTilePress( mPressedActor, PointState::UP );
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
      titleActor.SetName( "titleActor" );
      titleActor.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

      Toolkit::TextLabel contentActor = Toolkit::TextLabel::New( stream.str() );
      contentActor.SetName( "contentActor" );
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
