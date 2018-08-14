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

#include <dali-toolkit/dali-toolkit.h>
#include <vector>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const Vector4 BACKGROUND_COLOR( Color::WHITE );

const char* IMAGE_PATHS[] = {
    DEMO_IMAGE_DIR "gallery-medium-1.jpg",
    DEMO_IMAGE_DIR "gallery-medium-2.jpg",
    DEMO_IMAGE_DIR "gallery-medium-3.jpg",
    DEMO_IMAGE_DIR "gallery-medium-4.jpg",
    DEMO_IMAGE_DIR "gallery-medium-5.jpg",
    DEMO_IMAGE_DIR "gallery-medium-6.jpg",
    DEMO_IMAGE_DIR "gallery-medium-7.jpg",
    DEMO_IMAGE_DIR "gallery-medium-8.jpg",
    DEMO_IMAGE_DIR "gallery-medium-9.jpg",
    DEMO_IMAGE_DIR "gallery-medium-10.jpg",
    DEMO_IMAGE_DIR "gallery-medium-11.jpg",
    DEMO_IMAGE_DIR "gallery-medium-12.jpg",
    DEMO_IMAGE_DIR "gallery-medium-13.jpg",
    DEMO_IMAGE_DIR "gallery-medium-14.jpg",
    DEMO_IMAGE_DIR "gallery-medium-15.jpg",
    DEMO_IMAGE_DIR "gallery-medium-16.jpg",
    DEMO_IMAGE_DIR "gallery-medium-17.jpg",
    DEMO_IMAGE_DIR "gallery-medium-18.jpg",
    DEMO_IMAGE_DIR "gallery-medium-19.jpg",
    DEMO_IMAGE_DIR "gallery-medium-20.jpg",
    DEMO_IMAGE_DIR "gallery-medium-21.jpg",
    DEMO_IMAGE_DIR "gallery-medium-22.jpg",
    DEMO_IMAGE_DIR "gallery-medium-23.jpg",
    DEMO_IMAGE_DIR "gallery-medium-24.jpg",
    DEMO_IMAGE_DIR "gallery-medium-25.jpg",
    DEMO_IMAGE_DIR "gallery-medium-26.jpg",
    DEMO_IMAGE_DIR "gallery-medium-27.jpg",
    DEMO_IMAGE_DIR "gallery-medium-28.jpg",
    DEMO_IMAGE_DIR "gallery-medium-29.jpg",
    DEMO_IMAGE_DIR "gallery-medium-30.jpg",
    DEMO_IMAGE_DIR "gallery-medium-31.jpg",
    DEMO_IMAGE_DIR "gallery-medium-32.jpg",
    DEMO_IMAGE_DIR "gallery-medium-33.jpg",
    DEMO_IMAGE_DIR "gallery-medium-34.jpg",
    DEMO_IMAGE_DIR "gallery-medium-35.jpg",
    DEMO_IMAGE_DIR "gallery-medium-36.jpg",
    DEMO_IMAGE_DIR "gallery-medium-37.jpg",
    DEMO_IMAGE_DIR "gallery-medium-38.jpg",
    DEMO_IMAGE_DIR "gallery-medium-39.jpg",
    DEMO_IMAGE_DIR "gallery-medium-40.jpg",
    DEMO_IMAGE_DIR "gallery-medium-41.jpg",
    DEMO_IMAGE_DIR "gallery-medium-42.jpg",
    DEMO_IMAGE_DIR "gallery-medium-43.jpg",
    DEMO_IMAGE_DIR "gallery-medium-44.jpg",
    DEMO_IMAGE_DIR "gallery-medium-45.jpg",
    DEMO_IMAGE_DIR "gallery-medium-46.jpg",
    DEMO_IMAGE_DIR "gallery-medium-47.jpg",
    DEMO_IMAGE_DIR "gallery-medium-48.jpg",
    DEMO_IMAGE_DIR "gallery-medium-49.jpg",
    DEMO_IMAGE_DIR "gallery-medium-50.jpg",
    DEMO_IMAGE_DIR "gallery-medium-51.jpg",
    DEMO_IMAGE_DIR "gallery-medium-52.jpg",
    DEMO_IMAGE_DIR "gallery-medium-53.jpg",
};
const unsigned int NUM_IMAGES = sizeof( IMAGE_PATHS ) / sizeof( char* );

