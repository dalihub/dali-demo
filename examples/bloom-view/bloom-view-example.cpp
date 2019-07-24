/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/bloom-view/bloom-view.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* BACKGROUND_IMAGE_PATH( DEMO_IMAGE_DIR "desktop_background_1440x2560.png" );
const char* UI_DIFFUSE_IMAGE( DEMO_IMAGE_DIR "UI-Leather-DIFF.png" );

const Rect<int> UI_PIXEL_AREA( 0, 0, 720, 1280 );

const Rect<int> PANEL1_PIXEL_AREA( 0, 0, 720,  39 );
const Rect<int> PANEL2_PIXEL_AREA( 0, 39, 720, 100 );

const unsigned int NUM_MOVEMENT_ANIMATIONS = 2;

// for animating bloom intensity on tap gesture
const float PULSE_BLOOM_INCREASE_ANIM_TIME = 1.175;
const float PULSE_BLOOM_DECREASE_ANIM_TIME = 2.4;
const float PULSE_BLOOM_TOTAL_ANIM_TIME = PULSE_BLOOM_INCREASE_ANIM_TIME + PULSE_BLOOM_DECREASE_ANIM_TIME;
const float PULSE_BLOOM_INTENSITY_DEFAULT = 1.0f;
const float PULSE_BLOOM_INTENSITY_INCREASE = 3.0f;

// These values depend on the button background image
const Vector4 BUTTON_IMAGE_BORDER(16.0f, 16.0f, 16.0f, 20.0f);

const float UI_MARGIN = 4.0f;                              ///< Screen Margin for placement of UI buttons
const Vector3 BUTTON_SIZE_CONSTRAINT( 0.24f, 0.09f, 1.0f );

} // namespace

/**
 * This example demonstrates a bloom effect.
 */
class BloomExample : public ConnectionTracker
{
public:

  BloomExample( Application &application )
  : mApplication(application),
    mCurrentAnimation(0)
  {
    application.InitSignal().Connect( this, &BloomExample::Create );
  }

  ~BloomExample()
  {
  }

public:

  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();
    Vector2 viewSize( stageSize );

    mRootActor = Actor::New();
    mRootActor.SetParentOrigin( ParentOrigin::CENTER );
    mRootActor.SetSize( stageSize );
    stage.Add( mRootActor );

    // Create the object that will perform the blooming work
    mBloomView = Dali::Toolkit::BloomView::New();
    mBloomView.SetParentOrigin( ParentOrigin::CENTER );
    mBloomView.SetSize( viewSize );
    mRootActor.Add( mBloomView );
    mBloomView.Activate();

    Layer backgroundLayer = Layer::New();
    backgroundLayer.SetSize( viewSize );
    backgroundLayer.SetParentOrigin( ParentOrigin::CENTER );
    mBloomView.Add( backgroundLayer );

    // Create the background image
    ImageView backgroundImage = ImageView::New( BACKGROUND_IMAGE_PATH );
    backgroundImage.SetParentOrigin( ParentOrigin::CENTER );
    backgroundImage.SetSize( viewSize );
    backgroundLayer.Add( backgroundImage );

    Layer foregroundLayer = Layer::New();
    foregroundLayer.SetSize( viewSize );
    foregroundLayer.SetParentOrigin( ParentOrigin::CENTER );
    mBloomView.Add( foregroundLayer );

    // Create visible actors
    mObjectRootActor = Actor::New();
    mObjectRootActor.SetParentOrigin( ParentOrigin::CENTER );
    mObjectRootActor.SetSize( viewSize );
    foregroundLayer.Add( mObjectRootActor );

    ImageView imageView = ImageView::New( UI_DIFFUSE_IMAGE );
    imageView.SetParentOrigin( ParentOrigin::CENTER );
    imageView.SetSize( viewSize );
    mObjectRootActor.Add( imageView );

