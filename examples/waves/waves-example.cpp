/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (   "License");
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
#include "utils.h"
#include "dali/devel-api/adaptor-framework/tilt-sensor.h"
#include "dali/public-api/adaptor-framework/application.h"
#include "dali/public-api/adaptor-framework/key.h"
#include "dali/public-api/animation/animation.h"
#include "dali/public-api/events/pan-gesture-detector.h"
#include "dali/public-api/events/tap-gesture-detector.h"
#include "dali/public-api/events/key-event.h"
#include "dali/public-api/actors/camera-actor.h"
#include "dali/public-api/actors/layer.h"
#include "dali/public-api/render-tasks/render-task.h"
#include "dali/public-api/render-tasks/render-task-list.h"
#include <fstream>
#include <iostream>
#include <numeric>

#include "generated/waves-vert.h"
#include "generated/waves-frag.h"

using namespace Dali;

namespace
{

const float TIME_STEP = 0.0952664626;

const std::string UNIFORM_LIGHT_COLOR_SQR = "uLightColorSqr";
const std::string UNIFORM_AMBIENT_COLOR = "uAmbientColor";
const std::string UNIFORM_INV_LIGHT_DIR = "uInvLightDir";
const std::string UNIFORM_SCROLL_SCALE = "uScrollScale";
const std::string UNIFORM_WAVE_RATE = "uWaveRate";
const std::string UNIFORM_WAVE_AMPLITUDE = "uWaveAmplitude";
const std::string UNIFORM_NORMAL_MAP_WEIGHT = "uNormalMapWeight";
const std::string UNIFORM_SPECULARITY = "uSpecularity";
const std::string UNIFORM_PARALLAX_AMOUNT = "uParallaxAmount";
const std::string UNIFORM_TIME = "uTime";

const Vector3 WAVES_COLOR { .78f, .64f, .26f };
const Vector3 LIGHT_COLOR { 1.0f, 0.91f, 0.6f };
const Vector3 AMBIENT_COLOR { .002f, .001f, .001f };

const Vector3 INV_LIGHT_DIR = Normalized(Vector3{ .125f, .8f, -.55f });

const Vector2 SCROLL_SCALE{ 1.f, 3.5f };
const float WAVE_RATE = 12.17f;
const float WAVE_AMPLITUDE = 1.f;
const float NORMAL_MAP_WEIGHT = 0.05f;
const float SPECULARITY = 512.f;
const float PARALLAX_AMOUNT = .25f;

const float TILT_RANGE_DEGREES = 30.f;

const float TRANSITION_DURATION = 1.2f;
const float TRANSITION_TIME_SCALE = 6.f;

const std::string_view NORMAL_MAP_NAME = "noise512.png";

Vector3 RandomColor()
{
  float r = .5f + (rand() % RAND_MAX) / float(RAND_MAX) * .5f;
  float g = .5f + (rand() % RAND_MAX) / float(RAND_MAX) * .5f;
  float b = .5f + (rand() % RAND_MAX) / float(RAND_MAX) * .5f;
  return Vector3(r, g, b);
}

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
    mIdxNextSample = (mIdxNextSample + 1) % FILTER_SIZE;
  }

  Dali::Vector2 Filter() const
  {
    return std::accumulate(mTiltSamples, mTiltSamples + FILTER_SIZE, Vector2(.0f, .0f)) / FILTER_SIZE;
  }

private:
  enum { FILTER_SIZE = 8u };

  Dali::Vector2 mTiltSamples[FILTER_SIZE];
  size_t mIdxNextSample = 0;
};

} // nonamespace

class WavesExample : public ConnectionTracker
{
public:
  WavesExample( Application& app )
  : mApp( app )
  {
    mApp.InitSignal().Connect( this, &WavesExample::Create );
    mApp.TerminateSignal().Connect( this, &WavesExample::Destroy );
  }

  ~WavesExample() = default;

private:
  Application& mApp;

  CameraActor mCamera;  // no ownership

  Actor mWaves;
  Shader mWaveShader;

  Property::Index mUInvLightDir{Property::INVALID_INDEX};
  Property::Index mULightColorSqr{Property::INVALID_INDEX};
  Property::Index mUAmbientColor{Property::INVALID_INDEX};
  Property::Index mUWaveRate{Property::INVALID_INDEX};
  Property::Index mUWaveAmplitude{Property::INVALID_INDEX};
  Property::Index mUScrollScale{Property::INVALID_INDEX};
  Property::Index mUNormalMapWeight{Property::INVALID_INDEX};
  Property::Index mUSpecularity{Property::INVALID_INDEX};
  Property::Index mUParallaxAmount{Property::INVALID_INDEX};
  Property::Index mUTime{Property::INVALID_INDEX};

  TapGestureDetector mDoubleTapGesture;

  TiltSensor mTiltSensor;
  TiltFilter mTiltFilter;

  PanGestureDetector mPanGesture;

  Animation mTimeAnim;
  Animation mTransitionAnim;

