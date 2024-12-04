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
#include <dali/dali.h>

#include <algorithm>
#include <chrono> // std::chrono::system_clock
#include <map>
#include <random> // std::default_random_engine
#include <sstream>

#include "shared/utility.h"
#include "sparkle-effect.h"

using namespace Dali;
using Dali::Toolkit::ImageView;

using namespace SparkleEffect;

namespace // unnamed namespace
{
//background image for normal status
const char* const CIRCLE_BACKGROUND_IMAGE(DEMO_IMAGE_DIR "sparkle_normal_background.png");
//particle shape image
const char* const PARTICLE_IMAGE(DEMO_IMAGE_DIR "sparkle_particle.png");

float EaseOutSquare(float progress)
{
  return 1.0f - (1.0f - progress) * (1.0f - progress);
}

float CustomBounce(float progress)
{
  float p = 1.f - progress;
  p *= p;
  return 17.68f * p * p * p * progress;
}

float Mix(const Vector2& range, float a)
{
  return range.x * a + range.y * (1.f - a) - 0.001f;
}

const Vector4 BACKGROUND_COLOR(0.f, 0.f, 0.05f, 1.f);

} // unnamed namespace

// This example shows a sparkle particle effect
//
class SparkleEffectExample : public ConnectionTracker
{
public:
  /**
   * Create the SparkleEffectExample
   * @param[in] application The DALi application instance
   */
  SparkleEffectExample(Application& application)
  : mApplication(application),
    mAnimationIndex(0u),
    mShaking(false)
  {
    mApplication.InitSignal().Connect(this, &SparkleEffectExample::OnInit);
  }

private:
  /**
   * Initialize the SparkleEffectExample
   * @param[in] application The DALi application instance
   */
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.KeyEventSignal().Connect(this, &SparkleEffectExample::OnKeyEvent);
    window.SetBackgroundColor(BACKGROUND_COLOR);

