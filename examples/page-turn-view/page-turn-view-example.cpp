/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/page-turn-view/page-factory.h>
#include <dali-toolkit/devel-api/controls/page-turn-view/page-turn-landscape-view.h>
#include <dali-toolkit/devel-api/controls/page-turn-view/page-turn-portrait-view.h>
#include <dali-toolkit/devel-api/controls/page-turn-view/page-turn-view.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* BOOK_COVER_PORTRAIT = ( DEMO_IMAGE_DIR "book-portrait-cover.jpg" );
const char* BOOK_COVER_LANDSCAPE = ( DEMO_IMAGE_DIR "book-landscape-cover.jpg" );
const char* BOOK_COVER_BACK_LANDSCAPE = ( DEMO_IMAGE_DIR "book-landscape-cover-back.jpg" );

const char* PAGE_IMAGES_PORTRAIT[] =
{
  DEMO_IMAGE_DIR "book-portrait-p1.jpg",
  DEMO_IMAGE_DIR "book-portrait-p2.jpg",
  DEMO_IMAGE_DIR "book-portrait-p3.jpg",
  DEMO_IMAGE_DIR "book-portrait-p4.jpg",
  DEMO_IMAGE_DIR "book-portrait-p5.jpg",
  DEMO_IMAGE_DIR "book-portrait-p6.jpg",
  DEMO_IMAGE_DIR "book-portrait-p7.jpg",
  DEMO_IMAGE_DIR "book-portrait-p8.jpg"
};
const unsigned int NUMBER_OF_PORTRAIT_IMAGE( sizeof(PAGE_IMAGES_PORTRAIT) / sizeof(PAGE_IMAGES_PORTRAIT[0]) );

const char* PAGE_IMAGES_LANDSCAPE[] =
{
  DEMO_IMAGE_DIR "book-landscape-p1.jpg",
  DEMO_IMAGE_DIR "book-landscape-p2.jpg",
  DEMO_IMAGE_DIR "book-landscape-p3.jpg",
  DEMO_IMAGE_DIR "book-landscape-p4.jpg",
  DEMO_IMAGE_DIR "book-landscape-p5.jpg",
  DEMO_IMAGE_DIR "book-landscape-p6.jpg",
  DEMO_IMAGE_DIR "book-landscape-p7.jpg",
  DEMO_IMAGE_DIR "book-landscape-p8.jpg"
};
const unsigned int NUMBER_OF_LANDSCAPE_IMAGE( sizeof(PAGE_IMAGES_LANDSCAPE) / sizeof(PAGE_IMAGES_LANDSCAPE[0]) );

enum DemoOrientation
{
  PORTRAIT,
  LANDSCAPE,
  UNKNOWN
};

}

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
   * Create texture to represent a page.
   * @param[in] pageId The ID of the page to create.
   * @return The texture.
   */
  virtual Texture NewPage( unsigned int pageId )
  {
    Texture texture;

    PixelData pixels;
    if( pageId == 0 )
    {
      pixels = SyncImageLoader::Load( BOOK_COVER_PORTRAIT );
    }
    else
    {
      pixels = SyncImageLoader::Load( PAGE_IMAGES_PORTRAIT[ (pageId-1) % NUMBER_OF_PORTRAIT_IMAGE ] );
    }

    if( pixels )
    {
      texture = Texture::New( TextureType::TEXTURE_2D, pixels.GetPixelFormat(), pixels.GetWidth(), pixels.GetHeight() );
      texture.Upload( pixels, 0, 0, 0, 0, pixels.GetWidth(), pixels.GetHeight() );
    }

    return texture;
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
   * Create texture to represent a page.
   * @param[in] pageId The ID of the page to create.
   * @return The texture.
   */
  virtual Texture NewPage( unsigned int pageId )
  {
    Texture texture;

    PixelData pixelsFront;
    PixelData pixelsBack;

    if( pageId == 0 )
    {
       pixelsFront = SyncImageLoader::Load( BOOK_COVER_LANDSCAPE );
       pixelsBack  = SyncImageLoader::Load( BOOK_COVER_BACK_LANDSCAPE );
    }
    else
    {
      unsigned int imageId = (pageId-1)*2;
      pixelsFront = SyncImageLoader::Load( PAGE_IMAGES_LANDSCAPE[ imageId     % NUMBER_OF_LANDSCAPE_IMAGE ] );
      pixelsBack  = SyncImageLoader::Load( PAGE_IMAGES_LANDSCAPE[ (imageId+1) % NUMBER_OF_LANDSCAPE_IMAGE ] );
    }

    if( pixelsFront && pixelsBack )
    {
      texture = Texture::New( TextureType::TEXTURE_2D, pixelsFront.GetPixelFormat(), pixelsFront.GetWidth()*2, pixelsFront.GetHeight() );
      texture.Upload( pixelsFront, 0, 0, 0, 0, pixelsFront.GetWidth(), pixelsFront.GetHeight() );
      texture.Upload( pixelsBack,  0, 0, pixelsFront.GetWidth(), 0, pixelsBack.GetWidth(), pixelsBack.GetHeight() );
    }

    return texture;
  }
};

