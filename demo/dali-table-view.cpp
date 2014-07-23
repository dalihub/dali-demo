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

// CLASS HEADER
#include "dali-table-view.h"
#include "examples/shared/view.h"

// EXTERNAL INCLUDES
#include <algorithm>
#include <sstream>
#include<unistd.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

namespace
{

const std::string BUTTON_BACKWARD( "Backward" );
const std::string BUTTON_FORWARD( "Forward" );
const std::string BUTTON_QUIT( "Quit" );
const std::string BUTTON_OK( "Ok" );
const std::string BUTTON_CANCEL( "Cancel" );

const std::string DEFAULT_BACKGROUND_IMAGE_PATH( DALI_IMAGE_DIR "background-gradient.jpg" );
const std::string LOGO_PATH( DALI_IMAGE_DIR "dali-logo.png" );
const std::string DEFAULT_TOOLBAR_IMAGE_PATH( DALI_IMAGE_DIR "top-bar.png" );
const std::string BUTTON_BACKGROUND(DALI_IMAGE_DIR "button-background.png");
const std::string TILE_BACKGROUND(DALI_IMAGE_DIR "item-background.png");
const std::string TILE_BACKGROUND_ALPHA(DALI_IMAGE_DIR "item-background-alpha.png");

const char * const DEFAULT_TOOLBAR_TEXT( "TOUCH TO LAUNCH EXAMPLE" );

const float BUTTON_PRESS_ANIMATION_TIME = 0.25f;                ///< Time to perform button scale effect.
const float ROTATE_ANIMATION_TIME = 0.5f;                       ///< Time to perform rotate effect.
const int MAX_PAGES = 256;                                      ///< Maximum pages (arbitrary safety limit)
const int EXAMPLES_PER_ROW = 3;
const int ROWS_PER_PAGE = 3;
const int EXAMPLES_PER_PAGE = EXAMPLES_PER_ROW * ROWS_PER_PAGE;
const float TOP_ROW_HEIGHT = 35.0f;
const float BOTTOM_ROW_HEIGHT = 35.0f;
const int BOTTOM_PADDING_HEIGHT = 40;
const int LOGO_BOTTOM_PADDING_HEIGHT = 30;
const Vector3 TABLE_RELATIVE_SIZE(0.9f, 1.0f, 0.8f );          ///< TableView's relative size to the entire stage.
const float STENCIL_RELATIVE_SIZE = 1.0f;

const float EFFECT_SNAP_DURATION = 0.66f;                       ///< Scroll Snap Duration for Effects
const float EFFECT_FLICK_DURATION = 0.5f;                       ///< Scroll Flick Duration for Effects
const Vector3 ANGLE_CUBE_PAGE_ROTATE(Math::PI * 0.5f, Math::PI * 0.5f, 0.0f);

const int NUM_BACKGROUND_IMAGES = 20;
const float BACKGROUND_SWIPE_SCALE = 0.025f;
const float BACKGROUND_SPREAD_SCALE = 1.5f;
const float SCALE_MOD = 1000.0f * Math::PI * 2.0f;
const float SCALE_SPEED = 10.0f;
const float SCALE_SPEED_SIN = 0.1f;

const unsigned int BACKGROUND_ANIMATION_DURATION = 15000; // 15 secs

const float BACKGROUND_Z = -1000.0f;
const float BACKGROUND_SIZE_SCALE = 2.0f;
const Vector4 BACKGROUND_COLOR( 1.0f, 1.0f, 1.0f, 1.0f );


const std::string             DEFAULT_TEXT_STYLE_FONT_FAMILY("HelveticaNeue");
const std::string             DEFAULT_TEXT_STYLE_FONT_STYLE("Regular");
const Dali::PointSize         DEFAULT_TEXT_STYLE_POINT_SIZE( 8.0f );
const Dali::TextStyle::Weight DEFAULT_TEXT_STYLE_WEIGHT(Dali::TextStyle::REGULAR);
const Dali::Vector4           DEFAULT_TEXT_STYLE_COLOR(0.7f, 0.7f, 0.7f, 1.0f);

const std::string             TABLE_TEXT_STYLE_FONT_FAMILY("HelveticaNeue");
const std::string             TABLE_TEXT_STYLE_FONT_STYLE("Regular");
const Dali::PointSize         TABLE_TEXT_STYLE_POINT_SIZE( 8.0f );
const Dali::TextStyle::Weight TABLE_TEXT_STYLE_WEIGHT(Dali::TextStyle::LIGHT);
const Dali::Vector4           TABLE_TEXT_STYLE_COLOR(0.0f, 0.0f, 0.0f, 1.0f);


TextStyle GetDefaultTextStyle()
{
  TextStyle textStyle;
  textStyle.SetFontName(DEFAULT_TEXT_STYLE_FONT_FAMILY);
  textStyle.SetFontStyle(DEFAULT_TEXT_STYLE_FONT_STYLE);
  textStyle.SetFontPointSize( Dali::PointSize(DemoHelper::ScalePointSize(DEFAULT_TEXT_STYLE_POINT_SIZE)));
  textStyle.SetWeight(DEFAULT_TEXT_STYLE_WEIGHT);
  textStyle.SetTextColor(DEFAULT_TEXT_STYLE_COLOR);
  textStyle.SetShadow( true );
  return textStyle;
}

TextStyle GetTableTextStyle()
{
  TextStyle textStyle;
  textStyle.SetFontName(TABLE_TEXT_STYLE_FONT_FAMILY);
  textStyle.SetFontStyle(TABLE_TEXT_STYLE_FONT_STYLE);
  textStyle.SetFontPointSize( Dali::PointSize(DemoHelper::ScalePointSize(TABLE_TEXT_STYLE_POINT_SIZE)));
  textStyle.SetWeight(TABLE_TEXT_STYLE_WEIGHT);
  textStyle.SetTextColor(TABLE_TEXT_STYLE_COLOR);
  return textStyle;
}

/**
 * Creates the background image
 */
ImageActor CreateBackground( std::string imagePath )
{
  Image image = Image::New( imagePath );
  ImageActor background = ImageActor::New( image );

  background.SetAnchorPoint( AnchorPoint::CENTER );
  background.SetParentOrigin( ParentOrigin::CENTER );
  background.SetZ( -1.0f );

  return background;
}

// These values depend on the tile image
const float IMAGE_BORDER_LEFT = 11.0f;
const float IMAGE_BORDER_RIGHT = IMAGE_BORDER_LEFT;
const float IMAGE_BORDER_TOP = IMAGE_BORDER_LEFT;
const float IMAGE_BORDER_BOTTOM = IMAGE_BORDER_LEFT;

/**
 * TableViewVisibilityConstraint
 */
struct TableViewVisibilityConstraint
{
  bool operator()( const bool& current,
              const PropertyInput& pagePositionProperty,
              const PropertyInput& pageSizeProperty )
  {
    // Only the tableview in the current page should be visible.
    const Vector3& pagePosition = pagePositionProperty.GetVector3();
    const Vector3& pageSize = pageSizeProperty.GetVector3();
    return fabsf( pagePosition.x ) < pageSize.x;
  }
};

/**
 * Constraint to wrap an actor in y that is moving vertically
 */
Vector3 ShapeMovementConstraint( const Vector3& current,
                         const PropertyInput& shapeSizeProperty,
                         const PropertyInput& parentSizeProperty )
{
  const Vector3& shapeSize = shapeSizeProperty.GetVector3();
  const Vector3& parentSize = parentSizeProperty.GetVector3();

  Vector3 pos( current );
  if( pos.y + shapeSize.y * 0.5f < -parentSize.y * 0.5f )
  {
    pos.y += parentSize.y + shapeSize.y;
  }

  return pos;
}

/**
 * Constraint to return a bool value based on the alpha channel value
 */
bool AlphaVisibleConstraint( bool current, const PropertyInput& alphaProperty )
{
  Vector4 colour = alphaProperty.GetVector4();
  return ( colour.a > Math::MACHINE_EPSILON_10000 );
}

/**
 * Constraint to return a position for the background based on the scroll value
 */
struct AnimScrollConstraint
{
public:

