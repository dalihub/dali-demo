/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include "../shared/view.h"

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;
using std::string;
using namespace DemoHelper;

namespace
{
const char* gModelFile = DALI_MODEL_DIR "AlbumCute.dae";
const char* gBinaryModelFile = DALI_MODEL_DIR "AlbumCute.dali-bin";

const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-default.png" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );

const char* APPLICATION_TITLE_PAN_LIGHT( "Lighting: Pan Light" );
const char* APPLICATION_TITLE_PAN_OBJECT( "Lighting: Pan Object" );
const char* APPLICATION_TITLE_PAN_SCENE( "Lighting: Pan Scene" );
const char* APPLICATION_TITLE_ROTATE_SCENE( "Lighting: Rotate Scene" );
const char* CHANGE_EFFECT_IMAGE( DALI_IMAGE_DIR "icon-change.png" );
const char* RESET_ICON( DALI_IMAGE_DIR "icon-reset.png" );

const Quaternion JAUNTY_ROTATION(Math::PI/5.0f, Math::PI/5.0f, 0.0f); // Euler angles
const float MIN_PINCH_SCALE( 0.3f );
const float MAX_PINCH_SCALE( 2.05f );

const float R3_2(0.8660254);
const Vector3 TOP_POINT(  0.0f, -1.0f,  0.0f);
const Vector3 LEFT_POINT( -R3_2, 0.5f,  0.0f);
const Vector3 RIGHT_POINT( R3_2, 0.5f,  0.0f);
const Vector3 FRONT_POINT( 0.0f, 0.0f, 20.0f);

const Vector2 DEFAULT_STAGE_SIZE( 480.0f, 800.0f );

}

/**
 * This example shows a fixed point light onto an animating model
 * casting a shadow onto a wall. The whole scene can be rotated.
 */

class TestApp : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  TestApp(Application &app)
  : mApp(app),
    mPaused(false),
    mTranslation(Vector3::ZERO),
    mLongitudinal(15.0f),
    mAxisTilt(30.0f),
    mLightLongitudinal(0.0f),
    mLightAxisTilt(0.0f),
    mObjectLongitudinal(0.0f),
    mObjectAxisTilt(0.0f),
    mPinchScale(0.5f),
    mScaleAtPinchStart(0.5f),
    mPanState(PAN_SCENE)
  {
    app.InitSignal().Connect(this, &TestApp::Create);
    app.TerminateSignal().Connect(this, &TestApp::Terminate);
  }

  ~TestApp()
  {
    // Nothing to do here; All the members of this class get deleted automatically and they delete their children
  }

