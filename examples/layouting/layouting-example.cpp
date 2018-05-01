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
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/hbox-layout.h>
#include <dali-toolkit/devel-api/layouting/vbox-layout.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* BACKGROUND_IMAGE( DEMO_IMAGE_DIR "lake_front.jpg" );
const char* TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );

const char* LTR_IMAGE( DEMO_IMAGE_DIR "icon-play.png" );
const char* LTR_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-play-selected.png" );

const char* RTL_IMAGE( DEMO_IMAGE_DIR "icon-reverse.png" );
const char* RTL_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-reverse-selected.png" );

const char* ROTATE_CLOCKWISE_IMAGE( DEMO_IMAGE_DIR "icon-reset.png" );
const char* ROTATE_CLOCKWISE_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-reset-selected.png" );

//const char* ROTATE_ANTICLOCKWISE_IMAGE( DEMO_IMAGE_DIR "icon-rotate-anticlockwise.png" );
//const char* ROTATE_ANTICLOCKWISE_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-rotate-anticlockwise-selected.png" );


const char* APPLICATION_TITLE( "Layout tester" );

const char* IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "application-icon-101.png",
  DEMO_IMAGE_DIR "application-icon-102.png",
  DEMO_IMAGE_DIR "application-icon-103.png",
  DEMO_IMAGE_DIR "application-icon-104.png"
};
const unsigned int NUMBER_OF_RESOURCES = sizeof(IMAGE_PATH) / sizeof(char*);

}  // namespace

class LayoutingExample: public ConnectionTracker
{
 public:

  LayoutingExample( Application& application )
  : mApplication( application ),
    mDirection( false )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &LayoutingExample::Create );
  }

  ~LayoutingExample()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime
    auto stage = Stage::GetCurrent();

    auto bg = ImageView::New( BACKGROUND_IMAGE );
    bg.SetParentOrigin( ParentOrigin::CENTER );
    stage.Add( bg );
    auto toolbar = ImageView::New( TOOLBAR_IMAGE );
    toolbar.SetParentOrigin( ParentOrigin::TOP_CENTER );
    toolbar.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    toolbar.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    toolbar.SetProperty( Actor::Property::SIZE_HEIGHT, 50.0f);

    stage.Add( toolbar );

    auto title = TextLabel::New( APPLICATION_TITLE );
    title.SetParentOrigin( ParentOrigin::CENTER );
    title.SetAnchorPoint( AnchorPoint::CENTER );
    title.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLUE );
    title.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::CENTER );
    toolbar.Add( title );

    mDirectionButton = PushButton::New();
    mDirectionButton.SetProperty( PushButton::Property::UNSELECTED_ICON, RTL_IMAGE );
    mDirectionButton.SetProperty( PushButton::Property::SELECTED_ICON, RTL_SELECTED_IMAGE );
    mDirectionButton.ClickedSignal().Connect( this, &LayoutingExample::OnDirectionClicked );
    mDirectionButton.SetParentOrigin( Vector3(0.33f, 1.0f, 0.5f ) );
    mDirectionButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mDirectionButton.SetSize(75, 75);
    stage.Add( mDirectionButton );

    mRotateButton = PushButton::New();
    mRotateButton.SetProperty( PushButton::Property::UNSELECTED_ICON, ROTATE_CLOCKWISE_IMAGE );
    mRotateButton.SetProperty( PushButton::Property::SELECTED_ICON, ROTATE_CLOCKWISE_SELECTED_IMAGE );
    mRotateButton.ClickedSignal().Connect( this, &LayoutingExample::OnRotateClicked );
    mRotateButton.SetParentOrigin( Vector3(0.66f, 1.0f, 0.5f ));
    mRotateButton.SetAnchorPoint( Vector3(0.5f, 1.0f, 0.5f));
    mRotateButton.SetSize(75, 75);
    stage.Add( mRotateButton );

    // Create a hbox layout
    mLinearContainer = Control::New();
    mIsHorizontal = false;

    auto layout = VboxLayout::New();
    layout.SetAnimateLayout(true);
    DevelControl::SetLayout( mLinearContainer, layout );

    mLinearContainer.SetParentOrigin( ParentOrigin::CENTER );
    mLinearContainer.SetAnchorPoint( AnchorPoint::CENTER );
    mLinearContainer.SetName( "linearContainer" );
    stage.Add( mLinearContainer );
    mLinearContainer.SetProperty( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
    mLinearContainer.SetProperty( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
    mLinearContainer.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT );

    for( unsigned int x = 0; x < NUMBER_OF_RESOURCES; ++x )
    {
      mImageViews[x] = Toolkit::ImageView::New();
      Property::Map imagePropertyMap;
      imagePropertyMap[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
      imagePropertyMap[ Toolkit::ImageVisual::Property::URL ] = IMAGE_PATH[ x ];
      imagePropertyMap[ ImageVisual::Property::DESIRED_WIDTH ] = 100.0f;
      imagePropertyMap[ ImageVisual::Property::DESIRED_HEIGHT ] = 100.0f;
      mImageViews[x].SetProperty(Toolkit::ImageView::Property::IMAGE , imagePropertyMap );
      mImageViews[x].SetName("ImageView");
      mImageViews[x].SetAnchorPoint( AnchorPoint::TOP_LEFT );
      mImageViews[x].SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

      if( 1 == x )
      {
        mImageViews[x].SetProperty(Toolkit::Control::Property::PADDING, Extents(10.0f,10.0f,5.0f, 5.0f));
      }

      if( 0 == x )
      {
        mImageViews[x].SetProperty(Toolkit::Control::Property::MARGIN, Extents(10.0f,10.0f,5.0f, 5.0f));
      }

      mLinearContainer.Add( mImageViews[x] );
    }

    Stage::GetCurrent().KeyEventSignal().Connect(this, &LayoutingExample::OnKeyEvent);
  }

private:
  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

  bool OnDirectionClicked( Button button )
  {
    if( mDirection )
    {
      mDirectionButton.SetProperty( PushButton::Property::UNSELECTED_ICON, LTR_IMAGE );
      mDirectionButton.SetProperty( PushButton::Property::SELECTED_ICON, LTR_SELECTED_IMAGE );
      mLinearContainer.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT );
    }
    else
    {
      mDirectionButton.SetProperty( PushButton::Property::UNSELECTED_ICON, RTL_IMAGE );
      mDirectionButton.SetProperty( PushButton::Property::SELECTED_ICON, RTL_SELECTED_IMAGE );
      mLinearContainer.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT );
    }
    mDirection = !mDirection;
    return true;
  }

  bool OnRotateClicked( Button button )
  {
    mIsHorizontal = !mIsHorizontal;
    if( mIsHorizontal )
    {
      auto hboxLayout = HboxLayout::New();
      hboxLayout.SetAnimateLayout(true);
      DevelControl::SetLayout( mLinearContainer, hboxLayout );
    }
    else
    {
      auto vboxLayout = VboxLayout::New();
      vboxLayout.SetAnimateLayout(true);
      DevelControl::SetLayout( mLinearContainer, vboxLayout );
    }
    return true;
  }

private:
  Application& mApplication;
  Toolkit::ImageView mImageViews[ NUMBER_OF_RESOURCES ];
  PushButton mDirectionButton;
  PushButton mRotateButton;
  Control mLinearContainer;
  bool mDirection;
  bool mIsHorizontal = true;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  LayoutingExample test( application );
  application.MainLoop();
  return 0;
}