  AnimScrollConstraint( const Vector3& initialPos, float scale )
      : mInitialPos( initialPos ),
        mScale( scale )
  {

  }

  Vector3 operator()( const Vector3& current, const PropertyInput& scrollProperty )
  {
    float scrollPos = scrollProperty.GetVector3().x;

    return mInitialPos + Vector3( -scrollPos * mScale, 0.0f, 0.0f );
  }

private:
  Vector3 mInitialPos;
  float mScale;
};

/**
 * Constraint to return a tracked world position added to the constant local position
 */
struct TranslateLocalConstraint
{
public:

  TranslateLocalConstraint( const Vector3& localPos )
      : mLocalPos( localPos )
  {
  }

  Vector3 operator()( const Vector3& current, const PropertyInput& pagePosProperty )
  {
    Vector3 worldPos = pagePosProperty.GetVector3();

    return ( worldPos + mLocalPos );
  }

private:
  Vector3 mLocalPos;
};


bool CompareByTitle( const Example& lhs, const Example& rhs )
{
  return lhs.title < rhs.title;
}

} // namespace

DaliTableView::DaliTableView( Application& application )
    : mApplication( application ),
        mScrolling( false ),
        mBackgroundImagePath( DEFAULT_BACKGROUND_IMAGE_PATH ),
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
  mExampleMap[ example.name ] = example;
}

