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

#include <string>
#include "shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/hbox-layout.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* IMAGE_PATH[] = {
    DEMO_IMAGE_DIR "gallery-small-23.jpg",
    DEMO_IMAGE_DIR "woodEffect.jpg",
    DEMO_IMAGE_DIR "heartsframe.9.png",
    DEMO_IMAGE_DIR "World.svg"
};

const unsigned int NUMBER_OF_IMAGES = 3;

enum CellPlacement
{
   TOP_BUTTON,
   LOWER_BUTTON,
   IMAGE,
   NUMBER_OF_ROWS
};


unsigned int GetButtonIndex( Toolkit::Button button )
{
  std::string buttonName = button.GetName();
  unsigned int index = 0;

  if ( buttonName != "")
  {
    index = std::stoul( buttonName );
  }

  return index;
}


const unsigned int NUMBER_OF_RESOURCES = sizeof(IMAGE_PATH) / sizeof(char*);

std::string EXAMPLE_INSTRUCTIONS = "Instructions: Change button cycles through different image visuals, "
                           "on/off takes the ImageView and it's current visual on or off stage.";

}  // namespace

class ImageViewController: public ConnectionTracker
{
 public:

  ImageViewController( Application& application )
    : mApplication( application ),
      mCurrentPositionToggle( 0, 0 ),
      mCurrentPositionImage( 0, 0 )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageViewController::Create );
  }

  ~ImageViewController()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    auto stage = Stage::GetCurrent();

    Control toplevel = Control::New();
    toplevel.SetName("topLevelControl");

    toplevel.SetAnchorPoint( AnchorPoint::CENTER );
    toplevel.SetParentOrigin( ParentOrigin::CENTER );
    stage.Add( toplevel);

    // Create a table view to show a pair of buttons above each image.
    mTable = Control::New();
    auto hboxLayout = HboxLayout::New();
    DevelControl::SetLayout( mTable, hboxLayout );
    mTable.SetName("TableControl");

    mTable.SetAnchorPoint( AnchorPoint::CENTER );
    mTable.SetParentOrigin( ParentOrigin::CENTER );
    //mTable.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    //Vector3 offset( 0.9f, 0.70f, 0.0f );
    //mTable.SetSizeModeFactor( offset );
    mTable.SetProperty( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
    mTable.SetProperty( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
    mTable.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT );
    toplevel.Add( mTable );

    Toolkit::TextLabel instructions = Toolkit::TextLabel::New( EXAMPLE_INSTRUCTIONS );
    instructions.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    instructions.SetParentOrigin(ParentOrigin::BOTTOM_CENTER);
    instructions.SetY(-50.0f);
    instructions.SetProperty( Toolkit::TextLabel::Property::ENABLE_AUTO_SCROLL, true  );
    instructions.SetProperty( Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_COUNT, 10  );

    for( unsigned int x = 0; x < NUMBER_OF_IMAGES; x++ )
    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetProperty( Toolkit::Button::Property::LABEL, "on/off" );
      button.SetParentOrigin( ParentOrigin::TOP_CENTER );
      button.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      button.ClickedSignal().Connect( this, &ImageViewController::ToggleImageOnStage );
      button.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      button.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      std::string s = std::to_string(x);
      button.SetName( s );
     // mTable.AddChild( button, Toolkit::TableView::CellPosition( CellPlacement::TOP_BUTTON, x )  );

      Toolkit::PushButton button2 = Toolkit::PushButton::New();
      button2.SetProperty( Toolkit::Button::Property::LABEL, "Change" );
      button2.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
      button2.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
      button2.ClickedSignal().Connect( this, &ImageViewController::ChangeImageClicked );
      button2.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      button2.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      button2.SetName( s );
      stage.Add( button2 );

      mImageViews[x] = Toolkit::ImageView::New( );
      Property::Map imagePropertyMap;
      imagePropertyMap.Insert( Toolkit::Visual::Property::TYPE,  Toolkit::Visual::IMAGE );
      imagePropertyMap.Insert( Toolkit::ImageVisual::Property::URL,  IMAGE_PATH[ 0 ]  );
      imagePropertyMap.Insert( ImageVisual::Property::DESIRED_WIDTH, 100.0f);
      imagePropertyMap.Insert( ImageVisual::Property::DESIRED_HEIGHT,100.0f);

      mImageViews[x].SetProperty(Toolkit::ImageView::Property::IMAGE , imagePropertyMap );
      std::string controlName = "imageView" + std::to_string(x);
      mImageViews[x].SetName( controlName );
      printf("name set to:%s", controlName.c_str() );

      //mImageViews[x].SetParentOrigin( ParentOrigin::CENTER );
      mImageViews[x].SetAnchorPoint( AnchorPoint::TOP_LEFT );
      //mImageViews[x].SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mImageViews[x].SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

      if( 1 == x )
      {
        mImageViews[x].SetProperty(Toolkit::Control::Property::PADDING, Extents(10.0f,10.0f,5.0f, 5.0f));
      }
      if( 0 == x )
      {
        mImageViews[x].SetProperty(Toolkit::Control::Property::MARGIN, Extents(10.0f,10.0f,0.0f, 0.0f));
      }
    //  mTable.AddChild( mImageViews[x], Toolkit::TableView::CellPosition( CellPlacement::IMAGE, x ) );
      mTable.Add( mImageViews[x] );
      // Set changeable counter and toggle for each ImageView
      mImageViewImageIndexStatus[x] = true;
      mImageViewToggleStatus[x] = true;
    }

    Stage::GetCurrent().KeyEventSignal().Connect(this, &ImageViewController::OnKeyEvent);
  }

