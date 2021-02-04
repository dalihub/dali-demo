/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include "particle-view.h"
#include "dali/public-api/animation/constraints.h"
#include "utils.h"

#include "generated/particle-view-frag.h"
#include "generated/particle-view-simple-frag.h"
#include "generated/particle-view-simple-vert.h"
#include "generated/particle-view-vert.h"

//#define ENABLE_DEBUG_VOLUME

#define USE_GLSL_VERSION(version) "#version " #version "\n"

using namespace Dali;

namespace
{
const uint32_t POPULATION_GRANULARITY = 128;

uint32_t GetSkipValue(uint32_t count, uint32_t prime)
{
  uint32_t skip = 0;
  do
  {
    skip = (rand() % prime) * count * count + (rand() % prime) * count + (rand() % prime);
  } while(skip % prime == 0);
  return skip;
}

} // namespace

ParticleView::ParticleView(const ParticleField& field, Dali::Actor world, Dali::CameraActor camera, Dali::Geometry particleGeom)
: mWorld(world),
  mParticleBoxSize(field.mBoxSize)
{
  if(!particleGeom)
  {
    // create particles
    particleGeom = field.MakeGeometry();
  }

  // create shader
  Shader particleShader = Shader::New(SHADER_PARTICLE_VIEW_VERT, SHADER_PARTICLE_VIEW_FRAG, Shader::Hint::MODIFIES_GEOMETRY);

  float         zNear = camera.GetNearClippingPlane();
  float         zFar  = camera.GetFarClippingPlane();
  const Vector2 depthRange(zNear, 1.f / (zFar - zNear));
  particleShader.RegisterProperty("uDepthRange", depthRange);

  particleShader.RegisterProperty("uTwinkleFrequency", field.mTwinkleFrequency);
  particleShader.RegisterProperty("uTwinkleSizeScale", field.mTwinkleSizeScale);
  particleShader.RegisterProperty("uTwinkleOpacityWeight", field.mTwinkleOpacityWeight);

  mPropPopulation        = particleShader.RegisterProperty("uPopulation", 1.f);
  mPropFocalLength       = particleShader.RegisterProperty("uFocalLength", .5f);
  mPropAperture          = particleShader.RegisterProperty("uAperture", 8.f);
  mPropAlphaTestRefValue = particleShader.RegisterProperty("uAlphaTestRefValue", 0.f);
  mPropFadeRange         = particleShader.RegisterProperty("uFadeRange", Vector2(0.f, 1.f));

  // scatter variables
  char  nameBuffer[64];
  char* writep = nameBuffer + snprintf(nameBuffer, sizeof(nameBuffer), "uScatter[");
  for(uint32_t i = 0; i < std::extent<decltype(mScatterProps)>::value; ++i)
  {
    char* writep2 = writep + snprintf(writep, sizeof(nameBuffer) - std::distance(nameBuffer, writep), "%d].", i);

    snprintf(writep2, sizeof(nameBuffer) - std::distance(nameBuffer, writep2), "radiusSqr");
    mScatterProps[i].mPropRadius = particleShader.RegisterProperty(nameBuffer, 0.f);

    snprintf(writep2, sizeof(nameBuffer) - std::distance(nameBuffer, writep2), "amount");
    mScatterProps[i].mPropAmount = particleShader.RegisterProperty(nameBuffer, 0.f);

    snprintf(writep2, sizeof(nameBuffer) - std::distance(nameBuffer, writep2), "ray");
    mScatterProps[i].mPropRay = particleShader.RegisterProperty(nameBuffer, Vector3::ZERO);
  }

  // Create a look-up table for pseudo-random traversal of particles.
  // Our particle mesh is sorted in Z; changing the population should remove
  // particles "randomly", not from one end.
  // Algorithm described in Mike McShaffry & al: Game Coding Complete.
  const uint32_t prime = 131; // next prime after POPULATION_GRANULARITY
  const uint32_t skip  = GetSkipValue(POPULATION_GRANULARITY, prime);
  uint32_t       next  = 0;

  writep = nameBuffer + snprintf(nameBuffer, sizeof(nameBuffer), "uOrderLookUp[");
  for(uint32_t i = 0; i < POPULATION_GRANULARITY; ++i)
  {
    do
    {
      next += skip;
      next %= prime;
    } while(next == 0 || next > POPULATION_GRANULARITY);

    snprintf(writep, sizeof(nameBuffer) - std::distance(nameBuffer, writep), "%d]", i);
    particleShader.RegisterProperty(nameBuffer, float(next - 1));
  }

  // create animation for time in shader
  auto propTime = particleShader.RegisterProperty("uTime", 0.f);

  Animation animTime = Animation::New(field.mMotionCycleLength);
  animTime.AnimateTo(Property(particleShader, propTime), static_cast<float>(M_PI * 2.f));
  animTime.SetLoopCount(0);
  animTime.Play();

  mParticleShader = particleShader;

  auto renderer        = CreateRenderer(TextureSet::New(), particleGeom, particleShader, OPTION_BLEND);
  auto masterParticles = CreateActor();
  masterParticles.SetProperty(Actor::Property::SIZE, field.mBoxSize);
  masterParticles.SetProperty(Actor::Property::VISIBLE, true);
  masterParticles.AddRenderer(renderer);

  mPropSecondaryColor = masterParticles.RegisterProperty("uSecondaryColor", Vector3::XAXIS);

#ifdef ENABLE_DEBUG_VOLUME
  Geometry cubeGeom = CreateCuboidWireframeGeometry();
  renderer          = CreateRenderer(renderer.GetTextures(), cubeGeom, Shader::New(SHADER_PARTICLE_VIEW_SIMPLE_VERT, SHADER_PARTICLE_VIEW_SIMPLE_FRAG));
  masterParticles.AddRenderer(renderer);
#endif

  world.Add(masterParticles);
  mMasterParticles = masterParticles;
}

