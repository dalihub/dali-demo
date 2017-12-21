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

/**
 * @file text-memory-profiling-example.cpp
 * @brief Memory consumption profiling for TextLabel
 */

// EXTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>
#include <dali-toolkit/devel-api/controls/navigation-view/navigation-view.h>

// INTERNAL INCLUDES
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

enum TextType
{
  SINGLE_COLOR_TEXT,
  SINGLE_COLOR_TEXT_WITH_STYLE,
  SINGLE_COLOR_TEXT_WITH_EMOJI,
  SINGLE_COLOR_TEXT_WITH_STYLE_EMOJI,
  MULTI_COLOR_TEXT,
  MULTI_COLOR_TEXT_WITH_STYLE,
  MULTI_COLOR_TEXT_WITH_EMOJI,
  MULTI_COLOR_TEXT_WITH_STYLE_EMOJI,
  SMALL_TEXT_IN_LARGE_TEXT_LABEL,
  NUMBER_OF_TYPES
};

std::string TEXT_TYPE_STRING[ NUMBER_OF_TYPES ] =
{
  "Single color text",
  "Single color text with style",
  "Single color text with emoji",
  "Single color text with style and emoji",
  "Multi color text",
  "Multi color text with style",
  "Multi color text with emoji",
  "Multi color text with style and emoji",
  "Small text in large Text Label"
};

const int NUMBER_OF_LABELS = 500;

const char* BACKGROUND_IMAGE( "" );
const char* TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char* BACK_IMAGE( DEMO_IMAGE_DIR "icon-change.png" );
const char* BACK_IMAGE_SELECTED( DEMO_IMAGE_DIR "icon-change-selected.png" );
const char* INDICATOR_IMAGE( DEMO_IMAGE_DIR "loading.png" );

} // anonymous namespace

/**
 * @brief The main class of the demo.
 */
class TextMemoryProfilingExample : public ConnectionTracker, public Toolkit::ItemFactory
{
public:

