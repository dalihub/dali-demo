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

#include <dali/devel-api/actors/actor-devel.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const float COLOR_ANIMATION_DURATION( 5.0f );
const float OPACITY_ANIMATION_DURATION( 1.0f );
} // unnamed namespace

/**
 * @brief An example that shows how to use property notifications.
 *
 * - Creates a text label and sets its text color to black.
 * - Animates the text color of a text label to red.
 * - Sets up a property notification so that we are informed when the color is 50% red.
 * - When we are notified, we start a new animation which animates the text label to transparent.
 */
class PropertyNotificationController : public ConnectionTracker
{
public:

  /**
   * @brief Constructor.
   * @param[in] application A reference to the Application class.
   */
  PropertyNotificationController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &PropertyNotificationController::Create );
  }

  /**
   * @brief Called to initialise the application content
   * @param[in] application A reference to the Application class.
   */
  void Create( Application& application )
  {
    // Set the stage background color and connect to the stage's key signal to allow Back and Escape to exit.
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );
    stage.KeyEventSignal().Connect( this, &PropertyNotificationController::OnKeyEvent );

    // Create a text label and set the text color to black
    mTextLabel = TextLabel::New( "Black to Red Animation\nNew opacity animation at 50% Red" );
    mTextLabel.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    mTextLabel.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    mTextLabel.SetProperty( TextLabel::Property::MULTI_LINE, true );
    mTextLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mTextLabel.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLACK );
    stage.Add( mTextLabel );

    // Create an animation and animate the text color to red
    Animation animation = Animation::New( COLOR_ANIMATION_DURATION );
    animation.AnimateTo( Property( mTextLabel, TextLabel::Property::TEXT_COLOR ), Color::RED );
    animation.Play();

    // Set up a property notification so we are notified when the red component of the text-color reaches 50%
    PropertyNotification notification = mTextLabel.AddPropertyNotification( TextLabel::Property::TEXT_COLOR_RED, GreaterThanCondition( 0.5f ) );
    notification.NotifySignal().Connect( this, &PropertyNotificationController::RedComponentNotification );
  }

  /**
   * @brief Called when any key event is received
   *
   * Will use this to quit the application if Back or the Escape key is received
   * @param[in] event The key event information
   */
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if ( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

  /**
   * @brief Called when the property notification condition is met.
   *
   * In our case, it's when the red component is greater than 50%.
   * Will use this notification to animate the opacity of the text-label to transparent.
   */
  void RedComponentNotification( PropertyNotification& /* source */ )
  {
    Animation animation = Animation::New( OPACITY_ANIMATION_DURATION );
    animation.AnimateTo( Property( mTextLabel, DevelActor::Property::OPACITY ), 0.0f );
    animation.Play();
  }

private:
  Application& mApplication;
  TextLabel mTextLabel;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  PropertyNotificationController test( application );
  application.MainLoop();
  return 0;
}
