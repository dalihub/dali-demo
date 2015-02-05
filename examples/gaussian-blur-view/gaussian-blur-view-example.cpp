//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

//#define ADD_REMOVE_FROM_STAGE_TEST

const char* BACKGROUND_IMAGE_PATH = DALI_IMAGE_DIR "desktop_background_1440x2560.png";
const char* UI_DIFFUSE_IMAGE( DALI_IMAGE_DIR "UI-Leather-DIFF.png" );

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


// buttons
const char* BUTTON_BACKGROUND_IMAGE_PATH = DALI_IMAGE_DIR "GreyThinLine30x30-76x78.png";

// These values depend on the button background image
const float BUTTON_IMAGE_BORDER_LEFT   = 16.0f;
const float BUTTON_IMAGE_BORDER_RIGHT  = 16.0f;
const float BUTTON_IMAGE_BORDER_TOP    = 13.0f;
const float BUTTON_IMAGE_BORDER_BOTTOM = 20.0f;

const Vector4 BUTTON_IMAGE_BORDER( BUTTON_IMAGE_BORDER_LEFT, BUTTON_IMAGE_BORDER_TOP, BUTTON_IMAGE_BORDER_RIGHT, BUTTON_IMAGE_BORDER_BOTTOM );

const float UI_MARGIN = 4.0f;                              ///< Screen Margin for placement of UI buttons
const Vector3 BUTTON_SIZE_CONSTRAINT( 0.24f, 0.09f, 1.0f );
const char * BUTTON_QUIT = "Quit";
const char * BUTTON_QUIT_LABEL ="Quit";

} // namespace



/**
 * This example demonstrates a blur effect.
 */
class TestApp : public ConnectionTracker
{
public:

  TestApp( Application &application )
  : mApplication(application)
  , mCurrentAnimation(0)
  {
    application.InitSignal().Connect(this, &TestApp::Create);
    application.TerminateSignal().Connect(this, &TestApp::Terminate);
  }

  ~TestApp()
  {
  }

public:

  void Create(Application& application)
  {
    // Preload images
    mDiffuseImage = Image::New( UI_DIFFUSE_IMAGE );

    // Creation is deferred until images have loaded
    mDiffuseImage.LoadingFinishedSignal().Connect( this, &TestApp::Create2 );
  }

#ifdef ADD_REMOVE_FROM_STAGE_TEST
  void Create2(Image loadedImage)
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    mStageRootActor = Actor::New();
    mStageRootActor.SetParentOrigin(ParentOrigin::CENTER);
    mStageRootActor.SetSize(stageSize);
    stage.Add(mStageRootActor);

    // Create the object that will perform the blurring work
    mGaussianBlurView = Dali::Toolkit::GaussianBlurView::New();
    mGaussianBlurView.SetParentOrigin(ParentOrigin::CENTER);
    mGaussianBlurView.SetSize(stageSize * 0.75f);
    mStageRootActor.Add(mGaussianBlurView);
    mGaussianBlurView.Activate();

    // Create the background image
    Image background = Image::New(BACKGROUND_IMAGE_PATH);
    ImageActor backgroundImage = ImageActor::New(background);
    backgroundImage.SetParentOrigin(ParentOrigin::CENTER);
    backgroundImage.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );

    Layer backgroundLayer = Layer::New();
    backgroundLayer.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    backgroundLayer.SetParentOrigin( ParentOrigin::CENTER );
    mGaussianBlurView.Add( backgroundLayer );
    backgroundLayer.Add( backgroundImage );

    Layer foregroundLayer = Layer::New();
    foregroundLayer.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    foregroundLayer.SetParentOrigin( ParentOrigin::CENTER );
    mGaussianBlurView.Add( foregroundLayer );

    // Create visible actors
    mObjectRootActor = Actor::New();
    mObjectRootActor.SetParentOrigin( ParentOrigin::CENTER );
    mObjectRootActor.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    foregroundLayer.Add( mObjectRootActor );

    ImageActor imageActor = ImageActor::New( mDiffuseImage );
    imageActor.SetParentOrigin( ParentOrigin::CENTER );
    imageActor.SetSize( stageSize * 0.75f);
    mObjectRootActor.Add( imageActor );

    imageActor = ImageActor::New( mDiffuseImage );
    imageActor.SetParentOrigin( ParentOrigin::CENTER );
    imageActor.SetSize( stageSize * 0.5f );
    imageActor.SetPosition(0.0f, 0.0f, 100.0f);
    mObjectRootActor.Add( imageActor );

    AnimateGaussianBlurView();

    // Gesture detection
    mTapGestureDetector = TapGestureDetector::New();
    mTapGestureDetector.Attach( mStageRootActor );
    mTapGestureDetector.DetectedSignal().Connect( this, &TestApp::OnTap );
  }

  void AnimateGaussianBlurView()
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
    float animDuration = 10.0f;
    mRotationAnimation = Animation::New(animDuration);
    mRotationAnimation.RotateBy(mGaussianBlurView, Degree(720), Vector3::YAXIS, AlphaFunctions::EaseInOut);
    mRotationAnimation.SetEndAction( Animation::Discard );
    mRotationAnimation.SetLooping( true );
    mRotationAnimation.Play();

    // BLUR
    mBlurAnimation = Animation::New( 4.0f );
    mBlurAnimation.AnimateTo( Property( mGaussianBlurView, mGaussianBlurView.GetBlurStrengthPropertyIndex() ), 0.0f, AlphaFunctions::Linear, TimePeriod(0.0f, 0.5f) );
    mBlurAnimation.AnimateTo( Property( mGaussianBlurView, mGaussianBlurView.GetBlurStrengthPropertyIndex() ), 1.0f, AlphaFunctions::Linear, TimePeriod(2.0f, 0.5f) );
    mBlurAnimation.SetEndAction( Animation::Discard );
    mBlurAnimation.SetLooping( true );
    mBlurAnimation.Play();
  }
