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

/**
 * @file image-scaling-irregular-grid-example.cpp
 * @brief Demonstrates how to use image scaling modes when loading images.
 *
 * If an image is going to be drawn on-screen at a lower resolution than it is
 * stored at on-disk, the scaling feature of the image loader can be used to
 * reduce the image to save memory, improve performance, and potentially display
 * a better small version of the image than if the default size were loaded.
 *
 * The functions CreateImage and CreateImageActor below show how to build an
 * image using a scaling mode to have %Dali resize it during loading.
 *
 * This demo defaults to the ScaleToFill mode of ImageAttributes which makes
 * sure that every pixel in the loaded image is filled with a source colour
 * from the image's central region while losing the minimum number of pixels
 * from its periphery.
 * It is the best option for producing thumbnails of input images that have
 * diverse aspect ratios.
 *
 * The other four scaling modes of dali can be cycled-through for the whole
 * grid  using the button in the top-right of the toolbar.
 * A single image can be cycled by clicking the image directly.
 *
 * @see CreateImage CreateImageActor
 */

// EXTERNAL INCLUDES
#include <algorithm>
#include <map>
#include <dali-toolkit/dali-toolkit.h>

// INTERNAL INCLUDES
#include "grid-flags.h"
#include "../../shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Demo;

namespace
{

/** Controls the output of application logging. */
//#define DEBUG_PRINT_DIAGNOSTICS;

const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-gradient.jpg" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* APPLICATION_TITLE( "Image Scaling Modes" );
const char* TOGGLE_SCALING_IMAGE( DALI_IMAGE_DIR "icon-change.png" );

/** The width of the grid in whole grid cells. */
const unsigned GRID_WIDTH = 9;
/** Limit the grid to be no higher than this in units of a cell. */
const unsigned GRID_MAX_HEIGHT = 600;

/** The space between the edge of a grid cell and the image embedded within it. */
const unsigned GRID_CELL_PADDING = 4;

/** The aspect ratio of cells in the image grid. */
const float CELL_ASPECT_RATIO = 1.33333333333333333333f;

const ImageAttributes::ScalingMode DEFAULT_SCALING_MODE = ImageAttributes::ScaleToFill;

/** The number of times to spin an image on touching, each spin taking a second.*/
const float SPIN_DURATION = 1.0f;

/** The target image sizes in grid cells. */
const Vector2 IMAGE_SIZES[] = {
 Vector2( 1, 1 ),
 Vector2( 2, 1 ),
 Vector2( 3, 1 ),
 Vector2( 1, 2 ),
 Vector2( 1, 3 ),
 Vector2( 2, 3 ),
 Vector2( 3, 2 ),
 // Large, tall configuration:
 Vector2( GRID_WIDTH / 2, GRID_WIDTH + GRID_WIDTH / 2 ),
 // Large, square-ish images to show shrink-to-fit well with wide and tall images:
 Vector2( GRID_WIDTH / 2, GRID_WIDTH / 2.0f * CELL_ASPECT_RATIO + 0.5f ),
 Vector2( GRID_WIDTH - 2, (GRID_WIDTH - 2) * CELL_ASPECT_RATIO + 0.5f ),
};
const unsigned NUM_IMAGE_SIZES = sizeof(IMAGE_SIZES) / sizeof(IMAGE_SIZES[0]);

/** Images to load into the grid. These are mostly large and non-square to
 *  show the scaling. */
const char* IMAGE_PATHS[] = {

  DALI_IMAGE_DIR "dali-logo.png",
  DALI_IMAGE_DIR "com.samsung.dali-demo.ico",
  DALI_IMAGE_DIR "square_primitive_shapes.bmp",
  DALI_IMAGE_DIR "gallery-large-14.wbmp",

  // Images that show aspect ratio changes clearly in primitive shapes:

  DALI_IMAGE_DIR "portrait_screen_primitive_shapes.gif",
  DALI_IMAGE_DIR "landscape_screen_primitive_shapes.gif",

  // Images from other demos that are tall, wide or just large:

  DALI_IMAGE_DIR "gallery-large-1.jpg",
  DALI_IMAGE_DIR "gallery-large-2.jpg",
  DALI_IMAGE_DIR "gallery-large-3.jpg",
  DALI_IMAGE_DIR "gallery-large-4.jpg",
  DALI_IMAGE_DIR "gallery-large-5.jpg",
  DALI_IMAGE_DIR "gallery-large-6.jpg",
  DALI_IMAGE_DIR "gallery-large-7.jpg",
  DALI_IMAGE_DIR "gallery-large-8.jpg",
  DALI_IMAGE_DIR "gallery-large-9.jpg",
  DALI_IMAGE_DIR "gallery-large-10.jpg",
  DALI_IMAGE_DIR "gallery-large-11.jpg",
  DALI_IMAGE_DIR "gallery-large-12.jpg",
  DALI_IMAGE_DIR "gallery-large-13.jpg",
  DALI_IMAGE_DIR "gallery-large-14.jpg",
  DALI_IMAGE_DIR "gallery-large-15.jpg",
  DALI_IMAGE_DIR "gallery-large-16.jpg",
  DALI_IMAGE_DIR "gallery-large-17.jpg",
  DALI_IMAGE_DIR "gallery-large-18.jpg",
  DALI_IMAGE_DIR "gallery-large-19.jpg",
  DALI_IMAGE_DIR "gallery-large-20.jpg",
  DALI_IMAGE_DIR "gallery-large-21.jpg",

  DALI_IMAGE_DIR "background-1.jpg",
  DALI_IMAGE_DIR "background-2.jpg",
  DALI_IMAGE_DIR "background-3.jpg",
  DALI_IMAGE_DIR "background-4.jpg",
  DALI_IMAGE_DIR "background-5.jpg",
  DALI_IMAGE_DIR "background-blocks.jpg",
  DALI_IMAGE_DIR "background-magnifier.jpg",

  DALI_IMAGE_DIR "background-1.jpg",
  DALI_IMAGE_DIR "background-2.jpg",
  DALI_IMAGE_DIR "background-3.jpg",
  DALI_IMAGE_DIR "background-4.jpg",
  DALI_IMAGE_DIR "background-5.jpg",
  DALI_IMAGE_DIR "background-blocks.jpg",
  DALI_IMAGE_DIR "background-magnifier.jpg",

  DALI_IMAGE_DIR "book-landscape-cover-back.jpg",
  DALI_IMAGE_DIR "book-landscape-cover.jpg",
  DALI_IMAGE_DIR "book-landscape-p1.jpg",
  DALI_IMAGE_DIR "book-landscape-p2.jpg",

  DALI_IMAGE_DIR "book-portrait-cover.jpg",
  DALI_IMAGE_DIR "book-portrait-p1.jpg",
  DALI_IMAGE_DIR "book-portrait-p2.jpg",
  NULL
};
const unsigned NUM_IMAGE_PATHS = sizeof(IMAGE_PATHS) / sizeof(IMAGE_PATHS[0]) - 1u;


/**
 * Creates an Image
 *
 * @param[in] filename The path of the image.
 * @param[in] width The width of the image in pixels.
 * @param[in] height The height of the image in pixels.
 * @param[in] scalingMode The mode to use when scaling the image to fit the desired dimensions.
 */
Image CreateImage(const std::string& filename, unsigned int width, unsigned int height, ImageAttributes::ScalingMode scalingMode )
{
#ifdef DEBUG_PRINT_DIAGNOSTICS
    fprintf( stderr, "CreateImage(%s, %u, %u, scalingMode=%u)\n", filename.c_str(), width, height, unsigned( scalingMode ) );
#endif
  ImageAttributes attributes;

  attributes.SetSize( width, height );
  attributes.SetScalingMode( scalingMode );
  Image image = ResourceImage::New( filename, attributes );
  return image;
}

/**
 * Creates an ImageActor
 *
 * @param[in] filename The path of the image.
 * @param[in] width The width of the image in pixels.
 * @param[in] height The height of the image in pixels.
 * @param[in] scalingMode The mode to use when scaling the image to fit the desired dimensions.
 */
ImageActor CreateImageActor(const std::string& filename, unsigned int width, unsigned int height, ImageAttributes::ScalingMode scalingMode )
{
  Image img = CreateImage( filename, width, height, scalingMode );
  ImageActor actor = ImageActor::New( img );
  actor.SetName( filename );
  actor.SetParentOrigin(ParentOrigin::CENTER);
  actor.SetAnchorPoint(AnchorPoint::CENTER);

  return actor;
}

/** Cycle the scaling mode options. */
ImageAttributes::ScalingMode NextMode( const ImageAttributes::ScalingMode oldMode )
{
  ImageAttributes::ScalingMode newMode = ImageAttributes::ShrinkToFit;
  switch ( oldMode )
  {
    case ImageAttributes::ShrinkToFit:
      newMode = ImageAttributes::ScaleToFill;
      break;
    case ImageAttributes::ScaleToFill:
      newMode = ImageAttributes::FitWidth;
      break;
    case ImageAttributes::FitWidth:
      newMode = ImageAttributes::FitHeight;
      break;
    case ImageAttributes::FitHeight:
      newMode = ImageAttributes::ShrinkToFit;
      break;
  }
  return newMode;
}

/**
 * Bundle an image path with the rectangle to pack it into.
 * */
struct ImageConfiguration
{
  ImageConfiguration( const char * const path, const Vector2 dimensions ) :
    path( path ),
    dimensions( dimensions )
  {}
  const char * path;
  Vector2 dimensions;
};

/**
 * Post-layout image data.
 */
struct PositionedImage
{
  PositionedImage(ImageConfiguration& configuration, unsigned cellX, unsigned cellY, Vector2 imageGridDims) :
    configuration( configuration ),
    cellX( cellX ),
    cellY( cellY ),
    imageGridDims( imageGridDims )
  {}

