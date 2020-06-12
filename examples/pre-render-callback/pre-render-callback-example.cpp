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
 */

#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/devel-api/adaptor-framework/application-devel.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>

using namespace Dali::Toolkit;

namespace Dali
{
const char* SCENE_IMAGE_1( DEMO_IMAGE_DIR "gallery-small-10.jpg");
const char* SCENE_IMAGE_2( DEMO_IMAGE_DIR "gallery-small-42.jpg");
const char* SCENE_IMAGE_3( DEMO_IMAGE_DIR "gallery-small-48.jpg");
const char* ROTATE_TEXT("-\\|/");
const float TEXT_HEIGHT = 40.0f;

void AddText( Control textContainer, std::string text, unsigned int yIndex )
{
  auto label = TextLabel::New(text);
  label.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER );
  label.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  label.SetProperty( Actor::Property::SIZE, Vector2( 300,TEXT_HEIGHT ) );
  label.SetProperty( Actor::Property::POSITION_Y,  yIndex*TEXT_HEIGHT );
  textContainer.Add( label );
}

class PreRenderCallbackController : public ConnectionTracker
{
public:

  /**
   * @brief Constructor.
   * @param[in]  application  The application.
   */
  PreRenderCallbackController ( Application& application )
  : mApplication( application ),
    mStage(),
    mTapDetector(),
    mKeepPreRender(false),
    mRotateTextCharacter(0),
    mLastRTC(-1),
    mImageActor1(),
    mImageActor2(),
    mImageActor3(),
    mAngle1Index( Property::INVALID_INDEX ),
    mAngle3Index( Property::INVALID_INDEX ),
    mSceneActor(),
    mSceneAnimation(),
    mSpinner()
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &PreRenderCallbackController::Create );
  }