#else //#ifdef ADD_REMOVE_FROM_STAGE_TEST

  void Create2(Image loadedImage)
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    mStageRootActor = Actor::New();
    mStageRootActor.SetParentOrigin(ParentOrigin::CENTER);
    mStageRootActor.SetSize(stageSize);
    stage.Add(mStageRootActor);

    // Create the object that will perform the blurring work
    mGaussianBlurView = Dali::Toolkit::GaussianBlurView::New();
    mGaussianBlurView.SetParentOrigin(ParentOrigin::CENTER);
    mGaussianBlurView.SetSize(stageSize * 0.75f);
    mStageRootActor.Add(mGaussianBlurView);
    mGaussianBlurView.Activate();

    // Create the Quit button
    PushButton button;
    button = CreateButton(BUTTON_QUIT, BUTTON_QUIT_LABEL, BUTTON_SIZE_CONSTRAINT);
    button.SetAnchorPoint(AnchorPoint::BOTTOM_RIGHT);
    button.SetParentOrigin(ParentOrigin::BOTTOM_RIGHT);
    button.SetPosition(-UI_MARGIN, -UI_MARGIN);

    // Create the background image
    Image background = Image::New(BACKGROUND_IMAGE_PATH);
    ImageActor backgroundImage = ImageActor::New(background);
    backgroundImage.SetParentOrigin(ParentOrigin::CENTER);
    backgroundImage.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );

    Layer backgroundLayer = Layer::New();
    backgroundLayer.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    backgroundLayer.SetParentOrigin( ParentOrigin::CENTER );
    mGaussianBlurView.Add( backgroundLayer );
    backgroundLayer.Add( backgroundImage );

    Layer foregroundLayer = Layer::New();
    foregroundLayer.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    foregroundLayer.SetParentOrigin( ParentOrigin::CENTER );
    mGaussianBlurView.Add( foregroundLayer );

    // Create visible actors
    mObjectRootActor = Actor::New();
    mObjectRootActor.SetParentOrigin( ParentOrigin::CENTER );
    mObjectRootActor.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    foregroundLayer.Add( mObjectRootActor );

    ImageActor imageActor = ImageActor::New( mDiffuseImage );
    imageActor.SetParentOrigin( ParentOrigin::CENTER );
    imageActor.SetSize( stageSize * 0.75f);
    mObjectRootActor.Add( imageActor );

    imageActor = ImageActor::New( mDiffuseImage );
    imageActor.SetParentOrigin( ParentOrigin::CENTER );
    imageActor.SetSize( stageSize * 0.5f );
    imageActor.SetPosition(0.0f, 0.0f, 100.0f);
    mObjectRootActor.Add( imageActor );

    ToggleAnimation();

    // Gesture detection
    mTapGestureDetector = TapGestureDetector::New();
    mTapGestureDetector.Attach( mStageRootActor );
    mTapGestureDetector.DetectedSignal().Connect( this, &TestApp::OnTap );
  }

  PushButton CreateButton(const std::string& name, const std::string& label, const Vector3& sizeConstraint)
  {
    // Create the button
    Image img = Image::New(BUTTON_BACKGROUND_IMAGE_PATH);
    ImageActor background = ImageActor::New(img);

    background.SetStyle(ImageActor::STYLE_NINE_PATCH);
    background.SetNinePatchBorder(BUTTON_IMAGE_BORDER);

    PushButton button = PushButton::New();
    button.SetName(name);
    button.SetBackgroundImage(background);

    // Set the button's size
    button.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), RelativeToConstraint( sizeConstraint ) ) );

    mStageRootActor.Add(button);

    button.ClickedSignal().Connect(this, &TestApp::OnButtonTouchEvent);

    SetButtonLabel(button, label, TextView::Center);

    return button;
  }

  void SetButtonLabel(PushButton button, const std::string& label, const TextView::LineJustification justification)
  {
    Font font = Font::New();
    Vector3 textSize = font.MeasureText(label);

    // Add a text label to the button
    TextView textView = TextView::New( label );
    textView.SetAnchorPoint( AnchorPoint::CENTER );
    textView.SetParentOrigin( ParentOrigin::CENTER );
    textView.SetMultilinePolicy(TextView::SplitByWord);
    textView.SetWidthExceedPolicy(TextView::Original);
    textView.SetTextAlignment(static_cast< Alignment::Type >( Alignment::HorizontalCenter | Alignment::VerticalCenter ));
    textView.SetLineJustification(justification);
    textView.SetSize(textSize);

    Alignment align = Alignment::New();
    align.Add(textView);
    align.SetScaling( Alignment::ShrinkToFitKeepAspect );

    button.SetLabelText(align);
  }

  /**
   * Signal handler, called when the button is pressed
   */
  bool OnButtonTouchEvent(Button button)
  {
    if(button.GetName() == BUTTON_QUIT)
    {
      // quit the application
      mApplication.Quit();
    }

    return true;
  }

  void AnimateGaussianBlurView()
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
    float animDuration = 10.0f;
    mRotationAnimation = Animation::New(animDuration);
    mRotationAnimation.RotateBy(mGaussianBlurView, Degree(720), Vector3::YAXIS, AlphaFunctions::EaseInOut);
    mRotationAnimation.SetEndAction( Animation::Discard );
    mRotationAnimation.SetLooping( true );
    mRotationAnimation.Play();


    // RESIZE
    Stage stage = Stage::GetCurrent();
    Vector3 stageSize(stage.GetSize().width, stage.GetSize().height, 1.0f);
    mResizeAnimation = Animation::New( 7.5f );
    mResizeAnimation.AnimateTo( Property(mGaussianBlurView, Actor::SIZE), stageSize * Vector3(0.5f, 0.5f, 1.0f), AlphaFunctions::Bounce, TimePeriod(2.5f) );
    mResizeAnimation.SetEndAction( Animation::Discard );
    mResizeAnimation.SetLooping( true );
    mResizeAnimation.Play();


    // TRANSLATE
    mTranslationAnimation = Animation::New( 7.5f );
    mTranslationAnimation.AnimateBy( Property(mObjectRootActor, Actor::POSITION), Vector3(100.0f, 0.0f, 0.0f), AlphaFunctions::Bounce, TimePeriod(2.5f) );
    mTranslationAnimation.AnimateBy( Property(mObjectRootActor, Actor::POSITION), Vector3(300.0f, 0.0f, 0.0f), AlphaFunctions::Bounce, TimePeriod(2.5f, 2.5f) );
    mTranslationAnimation.AnimateBy( Property(mObjectRootActor, Actor::POSITION), Vector3(0.0f, 0.0f, 0.0f), AlphaFunctions::Bounce, TimePeriod(5.0f, 2.5f) );
    mTranslationAnimation.SetEndAction( Animation::Discard );
    mTranslationAnimation.SetLooping( true );
    mTranslationAnimation.Play();


    // BLUR
    mBlurAnimation = Animation::New( 4.0f );
    mBlurAnimation.AnimateTo( Property( mGaussianBlurView, mGaussianBlurView.GetBlurStrengthPropertyIndex() ), 0.0f, AlphaFunctions::Linear, TimePeriod(0.0f, 0.5f) );
    mBlurAnimation.AnimateTo( Property( mGaussianBlurView, mGaussianBlurView.GetBlurStrengthPropertyIndex() ), 1.0f, AlphaFunctions::Linear, TimePeriod(2.0f, 0.5f) );
    mBlurAnimation.SetEndAction( Animation::Discard );
    mBlurAnimation.SetLooping( true );
    mBlurAnimation.Play();
  }

  void AnimateActorRoot()
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
    float animDuration = 10.0f;
    mRotationAnimation = Animation::New(animDuration);
    mRotationAnimation.RotateBy(mObjectRootActor, Degree(720), Vector3::YAXIS, AlphaFunctions::EaseInOut);
    mRotationAnimation.SetEndAction( Animation::Discard );
    mRotationAnimation.SetLooping( true );
    mRotationAnimation.Play();


    // RESIZE
    Stage stage = Stage::GetCurrent();
    Vector3 stageSize(stage.GetSize().width, stage.GetSize().height, 1.0f);
    mResizeAnimation = Animation::New( 7.5f );
    mResizeAnimation.AnimateTo( Property(mGaussianBlurView, Actor::SIZE), stageSize * Vector3(0.5f, 0.5f, 1.0f), AlphaFunctions::Bounce, TimePeriod(2.5f) );
    mResizeAnimation.SetEndAction( Animation::Discard );
    mResizeAnimation.SetLooping( true );
    mResizeAnimation.Play();


    // TRANSLATE
    mTranslationAnimation = Animation::New( 7.5f );
    mTranslationAnimation.AnimateBy( Property(mObjectRootActor, Actor::POSITION), Vector3(100.0f, 0.0f, 0.0f), AlphaFunctions::Bounce, TimePeriod(2.5f) );
    mTranslationAnimation.AnimateBy( Property(mObjectRootActor, Actor::POSITION), Vector3(300.0f, 0.0f, 0.0f), AlphaFunctions::Bounce, TimePeriod(2.5f, 2.5f) );
    mTranslationAnimation.AnimateBy( Property(mObjectRootActor, Actor::POSITION), Vector3(0.0f, 0.0f, 0.0f), AlphaFunctions::Bounce, TimePeriod(5.0f, 2.5f) );
    mTranslationAnimation.SetEndAction( Animation::Discard );
    mTranslationAnimation.SetLooping( true );
    mTranslationAnimation.Play();


    // BLUR
    mBlurAnimation = Animation::New( 4.0f );
    mBlurAnimation.AnimateTo( Property( mGaussianBlurView, mGaussianBlurView.GetBlurStrengthPropertyIndex() ), 0.0f, AlphaFunctions::Linear, TimePeriod(0.0f, 0.5f) );
    mBlurAnimation.AnimateTo( Property( mGaussianBlurView, mGaussianBlurView.GetBlurStrengthPropertyIndex() ), 1.0f, AlphaFunctions::Linear, TimePeriod(2.0f, 0.5f) );
    mBlurAnimation.SetEndAction( Animation::Discard );
    mBlurAnimation.SetLooping( true );
    mBlurAnimation.Play();
  }

  void ToggleAnimation()
  {
    mCurrentAnimation++;
    if(mCurrentAnimation == NUM_MOVEMENT_ANIMATIONS)
    {
      mCurrentAnimation = 0;
    }

    switch(mCurrentAnimation)
    {
    case 0:
      AnimateActorRoot();
      break;

    case 1:
      AnimateGaussianBlurView();
      break;
    }
  }
