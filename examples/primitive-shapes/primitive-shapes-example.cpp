/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
//Button image urls
const char* BUTTON_IMAGE_URL[] =
  {
    DEMO_IMAGE_DIR "sphere-button.png",
    DEMO_IMAGE_DIR "cone-button.png",
    DEMO_IMAGE_DIR "conical-frustum-button.png",
    DEMO_IMAGE_DIR "cylinder-button.png",
    DEMO_IMAGE_DIR "cube-button.png",
    DEMO_IMAGE_DIR "bevelled-cube-button.png",
    DEMO_IMAGE_DIR "octahedron-button.png"};

//Prefix of all shape titles.
const std::string SHAPE_TITLE_PREFIX = "Current Shape: ";

//Shape property defaults
const int   DEFAULT_SLICES              = 32;
const int   DEFAULT_STACKS              = 32;
const float DEFAULT_SCALE_HEIGHT        = 16.0f;
const float DEFAULT_SCALE_BOTTOM_RADIUS = 8.0f;
const float DEFAULT_SCALE_TOP_RADIUS    = 4.0f;
const float DEFAULT_SCALE_RADIUS        = 8.0f;
const float DEFAULT_BEVEL_PERCENTAGE    = 0.3f;
const float DEFAULT_BEVEL_SMOOTHNESS    = 0.0f;

//Shape property limits
const int SLICES_LOWER_BOUND = 3;
const int SLICES_UPPER_BOUND = 16;
const int STACKS_LOWER_BOUND = 2;
const int STACKS_UPPER_BOUND = 16;

//Used to the control rate of rotation when panning an object.
const float X_ROTATION_DISPLACEMENT_FACTOR = 60.0f;
const float Y_ROTATION_DISPLACEMENT_FACTOR = 60.0f;

const int NUM_MODELS     = 7; //Total number of possible base shapes.
const int MAX_PROPERTIES = 3; //Maximum number of properties a shape may require. (For displaying sliders.)

} //End namespace

