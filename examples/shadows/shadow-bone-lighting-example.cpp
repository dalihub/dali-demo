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
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;
using std::string;
using namespace DemoHelper;

namespace
{
const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-default.png" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );

//const char* APPLICATION_TITLE_PAN_LIGHT( "Lighting: Pan Light" );
//const char* APPLICATION_TITLE_PAN_OBJECT( "Lighting: Pan Object" );
//const char* APPLICATION_TITLE_PAN_SCENE( "Lighting: Pan Scene" );
//const char* APPLICATION_TITLE_ROTATE_SCENE( "Lighting: Rotate Scene" );
const char* CHANGE_EFFECT_IMAGE( DALI_IMAGE_DIR "icon-change.png" );
const char* RESET_ICON( DALI_IMAGE_DIR "icon-reset.png" );

const char* SCENE_IMAGE_1( DALI_IMAGE_DIR "gallery-small-10.jpg");
const char* SCENE_IMAGE_2( DALI_IMAGE_DIR "gallery-small-42.jpg");
const char* SCENE_IMAGE_3( DALI_IMAGE_DIR "gallery-small-48.jpg");

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
 * This example shows a fixed point light onto an animating set of images
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

  struct RotationConstraint
  {
    RotationConstraint(float sign)
    : mSign(sign)
    {
    }

    Quaternion operator()( const Quaternion& current, const PropertyInput& property )
    {
      Degree angle(property.GetFloat());
      return Quaternion( Radian(angle) * mSign, Vector3::YAXIS );
    }

    float mSign;
  };

  /**
   * This method gets called once the main loop of application is up and running
   */
  void Create(Application& app)
  {
    srand(0); // Want repeatable path

    Stage::GetCurrent().KeyEventSignal().Connect(this, &TestApp::OnKeyEvent);

    CreateToolbarAndView(app);
    CreateShadowViewAndLights();
    CreateScene();
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
    Image imageChangeEffect = ResourceImage::New( CHANGE_EFFECT_IMAGE );
    Toolkit::PushButton effectChangeButton = Toolkit::PushButton::New();
    effectChangeButton.SetBackgroundImage(imageChangeEffect);
    effectChangeButton.ClickedSignal().Connect( this, &TestApp::OnEffectButtonClicked );
    toolBar.AddControl( effectChangeButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING );

    // Add title to the tool bar.
    // TODO

    //Add a reset button
    Image resetImage = ResourceImage::New( RESET_ICON );
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



  void CreateShadowViewAndLights()
  {
    mShadowView = Toolkit::ShadowView::New();
    mShadowView.SetName("Container");
    mShadowView.SetParentOrigin(ParentOrigin::CENTER);
    mShadowView.SetAnchorPoint(AnchorPoint::CENTER);
    mShadowView.SetSizeMode( SIZE_EQUAL_TO_PARENT );
    mShadowView.SetPointLightFieldOfView( Math::PI / 2.0f);
    mContents.Add(mShadowView);

    Image brickWall = ResourceImage::New(DALI_IMAGE_DIR "brick-wall.jpg");
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
    // Need to scale light position as scene actor size is based on stage size (i.e. much bigger on device)
    Vector2 stageSize( Stage::GetCurrent().GetSize() );
    float scaleFactor = stageSize.x / DEFAULT_STAGE_SIZE.x;

    mCastingLight = Actor::New();
    mCastingLight.SetParentOrigin(ParentOrigin::CENTER);
    mCastingLight.SetAnchorPoint(AnchorPoint::CENTER);
    mCastingLight.SetPosition( Vector3( 0.0f, 0.0f, 800.0f ) * scaleFactor );

    mLightAnchor.Add(mCastingLight);
    mShadowPlaneBg.Add(mLightAnchor);

    mShadowView.SetPointLight(mCastingLight);
  }

  void CreateScene()
  {
    mSceneActor = Actor::New();
    mSceneActor.SetParentOrigin(ParentOrigin::CENTER);

    // Create and add images to the scene actor:
    mImageActor1 = ImageActor::New( ResourceImage::New(SCENE_IMAGE_1) );
    mImageActor2 = ImageActor::New( ResourceImage::New(SCENE_IMAGE_2) );
    mImageActor3 = ImageActor::New( ResourceImage::New(SCENE_IMAGE_3) );


    mImageActor2.SetParentOrigin(ParentOrigin::CENTER);

    mImageActor1.SetParentOrigin(ParentOrigin::CENTER_LEFT);
    mImageActor1.SetAnchorPoint(AnchorPoint::CENTER_RIGHT);

    mImageActor3.SetParentOrigin(ParentOrigin::CENTER_RIGHT);
    mImageActor3.SetAnchorPoint(AnchorPoint::CENTER_LEFT);

    mSceneActor.Add(mImageActor2);
    mImageActor2.Add(mImageActor1);
    mImageActor2.Add(mImageActor3);

    Property::Index angleIndex = mImageActor2.RegisterProperty("angle", Property::Value(30.0f));
    Source angleSrc( mImageActor2, angleIndex );
    mImageActor1.ApplyConstraint(Constraint::New<Quaternion>( Actor::ROTATION, angleSrc,
                                                              RotationConstraint(-1.0f)));
    mImageActor3.ApplyConstraint(Constraint::New<Quaternion>( Actor::ROTATION, angleSrc,
                                                              RotationConstraint(+1.0f)));

    mSceneAnimation = Animation::New(2.5f);

    // Want to animate angle from 30 => -30 and back again smoothly.

    mSceneAnimation.AnimateTo( Property( mImageActor2, angleIndex ), Property::Value(-30.0f), AlphaFunctions::Sin );

    mSceneAnimation.SetLooping(true);
    mSceneAnimation.Play();

    mSceneActor.SetSize(250.0f, 250.0f);
    mSceneActor.SetPosition(0.0f, 0.0f, 130.0f);
    mShadowView.Add(mSceneActor);
  }


  Quaternion CalculateWorldRotation(Radian longitude, Radian axisTilt )
  {
    Quaternion q(longitude, Vector3::YAXIS);
    Quaternion p(axisTilt, Vector3::XAXIS);
    return p*q;
  }

  void OnTap(Dali::Actor actor, const TapGesture& gesture)
  {
    if( mSceneAnimation )
    {
      if( ! mPaused )
      {
        mSceneAnimation.Pause();
        mPaused = true;
      }
      else
      {
        mSceneAnimation.Play();
        mPaused = false;
      }
    }
  }

  void OnPan(Actor actor, const PanGesture& gesture)
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
            mSceneActor.SetRotation(CalculateWorldRotation(Radian(mObjectLongitudinal), Radian(mObjectAxisTilt)));
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

  void OnPinch(Actor actor, const PinchGesture& gesture)
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
    if( mSceneActor )
    {
      Stage::GetCurrent().Remove(mSceneActor);
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
  Actor                     mSceneActor;
  Animation                 mAnimation;
  Animation                 mSceneAnimation;
  bool                      mPaused;
  Toolkit::ShadowView       mShadowView;
  ImageActor                mShadowPlaneBg;
  ImageActor                mShadowPlane;
  Actor                     mCastingLight;
  Actor                     mLightAnchor;
  ImageActor                mImageActor1;
  ImageActor                mImageActor2;
  ImageActor                mImageActor3;
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

  Property::Index           mAngle1Index;
  Property::Index           mAngle3Index;

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
