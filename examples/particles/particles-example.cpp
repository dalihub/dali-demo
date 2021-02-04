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

// INTERNAL INCLUDES
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <numeric>
#include <random>
#include "dali/devel-api/adaptor-framework/tilt-sensor.h"
#include "dali/public-api/actors/camera-actor.h"
#include "dali/public-api/actors/layer.h"
#include "dali/public-api/adaptor-framework/application.h"
#include "dali/public-api/adaptor-framework/key.h"
#include "dali/public-api/events/pan-gesture-detector.h"
#include "dali/public-api/events/tap-gesture-detector.h"
#include "dali/public-api/events/touch-event.h"
#include "dali/public-api/object/property-index-ranges.h"
#include "dali/public-api/render-tasks/render-task-list.h"
#include "dali/public-api/render-tasks/render-task.h"
#include "float-rand.h"
#include "particle-field.h"
#include "particle-view.h"
#include "utils.h"

using namespace Dali;

namespace
{
const float PARTICLE_ALPHA       = .75;
const float ALPHA_TEST_REF_VALUE = .0625;

const float NEAR_FADE = 0.04; // normalized depth
const float FAR_FADE  = 0.8;  // normalized depth

const ParticleField PARTICLE_FIELD = {
  200.f,                        // particle size
  Vector3(2500., 2500., 7800.), // box size - depth needs to be >= camera depth range
  Vector3(6., 6., 12.),         // number of particles
  .333,                         // size variance
  1.,                           // noise amount
  200.,                         // disperse
  250.,                         // motion scale
  15.,                          // motion cycle length
  6.,                           // twinkle frequency
  .11,                          // twinkle size scale
  .333                          // twinkle opacity weight
};

const float WORLD_ANGULAR_VELOCITY = .08;  // radians per seconds
const float WORLD_LINEAR_VELOCITY  = -500; // units along z

const float SCATTER_RADIUS       = 450.0f;
const float SCATTER_AMOUNT       = 180.0f;
const float SCATTER_DURATION_OUT = .8f;
const float SCATTER_DURATION_IN  = 1.5f;

const float FADE_DURATION            = 1.2f;
const float FADEOUT_SPEED_MULTIPLIER = 4.f; // speed multiplier upon fading out.

const float FOCAL_LENGTH = 0.5f; // normalized depth value where particles appear sharp.
const float APERTURE     = 2.2f; // distance scale - the higher it is, the quicker the particles get blurred out moving away from the focal length.

const ColorRange DEFAULT_COLOR_RANGE{Vector3(0., 48. / 255., 1.), Vector3(0., 216. / 255., 1.)};

const float TILT_SCALE         = 0.2;
const float TILT_RANGE_DEGREES = 30.f;

FloatRand sFloatRand;

class TiltFilter
{
public:
  void Reset()
  {
    std::fill(mTiltSamples, mTiltSamples + FILTER_SIZE, Vector2(.0f, .0f));
  }

  void Add(Dali::Vector2 tilt)
  {
    mTiltSamples[mIdxNextSample] = tilt;
    mIdxNextSample               = (mIdxNextSample + 1) % FILTER_SIZE;
  }

  Dali::Vector2 Filter() const
  {
    return std::accumulate(mTiltSamples, mTiltSamples + FILTER_SIZE, Vector2(.0f, .0f)) / FILTER_SIZE;
  }

private:
  enum
  {
    FILTER_SIZE = 8u
  };

  Dali::Vector2 mTiltSamples[FILTER_SIZE];
  size_t        mIdxNextSample = 0;
};

class ParticlesExample : public ConnectionTracker
{
public:
  ParticlesExample(Application& app)
  : mApp(app)
  {
    mApp.InitSignal().Connect(this, &ParticlesExample::OnInit);
    mApp.TerminateSignal().Connect(this, &ParticlesExample::OnTerminate);
  }

  ~ParticlesExample() = default;

private:
  Application& mApp;

  CameraActor mCamera;

  Actor      mWorld;
  Vector2    mAngularPosition;
  ColorRange mColors;

  std::unique_ptr<ParticleView> mParticles;
  std::unique_ptr<ParticleView> mExpiringParticles;

  TapGestureDetector mDoubleTapGesture;

  TiltSensor mTiltSensor;
  TiltFilter mTiltFilter;

  PanGestureDetector mPanGesture;

  void OnInit(Application& application)
  {
    Window window    = application.GetWindow();
    auto   rootLayer = window.GetRootLayer();
    rootLayer.SetProperty(Layer::Property::BEHAVIOR, Layer::Behavior::LAYER_3D);

    window.KeyEventSignal().Connect(this, &ParticlesExample::OnKeyEvent);
    window.GetRootLayer().TouchedSignal().Connect(this, &ParticlesExample::OnTouched);

    auto tiltSensor = TiltSensor::Get();
    if(tiltSensor.Start())
    {
      // Get notifications when the device is tilted
      tiltSensor.TiltedSignal().Connect(this, &ParticlesExample::OnTilted);
    }
    else
    {
      mPanGesture = PanGestureDetector::New();
      mPanGesture.Attach(rootLayer);
      mPanGesture.DetectedSignal().Connect(this, &ParticlesExample::OnPan);
    }

    // Get camera
    RenderTaskList tasks    = window.GetRenderTaskList();
    RenderTask     mainPass = tasks.GetTask(0);
    CameraActor    camera   = mainPass.GetCameraActor();
    mCamera                 = camera;

    // Create world - particles and clock are added to it; this is what we apply tilt to.
    auto world = CreateActor();
    world.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    window.Add(world);
    mWorld = world;

    // Create particles
    TriggerColorTransition(DEFAULT_COLOR_RANGE);

    // Setup double tap detector for color change
    mDoubleTapGesture = TapGestureDetector::New(2);
    mDoubleTapGesture.Attach(rootLayer);
    mDoubleTapGesture.DetectedSignal().Connect(this, &ParticlesExample::OnDoubleTap);
  }