public:
  struct PositionInFrontOf
  {
    PositionInFrontOf()
    {
    }

    Vector3 operator()( const Vector3& current, const PropertyInput& property )
    {
      Vector3 position = property.GetVector3();
      position.z += 1.0f;
      return position;
    }
  };

  struct QuaternionEqualToConstraint
  {
    QuaternionEqualToConstraint()
    {
    }

    Quaternion operator()( const Quaternion& current, const PropertyInput& property )
    {
      return property.GetQuaternion();
    }
  };


  /**
   * This method gets called once the main loop of application is up and running
   */
  void Create(Application& app)
  {
    srand(0); // Want repeatable path

    Stage::GetCurrent().KeyEventSignal().Connect(this, &TestApp::OnKeyEvent);

    mModel = Model::New(gBinaryModelFile);  // trigger model load
    mModel.LoadingFinishedSignal().Connect(this, &TestApp::BinaryModelLoaded);

    CreateToolbarAndView(app);
    CreateShadowViewAndLights();
  }

  void CreateToolbarAndView(Application& app)
  {
    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Toolkit::ToolBar toolBar;
    mContents = DemoHelper::CreateView( app,
                                        mView,
                                        toolBar,
                                        BACKGROUND_IMAGE,
                                        TOOLBAR_IMAGE,
                                        "" );

    // Add an effect-changing button on the right of the tool bar.
    Image imageChangeEffect = Image::New( CHANGE_EFFECT_IMAGE );
    Toolkit::PushButton effectChangeButton = Toolkit::PushButton::New();
    effectChangeButton.SetBackgroundImage(imageChangeEffect);
    effectChangeButton.ClickedSignal().Connect( this, &TestApp::OnEffectButtonClicked );
    toolBar.AddControl( effectChangeButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

    // Add title to the tool bar.
    mTitleActor = Toolkit::TextView::New();
    toolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Toolkit::Alignment::HorizontalCenter );

    // Set Title text
    mTitleActor.SetText( APPLICATION_TITLE_PAN_SCENE );
    mTitleActor.SetSize( Font::New().MeasureText( APPLICATION_TITLE_PAN_SCENE ) );
    mTitleActor.SetStyleToCurrentText( DemoHelper::GetDefaultTextStyle() );

    //Add a reset button
    Image resetImage = Image::New( RESET_ICON );
    Toolkit::PushButton resetButton = Toolkit::PushButton::New();
    resetButton.SetBackgroundImage( resetImage );
    resetButton.ClickedSignal().Connect( this, &TestApp::OnResetPressed );
    toolBar.AddControl( resetButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalCenter, DemoHelper::DEFAULT_PLAY_PADDING );

    // Setup
    mView.SetPosition(Vector3(0.0f, 0.0f, -50));

    mContents.SetPosition(mTranslation);
    mContents.SetRotation(CalculateWorldRotation(Radian(mLongitudinal), Radian(mAxisTilt)));
    mContents.SetScale(mPinchScale, mPinchScale, mPinchScale);

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( mView );
    mPanGestureDetector.DetectedSignal().Connect(this, &TestApp::OnPan);

    mPinchGestureDetector = PinchGestureDetector::New();
    mPinchGestureDetector.Attach( mView );
    mPinchGestureDetector.DetectedSignal().Connect(this, &TestApp::OnPinch);

    mTapGestureDetector = TapGestureDetector::New(1, 1);
    mTapGestureDetector.Attach( mView );
    mTapGestureDetector.DetectedSignal().Connect(this, &TestApp::OnTap);
  }

  /**
   * This method gets called once the model is loaded by the resource manager
   */
  void BinaryModelLoaded(Model model)
  {
    if( model.GetLoadingState() == ResourceLoadingSucceeded )
    {
      std::cout << "Succeeded loading binary model" << std::endl;

      ModelReady();
    }
    else
    {
      std::cout << "Failed loading binary model" << std::endl;

      mModel = Model::New(gModelFile);
      mModel.LoadingFinishedSignal().Connect(this, &TestApp::ModelLoaded);
    }
  }

  void ModelLoaded(Model model)
  {
    if( model.GetLoadingState() == ResourceLoadingSucceeded )
    {
      std::cout << "Succeeded loading collada model" << std::endl;

      model.Save(gBinaryModelFile);
      ModelReady();
    }
    else
    {
      std::cout << "Failed loading collada model" << std::endl;

      mApp.Quit();
    }
  }

  void CreateShadowViewAndLights()
  {
    mShadowView = Toolkit::ShadowView::New();
    mShadowView.SetName("Container");
    mShadowView.SetParentOrigin(ParentOrigin::CENTER);
    mShadowView.SetAnchorPoint(AnchorPoint::CENTER);
    mShadowView.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    mShadowView.SetPointLightFieldOfView( Math::PI / 2.0f);
    mContents.Add(mShadowView);

    Image brickWall = Image::New(DALI_IMAGE_DIR "brick-wall.jpg");
    mShadowPlaneBg = ImageActor::New(brickWall);
    mShadowPlaneBg.SetParentOrigin(ParentOrigin::CENTER);
    mShadowPlaneBg.SetAnchorPoint(AnchorPoint::CENTER);
    mShadowPlaneBg.SetName("Plane");
    mShadowPlaneBg.SetSize(1000.0f, 1000.0f);
    mContents.Add(mShadowPlaneBg);
    mShadowPlaneBg.SetPosition(Vector3(50.0f, 50.0f, -200.0f));

    mShadowView.SetShadowPlane(mShadowPlaneBg);
    mShadowView.Activate();

    mLightAnchor = Actor::New();
    mLightAnchor.SetParentOrigin(ParentOrigin::CENTER);
    mLightAnchor.SetAnchorPoint(AnchorPoint::CENTER);
    mLightAnchor.SetRotation(CalculateWorldRotation(Radian(mLightLongitudinal), Radian(mLightAxisTilt)));

    // Work out a scaling factor as the initial light position was calculated for desktop
    // Need to scale light position as model size is based on stage size (i.e. much bigger on device)
    Vector2 stageSize( Stage::GetCurrent().GetSize() );
    float scaleFactor = stageSize.x / DEFAULT_STAGE_SIZE.x;

    mCastingLight = Actor::New();
    mCastingLight.SetParentOrigin(ParentOrigin::CENTER);
    mCastingLight.SetAnchorPoint(AnchorPoint::CENTER);
    mCastingLight.SetPosition( Vector3( 0.0f, 0.0f, 800.0f ) * scaleFactor );

    TextStyle style;
    style.SetFontPointSize( PointSize(DemoHelper::ScalePointSize(20.0f)) );
    style.SetFontName("Times New Roman");
    style.SetFontStyle("Book");

    TextActorParameters parameters( style, TextActorParameters::FONT_DETECTION_ON );
    TextActor text = TextActor::New("Light", parameters);
    text.SetColor(Color::BLUE);

    mCastingLight.Add(text);
    mLightAnchor.Add(mCastingLight);
    mShadowPlaneBg.Add(mLightAnchor);

    text.SetParentOrigin(ParentOrigin::CENTER);
    mShadowView.SetPointLight(mCastingLight);
  }

  void ModelReady()
  {
    mModelActor = ModelActorFactory::BuildActorTree(mModel, "");  // Gets root actor

    if (mModelActor)
    {
      Vector2 stageSize(Stage::GetCurrent().GetSize());

      mModelActor.SetSize(250.0f, 250.0f);
      mModelActor.SetPosition(0.0f, 0.0f, 130.0f);

      //Create a Key light
      Light keylight = Light::New("KeyLight");
      keylight.SetFallOff(Vector2(10000.0f, 10000.0f));
      //keylight.SetSpecularColor(Vector3::ZERO);
      mKeyLightActor = LightActor::New();
      mKeyLightActor.SetParentOrigin(ParentOrigin::CENTER);
      mKeyLightActor.SetName(keylight.GetName());

      //Add all the actors to the stage
      mCastingLight.Add(mKeyLightActor);
      mKeyLightActor.SetLight(keylight);

      mShadowView.Add(mModelActor);


      if (mModel.NumberOfAnimations())
      {
        mModelAnimation = ModelActorFactory::BuildAnimation(mModel, mModelActor, 0);
        mModelAnimation.SetDuration(4.0f);
        mModelAnimation.SetLooping(true);
        mModelAnimation.Play();
      }

      //StartAnimation();
    }
  }

  Quaternion CalculateWorldRotation(Radian longitude, Radian axisTilt )
  {
    Quaternion q(longitude, Vector3::YAXIS);
    Quaternion p(axisTilt, Vector3::XAXIS);
    return p*q;
  }

  void OnTap(Dali::Actor actor, TapGesture gesture)
  {
    if( ! mPaused )
    {
      //mAnimation.Pause();
      mModelAnimation.Pause();
      mPaused = true;
    }
    else
    {
      //mAnimation.Play();
      mModelAnimation.Play();
      mPaused = false;
    }
  }

  void OnPan(Actor actor, PanGesture gesture)
  {
    switch (gesture.state)
    {
      case Gesture::Continuing:
      {
        switch(mPanState)
        {
          case PAN_LIGHT:
          {
            mLightLongitudinal += gesture.displacement.x/4.0f;
            mLightAxisTilt -= gesture.displacement.y/6.0f;
            mLightAxisTilt = Clamp<float>(mLightAxisTilt, -90.0f, 90.0f);
            mLightAnchor.SetRotation(CalculateWorldRotation(Radian(mLightLongitudinal), Radian(mLightAxisTilt)));
            break;
          }

          case PAN_SCENE:
          {
            mTranslation += Vector3(gesture.displacement.x, gesture.displacement.y, 0.f);
            mContents.SetPosition(mTranslation);
            break;
          }

          case ROTATE_SCENE:
          {
            mLongitudinal += gesture.displacement.x/4.0f;
            mAxisTilt -= gesture.displacement.y/6.0f;
            mAxisTilt = Clamp<float>(mAxisTilt, -90.0f, 90.0f);
            mContents.SetRotation(CalculateWorldRotation(Radian(mLongitudinal), Radian(mAxisTilt)));
            break;
          }

          case PAN_OBJECT:
          {
            mObjectLongitudinal += gesture.displacement.x/4.0f;
            mObjectAxisTilt -= gesture.displacement.y/6.0f;
            mObjectAxisTilt = Clamp<float>(mObjectAxisTilt, -90.0f, 90.0f);
            mModelActor.SetRotation(CalculateWorldRotation(Radian(mObjectLongitudinal), Radian(mObjectAxisTilt)));
            break;
          }
        }
      }
      break;

      case Gesture::Finished:
        // Start animation at last known speed
        break;

      default:
        break;
    }
  }

  void OnPinch(Actor actor, PinchGesture gesture)
  {
    if (gesture.state == Gesture::Started)
    {
      mScaleAtPinchStart = mContents.GetCurrentScale().x;
    }
    mPinchScale = Clamp(mScaleAtPinchStart * gesture.scale, MIN_PINCH_SCALE, MAX_PINCH_SCALE);

    mContents.SetScale(mPinchScale, mPinchScale, mPinchScale);
  }

  void Terminate(Application& app)
  {
    if( mModelActor )
    {
      Stage::GetCurrent().Remove(mModelActor);
    }
    if( mKeyLightActor )
    {
      Stage::GetCurrent().Remove(mKeyLightActor);
    }
    if( mView )
    {
      Stage::GetCurrent().Remove(mView);
    }
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApp.Quit();
      }
    }
  }

  bool OnEffectButtonClicked( Toolkit::Button button )
  {
    switch(mPanState)
    {
      case PAN_SCENE:
        mPanState = ROTATE_SCENE;
        mTitleActor.SetText( APPLICATION_TITLE_ROTATE_SCENE );
        mTitleActor.SetSize( Font::New().MeasureText( APPLICATION_TITLE_ROTATE_SCENE ) );
        break;
      case ROTATE_SCENE:
        mPanState = PAN_LIGHT;
        mTitleActor.SetText( APPLICATION_TITLE_PAN_LIGHT );
        mTitleActor.SetSize( Font::New().MeasureText( APPLICATION_TITLE_PAN_LIGHT ) );
        break;
      case PAN_LIGHT:
        mPanState = PAN_OBJECT;
        mTitleActor.SetText( APPLICATION_TITLE_PAN_OBJECT );
        mTitleActor.SetSize( Font::New().MeasureText( APPLICATION_TITLE_PAN_OBJECT ) );
        break;
      case PAN_OBJECT:
        mPanState = PAN_SCENE;
        mTitleActor.SetText( APPLICATION_TITLE_PAN_SCENE );
        mTitleActor.SetSize( Font::New().MeasureText( APPLICATION_TITLE_PAN_SCENE ) );
        break;
      default:
        break;
    }

    mTitleActor.SetStyleToCurrentText(DemoHelper::GetDefaultTextStyle());

    return true;
  }

  bool OnResetPressed( Toolkit::Button button )
  {
    // Reset translation
    mTranslation = Vector3::ZERO;
    mContents.SetPosition(mTranslation);
    // Align scene so that light anchor orientation is Z Axis
    mAxisTilt = -mLightAxisTilt;
    mLongitudinal = -mLightLongitudinal;
    mContents.SetRotation(CalculateWorldRotation(Radian(mLongitudinal), Radian(mAxisTilt)));

    return true;
  }