  void Create( Application& application )
  {
    Window window = application.GetWindow();
    auto rootLayer = window.GetRootLayer();

    window.SetBackgroundColor(Vector4(WAVES_COLOR * .5f));

    // Get camera
    RenderTaskList tasks = window.GetRenderTaskList();
    RenderTask mainPass = tasks.GetTask(0);
    CameraActor camera = mainPass.GetCameraActor();
    mCamera = camera;

    // NOTE: watchface doesn't tolerate modification of the camera well;
    /// we're better off rotating the world.
    Quaternion baseOrientation (Radian(Degree(-150.f)), Radian(M_PI), Radian(0.f));

    auto shader = CreateShader();

    // Create geometry
    Geometry geom = CreateTesselatedQuad(16, 64, Vector2{ .25f, 3.8f }, [](const Vector2& v) {
      float y = v.y + .5f;  // 0..1
      y = std::sqrt(y) - .5f; // perspective correction - increase vertex density closer to viewer

      float x = v.x + v.x * (1.f - y) * 5.5f;

      y -= .24f;  // further translation
      return Vector2{ x, y };
    }, [](const Vector2& v) {
      return Vector2{ v.x, std::sqrt(v.y) };
    });

    // Create texture
    auto normalMap = LoadTexture(std::string(DEMO_IMAGE_DIR) + NORMAL_MAP_NAME.data());

    TextureSet textures = TextureSet::New();
    textures.SetTexture(0, normalMap);

    Sampler sampler = Sampler::New();
    sampler.SetFilterMode(FilterMode::NEAREST, FilterMode::NEAREST);
    sampler.SetWrapMode(WrapMode::REPEAT, WrapMode::REPEAT);
    textures.SetSampler(0, sampler);

    // Create renderer
    Renderer renderer = CreateRenderer(textures, geom, shader, OPTION_DEPTH_TEST | OPTION_DEPTH_WRITE);

    auto waves = CreateActor();
    auto size = Vector2(window.GetSize());
    waves.SetProperty(Actor::Property::SIZE, Vector3(size.x, 100.f, size.y));
    waves.SetProperty(Actor::Property::ORIENTATION, baseOrientation);
    waves.SetProperty(Actor::Property::COLOR, WAVES_COLOR);
    waves.AddRenderer(renderer);

    window.Add(waves);
    mWaves = waves;

    window.KeyEventSignal().Connect( this, &WavesExample::OnKeyEvent );

    // Setup double tap detector for color change
    mDoubleTapGesture = TapGestureDetector::New(2);
    mDoubleTapGesture.Attach(rootLayer);
    mDoubleTapGesture.DetectedSignal().Connect(this, &WavesExample::OnDoubleTap);

    // Touch controls
    mTiltSensor = TiltSensor::Get();
    if ( mTiltSensor.Start() )
    {
      // Get notifications when the device is tilted
      mTiltSensor.TiltedSignal().Connect( this, &WavesExample::OnTilted );
    }
    else
    {
      mPanGesture = PanGestureDetector::New();
      mPanGesture.Attach(rootLayer);
      mPanGesture.DetectedSignal().Connect(this, &WavesExample::OnPan);
    }

    // Register for suspend / resume
    application.PauseSignal().Connect(this, &WavesExample::OnPause);
    application.ResumeSignal().Connect(this, &WavesExample::OnResume);

    // Create animation for the simulation of time
    Animation animTime = Animation::New(1.f);
    animTime.AnimateBy(Property(mWaveShader, mUTime), TIME_STEP);
    animTime.FinishedSignal().Connect(this, &WavesExample::OnTimeAnimFinished);
    animTime.Play();
    mTimeAnim = animTime;
  }

  void Destroy( Application& app)
  {
    mCamera.Reset();

    mDoubleTapGesture.Reset();
    mPanGesture.Reset();

    UnparentAndReset(mWaves);
  }

