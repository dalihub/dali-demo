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

#include <assert.h>
#include <cstdlib>
#include <string.h>
#include <iostream>

#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

// LOCAL STUFF
namespace
{
const char* const CHANGE_IMAGE_ICON(DALI_IMAGE_DIR "icon-change.png");
const char* const CHANGE_IMAGE_ICON_SELECTED( DALI_IMAGE_DIR "icon-change-selected.png" );

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
    return 10*NUMBER_OF_PORTRAIT_IMAGE + 1;
  }
  /**
   * Create an image actor to represent a page.
   * @param[in] pageId The ID of the page to create.
   * @return An image actor, or an uninitialized pointer if the ID is out of range.
   */
  virtual Actor NewPage( unsigned int pageId )
  {
    ImageActor page;

    if( pageId == 0 )
    {
      page = ImageActor::New( ResourceImage::New( BOOK_COVER_PORTRAIT ) );
    }
    else
    {
      page = ImageActor::New( ResourceImage::New( PAGE_IMAGES_PORTRAIT[ (pageId-1) % NUMBER_OF_PORTRAIT_IMAGE ] ) );
    }

    return page;
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
    return 10*NUMBER_OF_LANDSCAPE_IMAGE / 2 + 1;
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
   * This method gets called when the button is clicked, switch between portrait and landscape views
   */
  bool OnButtonClicked(Toolkit::Button button);

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
  Layer                       mButtonLayer;

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

  mButtonLayer = Layer::New();
  mButtonLayer.SetAnchorPoint( Dali::AnchorPoint::CENTER );
  mButtonLayer.SetParentOrigin( Dali::ParentOrigin::CENTER );
  mButtonLayer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  Toolkit::PushButton button = Toolkit::PushButton::New();
  button.SetAnchorPoint( AnchorPoint::TOP_RIGHT );
  button.SetParentOrigin( ParentOrigin::TOP_RIGHT );
  button.SetUnselectedImage( CHANGE_IMAGE_ICON  );
  button.SetSelectedImage( CHANGE_IMAGE_ICON_SELECTED );
  button.SetLeaveRequired( true );
  button.SetScale(1.5f);
  button.PressedSignal().Connect( this, &PageTurnController::OnButtonClicked );
  stage.Add( mButtonLayer );
  mButtonLayer.Add(button);

  Vector2 bookSize( stageSize.x > stageSize.y ? stageSize.y : stageSize.x,
                    stageSize.x > stageSize.y ? stageSize.x : stageSize.y );

  mPageTurnPortraitView = PageTurnPortraitView::New( mPortraitPageFactory, bookSize );
  mPageTurnPortraitView.SetParentOrigin( ParentOrigin::CENTER );
  mPageTurnPortraitView.SetAnchorPoint( AnchorPoint::CENTER );
  mPageTurnPortraitView.SetProperty( PageTurnView::Property::SPINE_SHADOW, Vector2(70.f, 30.f) );
  mPageTurnPortraitView.PageTurnStartedSignal().Connect( this, &PageTurnController::OnPageStartedTurn );
  mPageTurnPortraitView.PageTurnFinishedSignal().Connect( this, &PageTurnController::OnPageFinishedTurn );
  mPageTurnPortraitView.PagePanStartedSignal().Connect( this, &PageTurnController::OnPageStartedPan );
  mPageTurnPortraitView.PagePanFinishedSignal().Connect( this, &PageTurnController::OnPageFinishedPan );

  mPageTurnLandscapeView = PageTurnLandscapeView::New( mLandscapePageFactory, Vector2(bookSize.y*0.5f, bookSize.x) );
  mPageTurnLandscapeView.SetParentOrigin( ParentOrigin::CENTER );
  mPageTurnLandscapeView.SetAnchorPoint( AnchorPoint::CENTER );
  mPageTurnLandscapeView.PageTurnStartedSignal().Connect( this, &PageTurnController::OnPageStartedTurn );
  mPageTurnLandscapeView.PageTurnFinishedSignal().Connect( this, &PageTurnController::OnPageFinishedTurn );
  mPageTurnLandscapeView.PagePanStartedSignal().Connect( this, &PageTurnController::OnPageStartedPan );
  mPageTurnLandscapeView.PagePanFinishedSignal().Connect( this, &PageTurnController::OnPageFinishedPan );

  if( stageSize.x > stageSize.y )
  {
    stage.Add(mPageTurnLandscapeView);
    mPageTurnPortraitView.SetOrientation(Degree(90.f), Vector3::ZAXIS);
    mIsPortrait = false;
  }
  else
  {
    stage.Add(mPageTurnPortraitView);
    mPageTurnLandscapeView.SetOrientation(Degree(90.f), Vector3::ZAXIS);
    mIsPortrait = true;
  }

  mButtonLayer.RaiseToTop();
}

bool PageTurnController::OnButtonClicked(Toolkit::Button button)
{
  if( mIsPortrait )
  {
    mPageTurnPortraitView.Unparent();
    Stage::GetCurrent().Add( mPageTurnLandscapeView );
    int pageId = mPageTurnPortraitView.GetProperty( PageTurnView::Property::CURRENT_PAGE_ID ).Get<int>();
    int currentPage = ceil( static_cast<float>(pageId) / PAGE_NUMBER_CORRESPONDING_RATIO );
    mPageTurnLandscapeView.SetProperty(PageTurnView::Property::CURRENT_PAGE_ID, currentPage );
  }
  else
  {
    mPageTurnLandscapeView.Unparent();
    Stage::GetCurrent().Add( mPageTurnPortraitView );
    int pageId = mPageTurnLandscapeView.GetProperty( PageTurnView::Property::CURRENT_PAGE_ID ).Get<int>();
    int currentPage = floor(pageId * PAGE_NUMBER_CORRESPONDING_RATIO );
    mPageTurnPortraitView.SetProperty(PageTurnView::Property::CURRENT_PAGE_ID, currentPage );
  }

  mIsPortrait = !mIsPortrait;
  mButtonLayer.RaiseToTop();
  return true;
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
  Application app = Application::New(&argc, &argv, DALI_DEMO_THEME_PATH);
  PageTurnController test ( app );

  app.MainLoop();

  return 0;
}