private:
  Application&              mApp;
  Toolkit::View             mView;
  Layer                     mContents;
  Model                     mModel;
  Actor                     mModelActor;
  LightActor                mKeyLightActor;
  Animation                 mAnimation;
  Animation                 mModelAnimation;
  bool                      mPaused;
  Toolkit::ShadowView       mShadowView;
  ImageActor                mShadowPlaneBg;
  ImageActor                mShadowPlane;
  Actor                     mCastingLight;
  Actor                     mLightAnchor;

  PanGestureDetector        mPanGestureDetector;
  PinchGestureDetector      mPinchGestureDetector;
  TapGestureDetector        mTapGestureDetector;
  Vector3                   mTranslation;
  Degree                    mLongitudinal;
  Degree                    mAxisTilt;
  Degree                    mLightLongitudinal;
  Degree                    mLightAxisTilt;
  Degree                    mObjectLongitudinal;
  Degree                    mObjectAxisTilt;
  float                     mPinchScale;
  float                     mScaleAtPinchStart;

  Toolkit::TextView         mTitleActor;

  enum PanState
  {
    PAN_SCENE,
    ROTATE_SCENE,
    PAN_LIGHT,
    PAN_OBJECT
  };

  PanState                  mPanState;
};

/*****************************************************************************/

static void
RunTest(Application& app)
{
  TestApp theApp(app);
  app.MainLoop();
}

/*****************************************************************************/

int
main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv);

  RunTest(app);

  return 0;
}