void DaliTableView::SetBackgroundPath( std::string imagePath )
{
  mBackgroundImagePath = imagePath;
}

void DaliTableView::SortAlphabetically( bool sortAlphabetically )
{
  mSortAlphabetically = sortAlphabetically;
}

void DaliTableView::Initialize( Application& application )
{
  Stage::GetCurrent().KeyEventSignal().Connect( this, &DaliTableView::OnKeyEvent );

  Vector2 stageSize = Stage::GetCurrent().GetSize();

  // Background
  mBackground = CreateBackground( mBackgroundImagePath );
  // set same size as parent actor
  mBackground.SetSize( stageSize );
  Stage::GetCurrent().Add( mBackground );

  // Render entire content as overlays, as is all on same 2D plane.
  mRootActor = TableView::New( 4, 1 );
  mRootActor.SetAnchorPoint( AnchorPoint::CENTER );
  mRootActor.SetParentOrigin( ParentOrigin::CENTER );
  mRootActor.SetFixedHeight( 3, BOTTOM_PADDING_HEIGHT );
  Stage::GetCurrent().Add( mRootActor );

  // Toolbar at top
  Dali::Toolkit::ToolBar toolbar;
  Dali::Layer toolBarLayer = DemoHelper::CreateToolbar(toolbar,
                                                       DEFAULT_TOOLBAR_IMAGE_PATH,
                                                       DEFAULT_TOOLBAR_TEXT,
                                                       DemoHelper::DEFAULT_VIEW_STYLE,
                                                       DemoHelper::GetDefaultTextStyle());

  mRootActor.AddChild( toolBarLayer, TableView::CellPosition( 0, 0 ) );
  mRootActor.SetFixedHeight( 0, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarHeight );

  // Add logo
  mLogo = CreateLogo( LOGO_PATH );
  Alignment alignment = Alignment::New();
  alignment.Add(mLogo);
  mRootActor.AddChild( alignment, TableView::CellPosition( 1, 0 ) );

  // scrollview occupying the majority of the screen
  mScrollView = ScrollView::New();

  mScrollView.SetAnchorPoint( AnchorPoint::CENTER );
  mScrollView.SetParentOrigin( ParentOrigin::CENTER );
  mScrollView.ApplyConstraint( Dali::Constraint::New<Dali::Vector3>( Dali::Actor::SIZE, Dali::ParentSource( Dali::Actor::SIZE ), Dali::RelativeToConstraint( TABLE_RELATIVE_SIZE ) ) );
  mScrollView.SetAxisAutoLock( true );
  mScrollView.ScrollCompletedSignal().Connect( this, &DaliTableView::OnScrollComplete );
  mScrollView.ScrollStartedSignal().Connect( this, &DaliTableView::OnScrollStart );
  mScrollView.TouchedSignal().Connect( this, &DaliTableView::OnScrollTouched );

  mScrollViewLayer = Layer::New();
  mScrollViewLayer.SetAnchorPoint( AnchorPoint::CENTER );
  mScrollViewLayer.SetParentOrigin( ParentOrigin::CENTER );
  mScrollViewLayer.SetSize( stageSize );
  mScrollViewLayer.Add( mScrollView );
  mRootActor.AddChild( mScrollViewLayer, TableView::CellPosition( 2, 0 ) );

  // Setup the scenegraph
  // 1) Add scroll view effect and setup constraints on pages
  ApplyScrollViewEffect();

  // 2) Add pages and tiles
  Populate();

  // 3) Populate scrollview with background so constraints on background layers can work with scrollview
  SetupBackground( mScrollView, stageSize );

  // 4) Remove constraints for inner cube effect
  for( TableViewListIter pageIter = mTableViewList.begin(); pageIter != mTableViewList.end(); ++pageIter )
  {
    TableView page = *pageIter;

    unsigned int numChildren = page.GetChildCount();
    Actor pageActor = page;
    for( unsigned int i=0; i<numChildren; ++i)
    {
      // Remove old effect's manual constraints.
      Actor child = pageActor.GetChildAt(i);
      if( child )
      {
        child.RemoveConstraints();
      }
    }
  }

  // Set initial orientation
  unsigned int degrees = application.GetOrientation().GetDegrees();
  Rotate( degrees );

  Dali::Window winHandle = application.GetWindow();
  winHandle.AddAvailableOrientation( Dali::Window::PORTRAIT );
  winHandle.RemoveAvailableOrientation( Dali::Window::LANDSCAPE );
  winHandle.AddAvailableOrientation( Dali::Window::PORTRAIT_INVERSE );
  winHandle.RemoveAvailableOrientation( Dali::Window::LANDSCAPE_INVERSE );

  Dali::Orientation orientation = winHandle.GetOrientation();
  orientation.ChangedSignal().Connect( this, &DaliTableView::OrientationChanged );

  winHandle.ShowIndicator( false );

  //
  mAnimationTimer = Timer::New( BACKGROUND_ANIMATION_DURATION );
  mAnimationTimer.TickSignal().Connect( this, &DaliTableView::PauseBackgroundAnimation );
  mAnimationTimer.Start();
  mBackgroundAnimsPlaying = true;

  KeyboardFocusManager::Get().PreFocusChangeSignal().Connect( this, &DaliTableView::OnKeyboardPreFocusChange );
  KeyboardFocusManager::Get().FocusedActorActivatedSignal().Connect( this, &DaliTableView::OnFocusedActorActivated );
}

