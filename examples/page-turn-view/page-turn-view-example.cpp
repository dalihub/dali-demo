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

#include <assert.h>
#include <cstdlib>
#include <string.h>
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

// LOCAL STUFF
namespace
{
// The content amount of one page between portrait and landscape view are different
// set a ratio to modify the current page number when the rotation is changed
const float PAGE_NUMBER_CORRESPONDING_RATIO(1.25f);

const char* BOOK_COVER_PORTRAIT = ( DALI_IMAGE_DIR "book-portrait-cover.jpg" );
const char* BOOK_COVER_LANDSCAPE = ( DALI_IMAGE_DIR "book-landscape-cover.jpg" );
const char* BOOK_COVER_BACK_LANDSCAPE = ( DALI_IMAGE_DIR "book-landscape-cover-back.jpg" );

const char* PAGE_IMAGES_PORTRAIT[] =
{
  DALI_IMAGE_DIR "book-portrait-p1.jpg",
  DALI_IMAGE_DIR "book-portrait-p2.jpg",
  DALI_IMAGE_DIR "book-portrait-p3.jpg",
  DALI_IMAGE_DIR "book-portrait-p4.jpg",
  DALI_IMAGE_DIR "book-portrait-p5.jpg"
};
const unsigned int NUMBER_OF_PORTRAIT_IMAGE( sizeof(PAGE_IMAGES_PORTRAIT) / sizeof(PAGE_IMAGES_PORTRAIT[0]) );

const char* PAGE_IMAGES_LANDSCAPE[] =
{
  DALI_IMAGE_DIR "book-landscape-p1.jpg",
  DALI_IMAGE_DIR "book-landscape-p2.jpg",
  DALI_IMAGE_DIR "book-landscape-p3.jpg",
  DALI_IMAGE_DIR "book-landscape-p4.jpg",
  DALI_IMAGE_DIR "book-landscape-p5.jpg",
  DALI_IMAGE_DIR "book-landscape-p6.jpg",
  DALI_IMAGE_DIR "book-landscape-p7.jpg",
  DALI_IMAGE_DIR "book-landscape-p8.jpg"
};
const unsigned int NUMBER_OF_LANDSCAPE_IMAGE( sizeof(PAGE_IMAGES_LANDSCAPE) / sizeof(PAGE_IMAGES_LANDSCAPE[0]) );

}// end LOCAL STUFF

class PortraitPageFactory : public PageFactory
{
  /**
   * Query the number of pages available from the factory.
   * The maximum available page has an ID of GetNumberOfPages()-1.
   */
  virtual unsigned int GetNumberOfPages()
  {
    return 5*NUMBER_OF_PORTRAIT_IMAGE + 1;
  }
  /**
   * Create an image actor to represent a page.
   * @param[in] pageId The ID of the page to create.
   * @return An image actor, or an uninitialized pointer if the ID is out of range.
   */
  virtual Actor NewPage( unsigned int pageId )
  {
    if( pageId == 0 )
    {
      return ImageActor::New( ResourceImage::New( BOOK_COVER_PORTRAIT ) );
    }
    else
    {
      return ImageActor::New( ResourceImage::New( PAGE_IMAGES_PORTRAIT[ (pageId-1) % NUMBER_OF_PORTRAIT_IMAGE ] ) );
    }
  }
};

class LandscapePageFactory : public PageFactory
{
  /**
   * Query the number of pages available from the factory.
   * The maximum available page has an ID of GetNumberOfPages()-1.
   */
  virtual unsigned int GetNumberOfPages()
  {
    return 5*NUMBER_OF_LANDSCAPE_IMAGE / 2 + 1;
  }
  /**
   * Create an image actor to represent a page.
   * @param[in] pageId The ID of the page to create.
   * @return An image actor, or an uninitialized pointer if the ID is out of range.
   */
  virtual Actor NewPage( unsigned int pageId )
  {
    ImageActor pageFront;
    ImageActor pageBack;
    if( pageId == 0 )
    {
       pageFront = ImageActor::New( ResourceImage::New( BOOK_COVER_LANDSCAPE ) );
       pageBack = ImageActor::New( ResourceImage::New( BOOK_COVER_BACK_LANDSCAPE ) );
    }
    else
    {
      unsigned int imageId = (pageId-1)*2;
      pageFront = ImageActor::New( ResourceImage::New( PAGE_IMAGES_LANDSCAPE[ imageId % NUMBER_OF_LANDSCAPE_IMAGE ] ) );
      pageBack = ImageActor::New( ResourceImage::New( PAGE_IMAGES_LANDSCAPE[ (imageId+1) % NUMBER_OF_LANDSCAPE_IMAGE ] ) );
    }
    pageFront.Add(pageBack);
    return pageFront;
  }
};

