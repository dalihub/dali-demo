/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/public-api/particle-system/particle-emitter.h>
#include <dali-toolkit/public-api/particle-system/particle-source.h>
#include <dali-toolkit/public-api/particle-system/particle-domain.h>
#include <dali-toolkit/public-api/particle-system/particle-list.h>
#include <dali-toolkit/public-api/particle-system/particle-renderer.h>
#include <dali-toolkit/public-api/particle-system/particle-types.h>

#include <utility>
#include <ctime>
#include <dali-toolkit/public-api/particle-system/particle-modifier.h>
#include <unistd.h>
#include <map>

#include "effects/particle-effect.h"

using namespace Dali;
using namespace Dali::Toolkit::ParticleSystem;
using namespace Dali::Toolkit;
using Dali::Toolkit::TextLabel;

using namespace Dali::ParticleEffect;

/**
 * This example shows Particle System feature
 */
class ParticleEffectController : public ConnectionTracker
{
public:

  ParticleEffectController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ParticleEffectController::Create);
  }

  ~ParticleEffectController() = default; // Nothing to do in destructor

  template<class ButtonType>
  ButtonType MakeButton( std::string title,
                        Vector2 position,
                        Vector2 size,
                        bool toggleable,
                        std::function<bool(Button)> onClick )
  {
    ButtonType button = ButtonType::New();
    button.SetProperty( Button::Property::LABEL, title);
    button.SetProperty( Actor::Property::POSITION, position);
    button.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    button.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    button.SetProperty(Button::Property::TOGGLABLE, toggleable);
    static std::map<RefObject*, std::function<bool(Button)>> callbackMap;
    struct OnClick
    {
      static bool Slot(Button btn)
      {
        auto ptr = btn.GetObjectPtr();
        return callbackMap[ptr](btn);
      }
    };

    mUILastControlPosition = position;
    mUILastControlSize = (size == Vector2::ZERO ? Vector2(button.GetNaturalSize()) : size);

    callbackMap[button.GetObjectPtr()] = onClick;
    button.ClickedSignal().Connect(OnClick::Slot);
    return button;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    using namespace Dali::ParticleEffect;

    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::BLACK);
    {
      Actor emitterActor = Actor::New();
      emitterActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      emitterActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      emitterActor.SetProperty(Actor::Property::POSITION, Vector2(0.0, 0.0f));
      emitterActor.SetProperty(Actor::Property::SIZE, Vector2(1.0, 1.0f));
      window.Add(emitterActor);

      mEmitterActor = emitterActor;
      PushButton lastButton;
      window.Add(
        MakeButton<PushButton>("Fire Effect", Vector2::ZERO, {}, true, [&](Button button){

          if(mCurrentEmitter)
          {
            mCurrentEmitter.Stop();
            mCurrentEmitter.Reset();
          }

          ParticleEffectParams params{};
          params.particleCount = 5000;
          params.emissionRate = 1000;
          params.initialParticleCount = 0;
          params.sourceSize = Vector2(200, 10);
          params.strTexture = "sparkle-part1.png";

          mCurrentEmitter = mParticleSystem->CreateEffectEmitter( EffectType::FIRE_RING, mEmitterActor, params );
          mCurrentEmitter.Start();
          return true;
        })
      );

      window.Add(
        MakeButton<PushButton>("Sparkle Effect", Vector2(0.0f, mUILastControlSize.height), {}, true, [&](Button button){
          if(mCurrentEmitter)
          {
            mCurrentEmitter.Stop();
            mCurrentEmitter.Reset();
          }

          ParticleEffectParams params{};
          params.particleCount = 10000;
          params.emissionRate = 500;
          params.initialParticleCount = 0;
          params.sourceSize = Vector2(10, 10);
          params.strTexture = "blue-part2.png";

          mCurrentEmitter = mParticleSystem->CreateEffectEmitter( EffectType::SPARKLES, mEmitterActor, params );
          mCurrentEmitter.Start();
          return true;
        })
      );

      window.Add(
        MakeButton<PushButton>("Image Source Effect", Vector2(0.0f, mUILastControlPosition.y + mUILastControlSize.height), {}, true, [&](Button button){
          if(mCurrentEmitter)
          {
            mCurrentEmitter.Stop();
            mCurrentEmitter.Reset();
          }

          ParticleEffectParams params{};
          params.particleCount = 20000;
          params.emissionRate = 0;
          params.initialParticleCount = 10;
          params.sourceSize = Vector2(64, 64);
          params.strImageSourceName = "particle-image-source.jpg";

          mCurrentEmitter = mParticleSystem->CreateEffectEmitter( EffectType::IMAGE_EXPLOSION, mEmitterActor, params );
          mCurrentEmitter.Start();
          return true;
        })
      );
      window.Add(
        MakeButton<PushButton>("Quit", Vector2(0.0f, mUILastControlPosition.y + mUILastControlSize.height * 2), {}, true, [&](Button button){
          if(mCurrentEmitter)
          {
            mCurrentEmitter.Stop();
            mCurrentEmitter.Reset();
          }
          mApplication.Quit();
          return true;
        })
      );
    }

    // Respond to key events
    window.KeyEventSignal().Connect(this, &ParticleEffectController::OnKeyEvent);
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:

  Application& mApplication;
  std::unique_ptr<Dali::ParticleEffect::ParticleEffect> mParticleSystem;
  ParticleEmitter mCurrentEmitter;
  Actor mEmitterActor;

  // Needed for buttons
  Vector2 mUILastControlPosition;
  Vector2 mUILastControlSize;

};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application          application = Application::New(&argc, &argv);
  ParticleEffectController test(application);
  application.MainLoop();
  return 0;
}