ParticleView::~ParticleView()
{
  UnparentAndReset(mMasterParticles);
  UnparentAndReset(mSlaveParticles);

  for(auto anim : {mAngularAnim, mLinearAnim})
  {
    if(anim)
    {
      anim.Stop();
      anim.Reset();
    }
  }

  for(auto& s : mScatterProps)
  {
    auto& anim = s.mAnim;
    if(anim)
    {
      anim.Stop();
      anim.Reset();
    }
  }
}

void ParticleView::SetColorRange(const ColorRange& range)
{
  mMasterParticles.SetProperty(Actor::Property::COLOR_RED, range.rgb0.r);
  mMasterParticles.SetProperty(Actor::Property::COLOR_GREEN, range.rgb0.g);
  mMasterParticles.SetProperty(Actor::Property::COLOR_BLUE, range.rgb0.b);

  mMasterParticles.SetProperty(mPropSecondaryColor, range.rgb1);
}

void ParticleView::SetPopulation(float percentage)
{
  percentage = 1.f - std::min(1.f, std::max(0.f, percentage));
  mParticleShader.SetProperty(mPropPopulation, POPULATION_GRANULARITY * percentage);
}

void ParticleView::SetFocalLength(float f)
{
  mParticleShader.SetProperty(mPropFocalLength, f);
}

void ParticleView::SetAperture(float a)
{
  mParticleShader.SetProperty(mPropAperture, a);
}

void ParticleView::SetAlphaTestRefValue(float rv)
{
  mParticleShader.SetProperty(mPropAlphaTestRefValue, rv);
}

void ParticleView::SetFadeRange(float near, float far)
{
  mParticleShader.SetProperty(mPropFadeRange, Vector2(near, far));
}

void ParticleView::SetAngularVelocity(float v)
{
  if(mAngularAnim)
  {
    mAngularAnim.Stop();
    mAngularAnim.Clear();
    mAngularAnim.Reset();
  }

  if(v * v > .0f)
  {
    float sign = Sign(v);
    auto  anim = Animation::New(std::abs(2. * M_PI / v));
    anim.AnimateTo(Property(mMasterParticles, Actor::Property::ORIENTATION),
                   Quaternion(Radian(Degree(120. * sign)), Vector3::ZAXIS),
                   TimePeriod(0., anim.GetDuration() / 3.));
    anim.AnimateTo(Property(mMasterParticles, Actor::Property::ORIENTATION),
                   Quaternion(Radian(Degree(240. * sign)), Vector3::ZAXIS),
                   TimePeriod(anim.GetDuration() / 3., anim.GetDuration() / 3.));
    anim.AnimateTo(Property(mMasterParticles, Actor::Property::ORIENTATION),
                   Quaternion(Radian(Degree(360. * sign)), Vector3::ZAXIS),
                   TimePeriod(2. * anim.GetDuration() / 3., anim.GetDuration() / 3.));
    anim.SetLoopCount(0);
    anim.Play();

    mAngularAnim = anim;
  }
}