/**
 * This example shows how to use the page turn UI control to implement the page-turn demo
 * The effect follows the pan gesture to animate the page
 * Pan the image inwards, the page will bent,
 * Depends on the distance of the panning, the image might turn over or slide back
 * Also, in portrait view, the pan gesture outwards from position near the spine could turn the previous page back
 * Allows to turn multiple pages one by one quickly towards the same direction, multiple animations are launched in this case
*/
class PageTurnController : public ConnectionTracker
{
public:
  PageTurnController( Application &app );
  ~PageTurnController();

  //This method gets called once the main loop of application is up and running
  void OnInit( Application& app );

private:

  /**
   * This method gets called when the screen is rotated, switch between portrait and landscape views
   * param [in] view The view receiving the orientation change signal
   * param [in] animation The Orientation Rotating animation
   * param [in] orientation The current screen orientation
   */
  void OnOrientationAnimationStarted( View view, Animation& animation, const Orientation& orientation );

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event);

  /**
   * Callback function of page turned signal
   * @param[in] pageTurnView The handle of the PageTurnPortraitView or PageTurnLandscapeView
   * @param[in] pageIndex The index of the page turned over
   * @param[in] isTurningForward The turning direction, forwards or backwards
   */
  void OnPageStartedTurn( PageTurnView pageTurnView, unsigned int pageIndex, bool isTurningForward );

  /**
   * Callback function of page turned signal
   * @param[in] pageTurnView The handle of the PageTurnPortraitView or PageTurnLandscapeView
   * @param[in] pageIndex The index of the page turned over
   * @param[in] isTurningForward The turning direction, forwards or backwards
   */
  void OnPageFinishedTurn( PageTurnView pageTurnView, unsigned int pageIndex, bool isTurningForward );

  /**
   * Callback function of page started pan signal
   *
   * @param[in] pageTurnView The calling page turn view
   */
  void OnPageStartedPan( PageTurnView pageTurnView );

  /**
   * Callback function of page finished pan signal
   *
   * @param[in] pageTurnView The calling page turn view
   */
  void OnPageFinishedPan( PageTurnView pageTurnView );

private:

  Application&                mApplication;
  View                        mView;

  PageTurnView                mPageTurnPortraitView;
  PageTurnView                mPageTurnLandscapeView;
  PortraitPageFactory         mPortraitPageFactory;
  LandscapePageFactory        mLandscapePageFactory;

  bool                        mIsPortrait;
};

PageTurnController::PageTurnController( Application &app )
:mApplication( app ),
 mIsPortrait( true )
{
  // Connect to the Application's Init signal
  app.InitSignal().Connect( this, &PageTurnController::OnInit );
}

PageTurnController::~PageTurnController()
{
}


