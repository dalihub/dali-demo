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

#include "game-renderer.h"
#include "game-model.h"
#include "game-texture.h"
#include "game-scene.h"

#include "fpp-game-tutorial-controller.h"

#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;

namespace
{

const char* SCENE_URL =
{
  DEMO_GAME_DIR "/scene.json"
};

}
/* This example creates 3D environment with first person camera control
   It contains following modules:

   GameScene  - responsible for loading and managing the scene data,
                it wraps around stage. Owns list of entities. Scene can be deserialised
                from json file ( see scene.json )
   GameEntity - the renderable object that has also a transformation. It wraps DALi actors.

   GameModel  - loads models ( '.mod' file format ) and wraps DALi Geometry object. 'mod' format
                is binary in order

   GameTexture - manages textures. Loads them, creates samplers and wraps DALi TextureSet

   GameRenderer - binds texture and model. It's created per entity. While renderer is always unique
                  for entity, the texture and model may be reused

   GameCamera - Wraps the CameraActor. It provides not only that but also handles user input and
                implements first-person-perspective camera behavior.
                GameCamera uses Dali::Timer to provide per-frame ( or rather every 16ms ) update tick.


                               .-----------.
               .---------------| GameScene |---------------.
               |               '-----------'               |
               |                     |                     |
               v                     |                     v
        .------------. .------------.|.------------. .------------.
        | GameEntity | | GameEntity |v|    ...     | | GameEntity |
        '------------' '------------' '------------' '------------'
               |
               v
        .--------------.
        | GameRenderer |
        '--------------'
                |
         <------'-------->
.--------------.  .--------------.
| GameTexture  |  |  GameModel   |
'--------------'  '--------------'
 */
class GameController : public ConnectionTracker
{
public:

  GameController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &GameController::Create );
  }

  ~GameController()
  {
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Disable indicator
    Dali::Window winHandle = application.GetWindow();
    winHandle.ShowIndicator( Dali::Window::INVISIBLE );

    // Get a handle to the stage
    mStage = Stage::GetCurrent();

    mStage.SetBackgroundColor( Color::BLACK );

    // Use 3D layer
    mStage.GetRootLayer().SetBehavior( Layer::LAYER_3D );

    // Load game scene
    mScene.Load( SCENE_URL );

    // Display tutorial
    mTutorialController.DisplayTutorial();

    // Connect OnKeyEvent signal
    mStage.KeyEventSignal().Connect( this, &GameController::OnKeyEvent );
  }

  // Handle a quit key event
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  Application&              mApplication;
  GameScene                 mScene;
  Stage                     mStage;
  FppGameTutorialController mTutorialController;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  GameController test( application );
  application.MainLoop();
  return 0;
}