const char* PHRASES[] = {
    "Hello World",
    "The Quick Brown Fox Jumps Over The Lazy Dog",
    "A bird in the hand is worth two in the bush",
    "A different kettle of fish",
    "A drop in the ocean",
    "Easy as pie",
    "Even at the turning of the tide",
    "Genius is one percent inspiration and 99 percent perspiration",
    "Go out on a limb",
    "I spy with my little eye",
    "If it ain't broke, don't fix it",
    "Imitation is the sincerest form of flattery",
    "Kettle of fish",
    "Know your onions",
    "Okey-dokey",
    "On a hiding to nothing",
    "One swallow doesn't make a summer",
    "Penny for your thoughts",
    "Pie in the sky",
    "Safe as houses",
    "Scapegoat",
    "Take care of the pence and the pounds will take care of themselves",
    "Talk to the hand",
    "The customer is always right",
    "Thick as thieves",
    "Think outside the box",
    "Yada yada",
    "You can lead a horse to water but you can't make it drink",
    "Zig-zag"
};
const unsigned int NUM_PHRASES = sizeof( PHRASES ) / sizeof( char* );

} // unnamed namespace

class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

private:

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( BACKGROUND_COLOR );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &HelloWorldController::OnTouch );

    // Respond to key events
    stage.KeyEventSignal().Connect( this, &HelloWorldController::OnKeyEvent );

    // Background
    Control background = Control::New();
    background.SetBackgroundColor( BACKGROUND_COLOR );
    background.SetSize( stage.GetSize() );
    background.SetParentOrigin( ParentOrigin::CENTER );
    stage.Add( background );
  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if ( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
      else if( event.keyPressed == "1" )
      {
        // Create new image-view
        Actor imageView = ImageView::New( IMAGE_PATHS[ static_cast< int >( Random::Range( 0, NUM_IMAGES ) ) ] );
        Add( imageView, mImageViews );
      }
      else if( event.keyPressed == "2" )
      {
        // Delete last added image-view
        Remove( mImageViews );
      }
      else if( event.keyPressed == "3" )
      {
        // Create new text-label
        Actor textLabel = TextLabel::New( PHRASES[ static_cast< int >( Random::Range( 0, NUM_PHRASES ) ) ] );
        textLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::WIDTH );
        textLabel.SetProperty(
            TextLabel::Property::TEXT_COLOR,
            Vector4( Random::Range( 0.0f, 1.0f ), Random::Range( 0.0f, 1.0f ), Random::Range( 0.0f, 1.0f ), 1.0f ) );
        Add( textLabel, mTextLabels );
      }
      else if( event.keyPressed == "4" )
      {
        // Delete last added text-label
        Remove( mTextLabels );
      }
    }
  }

  void Add( Actor& actor, std::vector< Actor >& mActors )
  {
    Stage stage = Stage::GetCurrent();
    const Vector2 stageSize = stage.GetSize();

    actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
    actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    Vector3 position;
    if( mActors.size() )
    {
      const Actor previous = *mActors.rbegin();
      const Vector3 previousPosition = previous.GetCurrentPosition();
      const Vector3 previousSize = previous.GetCurrentSize();
      position.x = previousPosition.x + previousSize.width;
      position.y = previousPosition.y;
      if ( position.x >= stageSize.width )
      {
        position.x = 0.0f;
        position.y = previousPosition.y + previousSize.height;
      }
      if( position.y >= stageSize.height )
      {
        position.x = position.y = 0.0f;
      }
    }
    actor.SetPosition( position );

    stage.Add( actor );
    mActors.push_back( actor );
  }

  void Remove( std::vector< Actor >& mActors )
  {
    if( mActors.size() )
    {
      Actor actor = *mActors.rbegin();
      actor.Unparent();
      mActors.erase( mActors.end() - 1 );
    }
  }

private:
  Application&  mApplication;
  std::vector< Actor > mImageViews;
  std::vector< Actor > mTextLabels;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  HelloWorldController test( application );
  application.MainLoop();
  return 0;
}