void DaliTableView::Populate()
{
  const Vector2 stageSize = Stage::GetCurrent().GetSize();

  const Size demoTileSize( 0.25f * stageSize.width, 0.25f * stageSize.height );

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
      // Create Table. (contains up to 9 Examples)
      TableView tableView = TableView::New( 4, 3 );
      // Add tableView to container.
      mScrollView.Add( tableView );
      ApplyEffectToPage( tableView, TABLE_RELATIVE_SIZE );

      tableView.SetAnchorPoint( AnchorPoint::CENTER );
      tableView.SetParentOrigin( ParentOrigin::CENTER );
      // 2 pixels of padding
      tableView.SetCellPadding( Size( 2.0f, 2.0f ) );

      Constraint constraint = Constraint::New<Vector3>( Actor::SCALE,
                                                        LocalSource( Actor::SIZE ),
                                                        ParentSource( Actor::SIZE ),
                                                        ScaleToFitConstraint() );
      tableView.ApplyConstraint(constraint);

      // Apply visibility constraint to table view
      Constraint visibleConstraint = Constraint::New< bool >( Actor::VISIBLE,
                                                              LocalSource( Actor::POSITION ),
                                                              ParentSource( Actor::SIZE ),
                                                              TableViewVisibilityConstraint() );
      visibleConstraint.SetRemoveAction( Constraint::Discard );
      tableView.ApplyConstraint( visibleConstraint );

      // add cells to table
      for( int y = 0; y < ROWS_PER_PAGE; y++ )
      {
        for( int x = 0; x < EXAMPLES_PER_ROW; x++ )
        {
          const Example& example = ( *iter );

          Actor tile = CreateTile( example.name, example.title, demoTileSize, true );
          FocusManager focusManager = FocusManager::Get();
          focusManager.SetFocusOrder( tile, ++exampleCount );
          focusManager.SetAccessibilityAttribute( tile, Dali::Toolkit::FocusManager::ACCESSIBILITY_LABEL,
                                                  example.title );
          focusManager.SetAccessibilityAttribute( tile, Dali::Toolkit::FocusManager::ACCESSIBILITY_TRAIT, "Tile" );
          focusManager.SetAccessibilityAttribute( tile, Dali::Toolkit::FocusManager::ACCESSIBILITY_HINT,
                                                  "You can run this example" );

          tableView.AddChild( tile, TableView::CellPosition( y, x ) );
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

      // last row is thin.
      tableView.SetFixedHeight( 3, BOTTOM_ROW_HEIGHT );

      std::stringstream out;
      out << ( t + 1 ) << " of " << mTotalPages;
      Actor pageNumberText = CreateTile( "", out.str(), Size( 0.8f * stageSize.width, BOTTOM_ROW_HEIGHT ), false );

      pageNumberText.ApplyConstraint( Constraint::New< Vector3 >( Actor::POSITION, Source( tableView, Actor::WORLD_POSITION),
                                                                   TranslateLocalConstraint( Vector3( 0.0f, stageSize.y * 0.4f, 0.0f ) ) ) );
      pageNumberText.ApplyConstraint( Constraint::New< Quaternion >( Actor::ROTATION, Source( tableView, Actor::WORLD_ROTATION ), EqualToConstraint() ) );
      pageNumberText.ApplyConstraint( Constraint::New< Vector4 >( Actor::COLOR, Source( tableView, Actor::COLOR ), EqualToConstraint() ) );

      //Stage::GetCurrent().Add( pageNumberText );

      // Set tableview position
      Vector3 tableViewPos( stageSize.x * TABLE_RELATIVE_SIZE.x * t, 0.0f, 0.0f );
      tableView.SetPosition( tableViewPos );

      mTableViewList.push_back( tableView );

      if( iter == mExampleList.end() )
      {
        break;
      }
    }
  }

  // Update Ruler info.
  mScrollRulerX = new FixedRuler( stageSize.width * TABLE_RELATIVE_SIZE.x );
  mScrollRulerY = new DefaultRuler();
  mScrollRulerX->SetDomain( RulerDomain( 0.0f, mTotalPages * stageSize.width * TABLE_RELATIVE_SIZE.x, true ) );
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
  mRotateAnimation.RotateTo( mRootActor, Degree( 360 - degrees ), Vector3::ZAXIS, AlphaFunctions::EaseOut );
  mRotateAnimation.Resize( mRootActor, targetSize, AlphaFunctions::EaseOut );
  mRotateAnimation.Play();
}