  ImageConfiguration configuration;
  unsigned cellX;
  unsigned cellY;
  Vector2 imageGridDims;
};

}

/**
 * @brief The main class of the demo.
 */
class ImageScalingIrregularGridController : public ConnectionTracker
{
public:

  ImageScalingIrregularGridController( Application& application )
  : mApplication( application ),
    mScrolling( false )
  {
    std::cout << "ImageScalingScaleToFillController::ImageScalingScaleToFillController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageScalingIrregularGridController::Create );
  }

  ~ImageScalingIrregularGridController()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    std::cout << "ImageScalingScaleToFillController::Create" << std::endl;

    // Get a handle to the stage:
    Stage stage = Stage::GetCurrent();

    // Connect to input event signals:
    stage.KeyEventSignal().Connect(this, &ImageScalingIrregularGridController::OnKeyEvent);

    // Hide the indicator bar
    mApplication.GetWindow().ShowIndicator(Dali::Window::INVISIBLE);

    // Create a default view with a default tool bar:
    mContentLayer = DemoHelper::CreateView( mApplication,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            "" );

    // Create an image scaling toggle button. (right of toolbar)
    Image toggleScalingImage = ResourceImage::New( TOGGLE_SCALING_IMAGE );
    Toolkit::PushButton toggleScalingButton = Toolkit::PushButton::New();
    toggleScalingButton.SetBackgroundImage( toggleScalingImage );
    toggleScalingButton.ClickedSignal().Connect( this, &ImageScalingIrregularGridController::OnToggleScalingTouched );
    mToolBar.AddControl( toggleScalingButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    SetTitle( APPLICATION_TITLE );

    // Build the main content of the widow:
    PopulateContentLayer( DEFAULT_SCALING_MODE );
  }

