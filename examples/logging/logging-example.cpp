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
#include <dali/devel-api/adaptor-framework/performance-logger.h>
#include <sstream>

using namespace Dali;

// Define this so that it is interchangeable
// "DP" stands for Device independent Pixels
#define DP(x) x

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

    Toolkit::TableView contentTable = Toolkit::TableView::New( 6, 1 );
    contentTable.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    contentTable.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    contentTable.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    contentTable.SetParentOrigin( ParentOrigin::TOP_LEFT );
    contentTable.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE * 0.5f ) );
//    contentTable.TouchedSignal().Connect( this, &LoggingController::OnTouchEvent );

    for( unsigned int i = 0; i < contentTable.GetRows(); ++i )
    {
      contentTable.SetFitHeight( i );
    }

    contentTable.SetPosition( 0.0f, TOP_MARGIN );

    mContentLayer.Add( contentTable );


    // Logger selector radio group
    Toolkit::TableView radioGroupBackground = Toolkit::TableView::New( 2, 1 );
    radioGroupBackground.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    radioGroupBackground.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    radioGroupBackground.SetBackgroundColor( BACKGROUND_COLOUR );
    radioGroupBackground.SetParentOrigin( ParentOrigin::TOP_LEFT );
    radioGroupBackground.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    radioGroupBackground.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE * 0.5f ) );

    contentTable.Add( radioGroupBackground );

    // Label
    {
      Toolkit::TextLabel label = Toolkit::TextLabel::New( LOGGER_TEXT );
      label.SetParentOrigin( ParentOrigin::TOP_LEFT );
      label.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      label.SetPosition( DP(MARGIN_SIZE), DP(MARGIN_SIZE) );
      label.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

      radioGroupBackground.Add( label );
      radioGroupBackground.SetFitHeight( 0 );
    }

    // Radio group
    Toolkit::TableView radioButtonsGroup = Toolkit::TableView::New( 3, 1 );
    radioButtonsGroup.SetCellPadding( Size( 0.0f, MARGIN_SIZE * 0.5f ) );
    radioButtonsGroup.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    for( unsigned int i = 0; i < radioButtonsGroup.GetRows(); ++i )
    {
      radioButtonsGroup.SetFitHeight( i );
    }
    radioButtonsGroup.SetFitWidth( 0 );

    radioGroupBackground.Add( radioButtonsGroup );
    radioGroupBackground.SetFitHeight( 1 );

    int radioX = 0;
    int radioY = 0;

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
    Toolkit::TableView createGroupBackground = Toolkit::TableView::New( 1, 2 );
    createGroupBackground.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    createGroupBackground.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    createGroupBackground.SetBackgroundColor( BACKGROUND_COLOUR );
    createGroupBackground.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );
    createGroupBackground.SetFitHeight( 0 );

    contentTable.Add( createGroupBackground );

    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetName( CREATE_BUTTON_ID );
      button.SetLabelText( CREATE_BUTTON_TEXT );
      button.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      button.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      button.ClickedSignal().Connect( this, &LoggingController::OnButtonClicked );

      createGroupBackground.Add( button );
    }

    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetName( DELETE_BUTTON_ID );
      button.SetLabelText( DELETE_BUTTON_TEXT );
      button.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      button.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      button.ClickedSignal().Connect( this, &LoggingController::OnButtonClicked );

      createGroupBackground.Add( button );
    }

    // Start/stop group

    Toolkit::TableView timingGroupBackground = Toolkit::TableView::New( 1, 2 );
    timingGroupBackground.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    timingGroupBackground.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    timingGroupBackground.SetBackgroundColor( BACKGROUND_COLOUR );
    timingGroupBackground.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );
    timingGroupBackground.SetFitHeight( 0 );

    contentTable.Add( timingGroupBackground );

    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetName( START_BUTTON_ID );
      button.SetLabelText( START_BUTTON_TEXT );
      button.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      button.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      button.ClickedSignal().Connect( this, &LoggingController::OnButtonClicked );

      timingGroupBackground.Add( button );
    }

    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetName( STOP_BUTTON_ID );
      button.SetLabelText( STOP_BUTTON_TEXT );
      button.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      button.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      button.ClickedSignal().Connect( this, &LoggingController::OnButtonClicked );

      timingGroupBackground.Add( button );
    }

    // Enable/disable group
    Toolkit::TableView enableGroupBackground = Toolkit::TableView::New( 1, 2 );
    enableGroupBackground.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    enableGroupBackground.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    enableGroupBackground.SetBackgroundColor( BACKGROUND_COLOUR );
    enableGroupBackground.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );
    enableGroupBackground.SetFitHeight( 0 );

    contentTable.Add( enableGroupBackground );

    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetName( ENABLE_BUTTON_ID );
      button.SetLabelText( ENABLE_BUTTON_TEXT );
      button.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      button.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      button.ClickedSignal().Connect( this, &LoggingController::OnButtonClicked );

      enableGroupBackground.Add( button );
    }

    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetName( DISABLE_BUTTON_ID );
      button.SetLabelText( DISABLE_BUTTON_TEXT );
      button.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      button.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      button.ClickedSignal().Connect( this, &LoggingController::OnButtonClicked );

      enableGroupBackground.Add( button );
    }

    // Logger selector radio group
    Toolkit::TableView frequencyRadioGroupBackground = Toolkit::TableView::New( 2, 1 );
    frequencyRadioGroupBackground.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    frequencyRadioGroupBackground.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    frequencyRadioGroupBackground.SetBackgroundColor( BACKGROUND_COLOUR );
    frequencyRadioGroupBackground.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE * 0.5f ) );
    frequencyRadioGroupBackground.SetFitHeight( 0 );
    frequencyRadioGroupBackground.SetFitHeight( 1 );

    contentTable.Add( frequencyRadioGroupBackground );

    // Label
    {
      Toolkit::TextLabel label = Toolkit::TextLabel::New( FREQUENCY_TEXT );

      frequencyRadioGroupBackground.Add( label );
    }

    // Radio group
    Toolkit::TableView frequencyRadioButtonsGroup = Toolkit::TableView::New( 1, 3 );
    frequencyRadioButtonsGroup.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    frequencyRadioButtonsGroup.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    frequencyRadioButtonsGroup.SetFitHeight( 0 );
    frequencyRadioButtonsGroup.SetPadding( Padding( 0.0f, 0.0f, MARGIN_SIZE, 0.0f ) );

    frequencyRadioGroupBackground.Add( frequencyRadioButtonsGroup );

    // Radio 1
    {
      Toolkit::RadioButton radioButton = Toolkit::RadioButton::New( FREQUENCY_1_RADIO_TEXT );
      radioButton.SetName( FREQUENCY_1_RADIO_ID );

      radioButton.StateChangedSignal().Connect( this, &LoggingController::FrequencyRadioSelect );

      frequencyRadioButtonsGroup.Add( radioButton );
      mFrequencyRadioButtons[0] = radioButton;
    }

    // Radio 2
    {
      Toolkit::RadioButton radioButton = Toolkit::RadioButton::New( FREQUENCY_2_RADIO_TEXT );
      radioButton.SetName( FREQUENCY_2_RADIO_ID );

      radioButton.SetSelected( true );

      radioButton.StateChangedSignal().Connect( this, &LoggingController::FrequencyRadioSelect );

      frequencyRadioButtonsGroup.Add( radioButton );
      mFrequencyRadioButtons[1] = radioButton;
    }

    // Radio 3
    {
      Toolkit::RadioButton radioButton = Toolkit::RadioButton::New( FREQUENCY_3_RADIO_TEXT );
      radioButton.SetName( FREQUENCY_3_RADIO_ID );

      radioButton.StateChangedSignal().Connect( this, &LoggingController::FrequencyRadioSelect );

      frequencyRadioButtonsGroup.Add( radioButton );
      mFrequencyRadioButtons[2] = radioButton;
    }

    // Vsync group
    Toolkit::TableView vsyncGroupBackground = Toolkit::TableView::New( 1, 1 );
    vsyncGroupBackground.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    vsyncGroupBackground.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    vsyncGroupBackground.SetBackgroundColor( BACKGROUND_COLOUR );
    vsyncGroupBackground.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );
    vsyncGroupBackground.SetFitHeight( 0 );

    contentTable.Add( vsyncGroupBackground );

    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetName( VSYNC_BUTTON_ID );
      button.SetLabelText( VSYNC_BUTTON_TEXT );
      button.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      button.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
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

      mLogRadioButtons[i].SetLabelText( ss.str() );
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

  bool OnTouchEvent( Actor actor, const TouchEvent& event )
  {
    if( 1u == event.GetPointCount() )
    {
      const TouchPoint::State state = event.GetPoint(0u).state;

      // Clamp to integer values; this is to reduce flicking due to pixel misalignment
      const float localPoint = static_cast<float>( static_cast<int>( event.GetPoint( 0 ).local.y ) );

      if( TouchPoint::Down == state )
      {
        mLastPoint = localPoint;
        mAnimation = Animation::New( 0.25f );
      }
      else if( TouchPoint::Motion == state )
      {
        if( mAnimation )
        {
          mAnimation.AnimateBy( Property(actor, Actor::Property::POSITION), Vector3( 0.f, localPoint - mLastPoint, 0.f ), AlphaFunction::LINEAR );
          mAnimation.Play();
          mLastPoint = localPoint;
        }
      }
    }

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
  Toolkit::Control  mView;                   ///< The View instance.
  Toolkit::ToolBar  mToolBar;                ///< The View's Toolbar.
  Layer             mContentLayer;           ///< Content layer

  Animation      mAnimation;
  float          mLastPoint;

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

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DALI_DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
