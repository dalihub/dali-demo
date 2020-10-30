#ifndef PARTICLES_PARTICLE_VIEW_H_
#define PARTICLES_PARTICLE_VIEW_H_
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include "particle-field.h"
#include "dali/public-api/actors/camera-actor.h"
#include "dali/public-api/animation/animation.h"
#include "dali/public-api/object/weak-handle.h"
#include "dali/public-api/rendering/shader.h"
#include <vector>
#include <functional>

struct ColorRange
{
  Dali::Vector3 rgb0;
  Dali::Vector3 rgb1;
};

class ParticleView: public Dali::ConnectionTracker
{
public:
  ParticleView(const ParticleField& field, Dali::Actor world, Dali::CameraActor camera,
    Dali::Geometry particleGeom = Dali::Geometry());
  ~ParticleView();

  void SetColorRange(const ColorRange& range);

  void SetPopulation(float percentage);
  void SetFocalLength(float f);
  void SetAperture(float a);
  void SetAlphaTestRefValue(float rv);
  void SetFadeRange(float near, float far);
  void SetAngularVelocity(float v);
  void SetLinearVelocity(float v);

  ///@brief Starts a scatter & regroup animation, cancelling any previously played animation
  /// of the same kind. Bigger particles, and those further away from the Z axis are affected
  /// less.
  ///@param radius the normalised radius, within which particles are affected.
  ///@param amount the amount of displacement applied to particles at the peak of the animation.
  ///@param durationOut the duration of scattering, in seconds.
  ///@param durationIn the duration of regrouping, in seconds.
  void Scatter(float radius, float amount, float durationOut, float durationIn);

  void SetScatterRay(Dali::Vector3 rayDir);

  ///@brief Starts an animation to change the opacity of the particles to @a target.
  ///@param duration Number of seconds to complete transition in.
  ///@param target Target opacity in the 0..1.f range.
  void Fade(float duration, float target, Dali::AlphaFunction alphaFn = Dali::AlphaFunction::DEFAULT,
    std::function<void(Dali::Animation&)> onFinished = nullptr);

  ///@brief Starts an animation to change the opacity of the particles to @a target.
  ///@param duration Number of seconds to complete transition in.
  ///@param target Target opacity in the 0..1.f range.
  ///@param from The value to set the opacity to prior to the animation.
  void Fade(float duration, float target, float from, Dali::AlphaFunction alphaFn = Dali::AlphaFunction::DEFAULT,
    std::function<void(Dali::Animation&)> onFinished = nullptr);

private: // DATA
  struct ScatterProps
  {
    Dali::Property::Index mPropRadius;
    Dali::Property::Index mPropAmount;
    Dali::Property::Index mPropRay;

    Dali::Animation mAnim;
  };

  Dali::WeakHandle<Dali::Actor> mWorld;
  Dali::Vector3 mParticleBoxSize;

  Dali::Shader mParticleShader;
  Dali::Property::Index mPropPopulation;
  Dali::Property::Index mPropFocalLength;
  Dali::Property::Index mPropAperture;
  Dali::Property::Index mPropAlphaTestRefValue;
  Dali::Property::Index mPropFadeRange;

  ScatterProps mScatterProps[6];
  int mActiveScatter = 0;

  Dali::Actor mMasterParticles;
  Dali::Property::Index mPropSecondaryColor;

  Dali::Actor mSlaveParticles;

  Dali::Animation mAngularAnim;
  Dali::Animation mLinearAnim;
  Dali::Animation mFadeAnim;
};

#endif //PARTICLES_PARTICLE_VIEW_H_