Actor DaliTableView::CreateTile( const string& name, const string& title, const Size& parentSize, bool addBackground )
{
  Actor tile = Actor::New();
  tile.SetName( name );
  tile.SetAnchorPoint( AnchorPoint::CENTER );
  tile.SetParentOrigin( ParentOrigin::CENTER );

  // make the tile 100% of parent
  tile.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );

  Actor content = Actor::New();
  content.SetAnchorPoint( AnchorPoint::CENTER );
  content.SetParentOrigin( ParentOrigin::CENTER );
  content.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  tile.Add(content);

  // create background image
  if( addBackground )
  {
    Image bg = Image::New( TILE_BACKGROUND );
    ImageActor image = ImageActor::New( bg );
    image.SetAnchorPoint( AnchorPoint::CENTER );
    image.SetParentOrigin( ParentOrigin::CENTER );
    // make the image 100% of tile
    image.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    // move image back to get text appear in front
    image.SetZ( -1 );
    image.SetStyle( ImageActor::STYLE_NINE_PATCH );
    image.SetNinePatchBorder( Vector4( IMAGE_BORDER_LEFT, IMAGE_BORDER_TOP, IMAGE_BORDER_RIGHT, IMAGE_BORDER_BOTTOM ) );

    content.Add( image );

    // Add stencil
    ImageActor stencil = NewStencilImage();
    stencil.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    image.Add( stencil );
  }

  TextView text = TextView::New( title );
  text.SetAnchorPoint( AnchorPoint::CENTER );
  text.SetParentOrigin( ParentOrigin::CENTER );
  text.SetWidthExceedPolicy( Toolkit::TextView::ShrinkToFit );
  text.SetMultilinePolicy( Toolkit::TextView::SplitByWord );
  text.SetLineJustification( Toolkit::TextView::Center );
  text.SetTextAlignment( Toolkit::Alignment::Type( Alignment::HorizontalCenter | Alignment::VerticalCenter ) );
  text.SetColor( Color::WHITE );
  text.SetZ( 1 );
  // make the text 90% of tile
  text.SetSize( 0.9f * parentSize.width, 0.9f * parentSize.height );
  text.SetStyleToCurrentText( GetTableTextStyle() );
  text.SetSnapshotModeEnabled( false );
  content.Add( text );

  // Set the tile to be keyboard focusable
  tile.SetKeyboardFocusable(true);

  // connect to the touch events
  tile.TouchedSignal().Connect( this, &DaliTableView::OnTilePressed );

  return tile;
}

