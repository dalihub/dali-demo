#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/object/property-map.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  //Keeps information about each model for access.
  struct Model
  {
    Control control; // Control housing the mesh visual of the model.
    Vector2 rotation; // Keeps track of rotation about x and y axis for manual rotation.
    Animation rotationAnimation; // Automatically rotates when left alone.
  };

  //Files for meshes
  const char * const MODEL_FILE_TABLE[] =
  {
      DEMO_MODEL_DIR "Dino.obj",
      DEMO_MODEL_DIR "ToyRobot-Metal.obj",
      DEMO_MODEL_DIR "Toyrobot-Plastic.obj"
  };

  const char * const MATERIAL_FILE_TABLE[] =
  {
      DEMO_MODEL_DIR "Dino.mtl",
      DEMO_MODEL_DIR "ToyRobot-Metal.mtl",
      DEMO_MODEL_DIR "Toyrobot-Plastic.mtl"
  };

  const char * const TEXTURES_PATH( DEMO_IMAGE_DIR "" );

  //Possible shading modes.
  MeshVisual::ShadingMode::Value SHADING_MODE_TABLE[] =
  {
    MeshVisual::ShadingMode::TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING,
    MeshVisual::ShadingMode::TEXTURED_WITH_SPECULAR_LIGHTING,
    MeshVisual::ShadingMode::TEXTURELESS_WITH_DIFFUSE_LIGHTING
  };

  const float X_ROTATION_DISPLACEMENT_FACTOR = 60.0f;
  const float Y_ROTATION_DISPLACEMENT_FACTOR = 60.0f;
  const float MODEL_SCALE =                    0.75f;
  const float LIGHT_SCALE =                    0.15f;
  const float BUTTONS_OFFSET_BOTTOM =          0.9f;
  const int   NUM_MESHES =                     2;

  //Used to identify actors.
  const int MODEL_TAG = 0;
  const int LIGHT_TAG = 1;
  const int LAYER_TAG = 2;

} //End namespace

class MeshVisualController : public ConnectionTracker
{
public:

  MeshVisualController( Application& application )
  : mApplication( application ),   //Store handle to the application.
    mModelIndex( 1 ),              //Start with metal robot.
    mShadingModeIndex( 0 ),        //Start with textured with detailed specular lighting.
    mTag( -1 ),                    //Non-valid default, which will get set to a correct value when used.
    mSelectedModelIndex( -1 ),     //Non-valid default, which will get set to a correct value when used.
    mPaused( false ),              //Animations play by default.
    mLightFixed( true )            //The light is fixed by default.
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &MeshVisualController::Create );
  }

  ~MeshVisualController()
  {
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Vector4( 0.0, 0.5, 1.0, 1.0 ) );

    //Set up layer to place objects on.
    Layer baseLayer = Layer::New();
    baseLayer.SetParentOrigin( ParentOrigin::CENTER );
    baseLayer.SetAnchorPoint( AnchorPoint::CENTER );
    baseLayer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    baseLayer.SetBehavior( Layer::LAYER_2D ); //We use a 2D layer as this is closer to UI work than full 3D scene creation.
    baseLayer.SetDepthTestDisabled( false ); //Enable depth testing, as otherwise the 2D layer would not do so.
    baseLayer.RegisterProperty( "Tag", LAYER_TAG ); //Used to differentiate between different kinds of actor.
    baseLayer.TouchedSignal().Connect( this, &MeshVisualController::OnTouch );
    stage.Add( baseLayer );

    //Place models on the scene.
    SetupModels( baseLayer );

    //Place buttons on the scene.
    SetupButtons( baseLayer );

    //Add a light to the scene.
    SetupLight( baseLayer );

    //Allow for exiting of the application via key presses.
    stage.KeyEventSignal().Connect( this, &MeshVisualController::OnKeyEvent );
  }

  //Loads and adds the models to the scene, inside containers for hit detection.
  void SetupModels( Layer layer )
  {
    //Add containers to house each renderer-holding-actor.
    for( int i = 0; i < NUM_MESHES; i++ )
    {
      mContainers[i] = Actor::New();
      mContainers[i].SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mContainers[i].RegisterProperty( "Tag", MODEL_TAG ); //Used to differentiate between different kinds of actor.
      mContainers[i].RegisterProperty( "Model", Property::Value( i ) ); //Used to index into the model.
      mContainers[i].TouchedSignal().Connect( this, &MeshVisualController::OnTouch );
      layer.Add( mContainers[i] );
    }

    //Position each container individually on screen

    //Main, central model
    mContainers[0].SetSizeModeFactor( Vector3( MODEL_SCALE, MODEL_SCALE, 0.0f ) );
    mContainers[0].SetParentOrigin( ParentOrigin::CENTER );
    mContainers[0].SetAnchorPoint( AnchorPoint::CENTER );

    //Top left model
    mContainers[1].SetSizeModeFactor( Vector3( MODEL_SCALE / 3.0f, MODEL_SCALE / 3.0f, 0.0f ) );
    mContainers[1].SetParentOrigin( Vector3( 0.05, 0.03, 0.5 ) ); //Offset from top left
    mContainers[1].SetAnchorPoint( AnchorPoint::TOP_LEFT );

    //Set up models
    for( int i = 0; i < NUM_MESHES; i++ )
    {
      //Create control to display model
      Control control = Control::New();
      control.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      control.SetParentOrigin( ParentOrigin::CENTER );
      control.SetAnchorPoint( AnchorPoint::CENTER );
      mContainers[i].Add( control );

      //Make model spin to demonstrate 3D
      Animation rotationAnimation = Animation::New( 15.0f );
      float spin = i % 2 == 0 ? 1.0f : -1.0f; //Make actors spin in different directions to better show independence.
      rotationAnimation.AnimateBy( Property( control, Actor::Property::ORIENTATION ),
                                   Quaternion( Degree( 0.0f ), Degree( spin * 360.0f ), Degree( 0.0f ) ) );
      rotationAnimation.SetLooping( true );
      rotationAnimation.Play();

      //Store model information in corresponding structs.
      mModels[i].control = control;
      mModels[i].rotation.x = 0.0f;
      mModels[i].rotation.y = 0.0f;
      mModels[i].rotationAnimation = rotationAnimation;
    }

    //Calling this sets the model in the controls.
    ReloadModel();
  }

  //Place the various buttons on the bottom of the screen, with title labels where necessary.
  void SetupButtons( Layer layer )
  {
    //Text label title for changing model or shader.
    TextLabel changeTitleLabel = TextLabel::New( "Switch" );
    changeTitleLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    changeTitleLabel.SetProperty( TextLabel::Property::UNDERLINE, "{\"thickness\":\"2.0\"}" );
    changeTitleLabel.SetParentOrigin( Vector3( 0.2, BUTTONS_OFFSET_BOTTOM, 0.5 ) );
    changeTitleLabel.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    layer.Add( changeTitleLabel );

    //Create button for model changing
    PushButton modelButton = Toolkit::PushButton::New();
    modelButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    modelButton.ClickedSignal().Connect( this, &MeshVisualController::OnChangeModelClicked );
    modelButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    modelButton.SetAnchorPoint( AnchorPoint::TOP_RIGHT );
    modelButton.SetLabelText( "Model" );
    changeTitleLabel.Add( modelButton );

    //Create button for shader changing
    PushButton shaderButton = Toolkit::PushButton::New();
    shaderButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    shaderButton.ClickedSignal().Connect( this, &MeshVisualController::OnChangeShaderClicked );
    shaderButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    shaderButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    shaderButton.SetLabelText( "Shader" );
    changeTitleLabel.Add( shaderButton );

    //Create button for pausing animations
    PushButton pauseButton = Toolkit::PushButton::New();
    pauseButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    pauseButton.ClickedSignal().Connect( this, &MeshVisualController::OnPauseClicked );
    pauseButton.SetParentOrigin( Vector3( 0.5, BUTTONS_OFFSET_BOTTOM, 0.5 ) );
    pauseButton.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    pauseButton.SetLabelText( " || " );
    layer.Add( pauseButton );

    //Text label title for light position mode.
    TextLabel lightTitleLabel = TextLabel::New( "Light Position" );
    lightTitleLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    lightTitleLabel.SetProperty( TextLabel::Property::UNDERLINE, "{\"thickness\":\"2.0\"}" );
    lightTitleLabel.SetParentOrigin( Vector3( 0.8, BUTTONS_OFFSET_BOTTOM, 0.5 ) );
    lightTitleLabel.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    layer.Add( lightTitleLabel );

    //Create button for switching between manual and fixed light position.
    PushButton lightButton = Toolkit::PushButton::New();
    lightButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    lightButton.ClickedSignal().Connect( this, &MeshVisualController::OnChangeLightModeClicked );
    lightButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    lightButton.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    lightButton.SetLabelText( "FIXED" );
    lightTitleLabel.Add( lightButton );
  }

  //Add a point light source the the scene, on a layer above the first.
  void SetupLight( Layer baseLayer )
  {
    //Create control to act as light source of scene.
    mLightSource = Control::New();
    mLightSource.RegisterProperty( "Tag", LIGHT_TAG );

    //Set size of control based on screen dimensions.
    Stage stage = Stage::GetCurrent();
    if( stage.GetSize().width < stage.GetSize().height )
    {
      //Scale to width.
      mLightSource.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::WIDTH );
      mLightSource.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
      mLightSource.SetSizeModeFactor( Vector3( LIGHT_SCALE, 0.0f, 0.0f ) );
    }
    else
    {
      //Scale to height.
      mLightSource.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::HEIGHT );
      mLightSource.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::WIDTH );
      mLightSource.SetSizeModeFactor( Vector3( 0.0f, LIGHT_SCALE, 0.0f ) );
    }

    //Set position relative to top left, as the light source property is also relative to the top left.
    mLightSource.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mLightSource.SetAnchorPoint( AnchorPoint::CENTER );
    mLightSource.SetPosition( Stage::GetCurrent().GetSize().x * 0.85f, Stage::GetCurrent().GetSize().y * 0.125 );

    //Supply an image to represent the light.
    Property::Map lightMap;
    lightMap.Insert( Visual::Property::TYPE, Visual::IMAGE );
    lightMap.Insert( ImageVisual::Property::URL, DEMO_IMAGE_DIR "light-icon.png" );
    mLightSource.SetProperty( Control::Property::BACKGROUND, Property::Value( lightMap ) );

    //Connect to touch signal for dragging.
    mLightSource.TouchedSignal().Connect( this, &MeshVisualController::OnTouch );

    //Place the light source on a layer above the base, so that it is rendered above everything else.
    Layer upperLayer = Layer::New();
    upperLayer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    upperLayer.SetParentOrigin( ParentOrigin::CENTER );
    upperLayer.SetAnchorPoint( AnchorPoint::CENTER );

    baseLayer.Add( upperLayer );
    upperLayer.Add( mLightSource );

    //Decide which light to use to begin with.
    if( mLightFixed )
    {
      UseFixedLight();
    }
    else
    {
      UseManualLight();
    }
  }

  //Updates the displayed models to account for parameter changes.
  void ReloadModel()
  {
    //Create mesh property map
    Property::Map map;
    map.Insert( Visual::Property::TYPE,  Visual::MESH );
    map.Insert( MeshVisual::Property::OBJECT_URL, MODEL_FILE_TABLE[mModelIndex] );
    map.Insert( MeshVisual::Property::MATERIAL_URL, MATERIAL_FILE_TABLE[mModelIndex] );
    map.Insert( MeshVisual::Property::TEXTURES_PATH, TEXTURES_PATH );
    map.Insert( MeshVisual::Property::SHADING_MODE, SHADING_MODE_TABLE[mShadingModeIndex] );

    //Set the two controls to use the mesh
    for( int i = 0; i < NUM_MESHES; i++ )
    {
      mModels[i].control.SetProperty( Control::Property::BACKGROUND, Property::Value( map ) );
    }
  }

  void UseFixedLight()
  {
    //Hide draggable source
    mLightSource.SetVisible( false );

    //Use stage dimensions to place light at center, offset outwards in z axis.
    Stage stage = Stage::GetCurrent();
    float width = stage.GetSize().width;
    float height = stage.GetSize().height;
    Vector3 lightPosition = Vector3( width / 2.0f, height / 2.0f, std::max( width, height ) * 5.0f );

    //Set global light position
    for( int i = 0; i < NUM_MESHES; ++i )
    {
      mModels[i].control.RegisterProperty( "lightPosition", lightPosition, Property::ANIMATABLE );
    }
  }

  void UseManualLight()
  {
    //Show draggable source
    mLightSource.SetVisible( true );

    //Update to switch light position to that off the source.
    UpdateLight();
  }

  //Updates the light position for each model to account for changes in the source on screen.
  void UpdateLight()
  {
    //Set light position to the x and y of the light control, offset out of the screen.
    Vector3 controlPosition = mLightSource.GetCurrentPosition();
    Vector3 lightPosition = Vector3( controlPosition.x, controlPosition.y, Stage::GetCurrent().GetSize().x / 2.0f );

    for( int i = 0; i < NUM_MESHES; ++i )
    {
      mModels[i].control.RegisterProperty( "lightPosition", lightPosition, Property::ANIMATABLE );
    }
  }

  //If the light source is touched, move it by dragging it.
  //If a model is touched, rotate it by panning around.
  bool OnTouch( Actor actor, const TouchEvent& event )
  {
    //Get primary touch point.
    const Dali::TouchPoint& point = event.GetPoint( 0 );

    switch( point.state )
    {
      case TouchPoint::Down:
      {
        //Determine what was touched.
        actor.GetProperty( actor.GetPropertyIndex( "Tag" ) ).Get( mTag );

        if( mTag == MODEL_TAG )
        {
          //Find out which model has been selected
          actor.GetProperty( actor.GetPropertyIndex( "Model" ) ).Get( mSelectedModelIndex );

          //Pause current animation, as the touch gesture will be used to manually rotate the model
          mModels[mSelectedModelIndex].rotationAnimation.Pause();

          //Store start points.
          mPanStart = point.screen;
          mRotationStart = mModels[mSelectedModelIndex].rotation;
        }

        break;
      }
      case TouchPoint::Motion:
      {
        //Switch on the kind of actor we're interacting with.
        switch( mTag )
        {
          case MODEL_TAG: //Rotate model
          {
            //Calculate displacement and corresponding rotation.
            Vector2 displacement = point.screen - mPanStart;
            mModels[mSelectedModelIndex].rotation = Vector2( mRotationStart.x - displacement.y / Y_ROTATION_DISPLACEMENT_FACTOR,   // Y displacement rotates around X axis
                                                             mRotationStart.y + displacement.x / X_ROTATION_DISPLACEMENT_FACTOR ); // X displacement rotates around Y axis
            Quaternion rotation = Quaternion( Radian( mModels[mSelectedModelIndex].rotation.x ), Vector3::XAXIS) *
                                  Quaternion( Radian( mModels[mSelectedModelIndex].rotation.y ), Vector3::YAXIS);

            //Apply rotation.
            mModels[mSelectedModelIndex].control.SetOrientation( rotation );

            break;
          }
          case LIGHT_TAG: //Drag light
          {
            //Set light source to new position and update the models accordingly.
            mLightSource.SetPosition( Vector3( point.screen ) );
            UpdateLight();

            break;
          }
        }

        break;
      }
      case TouchPoint::Interrupted: //Same as finished.
      case TouchPoint::Finished:
      {
        if( mTag == MODEL_TAG )
        {
          //Return to automatic animation
          if( !mPaused )
          {
            mModels[mSelectedModelIndex].rotationAnimation.Play();
          }
        }

        break;
      }
      default:
      {
        //Other touch states do nothing.
        break;
      }
    }

    return true;
  }

  //Cycle through the list of models.
  bool OnChangeModelClicked( Toolkit::Button button )
  {
    ++mModelIndex %= 3;

    ReloadModel();

    return true;
  }

  //Cycle through the list of shaders.
  bool OnChangeShaderClicked( Toolkit::Button button )
  {
    ++mShadingModeIndex %= 3;

    ReloadModel();

    return true;
  }

  //Pause all animations, and keep them paused even after user panning.
  //This button is a toggle, so pressing again will start the animations again.
  bool OnPauseClicked( Toolkit::Button button )
  {
    //Toggle pause state.
    mPaused = !mPaused;

    //If we wish to pause animations, do so and keep them paused.
    if( mPaused )
    {
      for( int i = 0; i < NUM_MESHES ; ++i )
      {
        mModels[i].rotationAnimation.Pause();
      }

      button.SetLabelText( " > " );
    }
    else //Unpause all animations again.
    {
      for( int i = 0; i < NUM_MESHES ; ++i )
      {
        mModels[i].rotationAnimation.Play();
      }

      button.SetLabelText( " || " );
    }

    return true;
  }


  //Switch between a fixed light source in front of the screen, and a light source the user can drag around.
  bool OnChangeLightModeClicked( Toolkit::Button button )
  {
    //Toggle state.
    mLightFixed = !mLightFixed;

    if( mLightFixed )
    {
      UseFixedLight();

      button.SetLabelText( "FIXED" );
    }
    else
    {
      UseManualLight();

      button.SetLabelText( "MANUAL" );
    }

    return true;
  }

  //If escape or the back button is pressed, quit the application (and return to the launcher)
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;

  //The models displayed on screen, including information about rotation.
  Model mModels[NUM_MESHES];
  Actor mContainers[NUM_MESHES];

  //Acts as a global light source, which can be dragged around.
  Control mLightSource;

  //Used to detect panning to rotate the selected model.
  Vector2 mPanStart;
  Vector2 mRotationStart;

  int mModelIndex; //Index of model to load.
  int mShadingModeIndex; //Index of shader type to use.
  int mTag; //Identifies what kind of actor has been selected in OnTouch.
  int mSelectedModelIndex; //Index of model selected on screen.
  bool mPaused; //If true, all animations are paused and should stay so.
  bool mLightFixed; //If false, the light is in manual.
};

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  MeshVisualController test( application );
  application.MainLoop();
  return 0;
}
