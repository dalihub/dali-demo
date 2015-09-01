/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

using namespace Dali;
using Dali::Toolkit::Model3dView;

namespace
{
const int MODEL_NUMBER(3);

const char * const MODEL_FILE[] = {
    DALI_MODEL_DIR "Dino.obj",
    DALI_MODEL_DIR "ToyRobot-Metal.obj",
    DALI_MODEL_DIR "Toyrobot-Plastic.obj"
};

const char * const MATERIAL_FILE[] = {
    DALI_MODEL_DIR "Dino.mtl",
    DALI_MODEL_DIR "ToyRobot-Metal.mtl",
    DALI_MODEL_DIR "Toyrobot-Plastic.mtl"
};

const char * const IMAGE_PATH( DALI_IMAGE_DIR "" );

const char * BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-1.jpg");

}

/*
 * This example shows how to create and display a model3d-view control.
 * The application can cycle between 3 different models, and 3 different shaders.
 * There are two animations running, one is a rotation for the model, one is a light that
 * goes from one side of the model to the other.
 * There are dedicated buttons for changing the models, the shaders and pausing the animations.
 * The animations can also be paused, resumed with the space key
 * A double tap in the model3d-view will make zoom-in/out of the zone clicked
 */

class Model3dViewController : public ConnectionTracker
{
public:

  Model3dViewController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &Model3dViewController::Create );
  }

  ~Model3dViewController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    Vector2 screenSize = stage.GetSize();

    //Add background
    Image imageBackground = ResourceImage::New( BACKGROUND_IMAGE );
    Toolkit::ImageView backView = Toolkit::ImageView::New(imageBackground);
    backView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    stage.Add(backView);

    //Add 3D model control
    m3DLayer = Layer::New();
    stage.GetRootLayer().Add(m3DLayer);

    //3D models require 3D based rendering method, so it can use depth buffer, etc.
    m3DLayer.SetBehavior(Layer::LAYER_3D);
    m3DLayer.SetParentOrigin( ParentOrigin::CENTER );
    m3DLayer.SetAnchorPoint( AnchorPoint::CENTER );

    mModelCounter = 0;

    mModel3dView = Model3dView::New( MODEL_FILE[0], MATERIAL_FILE[0], IMAGE_PATH );
    mModel3dView.SetParentOrigin( ParentOrigin::CENTER );
    mModel3dView.SetAnchorPoint( AnchorPoint::CENTER );
    mModel3dView.SetName( "model3d-view-control" );
    mModel3dView.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
    mModel3dView.SetSize(screenSize);

    mModel3dView.SetProperty(Model3dView::Property::LIGHT_POSITION, Vector3(5,10.,0));

    m3DLayer.Add( mModel3dView );

    mIlluminationShader = Model3dView::IlluminationType(mModel3dView.GetProperty<int>(Model3dView::Property::ILLUMINATION_TYPE));

    mButtonLayer = Layer::New();
    mButtonLayer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mButtonLayer.SetParentOrigin( ParentOrigin::CENTER );
    mButtonLayer.SetAnchorPoint( AnchorPoint::CENTER );
    stage.GetRootLayer().Add(mButtonLayer);

    // Create button for model changing
    Toolkit::PushButton editButton = Toolkit::PushButton::New();
    editButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    editButton.ClickedSignal().Connect( this, &Model3dViewController::OnChangeModelClicked);
    editButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    editButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    editButton.SetLabelText( "Change Model" );
    mButtonLayer.Add( editButton  );

    // Create button for shader changing
    editButton = Toolkit::PushButton::New();
    editButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    editButton.ClickedSignal().Connect( this, &Model3dViewController::OnChangeLightingClicked);
    editButton.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    editButton.SetAnchorPoint( AnchorPoint::TOP_RIGHT );
    editButton.SetLabelText( "Change Shader" );
    mButtonLayer.Add( editButton  );

    // Create button for pause/resume animation
    editButton = Toolkit::PushButton::New();
    editButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    editButton.ClickedSignal().Connect( this, &Model3dViewController::OnPauseAnimationsClicked);
    editButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    editButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    editButton.SetLabelText( "Pause Animations" );
    mButtonLayer.Add( editButton  );

    //Create animations
    mLightAnimation = Animation::New(6.f);
    mLightAnimation.AnimateTo(Property(mModel3dView, Model3dView::Property::LIGHT_POSITION), Vector3(-5,10.0,0), TimePeriod( 0.0f, 3.0f ));
    mLightAnimation.AnimateTo(Property(mModel3dView, Model3dView::Property::LIGHT_POSITION), Vector3(5,10.0,0), TimePeriod( 3.0f, 3.0f ));
    mLightAnimation.SetLooping(true);
    mLightAnimation.Play();

    mRotationAnimation = Animation::New(15.f);
    mRotationAnimation.AnimateBy(Property(mModel3dView, Actor::Property::ORIENTATION), Quaternion(Degree(0.f), Degree(360.f), Degree(0.f)) );
    mRotationAnimation.SetLooping(true);
    mRotationAnimation.Play();

    mPlaying = true;
    mScaled = false;

    // Respond to a click anywhere on the stage
    stage.KeyEventSignal().Connect(this, &Model3dViewController::OnKeyEvent);

    //Create a tap gesture detector for zoom
    mTapDetector = TapGestureDetector::New( 2 );
    mTapDetector.DetectedSignal().Connect(this, &Model3dViewController::OnTap);

    mTapDetector.Attach( backView );
  }

  /**
   * Main Tap event handler
   */
  void OnTap( Actor actor, const TapGesture& tap )
  {
    if (mScaled)
    {
      mModel3dView.SetScale(1.0);
      mModel3dView.SetPosition(0,0,0);
      mScaled = false;
    }
    else
    {
      Stage stage = Stage::GetCurrent();
      Vector2 screenSize = stage.GetSize();

      Vector2 position;
      position.x = tap.screenPoint.x - screenSize.x * 0.5;
      position.y = tap.screenPoint.y - screenSize.y * 0.5;

      float size = 2.5;

      mModel3dView.SetScale(size);
      mModel3dView.SetPosition(-position.x * size,-position.y * size, 0);
      mScaled = true;
    }
  }

  /**
   * Change models button signal function
   */
  bool OnChangeModelClicked(Toolkit::Button button)
  {
    mModelCounter = (mModelCounter + 1) % MODEL_NUMBER;
    mModel3dView.SetProperty(Model3dView::Property::GEOMETRY_URL, MODEL_FILE[mModelCounter]);
    mModel3dView.SetProperty(Model3dView::Property::MATERIAL_URL, MATERIAL_FILE[mModelCounter]);
    mModel3dView.SetProperty(Model3dView::Property::IMAGES_URL, IMAGE_PATH);

    return true;
  }

  /**
   * Change shader button signal function
   */
  bool OnChangeLightingClicked(Toolkit::Button button)
  {
    if( mIlluminationShader == Model3dView::DIFFUSE_WITH_NORMAL_MAP )
    {
      mModel3dView.SetProperty(Model3dView::Property::ILLUMINATION_TYPE, Model3dView::DIFFUSE_WITH_TEXTURE);
      mIlluminationShader = Model3dView::IlluminationType(mModel3dView.GetProperty<int>(Model3dView::Property::ILLUMINATION_TYPE));
    }
    else if( mIlluminationShader == Model3dView::DIFFUSE_WITH_TEXTURE )
    {
      mModel3dView.SetProperty(Model3dView::Property::ILLUMINATION_TYPE, Model3dView::DIFFUSE);
      mIlluminationShader = Model3dView::IlluminationType(mModel3dView.GetProperty<int>(Model3dView::Property::ILLUMINATION_TYPE));
   }
    else if( mIlluminationShader == Model3dView::DIFFUSE )
    {
      mModel3dView.SetProperty(Model3dView::Property::ILLUMINATION_TYPE, Model3dView::DIFFUSE_WITH_NORMAL_MAP);
      mIlluminationShader = Model3dView::IlluminationType(mModel3dView.GetProperty<int>(Model3dView::Property::ILLUMINATION_TYPE));
   }

    return true;
  }

  /**
   * Function to pause resume all animations
   */
  void PauseAnimations()
  {
    if( mPlaying )
    {
      mRotationAnimation.Pause();
      mLightAnimation.Pause();

      mPlaying = false;
    }
    else
    {
      mRotationAnimation.Play();
      mLightAnimation.Play();

      mPlaying = true;
    }
  }

  /**
   * Pause button signal function
   */
  bool OnPauseAnimationsClicked(Toolkit::Button button)
  {
    PauseAnimations();

    return true;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
      else
      {
        PauseAnimations();
      }
    }
  }


private:
  Application&  mApplication;

  int mModelCounter;
  Model3dView mModel3dView;

  Layer m3DLayer;
  Layer mButtonLayer;
  TapGestureDetector mTapDetector;

  Model3dView::IlluminationType mIlluminationShader;

  Animation mRotationAnimation;
  Animation mLightAnimation;
  bool mPlaying;

  bool mScaled;
};

void RunTest( Application& application )
{
  Model3dViewController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