    imageView = ImageView::New( UI_DIFFUSE_IMAGE );
    imageView.SetParentOrigin( ParentOrigin::CENTER );
    imageView.SetSize( stageSize * 0.5f );
    imageView.SetPosition( 0.0f, 0.0f, 100.0f );
    mObjectRootActor.Add( imageView );

    AnimateBloomView();
    PulseBloomIntensity();

    // Respond to key events
    stage.KeyEventSignal().Connect( this, &BloomExample::OnKeyEvent );
  }

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

  void AnimateBloomView()
  {
    if(mRotationAnimation)
    {
      mRotationAnimation.Stop();
    }
    if(mResizeAnimation)
    {
      mResizeAnimation.Stop();
    }
    if(mTranslationAnimation)
    {
      mTranslationAnimation.Stop();
    }
    if(mBlurAnimation)
    {
      mBlurAnimation.Stop();
    }

    // ROTATE
    mRotationAnimation = Animation::New( 5.0f );
    mRotationAnimation.AnimateBy( Property( mObjectRootActor, Actor::Property::ORIENTATION ), Quaternion( Radian( Degree( 360 )), Vector3::YAXIS ), AlphaFunction::EASE_IN_OUT );
    mRotationAnimation.SetEndAction( Animation::Discard );
    mRotationAnimation.SetLooping( true );
    mRotationAnimation.Play();

    // TRANSLATE
    mTranslationAnimation = Animation::New( 7.5f );
    mTranslationAnimation.AnimateBy( Property(mObjectRootActor, Actor::Property::POSITION), Vector3(100.0f, 0.0f, 0.0f), AlphaFunction::BOUNCE, TimePeriod(2.5f) );
    mTranslationAnimation.AnimateBy( Property(mObjectRootActor, Actor::Property::POSITION), Vector3(300.0f, 0.0f, 0.0f), AlphaFunction::BOUNCE, TimePeriod(2.5f, 2.5f) );
    mTranslationAnimation.AnimateBy( Property(mObjectRootActor, Actor::Property::POSITION), Vector3(0.0f, 0.0f, 0.0f),   AlphaFunction::BOUNCE, TimePeriod(5.0f, 2.5f) );
    mTranslationAnimation.SetEndAction( Animation::Discard );
    mTranslationAnimation.SetLooping( true );
    //mTranslationAnimation.Play();

    // BLUR
    mBlurAnimation = Animation::New( 4.0f );
    mBlurAnimation.AnimateTo( Property( mBloomView, mBloomView.GetBlurStrengthPropertyIndex() ), 0.0f, AlphaFunction::LINEAR, TimePeriod(0.0f, 0.5f) );
    mBlurAnimation.AnimateTo( Property( mBloomView, mBloomView.GetBlurStrengthPropertyIndex() ), 1.0f, AlphaFunction::LINEAR, TimePeriod(2.0f, 0.5f) );
    mBlurAnimation.SetEndAction( Animation::Discard );
    mBlurAnimation.SetLooping( true );
    mBlurAnimation.Play();
  }

  void PulseBloomIntensity()
  {
    mPulseBloomIntensityAnim = Animation::New( 2.5f );
    mPulseBloomIntensityAnim.AnimateTo( Property(mBloomView, mBloomView.GetBloomIntensityPropertyIndex()), 3.0f, AlphaFunction::BOUNCE, TimePeriod(2.5f) );
    mPulseBloomIntensityAnim.SetEndAction( Animation::Discard );
    mPulseBloomIntensityAnim.SetLooping( true );
    mPulseBloomIntensityAnim.Play();
  }

private:

  Application& mApplication;

  Actor mRootActor;

  Actor mObjectRootActor;

  unsigned int mCurrentAnimation;
  Animation mRotationAnimation;
  Animation mResizeAnimation;
  Animation mTranslationAnimation;
  Animation mBlurAnimation;
  Animation mPulseBloomIntensityAnim;

  BloomView mBloomView;
};

int main(int argc, char **argv)
{
  Application application = Application::New( &argc, &argv );

  BloomExample theApp( application );
  application.MainLoop();

  return 0;
}
