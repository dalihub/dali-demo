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

#include <dali/dali.h>
#include <dali/devel-api/images/atlas.h>

#include "shared/view.h"
#include <iostream>
#include <cstdio>

using namespace Dali;

class AtlasController;

namespace
{
const char * const BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-gradient.jpg" );
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char * const LOSE_CONTEXT_IMAGE( DALI_IMAGE_DIR "icon-cluster-wobble.png" );
const char * const LOSE_CONTEXT_IMAGE_SELECTED( DALI_IMAGE_DIR "icon-cluster-wobble-selected.png" );

Application gApplication;
AtlasController* gAtlasController(NULL);
}

class AtlasController : public ConnectionTracker
{
public:

  AtlasController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &AtlasController::Create );
  }

  ~AtlasController()
  {
    // Nothing to do here;
  }

  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor(Color::YELLOW);

    // Respond to a click anywhere on the stage
    stage.KeyEventSignal().Connect(this, &AtlasController::OnKeyEvent);

    mApplication.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    mContentLayer = DemoHelper::CreateView( mApplication,
        mView,
        mToolBar,
        BACKGROUND_IMAGE,
        TOOLBAR_IMAGE,
        "Atlas" );

    mLoseContextButton = Toolkit::PushButton::New();
    mLoseContextButton.SetUnselectedImage( LOSE_CONTEXT_IMAGE );
    mLoseContextButton.SetSelectedImage( LOSE_CONTEXT_IMAGE_SELECTED );
    mLoseContextButton.ClickedSignal().Connect( this, &AtlasController::OnLoseContextButtonClicked );
    mToolBar.AddControl( mLoseContextButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

    mAtlas = Atlas::New( 400,300, Pixel::RGBA8888);
    mAtlas.Clear(Vector4(0.f,0.5f,0.5f,0.5f));
    mAtlas.Upload( DALI_IMAGE_DIR "icon-change.png", 50, 30 );
    mAtlas.Upload( DALI_IMAGE_DIR "icon-cluster-carousel.png", 100, 30 );
    mAtlas.Upload( DALI_IMAGE_DIR "icon-effects-on.png", 150, 30 );
    mAtlas.Upload( DALI_IMAGE_DIR "icon-effect-cross.png", 100, 80 );
    mAtlas.Upload( DALI_IMAGE_DIR "icon-effect-fold.png", 150, 80 );
    mAtlas.Upload( DALI_IMAGE_DIR "icon-effect-wave.png", 200, 80 );
    mAtlas.Upload( DALI_IMAGE_DIR "icon-item-view-layout-depth.png", 150, 130 );
    mAtlas.Upload( DALI_IMAGE_DIR "icon-item-view-layout-grid.png", 200, 130 );
    mAtlas.Upload( DALI_IMAGE_DIR "icon-item-view-layout-spiral.png", 250, 130 );
    UploadBufferImages();

    Toolkit::ImageView imageActor1 = Toolkit::ImageView::New( mAtlas );
    imageActor1.SetY(-170.f);
    imageActor1.SetParentOrigin(ParentOrigin::CENTER);
    mContentLayer.Add( imageActor1 );

    Atlas atlas2 = Atlas::New( 400,400, Pixel::RGB888);
    atlas2.Clear( Color::RED );
    atlas2.Upload( DALI_IMAGE_DIR "gallery-small-1.jpg", 4, 4 );
    atlas2.Clear( Color::BLUE );
    atlas2.Upload( DALI_IMAGE_DIR "gallery-small-2.jpg", 136, 4 );
    atlas2.Upload( DALI_IMAGE_DIR "gallery-small-3.jpg", 268, 4 );
    atlas2.Upload( DALI_IMAGE_DIR "gallery-small-4.jpg", 4, 136 );
    atlas2.Upload( DALI_IMAGE_DIR "gallery-small-5.jpg", 136, 136 );
    atlas2.Upload( DALI_IMAGE_DIR "gallery-small-6.jpg", 268, 135 );
    atlas2.Upload( DALI_IMAGE_DIR "gallery-small-7.jpg", 4, 268 );
    atlas2.Upload( DALI_IMAGE_DIR "gallery-small-7.jpg", 136, 268 );
    atlas2.Upload( DALI_IMAGE_DIR "gallery-small-7.jpg", 268, 268 );


    Toolkit::ImageView imageView = Toolkit::ImageView::New( DALI_IMAGE_DIR "gallery-small-1.jpg" );

    imageView.SetY(200.f);
    imageView.SetZ(-1.f);
    imageView.SetParentOrigin(ParentOrigin::CENTER);
    mContentLayer.Add( imageView );

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect( this, &AtlasController::OnPanGesture );
    mPanGestureDetector.Attach( imageActor1 );
    mPanGestureDetector.Attach( imageView );

    stage.ContextLostSignal().Connect( this, &AtlasController::OnContextLost );
    stage.ContextRegainedSignal().Connect( this, &AtlasController::OnContextRegained );
  }

  void UploadBufferImages()
  {
    mAtlas.Upload( CreateBufferImage( Vector4(1.f, 1.f, 1.f, 0.5f  ), 80, 90 ), 0, 210 );
    mAtlas.Upload( CreateBufferImage( Vector4(1.f, 1.f, 0.75f, 0.5f  ), 80, 80 ), 40, 210 );
    mAtlas.Upload( CreateBufferImage( Vector4(1.f, 1.f, 0.5f, 0.5f  ), 80, 70 ), 80, 210 );
    mAtlas.Upload( CreateBufferImage( Vector4(1.f, 1.f, 0.25f, 0.5f  ), 80, 60 ), 120, 210 );
    mAtlas.Upload( CreateBufferImage( Vector4(1.f, 1.f, 0.f, 0.5f  ), 80, 50 ), 160, 210 );
    mAtlas.Upload( CreateBufferImage( Vector4(0.75f, 0.75f, 0.f, 0.5f  ), 80, 40 ), 200, 210 );
    mAtlas.Upload( CreateBufferImage( Vector4(0.5f, 0.5f, 0.f, 0.5f  ), 80, 30 ), 240, 210 );
    mAtlas.Upload( CreateBufferImage( Vector4(0.25f, 0.25f, 0.f, 0.5f  ), 80, 20 ), 280, 210 );
    mAtlas.Upload( CreateBufferImage( Vector4(0.1f, 0.1f, 0.f, 0.5f  ), 80, 10 ), 320, 210 );
    BufferImage redBlock = CreateBufferImage( Color::RED, 40, 40 );
    mAtlas.Upload(redBlock, 320, 30);
    mAtlas.Upload(redBlock, 320, 80);
    mAtlas.Upload(redBlock, 320, 130);
  }

  static void NewWindow(void)
  {
    PositionSize posSize(0, 0, 720, 1280);
    gApplication.ReplaceWindow(posSize, "NewWindow"); // Generates a new window
  }

  bool OnLoseContextButtonClicked( Toolkit::Button button )
  {
    // Add as an idle callback to avoid ProcessEvents being recursively called.
    mApplication.AddIdle(MakeCallback( AtlasController::NewWindow ));
    return true;
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

  void OnPanGesture( Actor actor, const PanGesture& gesture )
  {
    if( gesture.state == Gesture::Continuing )
    {
      actor.TranslateBy( Vector3( gesture.displacement ) );
    }
  }

  void OnContextLost()
  {
    printf("Stage reporting context loss\n");
  }

  void OnContextRegained()
  {
    printf("Stage reporting context regain\n");
    UploadBufferImages();
  }

private:

  BufferImage CreateBufferImage( const Vector4& color, const unsigned int width, const unsigned int height )
  {
    BufferImage imageData = BufferImage::New( width, height, Pixel::RGBA8888 );

    // Create the image
    PixelBuffer* pixbuf = imageData.GetBuffer();
    const unsigned int bitmapSize = width * height;
    for( size_t i = 0; i < bitmapSize; ++i )
    {
      pixbuf[i*4+0] = 0xFF * color.r;
      pixbuf[i*4+1] = 0xFF * color.g;
      pixbuf[i*4+2] = 0xFF * color.b;
      pixbuf[i*4+3] = 0xFF * color.a;
    }

    imageData.Update();

    return imageData;
  }


private:
  Application&  mApplication;
  PanGestureDetector mPanGestureDetector;

  Toolkit::Control           mView;                              ///< The View instance.
  Toolkit::ToolBar           mToolBar;                           ///< The View's Toolbar.
  Layer                      mContentLayer;                      ///< Content layer (scrolling cluster content)
  Toolkit::PushButton        mLoseContextButton;
  Atlas                      mAtlas;
};

void RunTest( Application& application )
{
  gAtlasController = new AtlasController(application);
  application.MainLoop(Configuration::APPLICATION_DOES_NOT_HANDLE_CONTEXT_LOSS);
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  gApplication = Application::New( &argc, &argv, DALI_DEMO_THEME_PATH );

  RunTest( gApplication );

  return 0;
}