/**
 * This example shows how to use the PageTurnView UI control
 */
class PageTurnExample : public ConnectionTracker
{
public:

  PageTurnExample( Application &app );

  ~PageTurnExample();

  void OnInit( Application& app );

private:


  void OnWindowResized( Window::WindowSize size );

  void Rotate( DemoOrientation orientation );

  void OnKeyEvent(const KeyEvent& event);

private:

  Application&                mApplication;

  PageTurnView                mPageTurnPortraitView;
  PageTurnView                mPageTurnLandscapeView;
  PortraitPageFactory         mPortraitPageFactory;
  LandscapePageFactory        mLandscapePageFactory;

  DemoOrientation             mOrientation;
};

PageTurnExample::PageTurnExample( Application &app )
: mApplication( app ),
  mOrientation( UNKNOWN )
{
  app.InitSignal().Connect( this, &PageTurnExample::OnInit );
}

PageTurnExample::~PageTurnExample()
{
}

/**
 * The Init signal is received once (only) during the Application lifetime
 */
void PageTurnExample::OnInit( Application& app )
{
  Stage::GetCurrent().KeyEventSignal().Connect(this, &PageTurnExample::OnKeyEvent);

  Window window = app.GetWindow();
  window.AddAvailableOrientation( Window::PORTRAIT );
  window.AddAvailableOrientation( Window::LANDSCAPE );
  window.AddAvailableOrientation( Window::PORTRAIT_INVERSE  );
  window.AddAvailableOrientation( Window::LANDSCAPE_INVERSE );
  window.ResizedSignal().Connect( this, &PageTurnExample::OnWindowResized );

  Window::WindowSize size = window.GetSize();
  Rotate( size.GetWidth() > size.GetHeight() ? LANDSCAPE : PORTRAIT );
}

void PageTurnExample::OnWindowResized( Window::WindowSize size )
{
  Rotate( size.GetWidth() > size.GetHeight() ? LANDSCAPE : PORTRAIT );
}

void PageTurnExample::Rotate( DemoOrientation orientation )
{
  Stage stage = Stage::GetCurrent();
  Vector2 stageSize = stage.GetSize();

  if( mOrientation != orientation )
  {
    mOrientation = orientation;

    if( PORTRAIT == orientation )
    {
      if( !mPageTurnPortraitView )
      {
        mPageTurnPortraitView = PageTurnPortraitView::New( mPortraitPageFactory, stageSize );
        mPageTurnPortraitView.SetParentOrigin( ParentOrigin::CENTER );
      }

      if( mPageTurnLandscapeView )
      {
        stage.Remove( mPageTurnLandscapeView );
      }
      stage.Add( mPageTurnPortraitView );
    }
    else if( LANDSCAPE == orientation )
    {
      if( !mPageTurnLandscapeView )
      {
        mPageTurnLandscapeView = PageTurnLandscapeView::New( mLandscapePageFactory, Vector2(stageSize.x*0.5f, stageSize.y) );
        mPageTurnLandscapeView.SetParentOrigin( ParentOrigin::CENTER );
      }

      if( mPageTurnPortraitView )
      {
        stage.Remove( mPageTurnPortraitView );
      }

      stage.Add( mPageTurnLandscapeView );
    }
  }
}

/**
 * Main key event handler
 */
void PageTurnExample::OnKeyEvent(const KeyEvent& event)
{
  if(event.state == KeyEvent::Down)
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
      mApplication.Quit();
    }
  }
}

// Entry point for applications
int main( int argc, char **argv )
{
  Application app = Application::New(&argc, &argv);
  PageTurnExample test ( app );

  app.MainLoop();

  return 0;
}