void PageTurnController::OnInit( Application& app )
{
  // The Init signal is received once ( only ) during the Application lifetime

  Stage::GetCurrent().KeyEventSignal().Connect(this, &PageTurnController::OnKeyEvent);

  Stage stage = Stage::GetCurrent();
  Vector2 stageSize =  stage.GetSize();

  // Create default View.
  mView = View::New();
  stage.Add( mView );

  Dali::Window winHandle = app.GetWindow();
  winHandle.AddAvailableOrientation( Dali::Window::PORTRAIT );
  winHandle.AddAvailableOrientation( Dali::Window::LANDSCAPE );
  winHandle.AddAvailableOrientation( Dali::Window::PORTRAIT_INVERSE  );
  winHandle.AddAvailableOrientation( Dali::Window::LANDSCAPE_INVERSE );
  // FIXME
  //app.GetOrientation().ChangedSignal().Connect( &mView, &View::OrientationChanged );
  // view will response to orientation change to display portrait or landscape views
  mView.OrientationAnimationStartedSignal().Connect( this, &PageTurnController::OnOrientationAnimationStarted );

  mPageTurnPortraitView = PageTurnPortraitView::New( mPortraitPageFactory, stageSize );
  mPageTurnPortraitView.SetSpineShadowParameter( Vector2(70.f, 30.f) );
  mPageTurnPortraitView.PageTurnStartedSignal().Connect( this, &PageTurnController::OnPageStartedTurn );
  mPageTurnPortraitView.PageTurnFinishedSignal().Connect( this, &PageTurnController::OnPageFinishedTurn );
  mPageTurnPortraitView.PagePanStartedSignal().Connect( this, &PageTurnController::OnPageStartedPan );
  mPageTurnPortraitView.PagePanFinishedSignal().Connect( this, &PageTurnController::OnPageFinishedPan );
  mPageTurnPortraitView.SetPositionInheritanceMode( USE_PARENT_POSITION );

  mPageTurnLandscapeView = PageTurnLandscapeView::New( mLandscapePageFactory, Vector2(stageSize.y*0.5f, stageSize.x) );
  mPageTurnLandscapeView.PageTurnStartedSignal().Connect( this, &PageTurnController::OnPageStartedTurn );
  mPageTurnLandscapeView.PageTurnFinishedSignal().Connect( this, &PageTurnController::OnPageFinishedTurn );
  mPageTurnLandscapeView.PagePanStartedSignal().Connect( this, &PageTurnController::OnPageStartedPan );
  mPageTurnLandscapeView.PagePanFinishedSignal().Connect( this, &PageTurnController::OnPageFinishedPan );
  mPageTurnLandscapeView.SetPositionInheritanceMode( USE_PARENT_POSITION );

  mView.Add(mPageTurnPortraitView);
}

void PageTurnController::OnOrientationAnimationStarted( View view, Animation& animation, const Orientation& orientation )
{
  switch( orientation.GetDegrees() )
  {
    // portrait view, display page in the right side only
    case 0:
    case 180:
    {
      if( !mIsPortrait )
      {
        mView.Remove( mPageTurnLandscapeView );
        mView.Add( mPageTurnPortraitView );
        int currentPage = floor( static_cast<float>(mPageTurnLandscapeView.GetCurrentPage()) * PAGE_NUMBER_CORRESPONDING_RATIO );
        mPageTurnPortraitView.GoToPage( currentPage );
        mIsPortrait = true;
      }
      break;
    }
    // display pages in both sides
    case 90:
    case 270:
    {
      if( mIsPortrait )
      {
        mView.Remove( mPageTurnPortraitView );
        mView.Add( mPageTurnLandscapeView );
        int currentPage = ceil( static_cast<float>(mPageTurnPortraitView.GetCurrentPage()) / PAGE_NUMBER_CORRESPONDING_RATIO );
        mPageTurnLandscapeView.GoToPage( currentPage );
        mIsPortrait = false;
      }
      break;
    }
    default:
    break;
  }
}

/**
 * Main key event handler
 */
void PageTurnController::OnKeyEvent(const KeyEvent& event)
{
  if(event.state == KeyEvent::Down)
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
      mApplication.Quit();
    }
  }
}

void PageTurnController::OnPageStartedTurn( PageTurnView pageTurnView, unsigned int pageIndex, bool isTurningForward )
{
  std::cout<< ( ( pageTurnView == mPageTurnPortraitView ) ? " portrait: " : " Landscape: " )
           << " page " << pageIndex
           << ( isTurningForward ? " is starting to turn forward" : " is starting to turn backward" )
           << std::endl;
}

void PageTurnController::OnPageFinishedTurn( PageTurnView pageTurnView, unsigned int pageIndex, bool isTurningForward )
{
  std::cout<< ( ( pageTurnView == mPageTurnPortraitView ) ? " portrait: " : " Landscape: " )
           << " page " << pageIndex
           << ( isTurningForward ? " has finished turning forward" : " has finished turning backward" )
           << std::endl;
}

void PageTurnController::OnPageStartedPan( PageTurnView pageTurnView )
{
  std::cout<< "Starting to pan" << std::endl;
}

void PageTurnController::OnPageFinishedPan( PageTurnView pageTurnView )
{
  std::cout<< "Finished panning" << std::endl;
}

// Entry point for applications
int main( int argc, char **argv )
{
  Application app = Application::New(&argc, &argv);
  PageTurnController test ( app );

  app.MainLoop();

  return 0;
}

