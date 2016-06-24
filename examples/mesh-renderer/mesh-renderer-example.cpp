#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/object/property-map.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  //Keeps information about each model for access.
  struct Model
  {
    Control control; // Control housing the mesh renderer of the model.
    Vector2 rotation; // Keeps track of rotation about x and y axis for manual rotation.
    Animation rotationAnimation; // Automatically rotates when left alone.
  };

  //Files for meshes
  const char * const MODEL_FILE[] =
  {
      DEMO_MODEL_DIR "Dino.obj",
      DEMO_MODEL_DIR "ToyRobot-Metal.obj",
      DEMO_MODEL_DIR "Toyrobot-Plastic.obj"
  };

  const char * const MATERIAL_FILE[] =
  {
      DEMO_MODEL_DIR "Dino.mtl",
      DEMO_MODEL_DIR "ToyRobot-Metal.mtl",
      DEMO_MODEL_DIR "Toyrobot-Plastic.mtl"
  };

  const char * const TEXTURES_PATH( DEMO_IMAGE_DIR "" );

  //Possible shader options.
  const char * const SHADER_TYPE[] =
  {
    "allTextures",
    "diffuseTexture",
    "textureless"
  };

  //Files for background and toolbar
  const char * const BACKGROUND_IMAGE( DEMO_IMAGE_DIR "background-1.jpg");

  const float X_ROTATION_DISPLACEMENT_FACTOR = 60.0f;
  const float Y_ROTATION_DISPLACEMENT_FACTOR = 60.0f;
  const float MODEL_SCALE = 0.45f;

} //End namespace

class SharedMeshRendererController : public ConnectionTracker
{
public:

