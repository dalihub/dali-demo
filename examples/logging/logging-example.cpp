/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

#include <sstream>

using namespace Dali;

namespace
{
// Used to produce visually same dimensions on desktop and device builds
float ScalePointSize( int pointSize )
{
  Dali::Vector2 dpi = Dali::Stage::GetCurrent().GetDpi();
  float meanDpi = (dpi.height + dpi.width) * 0.5f;
  return pointSize * meanDpi / 220.0f;
}

} // namespace

// Define this so that it is interchangeable
// "DP" stands for Device independent Pixels
#define DP(x) ScalePointSize(x)

//enum ButtonType
//{
//  PUSH_BUTTON,
//  TOGGLE_BUTTON
//};
//
//struct ButtonItem
//{
//  ButtonType type;
//  const char* name;
//  const char* text;
//  const char* altText;
//};

namespace
{

const char* const BACKGROUND_IMAGE = DALI_IMAGE_DIR "background-gradient.jpg";
const char* const TOOLBAR_IMAGE = DALI_IMAGE_DIR "top-bar.png";

const char* const TOOLBAR_TITLE = "Logging";
//const int TOOLBAR_HEIGHT = 62;

const int MARGIN_SIZE = 10;
const int TOP_MARGIN = 85;

const int LOGGER_GROUP_HEIGHT = 84;
const int LOGGER_RADIO_GROUP_HEIGHT = 200;

const int LOGGER_RADIO_SPACING = 48;

const int CONSOLE_HEIGHT = 84;

const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = LOGGER_GROUP_HEIGHT - MARGIN_SIZE * 2;

const Vector4 BACKGROUND_COLOUR( 1.0f, 1.0f, 1.0f, 0.15f );

const char* const PUSHBUTTON_PRESS_IMAGE = DALI_IMAGE_DIR "button-down.9.png";
const char* const PUSHBUTTON_BUTTON_IMAGE = DALI_IMAGE_DIR "button-up.9.png";
const char* const PUSHBUTTON_DISABLED_IMAGE = DALI_IMAGE_DIR "button-disabled.9.png";

// Button IDs
const char* const LOGGER_1_RADIO_ID = "LOGGER_1_RADIO";
const char* const LOGGER_2_RADIO_ID = "LOGGER_2_RADIO";
const char* const LOGGER_3_RADIO_ID = "LOGGER_3_RADIO";

const char* const FREQUENCY_1_RADIO_ID = "FREQUENCY_1_RADIO";
const char* const FREQUENCY_2_RADIO_ID = "FREQUENCY_2_RADIO";
const char* const FREQUENCY_3_RADIO_ID = "FREQUENCY_3_RADIO";

const char* const CREATE_BUTTON_ID = "CREATE_BUTTON";
const char* const DELETE_BUTTON_ID = "DELETE_BUTTON";
const char* const START_BUTTON_ID = "START_BUTTON";
const char* const STOP_BUTTON_ID = "STOP_BUTTON";
const char* const HIGH_FREQ_BUTTON_ID = "INC_FREQ_BUTTON";
const char* const LOW_FREQ_BUTTON_ID = "DEC_FREQ_BUTTON";
const char* const ENABLE_BUTTON_ID = "ENABLE_BUTTON";
const char* const DISABLE_BUTTON_ID = "DISABLE_BUTTON";
const char* const VSYNC_BUTTON_ID = "VSYNC_BUTTON";

const char* const CREATE_BUTTON_TEXT = "Create";
const char* const DELETE_BUTTON_TEXT = "Delete";
const char* const START_BUTTON_TEXT = "Start";
const char* const STOP_BUTTON_TEXT = "Stop";
const char* const ENABLE_BUTTON_TEXT = "Enable";
const char* const DISABLE_BUTTON_TEXT = "Disable";
const char* const VSYNC_BUTTON_TEXT = "Vsync";

const char* const FREQUENCY_1_RADIO_TEXT = "1";
const char* const FREQUENCY_2_RADIO_TEXT = "2";
const char* const FREQUENCY_3_RADIO_TEXT = "10";

const char* const LOGGER_TEXT = "Logger:";
const char* const FREQUENCY_TEXT = "Frequency (sec):";

const unsigned int NUM_LOGGERS = 3;

const unsigned int HIGH_FREQUENCY = 1;   // Seconds
const unsigned int MEDIUM_FREQUENCY = 2;   // Seconds
const unsigned int LOW_FREQUENCY = 10;   // Seconds
const unsigned int NUM_FREQUENCIES = 3;

}  // namespace