ImageActor DaliTableView::NewStencilImage()
{
  Image alpha = Image::New( TILE_BACKGROUND_ALPHA );

  ImageActor stencilActor = ImageActor::New( alpha );
  stencilActor.SetStyle( ImageActor::STYLE_NINE_PATCH );
  stencilActor.SetNinePatchBorder( Vector4( IMAGE_BORDER_LEFT, IMAGE_BORDER_TOP, IMAGE_BORDER_RIGHT, IMAGE_BORDER_BOTTOM ) );

  stencilActor.SetParentOrigin( ParentOrigin::CENTER );
  stencilActor.SetAnchorPoint( AnchorPoint::CENTER );
  stencilActor.SetDrawMode( DrawMode::STENCIL );

  Dali::ShaderEffect shaderEffect = AlphaDiscardEffect::New();
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
    std::string name = actor.GetName();
    ExampleMapConstIter iter = mExampleMap.find( name );

    FocusManager focusManager = FocusManager::Get();

    if( iter != mExampleMap.end() )
    {
      // ignore Example button presses when scrolling or button animating.
      if( ( !mScrolling ) && ( !mPressedAnimation ) )
      {
        // do nothing, until pressed animation finished.
        consumed = true;
      }
    }

    if( consumed )
    {
      mPressedAnimation = Animation::New( BUTTON_PRESS_ANIMATION_TIME );
      mPressedAnimation.SetDestroyAction( Animation::Discard );

      // scale the content actor within the Tile, as to not affect the placement within the Table.
      Actor content = actor.GetChildAt(0);
      mPressedAnimation.ScaleTo( content, Vector3( 0.9f, 0.9f, 1.0f ), AlphaFunctions::EaseInOut, 0.0f,
                                 BUTTON_PRESS_ANIMATION_TIME * 0.5f );
      mPressedAnimation.ScaleTo( content, Vector3::ONE, AlphaFunctions::EaseInOut, BUTTON_PRESS_ANIMATION_TIME * 0.5f,
                                 BUTTON_PRESS_ANIMATION_TIME * 0.5f );
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
    ExampleMapConstIter iter = mExampleMap.find( name );

    if( iter == mExampleMap.end() )
    {
      if( name == BUTTON_QUIT )
      {
        // Move focus to the OK button
        FocusManager focusManager = FocusManager::Get();

        // Enable the group mode and wrap mode
        focusManager.SetGroupMode( true );
        focusManager.SetWrapMode( true );
      }
    }
    else
    {
      const Example& example( iter->second );

      std::stringstream stream;
      stream << DALI_EXAMPLE_BIN << example.name.c_str();
      pid_t pid = fork();
      if( pid == 0)
      {
        execlp( stream.str().c_str(), example.name.c_str(), NULL );
        DALI_ASSERT_ALWAYS(false && "exec failed!");
      }
    }
    mPressedActor.Reset();
  }
}

void DaliTableView::OnScrollStart( const Dali::Vector3& position )
{
  mScrolling = true;

  PlayAnimation();
}

void DaliTableView::OnScrollComplete( const Dali::Vector3& position )
{
  mScrolling = false;

  // move focus to 1st item of new page
  FocusManager focusManager = FocusManager::Get();
  focusManager.SetCurrentFocusActor(mTableViewList[mScrollView.GetCurrentPage()].GetChildAt(TableView::CellPosition(1, 0)) );

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
  ScrollViewCustomEffect customEffect;
  mScrollViewEffect = customEffect = ScrollViewCustomEffect::New();
  mScrollView.SetScrollSnapDuration( EFFECT_SNAP_DURATION );
  mScrollView.SetScrollFlickDuration( EFFECT_FLICK_DURATION );
  mScrollView.SetScrollSnapAlphaFunction( AlphaFunctions::EaseOutBack );
  mScrollView.SetScrollFlickAlphaFunction( AlphaFunctions::EaseOutBack );
  mScrollView.RemoveConstraintsFromChildren();

  customEffect.SetPageSpacing( Vector2( 30.0f, 30.0f ) );
  customEffect.SetAngledOriginPageRotation( ANGLE_CUBE_PAGE_ROTATE );
  customEffect.SetSwingAngle( ANGLE_CUBE_PAGE_ROTATE.x, Vector3( 0, -1, 0 ) );
  customEffect.SetOpacityThreshold( 0.5f );   // Make fade out on edges
}

void DaliTableView::ApplyEffectToPage( Actor page, const Vector3& tableRelativeSize )
{
  page.RemoveConstraints();

  Constraint constraint = Constraint::New<Vector3>( Actor::SCALE,
                                                    LocalSource( Actor::SIZE ),
                                                    ParentSource( Actor::SIZE ),
                                                    ScaleToFitConstraint() );
  page.ApplyConstraint(constraint);

  ApplyCustomEffectToPage( page );
}

void DaliTableView::ApplyCustomEffectToPage( Actor page )
{
  ScrollViewCustomEffect customEffect = ScrollViewCustomEffect::DownCast( mScrollViewEffect );
  Vector2 vStageSize( Stage::GetCurrent().GetSize() );
  customEffect.ApplyToPage( page, Vector3( vStageSize.x, vStageSize.y, 1.0f ) );
}

void DaliTableView::OnKeyEvent( const KeyEvent& event )
{
  if( event.state == KeyEvent::Down )
  {
    if ( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
      mApplication.Quit();
    }
  }
}

