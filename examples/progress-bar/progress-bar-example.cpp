/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include "shared/view.h"
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/progress-bar/progress-bar.h>

using namespace Dali;
using namespace Dali::Toolkit;
using Dali::Toolkit::ProgressBar;

namespace
{
const char * const THEME_PATH( DEMO_STYLE_DIR "progress-bar-example-theme.json" ); ///< The theme used for this example
const char* const BACKGROUND_IMAGE = DEMO_IMAGE_DIR "background-gradient.jpg";
const char* const TOOLBAR_IMAGE = DEMO_IMAGE_DIR "top-bar.png";
const char* const TOOLBAR_TITLE = "Progress Bar";

const Vector4 BACKGROUND_COLOUR( 1.0f, 1.0f, 1.0f, 0.15f );

// Layout sizes
const int MARGIN_SIZE = 10;
const int TOP_MARGIN = 85;

const unsigned int TIMER_TIMEOUT_TIME = 50;
const float PROGRESS_INCREMENT_VALUE = 0.01f;

}  // namespace

/**
 * @brief Shows how to create a default progress bar and custom styled progress bars.
 */
class ProgressBarExample: public ConnectionTracker
{
public:

  ProgressBarExample( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mProgressValue = 0.0f;
    mApplication.InitSignal().Connect( this, &ProgressBarExample::Create );
  }

private:

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    // Respond to key events
    Stage::GetCurrent().KeyEventSignal().Connect( this, &ProgressBarExample::OnKeyEvent );

    // Creates a default view with a default tool bar.
    // The view is added to the stage.

    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            TOOLBAR_TITLE );

    mProgressBarDefault = ProgressBar::New();
    mProgressBarDefault.SetParentOrigin(ParentOrigin::TOP_CENTER);
    mProgressBarDefault.SetAnchorPoint(AnchorPoint::TOP_CENTER);
    mProgressBarDefault.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    mProgressBarDefault.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);

    mProgressBarCustomStyle1 = ProgressBar::New();
    mProgressBarCustomStyle1.SetStyleName( "ProgressBarCustomStyle1" );
    mProgressBarCustomStyle1.SetParentOrigin(ParentOrigin::TOP_CENTER);
    mProgressBarCustomStyle1.SetAnchorPoint(AnchorPoint::TOP_CENTER);
    mProgressBarCustomStyle1.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    mProgressBarCustomStyle1.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);

    mProgressBarCustomStyle2 = ProgressBar::New();
    mProgressBarCustomStyle2.SetStyleName( "ProgressBarCustomStyle2" );
    mProgressBarCustomStyle2.SetParentOrigin(ParentOrigin::TOP_CENTER);
    mProgressBarCustomStyle2.SetAnchorPoint(AnchorPoint::TOP_CENTER);
    mProgressBarCustomStyle2.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    mProgressBarCustomStyle2.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);

    Toolkit::TableView contentTable = Toolkit::TableView::New(2, 1);
    contentTable.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    contentTable.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
    contentTable.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    contentTable.SetParentOrigin(ParentOrigin::TOP_LEFT);
    contentTable.SetCellPadding(Size(MARGIN_SIZE, MARGIN_SIZE * 0.5f));

    for( unsigned int i = 0; i < contentTable.GetRows(); ++i )
    {
      contentTable.SetFitHeight( i );
    }

    contentTable.SetPosition( 0.0f, TOP_MARGIN );
    mContentLayer.Add( contentTable );

    // Image selector for progress bar
    Toolkit::TableView progressBackground = Toolkit::TableView::New( 3, 1 );
    progressBackground.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    progressBackground.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    progressBackground.SetBackgroundColor( BACKGROUND_COLOUR );
    progressBackground.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );
    progressBackground.SetRelativeWidth( 0, 1.0f );

    for( unsigned int i = 0; i < progressBackground.GetRows(); ++i )
    {
      progressBackground.SetFitHeight( i );
    }

    contentTable.Add( progressBackground );
    progressBackground.Add( mProgressBarDefault );
    progressBackground.Add( mProgressBarCustomStyle1 );
    progressBackground.Add( mProgressBarCustomStyle2 );

    // Create buttons
    Toolkit::TableView buttonBackground = Toolkit::TableView::New( 1, 1 );
    buttonBackground.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    buttonBackground.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    buttonBackground.SetBackgroundColor( BACKGROUND_COLOUR );
    buttonBackground.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );

    for( unsigned int i = 0; i < buttonBackground.GetRows(); ++i )
    {
      buttonBackground.SetFitHeight( i );
    }

    contentTable.Add( buttonBackground );

    mResetProgressButton = Toolkit::PushButton::New();
    mResetProgressButton.SetLabelText( "Reset" );
    mResetProgressButton.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mResetProgressButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    mResetProgressButton.ClickedSignal().Connect( this, &ProgressBarExample::OnResetProgressButtonSelected );

    buttonBackground.Add( mResetProgressButton );

    // Create a timer to update the progress of all progress bars
    mTimer = Timer::New( TIMER_TIMEOUT_TIME );
    mTimer.TickSignal().Connect( this, &ProgressBarExample::OnTimerTick );
    mTimer.Start();
  }

  bool OnTimerTick()
  {
    mProgressValue += PROGRESS_INCREMENT_VALUE;
    mProgressBarDefault.SetProperty(ProgressBar::Property::PROGRESS_VALUE, mProgressValue);
    mProgressBarCustomStyle1.SetProperty(ProgressBar::Property::PROGRESS_VALUE, mProgressValue);
    mProgressBarCustomStyle2.SetProperty(ProgressBar::Property::PROGRESS_VALUE, mProgressValue);

    return ( mProgressValue < 1.0f ); // Only call again if progress has NOT got to the end
  }

  bool OnResetProgressButtonSelected( Toolkit::Button button )
  {
    mProgressValue = 0.0f;
    mProgressBarDefault.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.0f);
    mProgressBarCustomStyle1.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.0f);
    mProgressBarCustomStyle2.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.0f);
    mTimer.Start();
    return true;
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        // Exit application when click back or escape.
        mApplication.Quit();
      }
    }
  }

  // Data

  Application&      mApplication;
  Timer             mTimer;
  Toolkit::Control  mView;                              ///< The View instance.
  Toolkit::ToolBar  mToolBar;                           ///< The View's Toolbar.
  Layer             mContentLayer;                      ///< Content layer.
  ProgressBar       mProgressBarDefault;
  ProgressBar       mProgressBarCustomStyle1;
  ProgressBar       mProgressBarCustomStyle2;
  Toolkit::PushButton mResetProgressButton;
  float mProgressValue;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, THEME_PATH );
  ProgressBarExample test( application );
  application.MainLoop();
  return 0;
}