  void OnTerminate(Application& app)
  {
    UnparentAndReset(mWorld);

    mDoubleTapGesture.Reset();
    mPanGesture.Reset();
    mTiltSensor.Reset();
  }

  void OnPause(Application& app)
  {
    mTiltSensor.Stop();
  }

  void OnResume(Application& app)
  {
    mTiltSensor.Start();
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::UP) // single keystrokes
    {
      if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
      {
        mApp.Quit();
      }
    }
  }

  bool OnTouched(Actor a, const TouchEvent& event)
  {
    if(event.GetPointCount() > 0)
    {
      auto screenPos = event.GetScreenPosition(0);
      switch(event.GetState(0))
      {
        case PointState::STARTED:
        {
          mParticles->Scatter(SCATTER_RADIUS, SCATTER_AMOUNT, SCATTER_DURATION_OUT, SCATTER_DURATION_IN);

          Vector3 ray = GetViewRay(screenPos);
          mParticles->SetScatterRay(ray);
        }
        break;

        default:
          break;
      }
    }

    return false;
  }

  void OnDoubleTap(Actor /*actor*/, const TapGesture& /*gesture*/)
  {
    if(!mExpiringParticles)
    {
      mColors.rgb0 = Vector3::ONE - mColors.rgb1;
      mColors.rgb1 = FromHueSaturationLightness(Vector3(sFloatRand() * 360.f, sFloatRand() * .5f + .5f, sFloatRand() * .25 + .75f));

      TriggerColorTransition(mColors);
    }
  }

  void OnPan(Actor actor, const PanGesture& gesture)
  {
    auto       tilt = gesture.GetDisplacement() / Vector2(mApp.GetWindow().GetSize()) * TILT_SCALE;
    Quaternion q(Radian(-tilt.y), Radian(tilt.x), Radian(0.f));
    Quaternion q0 = mWorld.GetProperty(Actor::Property::ORIENTATION).Get<Quaternion>();
    mWorld.SetProperty(Actor::Property::ORIENTATION, q * q0);
  }

  void OnTilted(const TiltSensor& sensor)
  {
    mTiltFilter.Add(Vector2(sensor.GetPitch(), sensor.GetRoll()));
    Vector2    tilt = mTiltFilter.Filter() * TILT_RANGE_DEGREES;
    Quaternion q(Radian(Degree(tilt.x)), Radian(Degree(tilt.y)), Radian(0.f));
    mWorld.SetProperty(Actor::Property::ORIENTATION, q);
  }

  Vector3 GetViewRay(const Vector2& screenPos)
  {
    Vector2 screenSize    = mApp.GetWindow().GetSize();
    Vector2 normScreenPos = (screenPos / screenSize) * 2.f - Vector2::ONE;

    const float fov    = mCamera.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get<float>();
    const float tanFov = std::tan(fov);

    const float zNear = mCamera.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get<float>();
    const float hProj = zNear * tanFov;

    const float aspectRatio = mCamera.GetProperty(CameraActor::Property::ASPECT_RATIO).Get<float>();
    const float wProj       = hProj * aspectRatio;

    // Get camera orientation for view space ray casting. Assume:
    // - this to be world space, i.e. no parent transforms;
    // - no scaling;
    Quaternion cameraOrientation = mCamera.GetProperty(Actor::Property::ORIENTATION).Get<Quaternion>();

    Matrix worldCamera;
    worldCamera.SetTransformComponents(Vector3::ONE, cameraOrientation, Vector3::ZERO);

    float*  data = worldCamera.AsFloat();
    Vector3 xWorldCamera(data[0], data[4], data[8]);
    xWorldCamera *= wProj * normScreenPos.x / xWorldCamera.Length();

    Vector3 yWorldCamera(data[1], data[5], data[9]);
    yWorldCamera *= hProj * normScreenPos.y / yWorldCamera.Length();

    Vector3 zWorldCamera(data[2], data[6], data[10]);
    Vector3 ray = xWorldCamera + yWorldCamera + zWorldCamera * -zNear;
    ray.Normalize();

    return ray;
  }

  void TriggerColorTransition(const ColorRange& range)
  {
    if(mParticles)
    {
      mExpiringParticles = std::move(mParticles);

      // NOTE: this will break the perfect looping, but we can get away with it because we're fading out.
      mExpiringParticles->SetLinearVelocity(WORLD_LINEAR_VELOCITY * FADEOUT_SPEED_MULTIPLIER);

      auto& p = mExpiringParticles;
      mExpiringParticles->Fade(FADE_DURATION, 0.f, AlphaFunction::EASE_IN, [&p](Animation&) {
        p.reset();
      });
    }

    mParticles.reset(new ParticleView(PARTICLE_FIELD, mWorld, mCamera));
    mParticles->SetColorRange(range);
    mParticles->SetFocalLength(FOCAL_LENGTH);
    mParticles->SetAperture(APERTURE);
    mParticles->SetAlphaTestRefValue(ALPHA_TEST_REF_VALUE);
    mParticles->SetFadeRange(NEAR_FADE, FAR_FADE);
    mParticles->SetAngularVelocity(WORLD_ANGULAR_VELOCITY);
    mParticles->SetLinearVelocity(WORLD_LINEAR_VELOCITY);
    mParticles->Fade(FADE_DURATION, PARTICLE_ALPHA, 0.f, AlphaFunction::EASE_OUT);
  }
};

} // namespace

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application      application = Application::New(&argc, &argv, DEMO_THEME_PATH);
  ParticlesExample example(application);
  application.MainLoop();
  return 0;
}