  /**
   * Build the main part of the application's view.
   */
  void PopulateContentLayer( const ImageAttributes::ScalingMode scalingMode )
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    float fieldHeight;
    Actor imageField = BuildImageField( stageSize.x, GRID_WIDTH, GRID_MAX_HEIGHT, scalingMode, fieldHeight );

    mScrollView = ScrollView::New();

    mScrollView.ScrollStartedSignal().Connect( this, &ImageScalingIrregularGridController::OnScrollStarted );
    mScrollView.ScrollCompletedSignal().Connect( this, &ImageScalingIrregularGridController::OnScrollCompleted );

    mScrollView.EnableScrollComponent( Scrollable::VerticalScrollBar );
    mScrollView.EnableScrollComponent( Scrollable::HorizontalScrollBar );

    mScrollView.SetAnchorPoint(AnchorPoint::CENTER);
    mScrollView.SetParentOrigin(ParentOrigin::CENTER);

    mScrollView.SetSize( stageSize );//Vector2( stageSize.width, fieldHeight ) );//stageSize );
    mScrollView.SetAxisAutoLock( true );
    mScrollView.SetAxisAutoLockGradient( 1.0f );

    // Restrict scrolling to mostly vertical only, but with some horizontal wiggle-room:

    RulerPtr rulerX = new FixedRuler( stageSize.width ); //< Pull the view back to the grid's centre-line when touch is release using a snapping ruler.
    rulerX->SetDomain( RulerDomain( stageSize.width * -0.125f, stageSize.width * 1.125f ) ); //< Scroll slightly left/right of image field.
    mScrollView.SetRulerX ( rulerX );