#endif //#ifdef ADD_REMOVE_FROM_STAGE_TEST

  void Terminate( Application& app )
  {
  }

  void OnTap( Actor actor, const TapGesture& tapGesture )
  {
    Stage stage = Stage::GetCurrent();

#ifdef ADD_REMOVE_FROM_STAGE_TEST
    if(mGaussianBlurView.OnStage())
    {
      mStageRootActor.Remove(mGaussianBlurView);
      mGaussianBlurView.Deactivate();
    }
    else
    {
      mStageRootActor.Add(mGaussianBlurView);
      mGaussianBlurView.Activate();
    }
#else //#ifdef ADD_REMOVE_FROM_STAGE_TEST
    ToggleAnimation();
#endif //#ifdef ADD_REMOVE_FROM_STAGE_TEST
  }

private:

  Application& mApplication;

  TapGestureDetector mTapGestureDetector;

  Actor mStageRootActor;

  Actor mObjectRootActor;
  Image mDiffuseImage;

  unsigned int mCurrentAnimation;
  Animation mRotationAnimation;
  Animation mResizeAnimation;
  Animation mTranslationAnimation;
  Animation mBlurAnimation;
  Animation mPulseBloomIntensityAnim;

  // for rendering the blur / bloom
  GaussianBlurView mGaussianBlurView;
  BloomView mBloomView;
};

/*****************************************************************************/

static void
RunTest( Application& application )
{
  TestApp theApp(application);
  application.MainLoop();
}

/*****************************************************************************/

int
main(int argc, char **argv)
{
  Application application = Application::New(&argc, &argv);

  RunTest(application);

  return 0;
}
