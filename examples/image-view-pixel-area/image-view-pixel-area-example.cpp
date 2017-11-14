/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>

#include "shared/view.h"

using namespace Dali;

namespace
{
const char* BIG_TEST_IMAGE( DEMO_IMAGE_DIR "book-landscape-cover.jpg" );
const char* SMALL_TEST_IMAGE( DEMO_IMAGE_DIR "gallery-large-1.jpg" );

const char * const APPLICATION_TITLE( "Pixel Area & Wrap Mode" );
const char * const TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char * const BUTTON_ICON( DEMO_IMAGE_DIR "icon-change.png" );
const char * const BUTTON_ICON_SELECTED( DEMO_IMAGE_DIR "icon-change-selected.png" );

const Vector4 ORIGINAL_PIXEL_AREA( -0.5f, -0.5f, 2.f, 2.f );
}

class ImageViewPixelAreaApp : public ConnectionTracker
{
public:
  ImageViewPixelAreaApp( Application& application )
  : mApplication( application ),
    mIndex(0u)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageViewPixelAreaApp::Create );
  }

  ~ImageViewPixelAreaApp()
  {
    // Nothing to do here;
  }

private:
  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &ImageViewPixelAreaApp::OnKeyEvent);

    Toolkit::ToolBar toolBar;
    Toolkit::Control background;
    // Creates a default view with a default tool bar.
    mContent = DemoHelper::CreateView( application,
                                            background,
                                            toolBar,
                                            "",
                                            TOOLBAR_IMAGE,
                                            APPLICATION_TITLE );

    // Add a button to switch the scene. (right of toolbar)
    Toolkit::PushButton switchButton = Toolkit::PushButton::New();
    switchButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, BUTTON_ICON );
    switchButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, BUTTON_ICON_SELECTED );
    switchButton.ClickedSignal().Connect( this, &ImageViewPixelAreaApp::OnButtonClicked );
    toolBar.AddControl( switchButton,
                        DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage,
                        Toolkit::Alignment::HorizontalRight,
                        DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );


    // for testing image WITH automatic atlasing
    visualPropertyMap[0][ Toolkit::ImageVisual::Property::URL ] =  SMALL_TEST_IMAGE;
    visualPropertyMap[0][ Toolkit::ImageVisual::Property::DESIRED_WIDTH ] = 500;
    visualPropertyMap[0][ Toolkit::ImageVisual::Property::DESIRED_HEIGHT ] = 500;
    visualPropertyMap[0][ Toolkit::ImageVisual::Property::WRAP_MODE_U ] = WrapMode::CLAMP_TO_EDGE;
    visualPropertyMap[0][ Toolkit::ImageVisual::Property::WRAP_MODE_V ] = WrapMode::MIRRORED_REPEAT;
    visualPropertyMap[0][ Toolkit::ImageVisual::Property::PIXEL_AREA ] = ORIGINAL_PIXEL_AREA;

    // for testing image WITHOUT automatic atlasing
    visualPropertyMap[1][ Toolkit::ImageVisual::Property::URL ] =  BIG_TEST_IMAGE;
    visualPropertyMap[1][ Toolkit::ImageVisual::Property::DESIRED_WIDTH ] = 640;
    visualPropertyMap[1][ Toolkit::ImageVisual::Property::DESIRED_HEIGHT ] = 720;
    visualPropertyMap[1][ Toolkit::ImageVisual::Property::WRAP_MODE_U ] = WrapMode::MIRRORED_REPEAT;
    visualPropertyMap[1][ Toolkit::ImageVisual::Property::WRAP_MODE_V ] = WrapMode::REPEAT;
    visualPropertyMap[1][ Toolkit::ImageVisual::Property::PIXEL_AREA ] = ORIGINAL_PIXEL_AREA;

    CreateScene(  visualPropertyMap[0] );

    mWrapLabel = Toolkit::TextLabel::New(" Automatic atlasing\n WrapMode: CLAMP_TO_EDGE, MIRRORED_REPEAT");
    mWrapLabel.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mWrapLabel.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER );
    mWrapLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mWrapLabel.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, true );
    mWrapLabel.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
    mContent.Add( mWrapLabel );

    mPixelAreaLabel = Toolkit::TextLabel::New( " Use ImageVisual::Property::PIXEL_AREA\n " );
    mPixelAreaLabel.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mPixelAreaLabel.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER );
    mPixelAreaLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mPixelAreaLabel.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, true );
    mPixelAreaLabel.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
    mWrapLabel.Add( mPixelAreaLabel );
  }

  void CreateScene( const Property::Value& propertyMap )
  {
    for( int i=0; i<3;i++ )
      for( int j=0; j<3; j++ )
      {
        mImageView[i][j] = Toolkit::ImageView::New();
        mImageView[i][j].SetProperty( Toolkit::ImageView::Property::IMAGE, propertyMap );
        mImageView[i][j].SetPosition( 50.f*(i-1), 50.f*(j-1) );
      }

    mImageView[1][1].SetParentOrigin( ParentOrigin::CENTER );
    mImageView[1][1].SetAnchorPoint(AnchorPoint::CENTER );
    mImageView[1][1].SetScale( 1.f/3.f );
    mContent.Add( mImageView[1][1] );

    mImageView[0][0].SetParentOrigin( ParentOrigin::TOP_LEFT );
    mImageView[0][0].SetAnchorPoint(AnchorPoint::BOTTOM_RIGHT );
    mImageView[0][0].SetPosition( -50.f, -50.f );
    mImageView[1][1].Add( mImageView[0][0] );

    mImageView[1][0].SetParentOrigin( ParentOrigin::TOP_CENTER );
    mImageView[1][0].SetAnchorPoint(AnchorPoint::BOTTOM_CENTER );
    mImageView[1][1].Add( mImageView[1][0] );

    mImageView[2][0].SetParentOrigin( ParentOrigin::TOP_RIGHT );
    mImageView[2][0].SetAnchorPoint(AnchorPoint::BOTTOM_LEFT );
    mImageView[1][1].Add( mImageView[2][0] );

    mImageView[0][1].SetParentOrigin( ParentOrigin::CENTER_LEFT );
    mImageView[0][1].SetAnchorPoint(AnchorPoint::CENTER_RIGHT );
    mImageView[1][1].Add( mImageView[0][1] );

    mImageView[2][1].SetParentOrigin( ParentOrigin::CENTER_RIGHT );
    mImageView[2][1].SetAnchorPoint(AnchorPoint::CENTER_LEFT );
    mImageView[1][1].Add( mImageView[2][1] );

    mImageView[0][2].SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    mImageView[0][2].SetAnchorPoint(AnchorPoint::TOP_RIGHT );
    mImageView[1][1].Add( mImageView[0][2] );

    mImageView[1][2].SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mImageView[1][2].SetAnchorPoint(AnchorPoint::TOP_CENTER );
    mImageView[1][1].Add( mImageView[1][2] );

    mImageView[2][2].SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );
    mImageView[2][2].SetAnchorPoint(AnchorPoint::TOP_LEFT );
    mImageView[1][1].Add( mImageView[2][2] );

  }

  bool OnButtonClicked( Toolkit::Button button )
    {
      if( mAnimation )
      {
        mAnimation.Stop();
        mAnimation.Clear();
      }

      mIndex = ( mIndex+1 ) % 4;
      if( mIndex%2 == 0 )
      {
        // switch to the other image
        // set the pixel area to image visual, the pixel area property is registered on the renderer
        mContent.Remove( mImageView[1][1] );
        CreateScene(  visualPropertyMap[mIndex/2] );
        if( mIndex == 0 )
        {
          mWrapLabel.SetProperty( Toolkit::TextLabel::Property::TEXT," Automatic atlasing\n WrapMode: CLAMP_TO_EDGE, MIRRORED_REPEAT");
        }
        else
        {
          mWrapLabel.SetProperty( Toolkit::TextLabel::Property::TEXT," No atlasing\n WrapMode: MIRRORED_REPEAT, REPEAT");
        }
        mPixelAreaLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, " Use ImageVisual::Property::PIXEL_AREA\n " );
      }
      else
      {
        // animate the pixel area property on image view,
        // the animatable pixel area property is registered on the actor, which overwrites the property on the renderer
        mAnimation = Animation::New( 10.f );
        float relativeSubSize = 0.33;
        for( int i=0; i<3;i++ )
          for( int j=0; j<3; j++ )
          {
            mImageView[i][j].SetProperty( Toolkit::ImageView::Property::PIXEL_AREA, ORIGINAL_PIXEL_AREA );
            mAnimation.AnimateTo( Property(mImageView[i][j], Toolkit::ImageView::Property::PIXEL_AREA),
                Vector4( relativeSubSize*i, relativeSubSize*j, relativeSubSize, relativeSubSize ),
                AlphaFunction::BOUNCE );
          }
        mAnimation.SetLooping( true );
        mAnimation.Play();

        mPixelAreaLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, " Animate ImageView::Property::PIXEL_AREA \n     (Overwrite the ImageVisual property) " );
      }
      return true;
    }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;
  Layer mContent;
  Toolkit::ImageView mImageView[3][3];
  Property::Map visualPropertyMap[2];
  Toolkit::TextLabel mWrapLabel;
  Toolkit::TextLabel mPixelAreaLabel;
  Animation mAnimation;
  unsigned int mIndex;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  ImageViewPixelAreaApp test( application );
  application.MainLoop();
  return 0;
}
