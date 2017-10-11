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

#include "shared/view.h"
#include <dali-toolkit/dali-toolkit.h>

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
    mSecondaryProgressValue = 0.1f;
    isDefaultTheme = true;
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
    mProgressBarDefault.ValueChangedSignal().Connect( this, &ProgressBarExample::OnValueChanged );

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
    Toolkit::TableView progressBackground = Toolkit::TableView::New( 1, 1 );
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

    // Create buttons
    Toolkit::TableView buttonBackground = Toolkit::TableView::New( 3, 1 );
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
    mResetProgressButton.SetProperty( Toolkit::Button::Property::LABEL, "Reset" );
    mResetProgressButton.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mResetProgressButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    mResetProgressButton.ClickedSignal().Connect( this, &ProgressBarExample::OnResetProgressButtonSelected );

    buttonBackground.Add( mResetProgressButton );

    mSetIndeterminateButton = Toolkit::PushButton::New();
    mSetIndeterminateButton.SetProperty( Toolkit::Button::Property::LABEL, "Toggle Indeterminate" );
    mSetIndeterminateButton.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mSetIndeterminateButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    mSetIndeterminateButton.ClickedSignal().Connect( this, &ProgressBarExample::OnSetIndeterminateButtonSelected );

    buttonBackground.Add( mSetIndeterminateButton );

    mChangeThemeButton = Toolkit::PushButton::New();
    mChangeThemeButton.SetProperty( Toolkit::Button::Property::LABEL, "Change Theme" );
    mChangeThemeButton.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mChangeThemeButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    mChangeThemeButton.ClickedSignal().Connect( this, &ProgressBarExample::OnChangeThemeButtonSelected );

    buttonBackground.Add( mChangeThemeButton );

    // Create a timer to update the progress of all progress bars
    mTimer = Timer::New( TIMER_TIMEOUT_TIME );
    mTimer.TickSignal().Connect( this, &ProgressBarExample::OnTimerTick );
    mTimer.Start();
  }

  bool OnTimerTick()
  {
    mProgressValue += PROGRESS_INCREMENT_VALUE;
    mSecondaryProgressValue = mProgressValue + 0.1f;
    mProgressBarDefault.SetProperty(ProgressBar::Property::PROGRESS_VALUE, mProgressValue);
    mProgressBarDefault.SetProperty(ProgressBar::Property::SECONDARY_PROGRESS_VALUE, mSecondaryProgressValue);

    return ( mProgressValue < 1.0f ); // Only call again if progress has NOT got to the end
  }

  bool OnResetProgressButtonSelected( Toolkit::Button button )
  {
    mProgressValue = 0.0f;
    mSecondaryProgressValue = 0.1f;
    mProgressBarDefault.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.0f);
    mProgressBarDefault.SetProperty(ProgressBar::Property::SECONDARY_PROGRESS_VALUE, 0.1f);
    mTimer.Start();
    return true;
  }

  void OnValueChanged( ProgressBar progressBar, float value, float secondaryValue )
  {
    std::stringstream newLabel;
    newLabel.precision( 2 );
    newLabel << std::fixed << "current : " << value << " / loaded : "  << secondaryValue;

    mProgressBarDefault.SetProperty(ProgressBar::Property::LABEL_VISUAL, newLabel.str() );
  }

  bool OnSetIndeterminateButtonSelected( Toolkit::Button button )
  {
    if( mProgressBarDefault.GetProperty<bool>(ProgressBar::Property::INDETERMINATE))
    {
      mProgressBarDefault.SetProperty(ProgressBar::Property::INDETERMINATE, false);
    }
    else
    {
      mProgressBarDefault.SetProperty(ProgressBar::Property::INDETERMINATE, true);
    }
    return true;
  }

  bool OnChangeThemeButtonSelected( Toolkit::Button button )
  {
    StyleManager styleManager = StyleManager::Get();

    if( isDefaultTheme )
    {
      styleManager.ApplyTheme( THEME_PATH );
      isDefaultTheme = false;
    }
    else
    {
      styleManager.ApplyDefaultTheme();
      isDefaultTheme = true;
    }
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
  Toolkit::PushButton mResetProgressButton;
  Toolkit::PushButton mSetIndeterminateButton;
  Toolkit::PushButton mChangeThemeButton;
  float mProgressValue;
  float mSecondaryProgressValue;
  bool isDefaultTheme;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  ProgressBarExample test( application );
  application.MainLoop();
  return 0;
}