    mCircleBackground = ImageView::New(CIRCLE_BACKGROUND_IMAGE);
    mCircleBackground.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mCircleBackground.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);

    window.Add(mCircleBackground);

    mEffect = SparkleEffect::New();

    mMeshActor = CreateMeshActor();

    window.Add(mMeshActor);

    mMeshActor.SetProperty(Actor::Property::POSITION, ACTOR_POSITION);
    mMeshActor.SetProperty(Actor::Property::SCALE, ACTOR_SCALE);

    mTapDetector = TapGestureDetector::New();
    mTapDetector.Attach(mCircleBackground);
    mTapDetector.DetectedSignal().Connect(this, &SparkleEffectExample::OnTap);

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect(this, &SparkleEffectExample::OnPan);
    mPanGestureDetector.Attach(mCircleBackground);

    PlayWanderAnimation(35.f);
  }

  /**
   * Create the mesh representing all the particles
   */
  Actor CreateMeshActor()
  {
    // shuffling to assign the color in random order
    unsigned int* shuffleArray = new unsigned int[NUM_PARTICLE];
    for(unsigned int i = 0; i < NUM_PARTICLE; i++)
    {
      shuffleArray[i] = i;
    }
    const unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(&shuffleArray[0], &shuffleArray[NUM_PARTICLE], std::default_random_engine(seed));

    // Create vertices

    std::vector<Vertex>         vertices;
    std::vector<unsigned short> faces;

    for(unsigned int i = 0; i < NUM_PARTICLE; i++)
    {
      float colorIndex = GetColorIndex(shuffleArray[i]);
      AddParticletoMesh(vertices, faces, PATHS[i], colorIndex);
    }

    delete[] shuffleArray;

    Property::Map vertexFormat;
    vertexFormat["aTexCoord"]      = Property::VECTOR2;
    vertexFormat["aParticlePath0"] = Property::VECTOR2;
    vertexFormat["aParticlePath1"] = Property::VECTOR2;
    vertexFormat["aParticlePath2"] = Property::VECTOR2;
    vertexFormat["aParticlePath3"] = Property::VECTOR2;
    vertexFormat["aParticlePath4"] = Property::VECTOR2;
    vertexFormat["aParticlePath5"] = Property::VECTOR2;

    VertexBuffer vertexBuffer = VertexBuffer::New(vertexFormat);
    vertexBuffer.SetData(&vertices[0], vertices.size());

    Geometry geometry = Geometry::New();
    geometry.AddVertexBuffer(vertexBuffer);
    geometry.SetIndexBuffer(&faces[0], faces.size());
    geometry.SetType(Geometry::TRIANGLES);

    Texture    particleTexture = DemoHelper::LoadTexture(PARTICLE_IMAGE);
    TextureSet textureSet      = TextureSet::New();
    textureSet.SetTexture(0u, particleTexture);

    Renderer renderer = Renderer::New(geometry, mEffect);
    renderer.SetTextures(textureSet);

    Actor meshActor = Actor::New();
    meshActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    meshActor.SetProperty(Actor::Property::SIZE, Vector2(1, 1));
    meshActor.SetProperty(Actor::Property::UPDATE_AREA_HINT, ACTOR_UPDATE_AREA_HINT);
    meshActor.AddRenderer(renderer);

    return meshActor;
  }

  /**
   * Defines a rule to assign particle with a color according to its index
   */
  float GetColorIndex(unsigned int particleIndex)
  {
    unsigned int thereshold = 0;
    for(unsigned int i = 0; i < NUM_COLOR; i++)
    {
      thereshold += PARTICLE_COLORS[i].numParticle;
      if(particleIndex < thereshold)
      {
        return i + Mix(PARTICLE_COLORS[i].AlphaRange, static_cast<float>(thereshold - particleIndex) / PARTICLE_COLORS[i].numParticle);
      }
    }
    return NUM_COLOR - 1;
  }

  /**
   * All a particle to the mesh by giving the moving path and color index
   *
   * Two triangles per particle
   *  0---------3
   *   |\      |
   *   |  \    |
   *   |    \  |
   *   |      \|
   *  1---------2
   *
   * The information we need to pass in through attribute include:
   *
   *   path which contains 12 integer
   *          ---- passed in 6 Vector2 attributes
   *
   *   color index, particle index and textureCoor( (0,0) or (1,0) or (0,1) or (1,1)  )
   *          ---- package these info into texCood attribute as: (+-colorIndex, +-particleIndex)
   */
  void AddParticletoMesh(std::vector<Vertex>&         vertices,
                         std::vector<unsigned short>& faces,
                         MovingPath&                  movingPath,
                         float                        colorIndex)
  {
    unsigned int idx = vertices.size();

    // store the path into position and normal, which would be decoded inside the shader
    Vector2 particlePath0(movingPath[0], movingPath[1]);
    Vector2 particlePath1(movingPath[2], movingPath[3]);
    Vector2 particlePath2(movingPath[4], movingPath[5]);
    Vector2 particlePath3(movingPath[6], movingPath[7]);
    Vector2 particlePath4(movingPath[8], movingPath[9]);
    Vector2 particlePath5(movingPath[10], movingPath[11]);

    float particleIdx = static_cast<float>(idx / 4 + 1); // count from 1
    float colorIdx    = colorIndex + 1.f;                // count from 1
    vertices.push_back(Vertex{Vector2(-colorIdx, -particleIdx), particlePath0, particlePath1, particlePath2, particlePath3, particlePath4, particlePath5});
    vertices.push_back(Vertex{Vector2(-colorIdx, particleIdx), particlePath0, particlePath1, particlePath2, particlePath3, particlePath4, particlePath5});
    vertices.push_back(Vertex{Vector2(colorIdx, particleIdx), particlePath0, particlePath1, particlePath2, particlePath3, particlePath4, particlePath5});
    vertices.push_back(Vertex{Vector2(colorIdx, -particleIdx), particlePath0, particlePath1, particlePath2, particlePath3, particlePath4, particlePath5});

    faces.push_back(idx);
    faces.push_back(idx + 1);
    faces.push_back(idx + 2);

    faces.push_back(idx);
    faces.push_back(idx + 2);
    faces.push_back(idx + 3);
  }

  /*
   * Main key event handler
   */
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

  /**
   * Callback of the TapGesture
   */
  void OnTap(Actor actor, const TapGesture& tap)
  {
    {
      PlayTapAnimation(5.f, tap.GetLocalPoint());
    }
  }

  /**
   * Callback of the PanGesture
   */
  void OnPan(Actor actor, const PanGesture& gesture)
  {
    if(gesture.GetState() == GestureState::FINISHED)
    {
      switch(mAnimationIndex)
      {
        case 0:
        {
          PlayParticleFadeAnimation(0, NUM_PARTICLE, 0.f, 3.f);
          break;
        }
        case 1:
        {
          PlayBreakAnimation(2.0f);
          break;
        }
        case 2:
        {
          PlayShakeAnimation(0.5f, 2.5f);
          break;
        }
        default:
        {
          break;
        }
      }

      mAnimationIndex = (mAnimationIndex + 1) % 3;
    }
  }

  /**
   * Animate the particle position to make them wandering on the screen with 'seemingly' random fade in/out
   * @param[in] duration The duration for the particle to move a cycle on the path. the bigger this value the slower the floating movement.
   * @param[in] looping Infinite playing or not
   */
  void PlayWanderAnimation(float duration, bool looping = true)
  {
    Animation wanderAnimation = Animation::New(duration);
    wanderAnimation.AnimateTo(Property(mEffect, PERCENTAGE_UNIFORM_NAME), 1.f);
    wanderAnimation.SetLooping(looping); // infinite playing

    wanderAnimation.Play();
  }

  /**
   * Accelerate the particle moving speed
   * @param[in] cycle How many extra cycles to move during the animation
   * @param[in] duration The duration for the animation
   */
  void PlayShakeAnimation(float cycle, float duration)
  {
    if(mShaking)
    {
      return;
    }
    DestroyAnimation(mTapAnimationAux);

    float accelaration = GetFloatUniformValue(ACCELARATION_UNIFORM_NAME);
    mEffect.SetProperty(mEffect.GetPropertyIndex(ACCELARATION_UNIFORM_NAME), accelaration - int(accelaration)); // Set the value as its fractional part
    Animation shakeAnimation = Animation::New(duration);
    shakeAnimation.AnimateBy(Property(mEffect, ACCELARATION_UNIFORM_NAME), cycle, AlphaFunction::EASE_OUT);
    shakeAnimation.FinishedSignal().Connect(this, &SparkleEffectExample::OnShakeAnimationFinished);

    shakeAnimation.Play();
    mShaking = true;
  }

  /**
   * Animate the particles to appear from center and spread all over around
   * @param[in] duration The duration for the animation
   */
  void PlayBreakAnimation(float duration)
  {
    if(GetFloatUniformValue(BREAK_UNIFORM_NAME) > 0.f)
    {
      return;
    }

    // Stop the fading / tap animation before the breaking
    DestroyAnimation(mFadeAnimation);
    mTapIndices.x = mTapIndices.y;
    mEffect.SetProperty(mEffect.GetPropertyIndex(TAP_INDICES_UNIFORM_NAME), mTapIndices);
    mEffect.SetProperty(mEffect.GetPropertyIndex(ACCELARATION_UNIFORM_NAME), 0.f);

    // prepare the animation by setting the uniform to the required value
    mEffect.SetProperty(mEffect.GetPropertyIndex(BREAK_UNIFORM_NAME), 1.f);
    mMeshActor.SetProperty(Actor::Property::SCALE, 0.01f);
    mEffect.SetProperty(mEffect.GetPropertyIndex("uEffectScale"), 0.01f);
    mMeshActor.SetProperty(Actor::Property::POSITION, Vector3(0.f, 0.f, 1.f));

    Animation breakAnimation = Animation::New(duration * 1.5f);
    breakAnimation.AnimateTo(Property(mMeshActor, Actor::Property::SCALE), Vector3(ACTOR_SCALE, ACTOR_SCALE, ACTOR_SCALE), EaseOutSquare);
    breakAnimation.AnimateTo(Property(mEffect, "uEffectScale"), ACTOR_SCALE, EaseOutSquare);
    breakAnimation.AnimateTo(Property(mMeshActor, Actor::Property::POSITION), ACTOR_POSITION, EaseOutSquare);
    breakAnimation.FinishedSignal().Connect(this, &SparkleEffectExample::OnBreakAnimationFinished);

    float              timeUnit = duration / (NUM_PARTICLE + 1) / (NUM_PARTICLE + 1);
    std::ostringstream oss;
    for(unsigned int i = 0; i < NUM_PARTICLE; i++)
    {
      oss.str("");
      oss << OPACITY_UNIFORM_NAME << i << "]";
      mEffect.SetProperty(mEffect.GetPropertyIndex(oss.str()), 0.01f);
      float timeSlice = timeUnit * i * i;
      breakAnimation.AnimateTo(Property(mEffect, oss.str()), 1.f, AlphaFunction::EASE_IN_OUT_SINE, TimePeriod(timeSlice * 0.5f, timeSlice));
    }

    breakAnimation.Play();
  }

  /**
   * Animate the particle opacity
   * Particles with index between startIndex ~ startIndex+numParticle-1 fade to the target opacity one after another
   * @param[in] startIndex The index of the first particle
   * @param[in] numParticle The number of particle to change opacity
   * @param[in] targetValue The final opacity
   * @param[in] duration The duration for the animation
   */
  void PlayParticleFadeAnimation(unsigned int startIndex, unsigned int numParticle, float targetValue, float duration)
  {
    if(GetFloatUniformValue(BREAK_UNIFORM_NAME) > 0.f)
    {
      return;
    }

    // start the opacity animation one particle after another gradually
    float timeSlice    = duration / (numParticle + 1);
    float fadeDuration = timeSlice > 0.5f ? timeSlice : 0.5f;

    Animation          fadeAnimation = Animation::New(duration + fadeDuration * 2.f);
    std::ostringstream oss;
    for(unsigned int i = startIndex; i < numParticle; i++)
    {
      if(i >= NUM_PARTICLE) break; // out of bound

      oss.str("");
      oss << OPACITY_UNIFORM_NAME << i << "]";
      fadeAnimation.AnimateTo(Property(mEffect, oss.str()), targetValue, TimePeriod(timeSlice * i, fadeDuration * 2.f));
    }

    fadeAnimation.Play();
    mFadeAnimation = fadeAnimation;
    mFadeAnimation.FinishedSignal().Connect(this, &SparkleEffectExample::OnFadeAnimationFinished);
  }

  /**
   * Push the particles to the edge all around the circle then bounce back
   * @param[in] duration The duration for the animation
   * @param[in] tapPoint The position of the tap point
   */
  void PlayTapAnimation(float duration, const Vector2& tapPoint)
  {
    if(mTapIndices.y > mTapIndices.x && mTapAnimation.GetCurrentProgress() < 0.2f)
    {
      return;
    }

    Animation animation = Animation::New(duration);
    int       idx       = int(mTapIndices.y) % MAXIMUM_ANIMATION_COUNT;
    mTapIndices.y += 1.f;

    std::ostringstream oss;
    oss << TAP_OFFSET_UNIFORM_NAME << idx << "]";
    mEffect.SetProperty(mEffect.GetPropertyIndex(oss.str()), 0.f);
    animation.AnimateTo(Property(mEffect, oss.str()), 0.75f, CustomBounce);

    oss.str("");
    oss << TAP_POINT_UNIFORM_NAME << idx << "]";
    mEffect.SetProperty(mEffect.GetPropertyIndex(oss.str()), tapPoint / ACTOR_SCALE);

    mEffect.SetProperty(mEffect.GetPropertyIndex(TAP_INDICES_UNIFORM_NAME), mTapIndices);

    if(!mShaking)
    {
      mTapAnimationAux = Animation::New(duration * 0.2f);
      mTapAnimationAux.AnimateBy(Property(mEffect, ACCELARATION_UNIFORM_NAME), 0.15f, AlphaFunction::EASE_IN_OUT);
      mTapAnimationAux.Play();
    }
    animation.Play();
    mTapAnimationIndexPair[animation] = static_cast<int>(mTapIndices.y - 1.f);
    animation.FinishedSignal().Connect(this, &SparkleEffectExample::OnTapAnimationFinished);
    mTapAnimation = animation;
  }

  /**
   * Callback of the animation finished signal
   */
  void OnShakeAnimationFinished(Animation& animation)
  {
    mShaking = false;
  }

  /**
   * Callback of the animation finished signal
   */
  void OnFadeAnimationFinished(Animation& animation)
  {
    mFadeAnimation.Clear();
    mFadeAnimation.Reset();
  }

  /**
   * Callback of the animation finished signal
   */
  void OnBreakAnimationFinished(Animation& animation)
  {
    mEffect.SetProperty(mEffect.GetPropertyIndex(BREAK_UNIFORM_NAME), 0.f);
  }

  /**
   * Callback of the animation finished signal
   */
  void OnTapAnimationFinished(Animation& animation)
  {
    if(mTapAnimationIndexPair[animation] == static_cast<int>(mTapIndices.x))
    {
      mTapIndices.x += 1.f;
      if(mTapIndices.x >= mTapIndices.y)
      {
        mTapIndices = Vector2::ZERO;
      }
      mEffect.SetProperty(mEffect.GetPropertyIndex(TAP_INDICES_UNIFORM_NAME), mTapIndices);
    }

    mTapAnimationIndexPair.erase(animation);
    if(mTapAnimationIndexPair.size() < 1 && mTapIndices != Vector2::ZERO)
    {
      mTapIndices = Vector2::ZERO;
      mEffect.SetProperty(mEffect.GetPropertyIndex(TAP_INDICES_UNIFORM_NAME), mTapIndices);
    }

    animation.Clear();
    animation.Reset();
  }

  /**
   * Helper retrieve a uniform value from the Sparkle effect shader
   * @param[in] uniformName The uniform
   * @return The float value
   */
  float GetFloatUniformValue(const std::string& uniformName)
  {
    float value;
    mEffect.GetProperty(mEffect.GetPropertyIndex(uniformName)).Get(value);
    return value;
  }

  /**
   * Terminate the given animation
   */
  void DestroyAnimation(Animation& animation)
  {
    if(animation)
    {
      animation.Clear();
      animation.Reset();
    }
  }

private:
  Application& mApplication;
  Shader       mEffect;
  ImageView    mCircleBackground;
  Actor        mMeshActor;

  PanGestureDetector mPanGestureDetector;
  TapGestureDetector mTapDetector;

  Animation mFadeAnimation;
  Animation mTapAnimation;
  Animation mTapAnimationAux;

  Vector2      mTapIndices;
  unsigned int mAnimationIndex;
  bool         mShaking;

  std::map<Animation, int> mTapAnimationIndexPair;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application          application = Application::New(&argc, &argv);
  SparkleEffectExample theApp(application);
  application.MainLoop();
  return 0;
}