/**
 * This example is a test harness for performance loggers.
 *
 * Press one of the create buttons to create a logger. This will output on vsync at the default frequency (2 seconds).
 * In case nothing appears in the log, force a vsync by touching anywhere on the screen. Loggers can be deleted
 * with the delete buttons. They can be enabled or disabled in which case logging will appear or disappear in the console
 * respectively. To record information in a logger press the start and then stop button in succession quickly in between
 * the time period when it would print to the console. This is necessary as the logger is cleared of information when
 * it prints. The output will contain the smallest and largest times between start and stop recorded (minimum and maximum),
 * the total time recorded by the logger as well as the average and standard deviation of all the times recorded. The
 * frequency of log output can be set to high frequency (every 1 second) or low frequency (every 10 seconds).
 */
class LoggingController: public ConnectionTracker
{
 public:

  LoggingController( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &LoggingController::Create );
  }

  ~LoggingController()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    mCurrentLogger = 0;
    mPerformanceLoggers.reserve( NUM_LOGGERS );
    mPerformanceLoggers.resize( NUM_LOGGERS );

    mPerformanceLoggerNames.reserve( NUM_LOGGERS );
    mPerformanceLoggerNames.resize( NUM_LOGGERS );

    mLoggerStates.reserve( NUM_LOGGERS );
    mLoggerStates.resize( NUM_LOGGERS );

    mLogRadioButtons.reserve( NUM_LOGGERS );
    mLogRadioButtons.resize( NUM_LOGGERS );

    mFrequencyRadioButtons.reserve( NUM_FREQUENCIES );
    mFrequencyRadioButtons.resize( NUM_FREQUENCIES );

    // Respond to key events
    Stage::GetCurrent().KeyEventSignal().Connect(this, &LoggingController::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            TOOLBAR_TITLE );

    Vector2 stageSize = Stage::GetCurrent().GetSize();

    int yPos = TOP_MARGIN + MARGIN_SIZE;

    // Logger selector radio group
    Actor radioGroupBackground = Toolkit::CreateSolidColorActor( BACKGROUND_COLOUR );
    radioGroupBackground.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    radioGroupBackground.SetParentOrigin( ParentOrigin::TOP_LEFT );
    radioGroupBackground.SetPosition( DP(MARGIN_SIZE), DP(yPos) );
    radioGroupBackground.SetSize( stageSize.width - 2 * DP(MARGIN_SIZE), DP(LOGGER_RADIO_GROUP_HEIGHT) );
    mContentLayer.Add( radioGroupBackground );

    // Label
    {
      Toolkit::TextView label = Toolkit::TextView::New( LOGGER_TEXT );
      label.SetParentOrigin( ParentOrigin::TOP_LEFT );
      label.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      label.SetPosition( DP(MARGIN_SIZE), DP(MARGIN_SIZE) );

      radioGroupBackground.Add( label );
    }

    // Radio group
    Actor radioButtonsGroup = Actor::New();
    radioButtonsGroup.SetParentOrigin( ParentOrigin::TOP_LEFT );
    radioButtonsGroup.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    radioButtonsGroup.SetPosition( DP(MARGIN_SIZE), 0 );

    radioGroupBackground.Add( radioButtonsGroup );

    int radioX = 0;
    int radioY = MARGIN_SIZE + 28;

    // Radio 1
    {
      Toolkit::RadioButton radioButton = Toolkit::RadioButton::New();
      radioButton.SetName( LOGGER_1_RADIO_ID );
      radioButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
      radioButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      radioButton.SetPosition( DP(radioX), DP(radioY) );
      radioButton.SetSelected( true );

      radioButton.StateChangedSignal().Connect( this, &LoggingController::LoggingRadioSelect );

      radioButtonsGroup.Add( radioButton );
      mLogRadioButtons[0] = radioButton;
    }

    // Radio 2
    {
      radioY += LOGGER_RADIO_SPACING;

      Toolkit::RadioButton radioButton = Toolkit::RadioButton::New();
      radioButton.SetName( LOGGER_2_RADIO_ID );
      radioButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
      radioButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      radioButton.SetPosition( DP(radioX), DP(radioY) );

      radioButton.StateChangedSignal().Connect( this, &LoggingController::LoggingRadioSelect );

      radioButtonsGroup.Add( radioButton );
      mLogRadioButtons[1] = radioButton;
    }

    // Radio 3
    {
      radioY += LOGGER_RADIO_SPACING;

      Toolkit::RadioButton radioButton = Toolkit::RadioButton::New();
      radioButton.SetName( LOGGER_3_RADIO_ID );
      radioButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
      radioButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      radioButton.SetPosition( DP(radioX), DP(radioY) );

      radioButton.StateChangedSignal().Connect( this, &LoggingController::LoggingRadioSelect );

      radioButtonsGroup.Add( radioButton );
      mLogRadioButtons[2] = radioButton;
    }

    // Create/delete/disable group
    yPos += LOGGER_RADIO_GROUP_HEIGHT + MARGIN_SIZE;

    Actor createGroupBackground = Toolkit::CreateSolidColorActor( BACKGROUND_COLOUR );
    createGroupBackground.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    createGroupBackground.SetParentOrigin( ParentOrigin::TOP_LEFT );
    createGroupBackground.SetPosition( DP(MARGIN_SIZE), DP(yPos) );
    createGroupBackground.SetSize( stageSize.width - 2 * DP(MARGIN_SIZE), DP(LOGGER_GROUP_HEIGHT) );
    mContentLayer.Add( createGroupBackground );

    int buttonXDP = DP(MARGIN_SIZE);
    int buttonWidthDP = (createGroupBackground.GetSize().width - DP(MARGIN_SIZE) * 3) / 2;

    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetName( CREATE_BUTTON_ID );
      button.SetLabel( CREATE_BUTTON_TEXT );
      button.SetParentOrigin( ParentOrigin::CENTER_LEFT );
      button.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
      button.SetPosition( buttonXDP, 0 );
      button.SetSize( buttonWidthDP, DP(BUTTON_HEIGHT) );

      button.SetSelectedImage( Dali::ResourceImage::New( PUSHBUTTON_PRESS_IMAGE ) );
      button.SetButtonImage( Dali::ResourceImage::New( PUSHBUTTON_BUTTON_IMAGE ) );
      button.SetDisabledImage( Dali::ResourceImage::New( PUSHBUTTON_DISABLED_IMAGE ) );

      button.ClickedSignal().Connect( this, &LoggingController::OnButtonClicked );

      createGroupBackground.Add( button );
    }

    {
      buttonXDP += DP(MARGIN_SIZE) + buttonWidthDP;

      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetName( DELETE_BUTTON_ID );
      button.SetLabel( DELETE_BUTTON_TEXT );
      button.SetParentOrigin( ParentOrigin::CENTER_LEFT );
      button.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
      button.SetPosition( buttonXDP, 0 );
      button.SetSize( buttonWidthDP, DP(BUTTON_HEIGHT) );

      button.SetSelectedImage( Dali::ResourceImage::New( PUSHBUTTON_PRESS_IMAGE ) );
      button.SetButtonImage( Dali::ResourceImage::New( PUSHBUTTON_BUTTON_IMAGE ) );
      button.SetDisabledImage( Dali::ResourceImage::New( PUSHBUTTON_DISABLED_IMAGE ) );

      button.ClickedSignal().Connect( this, &LoggingController::OnButtonClicked );

      createGroupBackground.Add( button );
    }

    // Start/stop group
    yPos += LOGGER_GROUP_HEIGHT + MARGIN_SIZE;

    Actor timingGroupBackground = Toolkit::CreateSolidColorActor( BACKGROUND_COLOUR );
    timingGroupBackground.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    timingGroupBackground.SetParentOrigin( ParentOrigin::TOP_LEFT );
    timingGroupBackground.SetPosition( DP(MARGIN_SIZE), DP(yPos) );
    timingGroupBackground.SetSize( stageSize.width - 2 * DP(MARGIN_SIZE), DP(LOGGER_GROUP_HEIGHT) );
    mContentLayer.Add( timingGroupBackground );

    buttonXDP = DP(MARGIN_SIZE);
    buttonWidthDP = (timingGroupBackground.GetSize().width - DP(MARGIN_SIZE) * 3) / 2;

    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetName( START_BUTTON_ID );
      button.SetLabel( START_BUTTON_TEXT );
      button.SetParentOrigin( ParentOrigin::CENTER_LEFT );
      button.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
      button.SetPosition( buttonXDP, 0 );
      button.SetSize( buttonWidthDP, DP(BUTTON_HEIGHT) );

      button.SetSelectedImage( Dali::ResourceImage::New( PUSHBUTTON_PRESS_IMAGE ) );
      button.SetButtonImage( Dali::ResourceImage::New( PUSHBUTTON_BUTTON_IMAGE ) );
      button.SetDisabledImage( Dali::ResourceImage::New( PUSHBUTTON_DISABLED_IMAGE ) );

      button.ClickedSignal().Connect( this, &LoggingController::OnButtonClicked );

      timingGroupBackground.Add( button );
    }

    {
      buttonXDP += DP(MARGIN_SIZE) + buttonWidthDP;

      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetName( STOP_BUTTON_ID );
      button.SetLabel( STOP_BUTTON_TEXT );
      button.SetParentOrigin( ParentOrigin::CENTER_LEFT );
      button.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
      button.SetPosition( buttonXDP, 0 );
      button.SetSize( buttonWidthDP, DP(BUTTON_HEIGHT) );

      button.SetSelectedImage( Dali::ResourceImage::New( PUSHBUTTON_PRESS_IMAGE ) );
      button.SetButtonImage( Dali::ResourceImage::New( PUSHBUTTON_BUTTON_IMAGE ) );
      button.SetDisabledImage( Dali::ResourceImage::New( PUSHBUTTON_DISABLED_IMAGE ) );

      button.ClickedSignal().Connect( this, &LoggingController::OnButtonClicked );

      timingGroupBackground.Add( button );
    }

    // Enable/disable group
    yPos += LOGGER_GROUP_HEIGHT + MARGIN_SIZE;

    Actor enableGroupBackground = Toolkit::CreateSolidColorActor( BACKGROUND_COLOUR );
    enableGroupBackground.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    enableGroupBackground.SetParentOrigin( ParentOrigin::TOP_LEFT );
    enableGroupBackground.SetPosition( DP(MARGIN_SIZE), DP(yPos) );
    enableGroupBackground.SetSize( stageSize.width - 2 * DP(MARGIN_SIZE), DP(LOGGER_GROUP_HEIGHT) );
    mContentLayer.Add( enableGroupBackground );

    buttonXDP = DP(MARGIN_SIZE);
    buttonWidthDP = (enableGroupBackground.GetSize().width - DP(MARGIN_SIZE) * 3) / 2;

    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetName( ENABLE_BUTTON_ID );
      button.SetLabel( ENABLE_BUTTON_TEXT );
      button.SetParentOrigin( ParentOrigin::CENTER_LEFT );
      button.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
      button.SetPosition( buttonXDP, 0 );
      button.SetSize( buttonWidthDP, DP(BUTTON_HEIGHT) );

      button.SetSelectedImage( Dali::ResourceImage::New( PUSHBUTTON_PRESS_IMAGE ) );
      button.SetButtonImage( Dali::ResourceImage::New( PUSHBUTTON_BUTTON_IMAGE ) );
      button.SetDisabledImage( Dali::ResourceImage::New( PUSHBUTTON_DISABLED_IMAGE ) );

      button.ClickedSignal().Connect( this, &LoggingController::OnButtonClicked );

      enableGroupBackground.Add( button );
    }

    {
      buttonXDP += DP(MARGIN_SIZE) + buttonWidthDP;

      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetName( DISABLE_BUTTON_ID );
      button.SetLabel( DISABLE_BUTTON_TEXT );
      button.SetParentOrigin( ParentOrigin::CENTER_LEFT );
      button.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
      button.SetPosition( buttonXDP, 0 );
      button.SetSize( buttonWidthDP, DP(BUTTON_HEIGHT) );

      button.SetSelectedImage( Dali::ResourceImage::New( PUSHBUTTON_PRESS_IMAGE ) );
      button.SetButtonImage( Dali::ResourceImage::New( PUSHBUTTON_BUTTON_IMAGE ) );
      button.SetDisabledImage( Dali::ResourceImage::New( PUSHBUTTON_DISABLED_IMAGE ) );

      button.ClickedSignal().Connect( this, &LoggingController::OnButtonClicked );

      enableGroupBackground.Add( button );
    }

    yPos += LOGGER_GROUP_HEIGHT + MARGIN_SIZE;

    // Logger selector radio group
    unsigned int groupHeight = LOGGER_GROUP_HEIGHT + 30;

    Actor frequencyRadioGroupBackground = Toolkit::CreateSolidColorActor( BACKGROUND_COLOUR );
    frequencyRadioGroupBackground.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    frequencyRadioGroupBackground.SetParentOrigin( ParentOrigin::TOP_LEFT );
    frequencyRadioGroupBackground.SetPosition( DP(MARGIN_SIZE), DP(yPos) );
    frequencyRadioGroupBackground.SetSize( stageSize.width - 2 * DP(MARGIN_SIZE), DP(groupHeight) );
    mContentLayer.Add( frequencyRadioGroupBackground );

    // Label
    {
      Toolkit::TextView label = Toolkit::TextView::New( FREQUENCY_TEXT );
      label.SetParentOrigin( ParentOrigin::TOP_LEFT );
      label.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      label.SetPosition( DP(MARGIN_SIZE), DP(MARGIN_SIZE) );

      frequencyRadioGroupBackground.Add( label );
    }

    // Radio group
    Actor frequencyRadioButtonsGroup = Actor::New();
    frequencyRadioButtonsGroup.SetParentOrigin( ParentOrigin::TOP_LEFT );
    frequencyRadioButtonsGroup.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    frequencyRadioButtonsGroup.SetPosition( DP(MARGIN_SIZE), DP(40) );

    frequencyRadioGroupBackground.Add( frequencyRadioButtonsGroup );

    radioX = 0;
    radioY = 0;
    const int frequencyRadioWidth = 100;

    // Radio 1
    {
      Toolkit::RadioButton radioButton = Toolkit::RadioButton::New( FREQUENCY_1_RADIO_TEXT );
      radioButton.SetName( FREQUENCY_1_RADIO_ID );
      radioButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
      radioButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      radioButton.SetPosition( DP(radioX), DP(radioY) );

      radioButton.StateChangedSignal().Connect( this, &LoggingController::FrequencyRadioSelect );

      frequencyRadioButtonsGroup.Add( radioButton );
      mFrequencyRadioButtons[0] = radioButton;
    }

    // Radio 2
    {
      radioX += frequencyRadioWidth;

      Toolkit::RadioButton radioButton = Toolkit::RadioButton::New( FREQUENCY_2_RADIO_TEXT );
      radioButton.SetName( FREQUENCY_2_RADIO_ID );
      radioButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
      radioButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      radioButton.SetPosition( DP(radioX), DP(radioY) );
      radioButton.SetSelected( true );

      radioButton.StateChangedSignal().Connect( this, &LoggingController::FrequencyRadioSelect );

      frequencyRadioButtonsGroup.Add( radioButton );
      mFrequencyRadioButtons[1] = radioButton;
    }

    // Radio 3
    {
      radioX += frequencyRadioWidth;

      Toolkit::RadioButton radioButton = Toolkit::RadioButton::New( FREQUENCY_3_RADIO_TEXT );
      radioButton.SetName( FREQUENCY_3_RADIO_ID );
      radioButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
      radioButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      radioButton.SetPosition( DP(radioX), DP(radioY) );

      radioButton.StateChangedSignal().Connect( this, &LoggingController::FrequencyRadioSelect );

      frequencyRadioButtonsGroup.Add( radioButton );
      mFrequencyRadioButtons[2] = radioButton;
    }

    // Vsync group
    yPos += groupHeight + MARGIN_SIZE;

    Actor vsyncGroupBackground = Toolkit::CreateSolidColorActor( BACKGROUND_COLOUR );
    vsyncGroupBackground.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    vsyncGroupBackground.SetParentOrigin( ParentOrigin::TOP_LEFT );
    vsyncGroupBackground.SetPosition( DP(MARGIN_SIZE), DP(yPos) );
    vsyncGroupBackground.SetSize( stageSize.width - 2 * DP(MARGIN_SIZE), DP(LOGGER_GROUP_HEIGHT) );
    mContentLayer.Add( vsyncGroupBackground );

    buttonXDP = DP(MARGIN_SIZE);
    buttonWidthDP = vsyncGroupBackground.GetSize().width - DP(MARGIN_SIZE) * 2;

    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetName( VSYNC_BUTTON_ID );
      button.SetLabel( VSYNC_BUTTON_TEXT );
      button.SetParentOrigin( ParentOrigin::CENTER_LEFT );
      button.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
      button.SetPosition( buttonXDP, 0 );
      button.SetSize( buttonWidthDP, DP(BUTTON_HEIGHT) );

      button.SetSelectedImage( Dali::ResourceImage::New( PUSHBUTTON_PRESS_IMAGE ) );
      button.SetButtonImage( Dali::ResourceImage::New( PUSHBUTTON_BUTTON_IMAGE ) );
      button.SetDisabledImage( Dali::ResourceImage::New( PUSHBUTTON_DISABLED_IMAGE ) );

      button.ClickedSignal().Connect( this, &LoggingController::OnButtonClicked );

      vsyncGroupBackground.Add( button );
    }

    WriteConsole();
  }

  void WriteConsole()
  {
    for( unsigned int i = 0; i < NUM_LOGGERS; ++i)
    {
      std::stringstream ss;
      ss << (i + 1) << ") " << ((mPerformanceLoggers[i]) ? "Created" : "Deleted")
         << ", " << ((mLoggerStates[i].isTiming) ? "Started" : "Stopped")
         << ", " << ((mLoggerStates[i].isEnabled) ? "Enabled" : "Disabled");

      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 7.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::REGULAR );

      Toolkit::TextView textView = Toolkit::TextView::New( ss.str() );
      textView.SetStyleToCurrentText( textStyle );

      mLogRadioButtons[i].SetLabel( textView );
    }
  }

  bool LoggingRadioSelect( Toolkit::Button button )
  {
    if( button.GetName() == LOGGER_1_RADIO_ID && button.IsSelected() == true )
    {
      mCurrentLogger = 0;
    }
    else if( button.GetName() == LOGGER_2_RADIO_ID && button.IsSelected() == true )
    {
      mCurrentLogger = 1;
    }
    else if( button.GetName() == LOGGER_3_RADIO_ID && button.IsSelected() == true )
    {
      mCurrentLogger = 2;
    }

    UpdateState();

    return true;
  }

  void UpdateState()
  {
    DALI_ASSERT_DEBUG( mCurrentLogger < mLoggerStates.size() );
    const unsigned int frequency = mLoggerStates[mCurrentLogger].frequency;
    if( frequency == HIGH_FREQUENCY )
    {
      mFrequencyRadioButtons[0].SetSelected( true );
    }
    else if( frequency == MEDIUM_FREQUENCY )
    {
      mFrequencyRadioButtons[1].SetSelected( true );
    }
    else if( frequency == LOW_FREQUENCY )
    {
      mFrequencyRadioButtons[2].SetSelected( true );
    }
  }

  bool FrequencyRadioSelect( Toolkit::Button button )
  {
    if( button.GetName() == FREQUENCY_1_RADIO_ID && button.IsSelected() == true )
    {
      if( mPerformanceLoggers[mCurrentLogger] )
      {
        DALI_ASSERT_DEBUG( mCurrentLogger < mPerformanceLoggers.size() );
        mPerformanceLoggers[mCurrentLogger].SetLoggingFrequency( HIGH_FREQUENCY );

        DALI_ASSERT_DEBUG( mCurrentLogger < mLoggerStates.size() );
        mLoggerStates[mCurrentLogger].frequency = HIGH_FREQUENCY;
      }
    }
    else if( button.GetName() == FREQUENCY_2_RADIO_ID && button.IsSelected() == true )
    {
      if( mPerformanceLoggers[mCurrentLogger] )
      {
        DALI_ASSERT_DEBUG( mCurrentLogger < mPerformanceLoggers.size() );
        mPerformanceLoggers[mCurrentLogger].SetLoggingFrequency( MEDIUM_FREQUENCY );

        DALI_ASSERT_DEBUG( mCurrentLogger < mLoggerStates.size() );
        mLoggerStates[mCurrentLogger].frequency = MEDIUM_FREQUENCY;
      }
    }
    else if( button.GetName() == FREQUENCY_3_RADIO_ID && button.IsSelected() == true )
    {
      if( mPerformanceLoggers[mCurrentLogger] )
      {
        DALI_ASSERT_DEBUG( mCurrentLogger < mPerformanceLoggers.size() );
        mPerformanceLoggers[mCurrentLogger].SetLoggingFrequency( LOW_FREQUENCY );

        DALI_ASSERT_DEBUG( mCurrentLogger < mLoggerStates.size() );
        mLoggerStates[mCurrentLogger].frequency = LOW_FREQUENCY;
      }
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

  bool OnButtonClicked(Toolkit::Button button)
  {
    if( button.GetName() == CREATE_BUTTON_ID )
    {
      std::stringstream ss;
      ss << "Test logger " << (mCurrentLogger + 1);

      DALI_ASSERT_DEBUG( mCurrentLogger < mPerformanceLoggerNames.size() );
      mPerformanceLoggerNames[mCurrentLogger] = ss.str();

      DALI_ASSERT_DEBUG( mCurrentLogger < mPerformanceLoggers.size() );
      mPerformanceLoggers[mCurrentLogger] = Dali::PerformanceLogger::New( mPerformanceLoggerNames[mCurrentLogger].c_str() );

      DALI_ASSERT_DEBUG( mCurrentLogger < mLoggerStates.size() );
      mLoggerStates[mCurrentLogger].isTiming = false;
      mLoggerStates[mCurrentLogger].isEnabled = true;
      mLoggerStates[mCurrentLogger].frequency = MEDIUM_FREQUENCY;

      UpdateState();
    }
    else if ( button.GetName() == DELETE_BUTTON_ID )
    {
      DALI_ASSERT_DEBUG( mCurrentLogger < mPerformanceLoggers.size() );
      mPerformanceLoggers[mCurrentLogger].Reset();

      DALI_ASSERT_DEBUG( mCurrentLogger < mLoggerStates.size() );
      mLoggerStates[mCurrentLogger].isTiming = false;
      mLoggerStates[mCurrentLogger].isEnabled = true;
      mLoggerStates[mCurrentLogger].frequency = MEDIUM_FREQUENCY;

      UpdateState();
    }
    else if ( button.GetName() == START_BUTTON_ID )
    {
      if( mPerformanceLoggers[mCurrentLogger] )
      {
        DALI_ASSERT_DEBUG( mCurrentLogger < mPerformanceLoggers.size() );
        mPerformanceLoggers[mCurrentLogger].AddMarker( Dali::PerformanceLogger::START_EVENT );

        DALI_ASSERT_DEBUG( mCurrentLogger < mLoggerStates.size() );
        mLoggerStates[mCurrentLogger].isTiming = true;
      }
    }
    else if ( button.GetName() == STOP_BUTTON_ID )
    {
      if( mPerformanceLoggers[mCurrentLogger] )
      {
        DALI_ASSERT_DEBUG( mCurrentLogger < mPerformanceLoggers.size() );
        mPerformanceLoggers[mCurrentLogger].AddMarker( Dali::PerformanceLogger::END_EVENT );

        DALI_ASSERT_DEBUG( mCurrentLogger < mLoggerStates.size() );
        mLoggerStates[mCurrentLogger].isTiming = false;
      }
    }
    else if ( button.GetName() == ENABLE_BUTTON_ID )
    {
      if( mPerformanceLoggers[mCurrentLogger] )
      {
        DALI_ASSERT_DEBUG( mCurrentLogger < mPerformanceLoggers.size() );
        mPerformanceLoggers[mCurrentLogger].EnableLogging( true );

        DALI_ASSERT_DEBUG( mCurrentLogger < mLoggerStates.size() );
        mLoggerStates[mCurrentLogger].isEnabled = true;
      }
    }
    else if ( button.GetName() == DISABLE_BUTTON_ID )
    {
      if( mPerformanceLoggers[mCurrentLogger] )
      {
        DALI_ASSERT_DEBUG( mCurrentLogger < mPerformanceLoggers.size() );
        mPerformanceLoggers[mCurrentLogger].EnableLogging( false );

        DALI_ASSERT_DEBUG( mCurrentLogger < mLoggerStates.size() );
        mLoggerStates[mCurrentLogger].isEnabled = false;
      }
    }

    WriteConsole();

    return true;
  }

 private:

  struct LoggerState
  {
    LoggerState() : frequency( 0 ), isTiming( false ), isEnabled( true ) {}

    unsigned int frequency;
    bool isTiming;
    bool isEnabled;
  };

  Application&      mApplication;
  Toolkit::View     mView;                   ///< The View instance.
  Toolkit::ToolBar  mToolBar;                ///< The View's Toolbar.
  Layer             mContentLayer;           ///< Content layer

  typedef std::vector< std::string > Strings;
  Strings mPerformanceLoggerNames;

  typedef std::vector< Dali::PerformanceLogger > PerformanceLoggers;
  PerformanceLoggers mPerformanceLoggers;
  unsigned int mCurrentLogger;

  typedef std::vector< LoggerState > LoggerStates;
  LoggerStates mLoggerStates;

  typedef std::vector< Toolkit::RadioButton > RadioButtons;
  RadioButtons mLogRadioButtons;
  RadioButtons mFrequencyRadioButtons;
};

void RunTest( Application& application )
{
  LoggingController test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
