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
#include "linear-example.h"
#include "padding-example.h"
#include "absolute-example.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* BACKGROUND_IMAGE( DEMO_IMAGE_DIR "lake_front.jpg" );
const char* TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );

const char* APPLICATION_TITLE( "Layout tester" );

}  // namespace

class LayoutingExample: public ConnectionTracker
{
 public:

  LayoutingExample( Application& application )
  : mApplication( application ),
    mLinearExample(),
    mPaddedExample(),
    mAbsoluteExample(),
    mLayoutIndex( 0 )
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
    title.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::LEFT );
    toolbar.Add( title );

    mNextLayout = PushButton::New();
    mNextLayout.SetProperty( Toolkit::Button::Property::LABEL, "change layout");
    mNextLayout.ClickedSignal().Connect( this, &LayoutingExample::ChangeLayout );
    mNextLayout.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    mNextLayout.SetAnchorPoint( AnchorPoint::TOP_RIGHT );
    mNextLayout.SetSize(175, 50);
    toolbar.Add( mNextLayout );

    mLinearExample.Demo::LinearExample::Create();
  }

  bool ChangeLayout( Button button )
  {
    mLayoutIndex++;

    switch( mLayoutIndex )
    {
      case 1 :
      {
        mLinearExample.Remove();
        mPaddedExample.Create();
        break;
      }
      case 2 :
      {
        mPaddedExample.Remove();
        mAbsoluteExample.Create();
        break;
      }
      case 3:
      {
        mAbsoluteExample.Remove();
        mNextLayout.SetProperty( Toolkit::Button::Property::LABEL, "end of test");
        mNextLayout.SetProperty( Toolkit::Button::Property::DISABLED, true );
        break;
      }
      default :
      {
        break;
      }
    }

    return true;
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


private:
  Application& mApplication;
  Demo::LinearExample mLinearExample;
  Demo::PaddingExample mPaddedExample;
  Demo::AbsoluteExample mAbsoluteExample;
  PushButton mNextLayout;
  unsigned int mLayoutIndex;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  LayoutingExample test( application );
  application.MainLoop();
  return 0;
};