  TextMemoryProfilingExample( Application& application )
  : mApplication( application ),
    mCurrentTextStyle( SINGLE_COLOR_TEXT )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextMemoryProfilingExample::Create );
  }

  ~TextMemoryProfilingExample()
  {
    // Nothing to do here.
  }

  /**
   * @brief Create a text label in the given type
   */
  TextLabel SetupTextLabel( int type )
  {
    TextLabel label = TextLabel::New();
    label.SetAnchorPoint( ParentOrigin::TOP_LEFT );
    label.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    label.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLACK );
    label.SetProperty( TextLabel::Property::POINT_SIZE, 12.0f );
    label.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::YELLOW );
    label.SetProperty( TextLabel::Property::ENABLE_MARKUP, true );

    Vector2 stageSize = Stage::GetCurrent().GetSize();
    label.SetPosition( Vector3( Random::Range( 0.0f, stageSize.x ), Random::Range( 0.0f, stageSize.y ), 0.0f) );

    switch ( type )
    {
      case SINGLE_COLOR_TEXT:
      {
        label.SetProperty( TextLabel::Property::TEXT, "A Quick Brown Fox Jumps Over The Lazy Dog" );
        label.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 0.0f, 0.0f ) );
        break;
      }
      case SINGLE_COLOR_TEXT_WITH_STYLE:
      {
        label.SetProperty( TextLabel::Property::TEXT, "A Quick Brown Fox Jumps Over The Lazy Dog" );
        label.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 2.0f, 2.0f ) );
        break;
      }
      case SINGLE_COLOR_TEXT_WITH_EMOJI:
      {
        label.SetProperty( TextLabel::Property::TEXT, "\xF0\x9F\x98\x81 A Quick Brown Fox Jumps Over The Lazy Dog" );
        label.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 0.0f, 0.0f ) );
        break;
      }
      case SINGLE_COLOR_TEXT_WITH_STYLE_EMOJI:
      {
        label.SetProperty( TextLabel::Property::TEXT, "\xF0\x9F\x98\x81 A Quick Brown Fox Jumps Over The Lazy Dog" );
        label.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 2.0f, 2.0f ) );
        break;
      }
      case MULTI_COLOR_TEXT:
      {
        label.SetProperty( TextLabel::Property::TEXT, "A <color value='cyan'>Quick Brown Fox</color> Jumps Over The <color value='yellow'>Lazy Dog</color>" );
        label.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 0.0f, 0.0f ) );
        break;
      }
      case MULTI_COLOR_TEXT_WITH_STYLE:
      {
        label.SetProperty( TextLabel::Property::TEXT, "A <color value='cyan'>Quick Brown Fox</color> Jumps Over The <color value='yellow'>Lazy Dog</color>" );
        label.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 2.0f, 2.0f ) );
        break;
      }
      case MULTI_COLOR_TEXT_WITH_EMOJI:
      {
        label.SetProperty( TextLabel::Property::TEXT, " \xF0\x9F\x98\x81 A <color value='cyan'>Quick Brown Fox</color> Jumps Over The <color value='yellow'>Lazy Dog</color>" );
        label.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 0.0f, 0.0f ) );
        break;
      }
      case MULTI_COLOR_TEXT_WITH_STYLE_EMOJI:
      {
        label.SetProperty( TextLabel::Property::TEXT, " \xF0\x9F\x98\x81 A <color value='cyan'>Quick Brown Fox</color> Jumps Over The <color value='yellow'>Lazy Dog</color>" );
        label.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 2.0f, 2.0f ) );
        break;
      }
      case SMALL_TEXT_IN_LARGE_TEXT_LABEL:
      {
        label.SetProperty( TextLabel::Property::TEXT, "A Quick Brown Fox Jumps Over The Lazy Dog" );
        label.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 0.0f, 0.0f ) );
        label.SetSize(stageSize.x, stageSize.y * 0.25f); // Set the text label in larger size
        break;
      }
      default:
        break;
    }

    return label;
  }

  /**
   * @brief Create the main menu
   */
  void CreateMainMenu()
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    mTapDetector = TapGestureDetector::New();
    mTapDetector.DetectedSignal().Connect( this, &TextMemoryProfilingExample::OnTap );

    // Create an item view for the main menu
    mItemView = ItemView::New( *this );

    mItemView.SetParentOrigin( ParentOrigin::CENTER );
    mItemView.SetAnchorPoint( AnchorPoint::CENTER );

    mLayout = DefaultItemLayout::New( DefaultItemLayout::LIST );
    mLayout->SetItemSize( Vector3( stageSize.width, 60.0f, 0.0f ) );

    mItemView.AddLayout( *mLayout );

    // Activate the layout
    mItemView.ActivateLayout( 0, Vector3( stageSize ), 0.0f );
  }

  /**
   * @brief Return the number of items in the main menu
   */
  virtual unsigned int GetNumberOfItems()
  {
    return NUMBER_OF_TYPES;
  }

  /**
   * @brief Create new item for the main menu
   */
  virtual Actor NewItem( unsigned int itemId )
  {
    TextLabel label = TextLabel::New( TEXT_TYPE_STRING[itemId] );
    label.SetStyleName( "BuilderLabel" );
    label.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );

    // Hook up tap detector
    mTapDetector.Attach( label );

    return label;
  }

  /**
   * @brief Create text labels for memory profiling
   */
  void CreateTextLabels( int type )
  {
    Stage stage = Stage::GetCurrent();

    // Render tasks may have been setup last load so remove them
    RenderTaskList taskList = stage.GetRenderTaskList();
    if( taskList.GetTaskCount() > 1 )
    {
      typedef std::vector<RenderTask> Collection;
      typedef Collection::iterator ColIter;
      Collection tasks;

      for( unsigned int i = 1; i < taskList.GetTaskCount(); ++i )
      {
        tasks.push_back( taskList.GetTask(i) );
      }

      for( ColIter iter = tasks.begin(); iter != tasks.end(); ++iter )
      {
        taskList.RemoveTask(*iter);
      }

      RenderTask defaultTask = taskList.GetTask( 0 );
      defaultTask.SetSourceActor( stage.GetRootLayer() );
      defaultTask.SetTargetFrameBuffer( FrameBufferImage() );
    }

    // Delete any existing text labels
    unsigned int numChildren = mLayer.GetChildCount();

    for( unsigned int i = 0; i < numChildren; ++i )
    {
      mLayer.Remove( mLayer.GetChildAt( 0 ) );
    }

    mLayer.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mLayer.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mLayer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mLayer.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::HEIGHT );
    mLayer.SetSizeModeFactor( Vector3( 0.0f, -DemoHelper::DEFAULT_VIEW_STYLE.mToolBarHeight, 0.0f ) );

    mNavigationView.Push( mLayer );

    // Create new text labels
    for ( int i = 0; i < NUMBER_OF_LABELS; i++ )
    {
      TextLabel label = SetupTextLabel( type );
      mLayer.Add( label );
    }

    mTitle.SetProperty( TextLabel::Property::TEXT, "Run memps on target" );
  }

  /**
   * @brief One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    stage.KeyEventSignal().Connect(this, &TextMemoryProfilingExample::OnKeyEvent);

    Layer contents = DemoHelper::CreateView( mApplication,
                                             mView,
                                             mToolBar,
                                             BACKGROUND_IMAGE,
                                             TOOLBAR_IMAGE,
                                             "" );

    mTitle = DemoHelper::CreateToolBarLabel( "" );
    mTitle.SetProperty( TextLabel::Property::TEXT, "Select the type of text" );

    // Add title to the tool bar.
    mToolBar.AddControl( mTitle, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Alignment::HorizontalCenter );

    // Create a layer to contain dynamically created text labels
    mLayer = Layer::New();

    mIndicator = Toolkit::ImageView::New(INDICATOR_IMAGE);
    mIndicator.SetParentOrigin( ParentOrigin::CENTER );
    mIndicator.SetAnchorPoint( AnchorPoint::CENTER );
    mIndicator.SetProperty( Actor::Property::VISIBLE, false );

    // Create a back button in the left of toolbar
    PushButton backButton = PushButton::New();
    backButton.SetProperty( DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, BACK_IMAGE );
    backButton.SetProperty( DevelButton::Property::SELECTED_BACKGROUND_VISUAL, BACK_IMAGE_SELECTED );
    backButton.ClickedSignal().Connect( this, &TextMemoryProfilingExample::OnBackButtonPressed );
    backButton.SetLeaveRequired( true );
    mToolBar.AddControl( backButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Alignment::HorizontalLeft, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

    // Create a navigation view to navigate different types of text labels
    mNavigationView = NavigationView::New();
    mNavigationView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mNavigationView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mNavigationView.SetBackgroundColor( Color::WHITE );
    stage.Add( mNavigationView );

    CreateMainMenu();
    mNavigationView.Push( mItemView );

    mItemView.Add(mIndicator);

    PropertyNotification notification = mIndicator.AddPropertyNotification( Actor::Property::VISIBLE, GreaterThanCondition(0.01f) );
    notification.NotifySignal().Connect( this, &TextMemoryProfilingExample::OnIndicatorVisible );
  }

  /**
   * @brief Main key event handler
   */
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        ReturnToPreviousScreen();
      }
    }
  }

  /**
   * @brief Tap gesture handler
   */
  void OnTap( Actor actor, const TapGesture& tap )
  {
    mCurrentTextStyle = mItemView.GetItemId( actor );

    // Show the loading indicator
    mIndicator.SetProperty( Actor::Property::VISIBLE, true );

    if ( mAnimation )
    {
      mAnimation.Clear();
      mAnimation.Reset();
    }

    mAnimation = Animation::New( 0.8f );
    mAnimation.AnimateBy( Property( mIndicator, Actor::Property::ORIENTATION ), Quaternion( Radian( Degree(180.0f) ), Vector3::ZAXIS ) );
    mAnimation.SetLooping( true );
    mAnimation.Play();
  }

  /**
   * @brief Property notification handler
   */
  void OnIndicatorVisible( PropertyNotification& source )
  {
    CreateTextLabels( mCurrentTextStyle );

    // Hide the loading indicator
    mAnimation.Stop();
    mIndicator.SetProperty( Actor::Property::VISIBLE, false );
  }

  /**
   * @brief Button signal handler
   */
  bool OnBackButtonPressed( Toolkit::Button button )
  {
    ReturnToPreviousScreen();
    return true;
  }

  /**
   * @brief Returns to the previous screen
   */
  void ReturnToPreviousScreen()
  {
    if ( mItemView.OnStage() )
    {
      // Quit the application if it is in the main menu
      mApplication.Quit();
    }
    else
    {
      // Return to the main menu
      mNavigationView.Pop();

      mTitle.SetProperty( TextLabel::Property::TEXT, "Select type of text to test" );
    }
  }

private:

  Application& mApplication;

  ItemLayoutPtr mLayout;
  ItemView mItemView;
  NavigationView mNavigationView;

  Control mView;
  ToolBar mToolBar;
  TextLabel mTitle;
  ImageView mIndicator;
  Animation mAnimation;

  Layer mLayer;

  TapGestureDetector mTapDetector;

  unsigned int mCurrentTextStyle;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  TextMemoryProfilingExample test( application );
  application.MainLoop();
  return 0;
}