void ParticleView::SetLinearVelocity(float v)
{
  if(mLinearAnim)
  {
    mLinearAnim.Stop();
    mLinearAnim.Clear();
    mLinearAnim.Reset();
  }
  UnparentAndReset(mSlaveParticles);

  if(v * v > .0f)
  {
    float sign         = Sign(v);
    float directedSize = sign * mParticleBoxSize.z;

    Actor   slaveParticles = CloneActor(mMasterParticles);
    Vector3 position       = mMasterParticles.GetCurrentProperty(Actor::Property::POSITION).Get<Vector3>();
    slaveParticles.SetProperty(Actor::Property::POSITION, position + Vector3(0., 0., directedSize));

    auto propSecondaryColor = slaveParticles.RegisterProperty("uSecondaryColor", Vector3::XAXIS);

    Actor world = mWorld.GetHandle();
    world.Add(slaveParticles);

    if(sign < 0.) // fix draw order
    {
      world.Remove(mMasterParticles);
      world.Add(mMasterParticles);
    }

    Constraint constraint = Constraint::New<Vector4>(slaveParticles, Actor::Property::COLOR, EqualToConstraint());
    constraint.AddSource(Source(mMasterParticles, Actor::Property::COLOR));
    constraint.Apply();

    constraint = Constraint::New<Vector3>(slaveParticles, propSecondaryColor, EqualToConstraint());
    constraint.AddSource(Source(mMasterParticles, mPropSecondaryColor));
    constraint.Apply();

    constraint = Constraint::New<Quaternion>(slaveParticles, Actor::Property::ORIENTATION, EqualToConstraint());
    constraint.AddSource(Source(mMasterParticles, Actor::Property::ORIENTATION));
    constraint.Apply();

    auto anim = Animation::New(std::abs(directedSize / v));
    anim.AnimateTo(Property(mMasterParticles, Actor::Property::POSITION_Z), position.z - directedSize);
    anim.AnimateTo(Property(slaveParticles, Actor::Property::POSITION_Z), position.z);
    anim.SetLoopCount(0);
    anim.Play();

    mLinearAnim     = anim;
    mSlaveParticles = slaveParticles;
  }
}

void ParticleView::Scatter(float radius, float amount, float durationOut, float durationIn)
{
  mActiveScatter = (mActiveScatter + 1) % std::extent<decltype(mScatterProps)>::value;

  auto& scatter = mScatterProps[mActiveScatter];
  if(scatter.mAnim)
  {
    scatter.mAnim.Stop();
  }

  radius /= mParticleBoxSize.y;
  radius *= radius;
  mParticleShader.SetProperty(scatter.mPropRadius, radius);

  Animation anim          = Animation::New(durationOut + durationIn);
  auto      scatterAmount = Property(mParticleShader, scatter.mPropAmount);
  anim.AnimateTo(scatterAmount, amount, AlphaFunction::EASE_OUT, TimePeriod(0.f, durationOut));
  anim.AnimateTo(scatterAmount, 0.f, AlphaFunction::EASE_IN_OUT_SINE, TimePeriod(durationOut, durationIn));
  anim.Play();

  scatter.mAnim = anim;
}

void ParticleView::SetScatterRay(Dali::Vector3 rayDir)
{
  auto& scatter = mScatterProps[mActiveScatter];
  mParticleShader.SetProperty(scatter.mPropRay, rayDir);
  ;
}

void ParticleView::Fade(float duration, float target, AlphaFunction alphaFn, std::function<void(Dali::Animation&)> onFinished)
{
  if(mFadeAnim)
  {
    mFadeAnim.Stop();
  }

  Animation anim = Animation::New(duration);
  anim.AnimateTo(Property(mMasterParticles, Actor::Property::COLOR_ALPHA), target, alphaFn);
  if(mSlaveParticles)
  {
    anim.AnimateTo(Property(mSlaveParticles, Actor::Property::COLOR_ALPHA), target, alphaFn);
  }

  if(onFinished)
  {
    anim.FinishedSignal().Connect(this, onFinished);
  }
  anim.Play();

  mFadeAnim = anim;
}

void ParticleView::Fade(float duration, float target, float from, AlphaFunction alphaFn, std::function<void(Dali::Animation&)> onFinished)
{
  mMasterParticles.SetProperty(Actor::Property::COLOR_ALPHA, from);
  if(mSlaveParticles)
  {
    mSlaveParticles.SetProperty(Actor::Property::COLOR_ALPHA, from);
  }

  Fade(duration, target, alphaFn, onFinished);
}