  SharedMeshRendererController( Application& application )
  : mApplication( application ),   //Store handle to the application.
    mModelIndex( 1 ),              //Start with metal robot.
    mShaderIndex( 0 ),             //Start with all textures.
    mSelectedModelIndex( 0 )       //Non-valid default, which will get set to a correct value when used.
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &SharedMeshRendererController::Create );
  }

  ~SharedMeshRendererController()
  {
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    //Add background
    ImageView backView = ImageView::New( BACKGROUND_IMAGE );
    backView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    stage.Add( backView );

    //Setup and load the 3D models and buttons
    LoadScene();
  }

  //Sets up the on-screen elements.
  void LoadScene()
  {
    Stage stage = Stage::GetCurrent();

    //Set up 3D layer to place objects on.
    Layer layer = Layer::New();
    layer.SetParentOrigin( ParentOrigin::CENTER );
    layer.SetAnchorPoint( AnchorPoint::CENTER );
    layer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    layer.SetBehavior( Layer::LAYER_3D );
    stage.Add( layer );

    //Containers to house each renderer-holding-actor, to provide a constant hitbox for pan detection.
    Actor container1 = Actor::New();
    container1.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    container1.SetSizeModeFactor( Vector3( MODEL_SCALE, MODEL_SCALE, 0.0f ) );
    container1.SetParentOrigin( ParentOrigin::CENTER );
    container1.SetAnchorPoint( AnchorPoint::CENTER );
    container1.SetPosition( stage.GetSize().width * 0.25, 0.0 ); //Place on right half of screen.
    container1.RegisterProperty( "Tag", Property::Value( 0 ) ); // Used to identify this actor and index into the model.
    layer.Add( container1 );

    Actor container2 = Actor::New();
    container2.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    container2.SetSizeModeFactor( Vector3( MODEL_SCALE / 2, MODEL_SCALE / 2, 0.0f ) );
    container2.SetParentOrigin( ParentOrigin::CENTER );
    container2.SetAnchorPoint( AnchorPoint::CENTER );
    container2.SetPosition( stage.GetSize().width * -0.25, 0.0 ); //Place on left half of screen.
    container2.RegisterProperty( "Tag", Property::Value( 1 ) ); // Used to identify this actor and index into the model.
    layer.Add( container2 );

    //Attach gesture detector to pan models when rotated.
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( container1 );
    mPanGestureDetector.Attach( container2 );
    mPanGestureDetector.DetectedSignal().Connect( this, &SharedMeshRendererController::OnPan );

    //Create actors to display meshes.
    Control control1 = Control::New();
    control1.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    control1.SetParentOrigin( ParentOrigin::CENTER );
    control1.SetAnchorPoint( AnchorPoint::CENTER );
    container1.Add( control1 );

    Control control2 = Control::New();
    control2.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    control2.SetParentOrigin( ParentOrigin::CENTER );
    control2.SetAnchorPoint( AnchorPoint::CENTER );
    container2.Add( control2 );

    //Make actors spin to demonstrate 3D.
    Animation rotationAnimation1 = Animation::New( 15.0f );
    rotationAnimation1.AnimateBy( Property( control1, Actor::Property::ORIENTATION ),
                                  Quaternion( Degree( 0.0f ), Degree( 360.0f ), Degree( 0.0f ) ) );
    rotationAnimation1.SetLooping( true );
    rotationAnimation1.Play();

    Animation rotationAnimation2 = Animation::New( 15.0f );
    rotationAnimation2.AnimateBy( Property( control2, Actor::Property::ORIENTATION ),
                                  Quaternion( Degree( 0.0f ), Degree( -360.0f ), Degree( 0.0f ) ) );
    rotationAnimation2.SetLooping( true );
    rotationAnimation2.Play();

    //Store model information in corresponding structs.
    mModels[0].control = control1;
    mModels[0].rotation.x = 0.0f;
    mModels[0].rotation.y = 0.0f;
    mModels[0].rotationAnimation = rotationAnimation1;

    mModels[1].control = control2;
    mModels[1].rotation.x = 0.0f;
    mModels[1].rotation.y = 0.0f;
    mModels[1].rotationAnimation = rotationAnimation2;

    //Calling this sets the model in the two actors.
    ReloadModel();

    //Create button for model changing
    Toolkit::PushButton modelButton = Toolkit::PushButton::New();
    modelButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    modelButton.ClickedSignal().Connect( this, &SharedMeshRendererController::OnChangeModelClicked );
    modelButton.SetParentOrigin( Vector3( 0.1, 0.9, 0.5 ) ); //Offset from bottom left
    modelButton.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    modelButton.SetLabelText( "Change Model" );
    layer.Add( modelButton );

    //Create button for shader changing
    Toolkit::PushButton shaderButton = Toolkit::PushButton::New();
    shaderButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    shaderButton.ClickedSignal().Connect( this, &SharedMeshRendererController::OnChangeShaderClicked );
    shaderButton.SetParentOrigin( Vector3( 0.9, 0.9, 0.5 ) ); //Offset from bottom right
    shaderButton.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    shaderButton.SetLabelText( "Change Shader" );
    layer.Add( shaderButton );
  }

  //Updates the displayed models to account for parameter changes.
  void ReloadModel()
  {
    //Create mesh property map
    Property::Map map;
    map.Insert( "rendererType", "mesh" );
    map.Insert( "objectUrl", MODEL_FILE[mModelIndex] );
    map.Insert( "materialUrl", MATERIAL_FILE[mModelIndex] );
    map.Insert( "texturesPath", TEXTURES_PATH );
    map.Insert( "shaderType", SHADER_TYPE[mShaderIndex] );

    //Set the two controls to use the mesh
    mModels[0].control.SetProperty( Control::Property::BACKGROUND, Property::Value( map ) );
    mModels[1].control.SetProperty( Control::Property::BACKGROUND, Property::Value( map ) );
  }

  //Rotates the panned model based on the gesture.
  void OnPan( Actor actor, const PanGesture& gesture )
  {
    switch( gesture.state )
    {
      case Gesture::Started:
      {
        //Find out which model has been selected
        actor.GetProperty( actor.GetPropertyIndex( "Tag" ) ).Get( mSelectedModelIndex );

        //Pause current animation, as the gesture will be used to manually rotate the model
        mModels[mSelectedModelIndex].rotationAnimation.Pause();

        break;
      }
      case Gesture::Continuing:
      {
        //Rotate based off the gesture.
        mModels[mSelectedModelIndex].rotation.x -= gesture.displacement.y / X_ROTATION_DISPLACEMENT_FACTOR; // Y displacement rotates around X axis
        mModels[mSelectedModelIndex].rotation.y += gesture.displacement.x / Y_ROTATION_DISPLACEMENT_FACTOR; // X displacement rotates around Y axis
        Quaternion rotation = Quaternion( Radian( mModels[mSelectedModelIndex].rotation.x ), Vector3::XAXIS) *
                              Quaternion( Radian( mModels[mSelectedModelIndex].rotation.y ), Vector3::YAXIS);

        mModels[mSelectedModelIndex].control.SetOrientation( rotation );

        break;
      }
      case Gesture::Finished:
      {
        //Return to automatic animation
        mModels[mSelectedModelIndex].rotationAnimation.Play();

        break;
      }
      case Gesture::Cancelled:
      {
        //Return to automatic animation
        mModels[mSelectedModelIndex].rotationAnimation.Play();

        break;
      }
      default:
      {
        //We can ignore other gestures and gesture states.
        break;
      }
    }
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
    ++mShaderIndex %= 3;

    ReloadModel();

    return true;
  }

private:
  Application&  mApplication;

  //The models displayed on screen, including information about rotation.
  Model mModels[2];

  //Used to detect panning to rotate the selected model.
  PanGestureDetector mPanGestureDetector;

  int mModelIndex; //Index of model to load.
  int mShaderIndex; //Index of shader type to use.
  int mSelectedModelIndex; //Index of model selected on screen.
};

void RunTest( Application& application )
{
  SharedMeshRendererController test( application );

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