private:

  void ImmediateLoadImage( const char* urlToLoad )
  {
    Property::Map imagePropertyMap;
    imagePropertyMap.Insert( Toolkit::Visual::Property::TYPE,  Toolkit::Visual::IMAGE );
    imagePropertyMap.Insert( Toolkit::ImageVisual::Property::URL, urlToLoad );
    Toolkit::Visual::Base visual =  Toolkit::VisualFactory::Get().CreateVisual( imagePropertyMap );
    visual.Reset();
  }

  bool ToggleImageOnStage( Toolkit::Button button )
  {
    unsigned int buttonIndex = GetButtonIndex( button );

    Toolkit::ImageView imageView =  mImageViews[ buttonIndex ];

    if( mImageViewToggleStatus[ buttonIndex ] )
    {
      imageView.Unparent();
    }
    else
    {
      //mTable.AddChild( imageView, Toolkit::TableView::CellPosition( 2, GetButtonIndex( button ) ) );
    }

    mImageViewToggleStatus[ buttonIndex ] = !mImageViewToggleStatus[ buttonIndex ];

    return true;
  }

  bool ChangeImageClicked( Toolkit::Button button )
  {
    mImageViews[1].SetProperty(Toolkit::Control::Property::PADDING, Extents(0.0f,0.0f,.0f, 5.0f));
    mImageViews[1].SetProperty(Actor::Property::COLOR_ALPHA, 0.5f);
    return true;
  }

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
      std::cout << ":"<< mImageViews[0].GetCurrentPosition().x << std::endl;
    }
  }

private:
  Application&  mApplication;

  Toolkit::Control           mTable;
  Toolkit::ImageView         mImageViews[ NUMBER_OF_IMAGES ];
  bool                       mImageViewToggleStatus[ NUMBER_OF_IMAGES ];
  unsigned int               mImageViewImageIndexStatus[ NUMBER_OF_IMAGES ];

  Toolkit::TableView::CellPosition mCurrentPositionToggle;
  Toolkit::TableView::CellPosition mCurrentPositionImage;

};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  ImageViewController test( application );
  application.MainLoop();
  return 0;
}