class PrimitiveShapesController : public ConnectionTracker
{
public:
  PrimitiveShapesController(Application& application)
  : mApplication(application),
    mColor(Vector4(0.3f, 0.7f, 1.0f, 1.0f))
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &PrimitiveShapesController::Create);
  }

  ~PrimitiveShapesController()
  {
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    //Set up layer to place UI on.
    Layer layer = Layer::New();
    layer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    layer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    layer.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    layer.SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_UI); //We use a 2D layer as this is closer to UI work than full 3D scene creation.
    layer.SetProperty(Layer::Property::DEPTH_TEST, true);          //Enable depth testing, as otherwise the 2D layer would not do so.
    window.Add(layer);

    //Set up model selection buttons.
    SetupButtons(layer);

    //Set up model parameter sliders.
    SetupSliders(layer);

    //Set up 3D model.
    SetupModel(layer);

    //Allow for exiting of the application via key presses.
    window.KeyEventSignal().Connect(this, &PrimitiveShapesController::OnKeyEvent);
  }

  //Place buttons on the top of the screen, which allow for selection of the shape to be displayed.
  //A title above indicates the currently selected shape.
  //The buttons are laid out like so:
  //
  //      ^    +--------------------------------+
  //      |    | Current Shape: ~~~~~           |
  //      |    |                                |
  //      |    | +----+ +----+ +----+ +----+    |
  //      |    | |    | |    | |    | |    |    |
  //  30% |    | +----+ +----+ +----+ +----+    |
  //      |    |                                |
  //      |    | +----+ +----+ +----+           |
  //      |    | |    | |    | |    |           |
  //      v    | +----+ +----+ +----+           |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           +--------------------------------+
  //
  void SetupButtons(Layer layer)
  {
    float containerPadding = 10.0f;
    float elementPadding   = 5.0f;

    //Used to layout the title and the buttons below it.
    Control topAlignment = Control::New();
    topAlignment.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
    topAlignment.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    topAlignment.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    topAlignment.SetResizePolicy(ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT);
    layer.Add(topAlignment);

    //Add a title to indicate the currently selected shape.
    mShapeTitle = TextLabel::New("DEFAULT");
    mShapeTitle.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mShapeTitle.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mShapeTitle.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);
    mShapeTitle.SetProperty(Actor::Property::PADDING, Padding(elementPadding, elementPadding, elementPadding, elementPadding));
    topAlignment.Add(mShapeTitle);

    //Create a variable-length container that can wrap buttons around as more are added.
    FlexContainer buttonContainer = FlexContainer::New();
    buttonContainer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
    buttonContainer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    buttonContainer.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    buttonContainer.SetResizePolicy(ResizePolicy::FIXED, Dimension::HEIGHT);
    buttonContainer.SetProperty(Actor::Property::PADDING, Padding(containerPadding, containerPadding, containerPadding, containerPadding));
    buttonContainer.SetProperty(FlexContainer::Property::FLEX_DIRECTION, FlexContainer::ROW);
    buttonContainer.SetProperty(FlexContainer::Property::FLEX_WRAP, FlexContainer::WRAP);
    topAlignment.Add(buttonContainer);

    //Create buttons and place them in the container.
    for(int modelNumber = 0; modelNumber < NUM_MODELS; modelNumber++)
    {
      PushButton button = Toolkit::PushButton::New();
      button.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      button.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      button.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);
      button.SetProperty(Actor::Property::PADDING, Padding(elementPadding, elementPadding, elementPadding, elementPadding));
      button.SetProperty(Button::Property::UNSELECTED_BACKGROUND_VISUAL, BUTTON_IMAGE_URL[modelNumber]);
      button.SetProperty(Button::Property::SELECTED_BACKGROUND_VISUAL, BUTTON_IMAGE_URL[modelNumber]);
      button.RegisterProperty("modelNumber", Property::Value(modelNumber));
      button.ClickedSignal().Connect(this, &PrimitiveShapesController::OnChangeShapeClicked);

      buttonContainer.Add(button);
    }
  }

  //Add sliders to the bottom of the screen, which allow for control of shape properties such as radius.
  //Each slider is placed next to a label that states the property it affects.
  //The sliders are laid out like so:
  //
  //           +--------------------------------+
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //      ^    | Label +----------O-----------+ |
  //      |    |                                |
  //      |    |                                |
  //      |    | Label +--O-------------------+ |
  //  30% |    |                                |
  //      |    |                                |
  //      |    | Label +----------------------O |
  //      |    |                                |
  //      v    +--------------------------------+
  //
  void SetupSliders(Layer layer)
  {
    //Create table to hold sliders and their corresponding labels.
    mSliderTable = Toolkit::TableView::New(MAX_PROPERTIES, 2);
    mSliderTable.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
    mSliderTable.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
    mSliderTable.SetResizePolicy(ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS);
    mSliderTable.SetProperty(Actor::Property::SIZE_MODE_FACTOR, Vector3(0.9, 0.3, 0.0)); //90% of width, 30% of height.
    mSliderTable.SetFitWidth(0);                                                         //Label column should fit to natural size of label.
    mSliderTable.SetRelativeWidth(1, 1.0f);                                              //Slider column should fill remaining space.
    mSliderTable.SetCellPadding(Vector2(10.0f, 0.0f));                                   //Leave a gap between the slider and its label.
    layer.Add(mSliderTable);

    //Set up sliders, and place labels next to them.
    for(int i = 0; i < MAX_PROPERTIES; i++)
    {
      //Create slider
      Slider slider = Slider::New();
      slider.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      slider.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      slider.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      slider.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
      slider.ValueChangedSignal().Connect(this, &PrimitiveShapesController::OnSliderValueChanged);
      mSliders.push_back(slider);

      //Setup slider cell properties
      mSliderTable.AddChild(slider, TableView::CellPosition(i, 1));
      mSliderTable.SetCellAlignment(TableView::CellPosition(i, 1), HorizontalAlignment::CENTER, VerticalAlignment::CENTER);

      //Create slider label
      TextLabel sliderLabel = TextLabel::New();
      sliderLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      sliderLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      sliderLabel.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);
      mSliderLabels.push_back(sliderLabel);

      //Setup slider-label cell properties
      mSliderTable.AddChild(sliderLabel, TableView::CellPosition(i, 0));
      mSliderTable.SetCellAlignment(TableView::CellPosition(i, 0), HorizontalAlignment::LEFT, VerticalAlignment::CENTER);
    }
  }

  //Adds a control to the centre of the window to display the 3D shapes.
  //The model is placed in the center of the screen, like so:
  //
  //           +--------------------------------+
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //       ^   |           ----------           |
  //       |   |          /          \          |
  //       |   |         /            \         |
  //       |   |        |              |        |
  //   30% |   |        |              |        |
  //       |   |        |              |        |
  //       |   |         \            /         |
  //       |   |          \          /          |
  //       v   |           ----------           |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           |                                |
  //           +--------------------------------+
  //
  void SetupModel(Layer layer)
  {
    //Create a container to house the visual-holding actor, to provide a constant hitbox.
    Actor container = Actor::New();
    container.SetResizePolicy(ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS);
    container.SetProperty(Actor::Property::SIZE_MODE_FACTOR, Vector3(0.9, 0.3, 0.0)); //90% of width, 30% of height.
    container.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    container.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    layer.Add(container);

    //Create control to display the 3D primitive.
    mModel = Control::New();
    mModel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mModel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mModel.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    container.Add(mModel);

    //Load default shape.
    LoadCube();

    //Make model spin to demonstrate 3D.
    mRotationAnimation = Animation::New(15.0f);
    mRotationAnimation.AnimateBy(Property(mModel, Actor::Property::ORIENTATION),
                                 Quaternion(Degree(0.0f), Degree(360.0f), Degree(0.0f)));
    mRotationAnimation.SetLooping(true);
    mRotationAnimation.Play();

    //Attach gesture detector to pan models when rotated.
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach(container);
    mPanGestureDetector.DetectedSignal().Connect(this, &PrimitiveShapesController::OnPan);
  }

  //Clears all sliders and resets the primitive visual property map.
  void InitialiseSlidersAndModel()
  {
    //Sliders
    for(unsigned i = 0; i < mSliders.size(); i++)
    {
      mSliders.at(i).SetProperty(Slider::Property::MARKS, Property::Value(0)); //Remove marks
      mSliders.at(i).SetProperty(Actor::Property::VISIBLE, false);
      mSliderLabels.at(i).SetProperty(TextLabel::Property::TEXT, Property::Value("Default"));
      mSliderLabels.at(i).SetProperty(Actor::Property::VISIBLE, false);
    }

    //Visual map for model
    mVisualMap.Clear();
    mVisualMap[Toolkit::Visual::Property::TYPE]      = Visual::PRIMITIVE;
    mVisualMap[PrimitiveVisual::Property::MIX_COLOR] = mColor;
    mVisualMap[Visual::Property::TRANSFORM] =
      Property::Map().Add(Visual::Transform::Property::ORIGIN, Align::CENTER).Add(Visual::Transform::Property::ANCHOR_POINT, Align::CENTER);
  }

  //Sets the 3D model to a sphere and modifies the sliders appropriately.
  void LoadSphere()
  {
    InitialiseSlidersAndModel();

    //Set up specific visual properties.
    mVisualMap[PrimitiveVisual::Property::SHAPE]  = PrimitiveVisual::Shape::SPHERE;
    mVisualMap[PrimitiveVisual::Property::SLICES] = DEFAULT_SLICES;
    mVisualMap[PrimitiveVisual::Property::STACKS] = DEFAULT_STACKS;

    //Set up sliders.
    SetupSlider(0, SLICES_LOWER_BOUND, SLICES_UPPER_BOUND, DEFAULT_STACKS, PrimitiveVisual::Property::SLICES, "slices");
    SetupMarks(0, SLICES_LOWER_BOUND, SLICES_UPPER_BOUND);
    mSliders.at(0).SetProperty(Slider::Property::VALUE_PRECISION, Property::Value(0));

    SetupSlider(1, STACKS_LOWER_BOUND, STACKS_UPPER_BOUND, DEFAULT_STACKS, PrimitiveVisual::Property::STACKS, "stacks");
    SetupMarks(1, STACKS_LOWER_BOUND, STACKS_UPPER_BOUND);
    mSliders.at(1).SetProperty(Slider::Property::VALUE_PRECISION, Property::Value(0));

    //Set model in control.
    mModel.SetProperty(Control::Property::BACKGROUND, Property::Value(mVisualMap));

    //Update title.
    mShapeTitle.SetProperty(TextLabel::Property::TEXT, SHAPE_TITLE_PREFIX + "Sphere");
  }

  //Sets the 3D model to a cone and modifies the sliders appropriately.
  void LoadCone()
  {
    InitialiseSlidersAndModel();

    //Set up specific visual properties.
    mVisualMap[PrimitiveVisual::Property::SHAPE]               = PrimitiveVisual::Shape::CONE;
    mVisualMap[PrimitiveVisual::Property::SCALE_HEIGHT]        = DEFAULT_SCALE_HEIGHT;
    mVisualMap[PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS] = DEFAULT_SCALE_BOTTOM_RADIUS;
    mVisualMap[PrimitiveVisual::Property::SLICES]              = DEFAULT_SLICES;

    //Set up sliders.
    SetupSlider(0, 1.0f, 32.0f, DEFAULT_SCALE_HEIGHT, PrimitiveVisual::Property::SCALE_HEIGHT, "scaleHeight");
    mSliders.at(0).SetProperty(Slider::Property::VALUE_PRECISION, Property::Value(1));

    SetupSlider(1, 1.0f, 32.0f, DEFAULT_SCALE_BOTTOM_RADIUS, PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS, "scaleBottomRadius");
    mSliders.at(1).SetProperty(Slider::Property::VALUE_PRECISION, Property::Value(1));

    SetupSlider(2, SLICES_LOWER_BOUND, SLICES_UPPER_BOUND, DEFAULT_STACKS, PrimitiveVisual::Property::SLICES, "slices");
    SetupMarks(2, SLICES_LOWER_BOUND, SLICES_UPPER_BOUND);
    mSliders.at(2).SetProperty(Slider::Property::VALUE_PRECISION, Property::Value(0));

    //Set model in control.
    mModel.SetProperty(Control::Property::BACKGROUND, Property::Value(mVisualMap));

    //Update title.
    mShapeTitle.SetProperty(TextLabel::Property::TEXT, SHAPE_TITLE_PREFIX + "Cone");
  }

  //Sets the 3D model to a conical frustum and modifies the sliders appropriately.
  void LoadConicalFrustum()
  {
    InitialiseSlidersAndModel();

    //Set up specific visual properties.
    mVisualMap[PrimitiveVisual::Property::SHAPE]               = PrimitiveVisual::Shape::CONICAL_FRUSTUM;
    mVisualMap[PrimitiveVisual::Property::SCALE_TOP_RADIUS]    = DEFAULT_SCALE_TOP_RADIUS;
    mVisualMap[PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS] = DEFAULT_SCALE_BOTTOM_RADIUS;
    mVisualMap[PrimitiveVisual::Property::SCALE_HEIGHT]        = DEFAULT_SCALE_HEIGHT;
    mVisualMap[PrimitiveVisual::Property::SLICES]              = DEFAULT_SLICES;

    //Set up used sliders.
    SetupSlider(0, 1.0f, 32.0f, DEFAULT_SCALE_HEIGHT, PrimitiveVisual::Property::SCALE_HEIGHT, "scaleHeight");
    mSliders.at(0).SetProperty(Slider::Property::VALUE_PRECISION, Property::Value(1));

    SetupSlider(1, 0.0f, 32.0f, DEFAULT_SCALE_BOTTOM_RADIUS, PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS, "scaleBottomRadius");
    mSliders.at(1).SetProperty(Slider::Property::VALUE_PRECISION, Property::Value(1));

    SetupSlider(2, 0.0f, 32.0f, DEFAULT_SCALE_TOP_RADIUS, PrimitiveVisual::Property::SCALE_TOP_RADIUS, "scaleTopRadius");
    mSliders.at(2).SetProperty(Slider::Property::VALUE_PRECISION, Property::Value(1));

    //Set model in control.
    mModel.SetProperty(Control::Property::BACKGROUND, Property::Value(mVisualMap));

    //Update title.
    mShapeTitle.SetProperty(TextLabel::Property::TEXT, SHAPE_TITLE_PREFIX + "Conical Frustum");
  }

  //Sets the 3D model to a cylinder and modifies the sliders appropriately.
  void LoadCylinder()
  {
    InitialiseSlidersAndModel();

    //Set up specific visual properties.
    mVisualMap[PrimitiveVisual::Property::SHAPE]        = PrimitiveVisual::Shape::CYLINDER;
    mVisualMap[PrimitiveVisual::Property::SCALE_HEIGHT] = DEFAULT_SCALE_HEIGHT;
    mVisualMap[PrimitiveVisual::Property::SCALE_RADIUS] = DEFAULT_SCALE_RADIUS;
    mVisualMap[PrimitiveVisual::Property::SLICES]       = DEFAULT_SLICES;

    //Set up used sliders.
    SetupSlider(0, 1.0f, 32.0f, DEFAULT_SCALE_HEIGHT, PrimitiveVisual::Property::SCALE_HEIGHT, "scaleHeight");
    mSliders.at(0).SetProperty(Slider::Property::VALUE_PRECISION, Property::Value(1));

    SetupSlider(1, 1.0f, 32.0f, DEFAULT_SCALE_RADIUS, PrimitiveVisual::Property::SCALE_RADIUS, "scaleRadius");
    mSliders.at(1).SetProperty(Slider::Property::VALUE_PRECISION, Property::Value(1));

    SetupSlider(2, SLICES_LOWER_BOUND, SLICES_UPPER_BOUND, DEFAULT_STACKS, PrimitiveVisual::Property::SLICES, "slices");
    SetupMarks(2, SLICES_LOWER_BOUND, SLICES_UPPER_BOUND);
    mSliders.at(2).SetProperty(Slider::Property::VALUE_PRECISION, Property::Value(0));

    //Set model in control.
    mModel.SetProperty(Control::Property::BACKGROUND, Property::Value(mVisualMap));

    //Update title.
    mShapeTitle.SetProperty(TextLabel::Property::TEXT, SHAPE_TITLE_PREFIX + "Cylinder");
  }

  //Sets the 3D model to a cube and modifies the sliders appropriately.
  void LoadCube()
  {
    InitialiseSlidersAndModel();

    //Set up specific visual properties.
    mVisualMap[PrimitiveVisual::Property::SHAPE] = PrimitiveVisual::Shape::CUBE;

    //Set model in control.
    mModel.SetProperty(Control::Property::BACKGROUND, Property::Value(mVisualMap));

    //Update title.
    mShapeTitle.SetProperty(TextLabel::Property::TEXT, SHAPE_TITLE_PREFIX + "Cube");
  }

  //Sets the 3D model to a bevelled cube and modifies the sliders appropriately.
  void LoadBevelledCube()
  {
    InitialiseSlidersAndModel();

    //Set up specific visual properties.
    mVisualMap[PrimitiveVisual::Property::SHAPE]            = PrimitiveVisual::Shape::BEVELLED_CUBE;
    mVisualMap[PrimitiveVisual::Property::BEVEL_PERCENTAGE] = DEFAULT_BEVEL_PERCENTAGE;
    mVisualMap[PrimitiveVisual::Property::BEVEL_SMOOTHNESS] = DEFAULT_BEVEL_SMOOTHNESS;

    //Set up used sliders.
    SetupSlider(0, 0.0f, 1.0f, DEFAULT_BEVEL_PERCENTAGE, PrimitiveVisual::Property::BEVEL_PERCENTAGE, "bevelPercentage");
    mSliders.at(0).SetProperty(Slider::Property::VALUE_PRECISION, Property::Value(2));

    SetupSlider(1, 0.0f, 1.0f, DEFAULT_BEVEL_SMOOTHNESS, PrimitiveVisual::Property::BEVEL_SMOOTHNESS, "bevelSmoothness");
    mSliders.at(1).SetProperty(Slider::Property::VALUE_PRECISION, Property::Value(2));

    //Set model in control.
    mModel.SetProperty(Control::Property::BACKGROUND, Property::Value(mVisualMap));

    //Update title.
    mShapeTitle.SetProperty(TextLabel::Property::TEXT, SHAPE_TITLE_PREFIX + "Bevelled Cube");
  }

  //Sets the 3D model to an octahedron and modifies the sliders appropriately.
  void LoadOctahedron()
  {
    InitialiseSlidersAndModel();

    //Set up specific visual properties.
    mVisualMap[PrimitiveVisual::Property::SHAPE] = PrimitiveVisual::Shape::OCTAHEDRON;

    //Set model in control.
    mModel.SetProperty(Control::Property::BACKGROUND, Property::Value(mVisualMap));

    //Update title.
    mShapeTitle.SetProperty(TextLabel::Property::TEXT, SHAPE_TITLE_PREFIX + "Octahedron");
  }

  //Sets up the slider at the given index for the supplied property, and labels it appropriately.
  // visualProperty is the property that will be set by this slider.
  void SetupSlider(int sliderIndex, float lowerBound, float upperBound, float startPoint, Property::Index visualProperty, std::string visualPropertyLabel)
  {
    //Set up the slider itself.
    mSliders.at(sliderIndex).RegisterProperty("visualProperty", Property::Value(visualProperty), Property::READ_WRITE);
    mSliders.at(sliderIndex).SetProperty(Slider::Property::LOWER_BOUND, Property::Value(lowerBound));
    mSliders.at(sliderIndex).SetProperty(Slider::Property::UPPER_BOUND, Property::Value(upperBound));
    mSliders.at(sliderIndex).SetProperty(Slider::Property::VALUE, Property::Value(startPoint));
    mSliders.at(sliderIndex).SetProperty(Actor::Property::VISIBLE, true);

    //Label the slider with the property.
    //We reset the TextLabel to force a relayout of the table.
    mSliderTable.RemoveChildAt(TableView::CellPosition(sliderIndex, 0));

    TextLabel sliderLabel = TextLabel::New(visualPropertyLabel);
    sliderLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    sliderLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    sliderLabel.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);

    mSliderTable.AddChild(sliderLabel, TableView::CellPosition(sliderIndex, 0));
    mSliderTable.SetCellAlignment(TableView::CellPosition(sliderIndex, 0), HorizontalAlignment::LEFT, VerticalAlignment::CENTER);

    mSliderLabels.at(sliderIndex).SetProperty(Actor::Property::VISIBLE, true);
    mSliderLabels.at(sliderIndex) = sliderLabel;
  }

  //Setup snapping to integer values between the two given values.
  void SetupMarks(int sliderIndex, int lower, int upper)
  {
    Property::Array marks;

    for(int mark = lower; mark <= upper; mark++)
    {
      marks.PushBack(Property::Value(mark));
    }

    mSliders.at(sliderIndex).SetProperty(Slider::Property::MARKS, Property::Value(marks));
    mSliders.at(sliderIndex).SetProperty(Slider::Property::SNAP_TO_MARKS, Property::Value(true));
  }

  //When a shape button is tapped, switch to the corresponding shape.
  bool OnChangeShapeClicked(Button button)
  {
    //Get the model number from the button.
    int modelNumber;
    button.GetProperty(button.GetPropertyIndex("modelNumber")).Get(modelNumber);

    //Switch to the shape that corresponds to the model number.
    switch(modelNumber)
    {
      case 0:
      {
        LoadSphere();
        break;
      }
      case 1:
      {
        LoadCone();
        break;
      }
      case 2:
      {
        LoadConicalFrustum();
        break;
      }
      case 3:
      {
        LoadCylinder();
        break;
      }
      case 4:
      {
        LoadCube();
        break;
      }
      case 5:
      {
        LoadBevelledCube();
        break;
      }
      case 6:
      {
        LoadOctahedron();
        break;
      }
    }

    return true;
  }

  //When the slider is adjusted, change the corresponding shape property accordingly.
  bool OnSliderValueChanged(Slider slider, float value)
  {
    //Update property map to reflect the change to the specific visual property.
    int visualProperty;
    slider.GetProperty(slider.GetPropertyIndex("visualProperty")).Get(visualProperty);
    mVisualMap[visualProperty] = value;

    //Reload the model to display the change.
    mModel.SetProperty(Control::Property::BACKGROUND, Property::Value(mVisualMap));

    return true;
  }

  //Panning around the shape rotates it.
  void OnPan(Actor actor, const PanGesture& gesture)
  {
    switch(gesture.GetState())
    {
      case GestureState::STARTED:
      {
        //Pause animation, as the gesture will be used to manually rotate the model
        mRotationAnimation.Pause();

        break;
      }
      case GestureState::CONTINUING:
      {
        //Rotate based off the gesture.
        Vector2 displacement = gesture.GetDisplacement();
        Vector2 rotation{
          -displacement.y / X_ROTATION_DISPLACEMENT_FACTOR, // Y displacement rotates around X axis
          displacement.x / Y_ROTATION_DISPLACEMENT_FACTOR   // X displacement rotates around Y axis
        };

        Quaternion q  = Quaternion(Radian(rotation.x), Radian(rotation.y), Radian(0.f));
        Quaternion q0 = mModel.GetProperty(Actor::Property::ORIENTATION).Get<Quaternion>();

        mModel.SetProperty(Actor::Property::ORIENTATION, q * q0);

        break;
      }
      case GestureState::FINISHED:
      {
        //Return to automatic animation
        mRotationAnimation.Play();

        break;
      }
      case GestureState::CANCELLED:
      {
        //Return to automatic animation
        mRotationAnimation.Play();

        break;
      }
      default:
      {
        break;
      }
    }
  }

  //If escape or the back button is pressed, quit the application (and return to the launcher)
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication;

  std::vector<Slider>    mSliders;      ///< Holds the sliders on screen that each shape accesses.
  std::vector<TextLabel> mSliderLabels; ///< Holds the labels to each slider.
  TableView              mSliderTable;  ///< A table to layout the sliders next to their labels.

  Property::Map mVisualMap;  ///< Property map to create a primitive visual.
  Control       mModel;      ///< Control to house the primitive visual.
  TextLabel     mShapeTitle; ///< Indicates what the currently selected shape is.

  PanGestureDetector mPanGestureDetector; ///< Detects pan gestures for rotation of the model.
  Animation          mRotationAnimation;  ///< Automatically rotates the model, unless it is being panned.

  Vector4 mColor; ///< Color to set all shapes.
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application               application = Application::New(&argc, &argv, DEMO_THEME_PATH);
  PrimitiveShapesController test(application);
  application.MainLoop();
  return 0;
}