Actor CreateBackgroundActor( const Vector2& size )
{
  Actor layer = Actor::New();
  layer.SetAnchorPoint( AnchorPoint::CENTER );
  layer.SetParentOrigin( ParentOrigin::CENTER );
  layer.SetSize( size );
  return layer;
}

void DaliTableView::SetupBackground( Actor addToLayer, const Vector2& size )
{
  // Create distance field shape
  BitmapImage distanceField;
  Size imageSize( 512, 512 );
  CreateShapeImage( CIRCLE, imageSize, distanceField );

  // Create layers
  Actor backgroundAnimLayer0 = CreateBackgroundActor( size );
  Actor backgroundAnimLayer1 = CreateBackgroundActor( size );
  Actor backgroundAnimLayer2 = CreateBackgroundActor( size );

  // Add constraints
  Constraint animConstraint0 = Constraint::New < Vector3 > ( Actor::POSITION,
      Source( mScrollView, mScrollView.GetPropertyIndex( ScrollView::SCROLL_POSITION_PROPERTY_NAME ) ),
      AnimScrollConstraint( backgroundAnimLayer0.GetCurrentPosition(), 0.75f ) );
  backgroundAnimLayer0.ApplyConstraint( animConstraint0 );

  Constraint animConstraint1 = Constraint::New < Vector3 > ( Actor::POSITION,
      Source( mScrollView, mScrollView.GetPropertyIndex( ScrollView::SCROLL_POSITION_PROPERTY_NAME ) ),
      AnimScrollConstraint( backgroundAnimLayer1.GetCurrentPosition(), 0.5f ) );
  backgroundAnimLayer1.ApplyConstraint( animConstraint1 );

  Constraint animConstraint2 = Constraint::New < Vector3 > ( Actor::POSITION,
      Source( mScrollView, mScrollView.GetPropertyIndex( ScrollView::SCROLL_POSITION_PROPERTY_NAME ) ),
      AnimScrollConstraint( backgroundAnimLayer2.GetCurrentPosition(), 0.25f ) );
  backgroundAnimLayer2.ApplyConstraint( animConstraint2 );

  // Background
  ImageActor layer = Dali::Toolkit::CreateSolidColorActor( BACKGROUND_COLOR );
  layer.SetAnchorPoint( AnchorPoint::CENTER );
  layer.SetParentOrigin( ParentOrigin::CENTER );
  layer.SetSize( size * BACKGROUND_SIZE_SCALE );
  layer.SetZ( BACKGROUND_Z );
  layer.SetPositionInheritanceMode( DONT_INHERIT_POSITION );

  addToLayer.Add( layer );

  // Parent the layers
  addToLayer.Add( backgroundAnimLayer0 );
  addToLayer.Add( backgroundAnimLayer1 );
  addToLayer.Add( backgroundAnimLayer2 );

  // Add all the children
  AddBackgroundActors( backgroundAnimLayer0, NUM_BACKGROUND_IMAGES / 3, distanceField, size );
  AddBackgroundActors( backgroundAnimLayer1, NUM_BACKGROUND_IMAGES / 3, distanceField, size );
  AddBackgroundActors( backgroundAnimLayer2, NUM_BACKGROUND_IMAGES / 3, distanceField, size );
}

void DaliTableView::AddBackgroundActors( Actor layer, int count, BitmapImage distanceField, const Dali::Vector2& size )
{
  for( int i = 0; i < count; ++i )
  {
    float randSize = Random::Range( 10.0f, 400.0f );
    float hue = Random::Range( 0.3f, 1.0f );
    Vector4 randColour( hue, hue*0.5, 0.0f, Random::Range( 0.3f, 0.6f ));

    ImageActor dfActor = ImageActor::New( distanceField );
    mBackgroundActors.push_back( dfActor );
    dfActor.SetSize( Vector2( randSize, randSize ) );
    dfActor.SetParentOrigin( ParentOrigin::CENTER );

    Toolkit::DistanceFieldEffect effect = Toolkit::DistanceFieldEffect::New();
    dfActor.SetShaderEffect( effect );
    dfActor.SetColor( randColour );
    effect.SetOutlineParams( Vector2( 0.55f, 0.00f ) );
    effect.SetSmoothingEdge( 0.5f );
    layer.Add( dfActor );

    // Setup animation
    Vector3 actorPos(
        Random::Range( -size.x * 0.5f * BACKGROUND_SPREAD_SCALE, size.x * 0.5f * BACKGROUND_SPREAD_SCALE ),
        Random::Range( -size.y * 0.5f - randSize, size.y * 0.5f + randSize ),
        Random::Range(-1.0f, 0.0f) );
    dfActor.SetPosition( actorPos );

    Constraint movementConstraint = Constraint::New < Vector3 > ( Actor::POSITION,
        LocalSource( Actor::SIZE ),
        ParentSource( Actor::SIZE ),
        ShapeMovementConstraint );
    dfActor.ApplyConstraint( movementConstraint );

    // Kickoff animation
    Animation animation = Animation::New( Random::Range( 40.0f, 200.0f ) );
    KeyFrames keyframes = KeyFrames::New();
    keyframes.Add( 0.0f, actorPos );
    Vector3 toPos( actorPos );
    toPos.y -= ( size.y + randSize );
    keyframes.Add( 1.0f, toPos );
    animation.AnimateBetween( Property( dfActor, Actor::POSITION ), keyframes );
    animation.SetLooping( true );
    animation.Play();
    mBackgroundAnimations.push_back( animation );
  }
}