private:
  struct RotationConstraint
  {
    RotationConstraint(float sign)
    : mSign(sign)
    {
    }

    void operator()( Quaternion& current, const PropertyInputContainer& inputs )
    {
      Radian angle( inputs[0]->GetFloat() );
      current = Quaternion( angle * mSign, Vector3::YAXIS );
    }

    float mSign;
  };

  /**
   * @brief Creates the scene.
   */
  void Create( Application& application )
  {
    mStage = Stage::GetCurrent();
    mStage.SetBackgroundColor( Color::WHITE );
    mStage.KeyEventSignal().Connect( this, &PreRenderCallbackController::OnKeyEvent );

    // Hide the indicator bar.
    mApplication.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    // Detect taps on the root layer.
    mTapDetector = TapGestureDetector::New();
    mTapDetector.Attach( mStage.GetRootLayer() );
    mTapDetector.DetectedSignal().Connect( this, &PreRenderCallbackController::OnTap );

    CreateAnimatingScene();

    auto textContainer = Control::New();
    textContainer.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
    AddText(textContainer, "Click to add callback", 1 );
    AddText(textContainer, "Press 1 to add callback", 2 );
    AddText(textContainer, "Press 2 to clear callback", 3 );
    AddText(textContainer, "Press 3 to toggle keep alive", 4 );


    mSpinner = TextLabel::New("");
    mSpinner.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    mSpinner.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
    mSpinner.SetProperty( Actor::Property::SIZE, Vector2(100,100) );

    mStage.Add(mSpinner);
    mStage.Add(textContainer);

    DevelApplication::AddIdleWithReturnValue( application, MakeCallback( this, &PreRenderCallbackController::OnIdle ) );
  }

  void CreateAnimatingScene()
  {
    mSceneActor = Layer::New();
    mSceneActor.SetProperty( Layer::Property::BEHAVIOR, Layer::LAYER_3D );
    mSceneActor.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER);

    // Create and add images to the scene actor:
    mImageActor1 = ImageView::New( SCENE_IMAGE_1 );
    mImageActor2 = ImageView::New( SCENE_IMAGE_2 );
    mImageActor3 = ImageView::New( SCENE_IMAGE_3 );

    mImageActor1.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    mImageActor2.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    mImageActor3.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

    mImageActor2.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER);

    mImageActor1.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER_LEFT);
    mImageActor1.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::CENTER_RIGHT);

    mImageActor3.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER_RIGHT);
    mImageActor3.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::CENTER_LEFT);

    mSceneActor.Add(mImageActor2);
    mImageActor2.Add(mImageActor1);
    mImageActor2.Add(mImageActor3);

    Property::Index angleIndex = mImageActor2.RegisterProperty("angle", Property::Value( Dali::ANGLE_30 ) );
    Source angleSrc( mImageActor2, angleIndex );

    Constraint constraint = Constraint::New<Quaternion>( mImageActor1, Actor::Property::ORIENTATION, RotationConstraint(-1.0f) );
    constraint.AddSource( angleSrc );
    constraint.Apply();

    constraint = Constraint::New<Quaternion>( mImageActor3, Actor::Property::ORIENTATION, RotationConstraint(+1.0f) );
    constraint.AddSource( angleSrc );
    constraint.Apply();

    mSceneAnimation = Animation::New(2.5f);

    // Want to animate angle from 30 => -30 and back again smoothly.
    mSceneAnimation.AnimateTo( Property( mImageActor2, angleIndex ), Property::Value(-Dali::ANGLE_30), AlphaFunction::SIN );

    mSceneAnimation.SetLooping(true);
    mSceneAnimation.Play();

    mSceneActor.SetProperty( Actor::Property::SIZE, Vector2(250.0f, 250.0f) );
    mSceneActor.SetProperty( Actor::Property::POSITION, Vector3( 0.0f, 0.0f, 130.0f ) );
    Quaternion p( Degree( -6.0f ), Vector3::XAXIS );
    Quaternion q( Degree( 20.0f ), Vector3::YAXIS );
    mSceneActor.SetProperty( Actor::Property::ORIENTATION, p * q );

    mStage.Add( mSceneActor );
  }

  void OnTap( Actor /* actor */, const TapGesture& /* tap */ )
  {
    Adaptor::Get().SetPreRenderCallback( MakeCallback( this, &PreRenderCallbackController::OnPreRender ) );
  }

  /**
   * @brief Called when any key event is received
   *
   * Will use this to quit the application if Back or the Escape key is received
   * @param[in] event The key event information
   */
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if ( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
      else if( event.keyPressedName.compare("1") == 0)
      {
        Adaptor::Get().SetPreRenderCallback( MakeCallback( this, &PreRenderCallbackController::OnPreRender ) );
      }
      else if( event.keyPressedName.compare("2") == 0)
      {
        Adaptor::Get().SetPreRenderCallback( NULL );
      }
      else if( event.keyPressedName.compare("3") == 0)
      {
        mKeepPreRender = !mKeepPreRender;
      }
    }
  }

  bool OnPreRender()
  {
    // Called from Update/Render thread
    printf("Pre-render callback\n");
    ++mRotateTextCharacter;
    return mKeepPreRender;
  }

  bool OnIdle()
  {
    // Called from Event thread on main loop
    int rotation = mRotateTextCharacter;
    if( rotation != mLastRTC )
    {
      mLastRTC = rotation;
      mSpinner.SetProperty(TextLabel::Property::TEXT, std::string(1, ROTATE_TEXT[rotation%4]));
    }
    return true;
  }


private:
  Application& mApplication;
  Stage mStage;
  TapGestureDetector  mTapDetector;          ///< Tap detector to enable the PreRenderCallback
  bool mKeepPreRender;
  int mRotateTextCharacter;
  int mLastRTC;

  // Scene objects:
  ImageView                 mImageActor1;
  ImageView                 mImageActor2;
  ImageView                 mImageActor3;
  Property::Index           mAngle1Index;
  Property::Index           mAngle3Index;
  Layer                     mSceneActor;
  Animation                 mSceneAnimation;
  TextLabel                 mSpinner;
};

} // namespace Dali

int DALI_EXPORT_API main( int argc, char **argv )
{
  Dali::Application application = Dali::Application::New( &argc, &argv );
  Dali::PreRenderCallbackController controller( application );
  application.MainLoop();
  return 0;
}
