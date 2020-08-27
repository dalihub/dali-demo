/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const int NUMBER_OF_PAGES = 9; ///< Number of Pages going across
const int ROWS_PER_PAGE   = 5; ///< Number of Images going down (rows) within a Page

const float DISTANCE_BETWEEN_IMAGES = 6.0f; ///< The distance between the images

} // unnamed namespace

/**
 * An example showing how to create a very simple scroll view
 */
class ExampleController : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  ExampleController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init and orientation changed signal
    mApplication.InitSignal().Connect(this, &ExampleController::OnInit);
  }

  ~ExampleController() = default;

private:

  /**
   * This method gets called once the main loop of application is up and running
   */
  void OnInit(Application& app)
  {
    Window window = app.GetWindow();
    Vector2 windowSize = window.GetSize();
    window.SetBackgroundColor( Color::WHITE );
    window.KeyEventSignal().Connect(this, &ExampleController::OnKeyEvent);

    // Make the scroll view's size a certain percentage of the window
    const Vector2 pageSize = windowSize * 0.75f;

    // Create a scroll view and set our desired properties
    ScrollView scrollView = ScrollView::New();
    scrollView.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    scrollView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    scrollView.SetProperty( Actor::Property::SIZE, pageSize );
    scrollView.SetAxisAutoLock( true );
    window.Add( scrollView );

    // We want to the scroll-view so only one page is shown
    scrollView.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX );

    // Create rulers for the X and Y domains, we want to disable vertical scrolling but enable horizontal scrolling
    RulerPtr rulerX = new FixedRuler( pageSize.width ); // Snaps to a multiple of this when flicking
    RulerPtr rulerY = new DefaultRuler;
    rulerX->SetDomain( RulerDomain( 0.0f, pageSize.width * NUMBER_OF_PAGES, true ) ); // Set the domain to equal the number of pages used
    rulerY->Disable();

    scrollView.SetRulerX( rulerX );
    scrollView.SetRulerY( rulerY );

    // Populate the Pages
    for( int column = 0, textNumber = 0; column < NUMBER_OF_PAGES; column++ )
    {
      Actor page = CreatePage( pageSize, textNumber );
      page.SetProperty( Actor::Property::POSITION, Vector2( column * pageSize.x, 0.0f ));
      scrollView.Add( page );
    }

    // Do a little animation from the last page to the first page on load
    scrollView.ScrollTo( NUMBER_OF_PAGES - 1, 0.0f );
    scrollView.ScrollTo( 0 );
  }

  /**
   * Creates a page using a source of images.
   */
  Actor CreatePage( const Vector2& pageSize, int& textNumber )
  {
    Actor page = Actor::New();
    page.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    page.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    page.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );

    Window window = mApplication.GetWindow();

    // Calculate the number of images going across (columns) within a page, the image size and the size of the text
    const int imageColumns = round( ROWS_PER_PAGE * ( pageSize.width ) / ( pageSize.height ) );
    const Vector3 imageSize( ( pageSize.width  / imageColumns ) - DISTANCE_BETWEEN_IMAGES,
                             ( pageSize.height / ROWS_PER_PAGE) - DISTANCE_BETWEEN_IMAGES,
                             0.0f);
    const float textPixelSize = imageSize.width / 3.0f;

    // Populate the page
    for( int row = 0; row < ROWS_PER_PAGE; row++ )
    {
      for( int column = 0; column < imageColumns; column++ )
      {
        const Vector3 position( DISTANCE_BETWEEN_IMAGES * 0.5f + ( imageSize.x + DISTANCE_BETWEEN_IMAGES ) * column - pageSize.width  * 0.5f,
                                DISTANCE_BETWEEN_IMAGES * 0.5f + ( imageSize.y + DISTANCE_BETWEEN_IMAGES ) * row    - pageSize.height * 0.5f,
                                0.0f);

        Control item = TextLabel::New( std::to_string( textNumber++ ) );
        item.SetProperty( Actor::Property::POSITION, position + imageSize * 0.5f );
        item.SetProperty( Actor::Property::SIZE, imageSize );
        item.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
        item.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
        item.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLACK );
        item.SetProperty( TextLabel::Property::PIXEL_SIZE, textPixelSize );
        item.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::CENTER );
        item.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, VerticalAlignment::CENTER );
        item.SetProperty( TextLabel::Property::OUTLINE, Property::Map{ { "width", 2 }, { "color", Color::WHITE } } );
        item.SetProperty( Control::Property::BACKGROUND, Vector4( Random::Range( 0.0f, 1.0f ), Random::Range( 0.0f, 1.0f ), Random::Range( 0.0f, 1.0f ), 0.7f ) );
        page.Add(item);
      }
    }

    return page;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication; ///< Application instance
};

int DALI_EXPORT_API main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv, DEMO_THEME_PATH);
  ExampleController test(app);
  app.MainLoop();
  return 0;
}
