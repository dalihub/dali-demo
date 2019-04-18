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

// EXTERNAL INCLUDES
#include <memory>
#include <string>
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>

// INTERNAL INCLUDES
#include "shared/view.h"
#include "animation-example.h"
#include "linear-example.h"
#include "padding-example.h"
#include "flex-example.h"
#include "grid-example.h"
#include "example.h"
#include "absolute-example.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const Vector4 BACKGROUND_GRADIENT_1 = Vector4( 0.0f, 0.352941176f, 0.654901961f, 1.0f );
const Vector4 BACKGROUND_GRADIENT_2 = Vector4( 1.0f, 0.992156863f, 0.894117647f, 1.0f );
const Vector2 BACKGROUND_GRADIENT_START_POSITION( 0.0f, -0.5f );
const Vector2 BACKGROUND_GRADIENT_END_POSITION( 0.0f,  0.5f );

const char* TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );

typedef std::unique_ptr< Demo::Example > ExamplePointer;

typedef std::vector< ExamplePointer > ExampleContainer;

/// All layouting examples to be shown should be added to this method
void CreateExamples( ExampleContainer& container )
{
  container.push_back( ExamplePointer(new Demo::AnimationExample) );
  container.push_back( ExamplePointer(new Demo::LinearExample) );
  container.push_back( ExamplePointer(new Demo::PaddingExample) );
  container.push_back( ExamplePointer(new Demo::AbsoluteExample) );
  container.push_back( ExamplePointer(new Demo::FlexExample) ) ;
  container.push_back( ExamplePointer(new Demo::GridExample) ) ;
}

} // anonymous namespace

class LayoutingExample: public ConnectionTracker
{
 public:

  LayoutingExample( Application& application )
  : mApplication( application ),
    mLayoutingExamples(),
    mLayoutIndex( 0 )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &LayoutingExample::Create );
  }

private:

  void Create( Application& application )
  {
    auto stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect( this, &LayoutingExample::OnKeyEvent );

    auto bg = Control::New();
    bg.SetParentOrigin( ParentOrigin::CENTER );
    bg.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    bg.SetProperty(
        Control::Property::BACKGROUND,
        Property::Map().Add( Toolkit::Visual::Property::TYPE, Visual::GRADIENT )
                       .Add( GradientVisual::Property::STOP_COLOR, Property::Array().Add( BACKGROUND_GRADIENT_1 )
                                                                                    .Add( BACKGROUND_GRADIENT_2 ) )
                       .Add( GradientVisual::Property::START_POSITION, BACKGROUND_GRADIENT_START_POSITION )
                       .Add( GradientVisual::Property::END_POSITION,   BACKGROUND_GRADIENT_END_POSITION ) );
    stage.Add( bg );
    auto toolbar = ImageView::New( TOOLBAR_IMAGE );
    toolbar.SetParentOrigin( ParentOrigin::TOP_CENTER );
    toolbar.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    toolbar.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    toolbar.SetProperty( Actor::Property::SIZE_HEIGHT, 50.0f);

    stage.Add( toolbar );

    mToolbarTitle = TextLabel::New( "");
    mToolbarTitle.SetParentOrigin( ParentOrigin::CENTER );
    mToolbarTitle.SetAnchorPoint( AnchorPoint::CENTER );
    mToolbarTitle.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLUE );
    mToolbarTitle.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::LEFT );
    toolbar.Add( mToolbarTitle );

    mNextLayout = PushButton::New();
    mNextLayout.SetStyleName( "ChangeLayoutButton" );
    mNextLayout.SetProperty( Toolkit::Button::Property::LABEL, "Change Layout" );
    mNextLayout.ClickedSignal().Connect( this, &LayoutingExample::ChangeLayout );
    mNextLayout.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    mNextLayout.SetAnchorPoint( AnchorPoint::TOP_RIGHT );
    mNextLayout.SetSize(175, 50);
    toolbar.Add( mNextLayout );

    CreateExamples( mLayoutingExamples );
    if( ! mLayoutingExamples.empty() )
    {
      mLayoutingExamples[ mLayoutIndex ]->Create();
      mToolbarTitle.SetProperty(Toolkit::TextLabel::Property::TEXT, mLayoutingExamples[ mLayoutIndex ]->GetExampleTitle() );
    }
  }

  bool ChangeLayout( Button button )
  {
    if( ! mLayoutingExamples.empty() )
    {
      mLayoutingExamples[ mLayoutIndex ]->Remove();
      mLayoutIndex = ( mLayoutIndex + 1 ) % mLayoutingExamples.size();
      mLayoutingExamples[ mLayoutIndex ]->Create();
      mToolbarTitle.SetProperty(Toolkit::TextLabel::Property::TEXT, mLayoutingExamples[ mLayoutIndex ]->Example::GetExampleTitle() );
    }
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
    }
  }


private:
  Application& mApplication;
  ExampleContainer mLayoutingExamples;
  PushButton mNextLayout;
  unsigned int mLayoutIndex;
  TextLabel mToolbarTitle;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  LayoutingExample test( application );
  application.MainLoop();
  return 0;
};