    RulerPtr rulerY = new DefaultRuler(); //stageSize.height ); //< Snap in multiples of a screen / stage height
    rulerY->SetDomain( RulerDomain( - fieldHeight * 0.5f + stageSize.height * 0.5f - GRID_CELL_PADDING, fieldHeight * 0.5f + stageSize.height * 0.5f + GRID_CELL_PADDING ) );
    mScrollView.SetRulerY ( rulerY );

    mContentLayer.Add( mScrollView );
    mScrollView.Add( imageField );
    mGridActor = imageField;
  }

  /**
   * Build a field of images scaled into a variety of shapes from very wide,
   * through square, to very tall. The images are direct children of the Dali::Actor
   * returned.
   **/
  Actor BuildImageField( const float fieldWidth,
                           const unsigned gridWidth,
                           const unsigned maxGridHeight,
                           ImageAttributes::ScalingMode scalingMode,
                           float & outFieldHeight )
  {
    // Generate the list of image configurations to be fitted into the field:

    std::vector<ImageConfiguration> configurations;
    configurations.reserve( NUM_IMAGE_PATHS * NUM_IMAGE_SIZES );
    for( unsigned imageIndex = 0; imageIndex < NUM_IMAGE_PATHS; ++imageIndex )
    {
      for( unsigned dimensionsIndex = 0; dimensionsIndex < NUM_IMAGE_SIZES; ++ dimensionsIndex )
      {
        configurations.push_back( ImageConfiguration( IMAGE_PATHS[imageIndex], IMAGE_SIZES[dimensionsIndex] ) );
      }
    }
    // Stir-up the list to get some nice irregularity in the generated field:
    std::random_shuffle( configurations.begin(), configurations.end() );
    std::random_shuffle( configurations.begin(), configurations.end() );

    // Place the images in the grid:

    std::vector<ImageConfiguration>::iterator config, end;
    GridFlags grid( gridWidth, maxGridHeight );
    std::vector<PositionedImage> placedImages;

    for( config = configurations.begin(), end = configurations.end(); config != end; ++config )
    {
      unsigned cellX, cellY;
      Vector2 imageGridDims;

      // Allocate a region of the grid for the image:
      bool allocated = grid.AllocateRegion( config->dimensions, cellX, cellY, imageGridDims );
      if( !allocated )
      {
#ifdef DEBUG_PRINT_DIAGNOSTICS
          fprintf( stderr, "Failed to allocate image in grid with dims (%f, %f) and path: %s.\n", config->dimensions.x, config->dimensions.y, config->path );
#endif
        continue;
      }

      placedImages.push_back( PositionedImage( *config, cellX, cellY, imageGridDims ) );
    }
    DALI_ASSERT_DEBUG( grid.DebugCheckGridValid() && "Cells were set more than once, indicating erroneous overlap in placing images on the grid." );
    const unsigned actualGridHeight = grid.GetHighestUsedRow() + 1;

    // Take the images images in the grid and turn their logical locations into
    // coordinates in a frame defined by a parent actor:

    Actor gridActor = Actor::New();
    gridActor.SetSizeMode( SIZE_EQUAL_TO_PARENT );
    gridActor.SetParentOrigin( ParentOrigin::CENTER );
    gridActor.SetAnchorPoint( AnchorPoint::CENTER );

    // Work out the constants of the grid and cell dimensions and positions:
    const float cellWidth = fieldWidth / gridWidth;
    const float cellHeight = cellWidth / CELL_ASPECT_RATIO;
    const Vector2 cellSize = Vector2( cellWidth, cellHeight );
    outFieldHeight = actualGridHeight * cellHeight;
    const Vector2 gridOrigin = Vector2( -fieldWidth * 0.5f, -outFieldHeight * 0.5 );

    // Build the image actors in their right locations in their parent's frame:
    for( std::vector<PositionedImage>::const_iterator i = placedImages.begin(), end = placedImages.end(); i != end; ++i )
    {
      const PositionedImage& imageSource = *i;
      const Vector2 imageSize = imageSource.imageGridDims * cellSize - Vector2( GRID_CELL_PADDING * 2, GRID_CELL_PADDING * 2 );
      const Vector2 imageRegionCorner = gridOrigin + cellSize * Vector2( imageSource.cellX, imageSource.cellY );
      const Vector2 imagePosition = imageRegionCorner + Vector2( GRID_CELL_PADDING , GRID_CELL_PADDING ) + imageSize * 0.5f;

      ImageActor image = CreateImageActor( imageSource.configuration.path, imageSize.x, imageSize.y, scalingMode );
      image.SetPosition( Vector3( imagePosition.x, imagePosition.y, 0 ) );
      image.SetSize( imageSize );
      image.TouchedSignal().Connect( this, &ImageScalingIrregularGridController::OnTouchImage );
      mScalingModes[image.GetId()] = scalingMode;
      mSizes[image.GetId()] = imageSize;

      gridActor.Add( image );
    }

    return gridActor;
  }

 /**
  * Upon Touching an image (Release), change its scaling mode and make it spin, provided we're not scrolling.
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
        // Spin the image a few times:
        Animation animation = Animation::New(SPIN_DURATION);
        animation.RotateBy( actor, Degree(360.0f * SPIN_DURATION), Vector3::XAXIS, AlphaFunctions::EaseOut);
        animation.Play();

        // Change the scaling mode:
        const unsigned id = actor.GetId();
        ImageAttributes::ScalingMode newMode = NextMode( mScalingModes[id] );
        const Vector2 imageSize = mSizes[actor.GetId()];

        ImageActor imageActor = ImageActor::DownCast( actor );
        Image oldImage = imageActor.GetImage();
        Image newImage = CreateImage( ResourceImage::DownCast(oldImage).GetUrl(), imageSize.width + 0.5f, imageSize.height + 0.5f, newMode );
        imageActor.SetImage( newImage );
        mScalingModes[id] = newMode;
      }
    }
    return false;
  }

 /**
  * Main key event handler.
  * Quit on escape key.
  */
  void OnKeyEvent(const KeyEvent& event)
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE )
          || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

 /**
  * Signal handler, called when the 'Scaling' button has been touched.
  *
  * @param[in] button The button that was pressed.
  */
  bool OnToggleScalingTouched( Button button )
  {
    const unsigned numChildren = mGridActor.GetChildCount();

    for( unsigned i = 0; i < numChildren; ++i )
    {
      ImageActor gridImageActor = ImageActor::DownCast( mGridActor.GetChildAt( i ) );
      if( gridImageActor )
      {
        // Cycle the scaling mode options:
        const Vector2 imageSize = mSizes[gridImageActor.GetId()];
        ImageAttributes::ScalingMode newMode = NextMode( mScalingModes[gridImageActor.GetId()] );
        Image oldImage = gridImageActor.GetImage();
        Image newImage = CreateImage(ResourceImage::DownCast(oldImage).GetUrl(), imageSize.width, imageSize.height, newMode );
        gridImageActor.SetImage( newImage );

        mScalingModes[gridImageActor.GetId()] = newMode;

        SetTitle( std::string( newMode == ImageAttributes::ShrinkToFit ? "ShrinkToFit" : newMode == ImageAttributes::ScaleToFill ?  "ScaleToFill" : newMode == ImageAttributes::FitWidth ? "FitWidth" : "FitHeight" ) );
      }
    }
    return true;
  }

  /**
   * Sets/Updates the title of the View
   * @param[in] title The new title for the view.
   */
  void SetTitle(const std::string& title)
  {
    if(!mTitleActor)
    {
      mTitleActor = TextView::New();
      // Add title to the tool bar.
      mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Alignment::HorizontalCenter );
    }

    Font font = Font::New();
    mTitleActor.SetText( title );
    mTitleActor.SetSize( font.MeasureText( title ) );
    mTitleActor.SetStyleToCurrentText(DemoHelper::GetDefaultTextStyle());
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
   * note this state (mScrolling = false).
   * @param[in] position Current Scroll Position
   */
  void OnScrollCompleted( const Vector3& position )
  {
    mScrolling = false;
  }

private:
  Application&  mApplication;

  Layer mContentLayer;                ///< The content layer (contains non gui chrome actors)
  Toolkit::View mView;                ///< The View instance.
  Toolkit::ToolBar mToolBar;          ///< The View's Toolbar.
  TextView mTitleActor;               ///< The Toolbar's Title.
  Actor mGridActor;                   ///< The container for the grid of images
  ScrollView mScrollView;             ///< ScrollView UI Component
  bool mScrolling;                    ///< ScrollView scrolling state (true = scrolling, false = stationary)
  std::map<unsigned, ImageAttributes::ScalingMode> mScalingModes; ///< Stores the current scaling mode of each image, keyed by image actor id.
  std::map<unsigned, Vector2> mSizes; ///< Stores the current size of each image, keyed by image actor id.
};

void RunTest( Application& application )
{
  ImageScalingIrregularGridController test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
