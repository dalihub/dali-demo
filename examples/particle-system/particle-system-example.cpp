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

#include <dali-toolkit/public-api/particle-system/particle-domain.h>
#include <dali-toolkit/public-api/particle-system/particle-emitter.h>
#include <dali-toolkit/public-api/particle-system/particle-list.h>
#include <dali-toolkit/public-api/particle-system/particle-renderer.h>
#include <dali-toolkit/public-api/particle-system/particle-source.h>
#include <dali-toolkit/public-api/particle-system/particle-types.h>

#include <dali-toolkit/public-api/particle-system/particle-modifier.h>

#include "effects/particle-effect.h"

using namespace Dali;
using namespace Dali::Toolkit::ParticleSystem;
using namespace Dali::Toolkit;
using Dali::Toolkit::TextLabel;

using namespace Dali::ParticleEffect;

constexpr uint16_t NUMBER_OF_EFFECTS         = 3;
constexpr float    TEXT_LABEL_ANIMATION_TIME = 5.0f;
const TimePeriod   TEXT_LABEL_ANIMATION_TIME_PERIOD(3.0, 2.0f);

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

private:
  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::BLACK);
    auto windowSize = window.GetSize();

    // Set up emitter actor to be the full size of the window as some particles may be outside a particular size
    mEmitterActor = Handle::New<Actor>({{Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER},
                                        {Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER},
                                        {Actor::Property::POSITION, Vector2::ZERO},
                                        {Actor::Property::SIZE, Vector2(windowSize.GetWidth(), windowSize.GetHeight())}});
    window.Add(mEmitterActor);

    // Create a tap gesture detector, attach the actor & connect
    mTapDetector = TapGestureDetector::New();
    mTapDetector.Attach(mEmitterActor);
    mTapDetector.DetectedSignal().Connect(this, [&](Actor actor, const TapGesture& tap) { NextEffect(); });

    // Respond to key events
    window.KeyEventSignal().Connect(this, &ParticleEffectController::OnKeyEvent);

    // Create a Text Label at the bottom of the screen
    mTextLabel = Handle::New<TextLabel>({{Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER},
                                         {Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER},
                                         {TextLabel::Property::MULTI_LINE, true},
                                         {TextLabel::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::CENTER},
                                         {TextLabel::Property::TEXT_COLOR, Color::WHITE}});
    window.Add(mTextLabel);

    // Create a fade out animation for the text label after a few seconds
    mTextLabelAnimation = Animation::New(TEXT_LABEL_ANIMATION_TIME);
    mTextLabelAnimation.AnimateTo(Property(mTextLabel, Actor::Property::COLOR_ALPHA), 0.0f, TEXT_LABEL_ANIMATION_TIME_PERIOD);
    mTextLabelAnimation.Play();

    // Start the default effect
    StartEffect(EffectType(mCurrentEffectType));

    // Add extra line to text for further instructions only the first time
    std::string label = mTextLabel[TextLabel::Property::TEXT];
    label += "\nTap to change particle effect";
    mTextLabel[TextLabel::Property::TEXT] = label;
  }

  void StartEffect(EffectType effectType)
  {
    if(mCurrentEmitter)
    {
      mCurrentEmitter.Stop();
      mCurrentEmitter.Reset();
    }

    ParticleEffectParams params{};
    std::string          effectName;

    switch(effectType)
    {
      case EffectType::FIRE_RING:
        params.particleCount        = 5000;
        params.emissionRate         = 1000;
        params.initialParticleCount = 0;
        params.sourceSize           = Vector2(200, 10);
        params.strTexture           = "sparkle-part1.png";
        effectName                  = "Fire Effect";
        break;

      case EffectType::SPARKLES:
        params.particleCount        = 10000;
        params.emissionRate         = 500;
        params.initialParticleCount = 0;
        params.sourceSize           = Vector2(10, 10);
        params.strTexture           = "blue-part2.png";
        effectName                  = "Sparkle Effect";
        break;

      case EffectType::IMAGE_EXPLOSION:
        params.particleCount        = 20000;
        params.emissionRate         = 0;
        params.initialParticleCount = 10;
        params.sourceSize           = Vector2(64, 64);
        params.strImageSourceName   = "particle-image-source.jpg";
        effectName                  = "Image Source Effect";
        break;
    }

    mCurrentEmitter = mParticleSystem->CreateEffectEmitter(effectType, mEmitterActor, params);
    mCurrentEmitter.Start();

    // Set text and reset TextLabel properties and animation
    mTextLabel[Toolkit::TextLabel::Property::TEXT] = effectName;
    mTextLabel[Actor::Property::COLOR_ALPHA]       = 1.0f;
    mTextLabelAnimation.SetCurrentProgress(0.0f);
    mTextLabelAnimation.Play();
  }

  void NextEffect()
  {
    StartEffect(EffectType(++mCurrentEffectType %= NUMBER_OF_EFFECTS));
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
      else
      {
        NextEffect();
      }
    }
  }

private:
  using ParticleEffectPtr = std::unique_ptr<Dali::ParticleEffect::ParticleEffect>;

  Application&      mApplication;
  ParticleEffectPtr mParticleSystem;
  ParticleEmitter   mCurrentEmitter;
  Actor             mEmitterActor;

  TapGestureDetector mTapDetector;

  uint32_t  mCurrentEffectType{0u};
  Actor     mTextLabel;
  Animation mTextLabelAnimation;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application              application = Application::New(&argc, &argv);
  ParticleEffectController test(application);
  application.MainLoop();
  return 0;
}