  Shader CreateShader()
  {
    Vector3 lightColorSqr{ LIGHT_COLOR };
    Vector3 ambientColor = AMBIENT_COLOR;
    Vector3 invLightDir = INV_LIGHT_DIR;
    Vector2 scrollScale = SCROLL_SCALE;
    float waveRate = WAVE_RATE;
    float waveAmp = WAVE_AMPLITUDE;
    float normalMapWeight = NORMAL_MAP_WEIGHT;
    float specularity = SPECULARITY;
    float parallaxAmount = PARALLAX_AMOUNT;
    if (mWaveShader)
    {
      lightColorSqr = mWaveShader.GetProperty(mULightColorSqr).Get<Vector3>();
      ambientColor = mWaveShader.GetProperty(mUAmbientColor).Get<Vector3>();
      invLightDir = mWaveShader.GetProperty(mUInvLightDir).Get<Vector3>();
      scrollScale = mWaveShader.GetProperty(mUScrollScale).Get<Vector2>();
      waveRate = mWaveShader.GetProperty(mUWaveRate).Get<float>();
      waveAmp = mWaveShader.GetProperty(mUWaveAmplitude).Get<float>();
      normalMapWeight = mWaveShader.GetProperty(mUNormalMapWeight).Get<float>();
      specularity = mWaveShader.GetProperty(mUSpecularity).Get<float>();
    }

    Shader shader = Shader::New(SHADER_WAVES_VERT, SHADER_WAVES_FRAG, Shader::Hint::MODIFIES_GEOMETRY);
    mULightColorSqr = shader.RegisterProperty(UNIFORM_LIGHT_COLOR_SQR, lightColorSqr);
    mUAmbientColor = shader.RegisterProperty(UNIFORM_AMBIENT_COLOR, ambientColor);
    mUInvLightDir = shader.RegisterProperty(UNIFORM_INV_LIGHT_DIR, invLightDir);
    mUScrollScale = shader.RegisterProperty(UNIFORM_SCROLL_SCALE, scrollScale);
    mUWaveRate = shader.RegisterProperty(UNIFORM_WAVE_RATE, waveRate);
    mUWaveAmplitude = shader.RegisterProperty(UNIFORM_WAVE_AMPLITUDE, waveAmp);
    mUNormalMapWeight = shader.RegisterProperty(UNIFORM_NORMAL_MAP_WEIGHT, normalMapWeight);
    mUSpecularity = shader.RegisterProperty(UNIFORM_SPECULARITY, specularity);
    mUParallaxAmount = shader.RegisterProperty(UNIFORM_PARALLAX_AMOUNT, parallaxAmount);
    mUTime = shader.RegisterProperty(UNIFORM_TIME, 0.f);

    auto window = mApp.GetWindow();
    shader.RegisterProperty("uScreenHalfSize", Vector2(window.GetSize()) * .5f);
    mWaveShader = shader;

    return shader;
  }

  void TriggerColorTransition(Vector3 wavesColor, Vector3 lightColor)
  {
    if (mTransitionAnim)
    {
      mTransitionAnim.Stop();
    }

    mTimeAnim.FinishedSignal().Disconnect(this, &WavesExample::OnTimeAnimFinished);
    mTimeAnim.Stop();

    Animation anim = Animation::New(TRANSITION_DURATION);
    anim.AnimateTo(Property(mWaves, Actor::Property::COLOR), Vector4(wavesColor), AlphaFunction::EASE_IN_OUT);
    anim.AnimateTo(Property(mWaveShader, mULightColorSqr), lightColor * lightColor, AlphaFunction::EASE_IN_OUT);
    anim.AnimateBy(Property(mWaveShader, mUTime), TRANSITION_DURATION * TIME_STEP * TRANSITION_TIME_SCALE, AlphaFunction::EASE_IN_OUT);
    anim.FinishedSignal().Connect(this, &WavesExample::OnTransitionFinished);
    anim.Play();
    mTransitionAnim = anim;
  }

  void OnTimeAnimFinished(Animation& anim)
  {
    anim.Play();
  }

  void OnTransitionFinished(Animation& anim)
  {
    mTransitionAnim.Reset();
    mTimeAnim.FinishedSignal().Connect(this, &WavesExample::OnTimeAnimFinished);
    mTimeAnim.Play();
  }

  void OnPause(Application& app)
  {
    mTimeAnim.Pause();
    mTiltSensor.Stop();
  }

  void OnResume(Application& app)
  {
    mTiltSensor.Start();
    mTimeAnim.Play();
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if ( event.GetState() == KeyEvent::UP)  // single keystrokes
    {
      if( IsKey( event, DALI_KEY_ESCAPE ) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApp.Quit();
      }
    }
  }

  void OnDoubleTap(Actor /*actor*/, const TapGesture& gesture)
  {
    Vector3 lightColor = mWaveShader.GetProperty(mULightColorSqr).Get<Vector3>();
    TriggerColorTransition(lightColor, RandomColor());
  }

  void OnPan(Actor actor, const PanGesture& gesture)
  {
    auto tilt = gesture.GetDisplacement() / Vector2(mApp.GetWindow().GetSize());
    switch (gesture.GetState())
    {
    case GestureState::STARTED:
      mTiltFilter.Add(tilt);
      break;

    case GestureState::CONTINUING:
      mTiltFilter.Add(mTiltFilter.Filter() + tilt);
      break;

    default:
      break;
    }

    UpdateLightDirection();
  }

  void OnTilted( const TiltSensor& sensor)
  {
    mTiltFilter.Add(Vector2(sensor.GetPitch(), sensor.GetRoll()));

    UpdateLightDirection();
  }

  void UpdateLightDirection()
  {
    Vector2 tilt = mTiltFilter.Filter();
    Quaternion q(Radian(tilt.y), Radian(-tilt.x), Radian(0.f));
    Vector3 lightDir = q.Rotate(INV_LIGHT_DIR);
    mWaveShader.SetProperty(mUInvLightDir, lightDir);
  }
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  WavesExample example( application);
  application.MainLoop();
  return 0;
}