void DaliTableView::CreateShapeImage( ShapeType shapeType, const Size& size, BitmapImage& distanceFieldOut )
{
  // this bitmap will hold the alpha map for the distance field shader
  distanceFieldOut = BitmapImage::New( size.width, size.height, Pixel::A8 );

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

  GenerateDistanceFieldMap( &imageDataA8[ 0 ], size, distanceFieldOut.GetBuffer(), size, 8.0f, size );
  distanceFieldOut.Update();
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

ImageActor DaliTableView::CreateLogo( std::string imagePath )
{
  Image image = Image::New( imagePath );
  image.LoadingFinishedSignal().Connect( this, &DaliTableView::OnLogoLoaded );

  ImageActor logo = ImageActor::New( image );

  logo.SetAnchorPoint( AnchorPoint::CENTER );
  logo.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );

  return logo;
}

void DaliTableView::OnLogoLoaded( Dali::Image image )
{
  mRootActor.SetFixedHeight( 1, image.GetHeight() + LOGO_BOTTOM_PADDING_HEIGHT );
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

      anim.Pause();
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

Dali::Actor DaliTableView::OnKeyboardPreFocusChange( Dali::Actor current, Dali::Actor proposed, Dali::Toolkit::Control::KeyboardFocusNavigationDirection direction )
{
  Actor nextFocusActor = proposed;

  if ( !current && !proposed  )
  {
    // Set the initial focus to the first tile in the current page should be focused.
    nextFocusActor = mTableViewList[mScrollView.GetCurrentPage()].GetChildAt(TableView::CellPosition(0, 0));
  }
  else if( !proposed || (proposed && proposed == mScrollViewLayer) )
  {
    // ScrollView is being focused but nothing in the current page can be focused further
    // in the given direction. We should work out which page to scroll to next.
    int currentPage = mScrollView.GetCurrentPage();
    int newPage = currentPage;
    if( direction == Dali::Toolkit::Control::Left )
    {
      newPage--;
    }
    else if( direction == Dali::Toolkit::Control::Right )
    {
      newPage++;
    }

    newPage = std::max(0, std::min(static_cast<int>(mScrollRulerX->GetTotalPages() - 1), newPage));
    if( newPage == currentPage )
    {
      if( direction == Dali::Toolkit::Control::Left )
      {
        newPage = mScrollRulerX->GetTotalPages() - 1;
      } else if( direction == Dali::Toolkit::Control::Right )
      {
        newPage = 0;
      }
    }

    // Scroll to the page in the given direction
    mScrollView.ScrollTo(newPage);

    if( direction == Dali::Toolkit::Control::Left )
    {
      // Work out the cell position for the last tile
      int remainingExamples = mExampleList.size() - newPage * EXAMPLES_PER_PAGE;
      int rowPos = (remainingExamples >= EXAMPLES_PER_PAGE) ? ROWS_PER_PAGE - 1 : ( (remainingExamples % EXAMPLES_PER_PAGE + EXAMPLES_PER_ROW) / EXAMPLES_PER_ROW - 1 );
      int colPos = remainingExamples >= EXAMPLES_PER_PAGE ? EXAMPLES_PER_ROW - 1 : ( remainingExamples % EXAMPLES_PER_PAGE - rowPos * EXAMPLES_PER_ROW - 1 );

      // Move the focus to the last tile in the new page.
      nextFocusActor = mTableViewList[newPage].GetChildAt(TableView::CellPosition(rowPos, colPos));
    }
    else
    {
      // Move the focus to the first tile in the new page.
      nextFocusActor = mTableViewList[newPage].GetChildAt(TableView::CellPosition(0, 0));
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
