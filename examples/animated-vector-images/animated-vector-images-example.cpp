/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <string>
#include "shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/animated-vector-image-visual-actions-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* BACKGROUND_IMAGE( DEMO_IMAGE_DIR "background-gradient.jpg" );
const char* TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char* APPLICATION_TITLE( "Animated Vector Images" );

const char* IMAGE_PATH[] = {
    DEMO_IMAGE_DIR "insta_camera.json",
    DEMO_IMAGE_DIR "you're_in!.json",
    DEMO_IMAGE_DIR "jolly_walker.json"
};

const unsigned int NUMBER_OF_IMAGES = 3;

enum CellPlacement
{
  TOP_BUTTON,
  LOWER_BUTTON,
  IMAGE,
  NUMBER_OF_ROWS
};

unsigned int GetButtonIndex( Button button )
{
  std::string buttonName = button.GetName();
  unsigned int index = 0;

  if ( buttonName != "")
  {
    index = std::stoul( buttonName );
  }

  return index;
}

}  // namespace

// This example shows the usage of AnimatedVectorImageVisual.
// It doesn't work on Ubuntu because the visual uses the external library to render frames.
class AnimatedVectorImageViewController: public ConnectionTracker
{
 public:

  AnimatedVectorImageViewController( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &AnimatedVectorImageViewController::Create );
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            APPLICATION_TITLE );


    // Create a table view to show a pair of buttons above each image.
    mTable = TableView::New( CellPlacement::NUMBER_OF_ROWS, NUMBER_OF_IMAGES );
    mTable.SetAnchorPoint( AnchorPoint::CENTER );
    mTable.SetParentOrigin( ParentOrigin::CENTER );
    mTable.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    Vector3 offset( 0.9f, 0.70f, 0.0f );
    mTable.SetSizeModeFactor( offset );
    mTable.SetFitHeight(CellPlacement::TOP_BUTTON);
    mTable.SetFitHeight(CellPlacement::LOWER_BUTTON);
    mContentLayer.Add( mTable );

    for( unsigned int x = 0; x < NUMBER_OF_IMAGES; x++ )
    {
      mPlayButtons[x] = PushButton::New();
      mPlayButtons[x].SetProperty( Button::Property::LABEL, "Play" );
      mPlayButtons[x].SetParentOrigin( ParentOrigin::TOP_CENTER );
      mPlayButtons[x].SetAnchorPoint( AnchorPoint::TOP_CENTER );
      mPlayButtons[x].ClickedSignal().Connect( this, &AnimatedVectorImageViewController::OnPlayButtonClicked );
      mPlayButtons[x].SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      mPlayButtons[x].SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      std::string s = std::to_string(x);
      mPlayButtons[x].SetName( s );
      mTable.AddChild( mPlayButtons[x], TableView::CellPosition( CellPlacement::TOP_BUTTON, x )  );

      mPauseButtons[x] = PushButton::New();
      mPauseButtons[x].SetProperty( Button::Property::LABEL, "Pause" );
      mPauseButtons[x].SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
      mPauseButtons[x].SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
      mPauseButtons[x].ClickedSignal().Connect( this, &AnimatedVectorImageViewController::OnPauseButtonClicked );
      mPauseButtons[x].SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      mPauseButtons[x].SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      mPauseButtons[x].SetName( s );
      mTable.AddChild( mPauseButtons[x], TableView::CellPosition( CellPlacement::LOWER_BUTTON, x )  );

      mImageViews[x] = ImageView::New( );
      Property::Map imagePropertyMap;
      imagePropertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
      imagePropertyMap.Insert( ImageVisual::Property::URL,  IMAGE_PATH[ x ]  );
      mImageViews[x].SetProperty( ImageView::Property::IMAGE , imagePropertyMap );

      mImageViews[x].SetParentOrigin( ParentOrigin::CENTER );
      mImageViews[x].SetAnchorPoint( AnchorPoint::CENTER );
      mImageViews[x].SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

      mTable.AddChild( mImageViews[x], TableView::CellPosition( CellPlacement::IMAGE, x ) );

      // Set changeable counter and toggle for each ImageView
      mImageViewPlayStatus[x] = false;
      mImageViewPauseStatus[x] = false;
    }

    Stage::GetCurrent().KeyEventSignal().Connect(this, &AnimatedVectorImageViewController::OnKeyEvent);
  }

private:

  bool OnPlayButtonClicked( Button button )
  {
    unsigned int buttonIndex = GetButtonIndex( button );

    ImageView imageView =  mImageViews[buttonIndex];
    if( !mImageViewPlayStatus[buttonIndex] )
    {
      mPlayButtons[buttonIndex].SetProperty( Button::Property::LABEL, "Stop" );

      DevelControl::DoAction( imageView, ImageView::Property::IMAGE, DevelAnimatedVectorImageVisual::Action::PLAY, Property::Value() );
    }
    else
    {
      mPlayButtons[buttonIndex].SetProperty( Button::Property::LABEL, "Play" );

      DevelControl::DoAction( imageView, ImageView::Property::IMAGE, DevelAnimatedVectorImageVisual::Action::STOP, Property::Value() );
    }

    mImageViewPlayStatus[buttonIndex] = !mImageViewPlayStatus[buttonIndex];

    return true;
  }

  bool OnPauseButtonClicked( Button button )
  {
    unsigned int buttonIndex = GetButtonIndex( button );

    ImageView imageView =  mImageViews[buttonIndex];
    if( !mImageViewPauseStatus[buttonIndex] )
    {
      mPauseButtons[buttonIndex].SetProperty( Button::Property::LABEL, "Resume" );

      DevelControl::DoAction( imageView, ImageView::Property::IMAGE, DevelAnimatedVectorImageVisual::Action::PAUSE, Property::Value() );
    }
    else
    {
      mPauseButtons[buttonIndex].SetProperty( Button::Property::LABEL, "Pause" );

      DevelControl::DoAction( imageView, ImageView::Property::IMAGE, DevelAnimatedVectorImageVisual::Action::RESUME, Property::Value() );
    }

    mImageViewPauseStatus[buttonIndex] = !mImageViewPauseStatus[buttonIndex];

    return true;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;

  Control           mView;                              ///< The View instance.
  ToolBar           mToolBar;                           ///< The View's Toolbar.
  Layer             mContentLayer;                      ///< Content layer
  TableView         mTable;
  ImageView         mImageViews[ NUMBER_OF_IMAGES ];
  PushButton        mPlayButtons[ NUMBER_OF_IMAGES ];
  PushButton        mPauseButtons[ NUMBER_OF_IMAGES ];
  bool              mImageViewPlayStatus[ NUMBER_OF_IMAGES ];
  bool              mImageViewPauseStatus[ NUMBER_OF_IMAGES ];

};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  AnimatedVectorImageViewController test( application );
  application.MainLoop();
  return 0;
}
