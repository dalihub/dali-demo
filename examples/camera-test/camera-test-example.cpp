#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <string>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* DEMO_THEME_TWO_PATH(DEMO_STYLE_DIR "style-example-theme-two.json");

const std::string images[5] =
  {
    DEMO_IMAGE_DIR "application-icon-97.png",
    DEMO_IMAGE_DIR "application-icon-84.png",
    DEMO_IMAGE_DIR "application-icon-76.png",
    DEMO_IMAGE_DIR "application-icon-69.png",
    DEMO_IMAGE_DIR "application-icon-72.png",
};

const char* BORDER_IMAGE(DEMO_IMAGE_DIR "border-4px.9.png");
const char* RESIZE_HANDLE_IMAGE(DEMO_IMAGE_DIR "resize-handle.png");
const char* CIRCLE_POINT(DEMO_IMAGE_DIR "circle_point.png");
const char* CIRCLE_STROKE(DEMO_IMAGE_DIR "circle_stroke_point.png");

const int BORDER_WIDTH(4);

} // namespace

struct CameraPosConstraint
{
  CameraPosConstraint() = default;

  void operator()(Vector3& current, const PropertyInputContainer& inputs)
  {
    auto& newPos = inputs[0]->GetVector3();
    current.x    = newPos.x;
    current.y    = newPos.y;
  }
};

struct BoundaryConstraint
{
  BoundaryConstraint(std::string nameParam, Vector2 size, bool centeredParam)
  : name(nameParam),
    boundarySize(size),
    centered(centeredParam)
  {
  }
  void operator()(Vector3& current, const PropertyInputContainer& inputs)
  {
    Vector3 curPos  = inputs[0]->GetVector3();
    Vector3 curSize = inputs[1]->GetVector3();

    if(centered)
    {
      // AnchorPoint is center of actor
      current.x = Dali::Clamp(curPos.x, 0.0f, boundarySize.x);
      current.y = Dali::Clamp(curPos.y, 0.0f, boundarySize.y);
    }
    else
    {
      // Anchor point is top left of actor.
      current.x = Dali::Clamp(curPos.x, 0.0f, boundarySize.x - curSize.x);
      current.y = Dali::Clamp(curPos.y, 0.0f, boundarySize.y - curSize.y);
    }
  }

  std::string name;
  Vector2     boundarySize;
  bool        centered;
};

struct ViewportPositionConstraint
{
  ViewportPositionConstraint(Vector2 windowOrigin)
  : origin(windowOrigin)
  {
  }
  void operator()(Vector2& current, const PropertyInputContainer& inputs)
  {
    Vector3 framePos = inputs[0]->GetVector3();
    current.x        = framePos.x + origin.x;
    current.y        = framePos.y + origin.y;
  }
  Vector2 origin;
};

struct ViewportSizeConstraint
{
  ViewportSizeConstraint() = default;

  void operator()(Vector2& current, const PropertyInputContainer& inputs)
  {
    Vector3 frameSize = inputs[0]->GetVector3();
    current.x         = frameSize.x;
    current.y         = frameSize.y;
  }
};

class CameraTestExample : public ConnectionTracker
{
public:
  CameraTestExample(Application app)
  : mApp(app)
  {
    mApp.InitSignal().Connect(this, &CameraTestExample::OnCreate);
  }

  void OnCreate(Application& app)
  {
    // Use default camera.
    Window  window = app.GetWindow();
    Vector2 windowSize(1280, 800);
    if(window.GetSize().GetWidth() < 1280)
    {
      window.SetSize(Uint16Pair(windowSize.x, windowSize.y)); // 1280 divides into 427, 427, 426
    }
    else
    {
      windowSize.x = window.GetSize().GetWidth();
      windowSize.y = window.GetSize().GetHeight();
    }

    window.KeyEventSignal().Connect(this, &CameraTestExample::OnKeyEvent);

    // Divide the screen up into 3 regions:
    // left: the scene + camera outline
    // mid: the viewport
    // right: actor / camera controls

    // Viewport frame position is constrained to the mid third, but is actually drawn into full screen overlay.
    // Camera frame position is constrained to the left third, but is actually drawn into full screen overlay.

    Vector2 sceneSize = Vector2(ceilf(windowSize.x / 3.0f), 720);
    Vector2 panelSize = Vector2(ceilf(windowSize.x / 3.0f), windowSize.y);

    viewLayer = CreateScene(window, Vector2::ZERO, sceneSize);

    sceneRenderTask = CreateSceneRenderTask(window, viewLayer, sceneSize);

    Layer overlayLayer = Layer::New(); // Will be drawn last by separate render tasks
    window.Add(overlayLayer);
    overlayLayer.SetProperty(Actor::Property::POSITION, Vector3::ZERO);
    overlayLayer.SetProperty(Actor::Property::SIZE, windowSize);
    overlayLayer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    overlayLayer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

    overlayRenderTask = CreateOverlayRenderTask(window, windowSize, overlayLayer);

    // Create camera frame
    initialCameraSize     = Vector3(windowSize.x / 3.0f, windowSize.y, 1);
    initialCameraPosition = Vector3(windowSize.x / 6.0f, windowSize.y * 0.5f, 0);

    cameraFrame = CreateFrame(Vector2(initialCameraSize), sceneSize, "Camera");
    cameraFrame.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    cameraFrame.SetProperty(Actor::Property::POSITION, initialCameraPosition);

    cameraGrabHandle = CreateCornerHandle(cameraFrame);
    cameraGrabCenter = CreateCenterHandle(cameraFrame);

    // Create viewport frame
    initialViewportSize     = sceneSize * 0.75f; // Maintain aspect ratio
    initialViewportPosition = Vector3(100, 100, 0);

    viewportFrame = CreateFrame(Vector2(initialViewportPosition), Vector2(initialViewportSize), "Viewport");
    viewportFrame.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(1.0f / 3.0f, 0.0f, 0.5f)); // TL of 2nd third of screen
    viewportFrame.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

    viewportGrabHandle = CreateCornerHandle(viewportFrame);
    viewportGrabCenter = CreateCenterHandle(viewportFrame);

    Vector2 viewportOrigin(windowSize.x / 3.0f, 0.0f); // Viewport origin in window coords (to match frame)
    ConstrainSceneViewportCamera(sceneRenderTask, viewportOrigin);

    Constraint constraint = Constraint::New<Vector3>(cameraFrame, Actor::Property::POSITION, BoundaryConstraint("cameraFrame", panelSize, true));
    constraint.AddSource(LocalSource(Actor::Property::POSITION));
    constraint.AddSource(LocalSource(Actor::Property::SIZE));
    constraint.Apply();

    constraint = Constraint::New<Vector3>(viewportFrame, Actor::Property::POSITION, BoundaryConstraint("viewportFrame", panelSize, false));
    constraint.AddSource(LocalSource(Actor::Property::POSITION));
    constraint.AddSource(LocalSource(Actor::Property::SIZE));
    constraint.Apply();

    overlayLayer.Add(viewportFrame);
    overlayLayer.Add(cameraFrame);

    CreateActorControls(overlayLayer, panelSize);

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach(viewportGrabHandle);
    mPanGestureDetector.Attach(viewportGrabCenter);
    mPanGestureDetector.Attach(cameraGrabHandle);
    mPanGestureDetector.Attach(cameraGrabCenter);
    mPanGestureDetector.DetectedSignal().Connect(this, &CameraTestExample::OnPan);

    mTapGestureDetector = TapGestureDetector::New(2);
    mTapGestureDetector.Attach(viewportGrabHandle);
    mTapGestureDetector.Attach(viewportGrabCenter);
    mTapGestureDetector.Attach(cameraGrabHandle);
    mTapGestureDetector.Attach(cameraGrabCenter);
    mTapGestureDetector.DetectedSignal().Connect(this, &CameraTestExample::OnDoubleTap);
  }

  Layer CreateScene(Window window, Vector2 position, Vector2 size)
  {
    const Vector3 offsets[] =
      {
        Dali::ParentOrigin::TOP_LEFT,
        Dali::ParentOrigin::TOP_RIGHT,
        Dali::ParentOrigin::BOTTOM_LEFT,
        Dali::ParentOrigin::BOTTOM_RIGHT,
        Dali::ParentOrigin::CENTER,
      };

    viewLayer = Layer::New();
    window.Add(viewLayer);

    viewLayer.SetProperty(Actor::Property::POSITION, Vector3(position));
    viewLayer.SetProperty(Actor::Property::SIZE, size);
    viewLayer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_LEFT);
    viewLayer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);

    auto bg = Control::New();
    bg.SetBackgroundColor(Vector4(0.1, 0.0, 0.1, 0.5));
    bg.SetProperty(Actor::Property::SIZE, size);
    bg.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    bg.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    viewLayer.Add(bg);

    auto label                                       = TextLabel::New("Scene");
    label[TextLabel::Property::POINT_SIZE]           = 12;
    label[TextLabel::Property::HORIZONTAL_ALIGNMENT] = HorizontalAlignment::CENTER;
    label[TextLabel::Property::TEXT_COLOR]           = Color::WHITE;
    label[Actor::Property::PARENT_ORIGIN]            = ParentOrigin::TOP_CENTER;
    label[Actor::Property::ANCHOR_POINT]             = AnchorPoint::TOP_CENTER;
    viewLayer.Add(label);

    int i = 0;
    for(auto& image : images)
    {
      auto imageView = ImageView::New(image);
      imageViews.push_back(imageView);
      viewLayer.Add(imageView);

      printf("ActorId: %d\n", imageView.GetProperty<int>(Actor::Property::ID));

      imageView.SetProperty(Actor::Property::PARENT_ORIGIN, offsets[i]);
      imageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      imageView.SetProperty(Actor::Property::POSITION, Vector3((0.5f - offsets[i].x) * 50.0f, (0.5f - offsets[i].y) * 50.0f, 0.0f));
      imageView.SetProperty(Actor::Property::SIZE, Vector2(97.0f, 97.0f));
      ++i;
    }
    return viewLayer;
  }

  void CreateActorControls(Layer parent, Vector2 sceneSize)
  {
    // Create a container that sits on the rightmost third of the window
    Actor layoutContainer                           = Actor::New();
    layoutContainer[Actor::Property::SIZE]          = Vector3(sceneSize.x, sceneSize.y, sceneSize.x);
    layoutContainer[Actor::Property::PARENT_ORIGIN] = ParentOrigin::TOP_RIGHT;
    layoutContainer[Actor::Property::ANCHOR_POINT]  = AnchorPoint::TOP_RIGHT;

    auto bg = Control::New();
    bg.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    bg[Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
    bg.SetBackgroundColor(Vector4(0.05f, 0.05f, 0.1f, 1.0f));
    layoutContainer.Add(bg);

    parent.Add(layoutContainer);

    // Add a table for actor rotations
    TableView actorProps = TableView::New(7, 2);
    actorProps.SetProperty(Actor::Property::NAME, "ActorSliderLayout");
    actorProps.SetProperty(Actor::Property::SIZE, Vector3(sceneSize.x, sceneSize.y, sceneSize.x));
    actorProps.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    actorProps.SetFitWidth(0);
    actorProps.SetResizePolicy(ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT);

    int i = 0;
    for(auto& image : images)
    {
      actorProps.SetFitHeight(i);

      auto icon = ImageView::New(image);
      icon.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      // I would like 50% of natural size...
      icon.SetProperty(Actor::Property::SIZE, Vector3(50, 50, 50));
      actorProps.AddChild(icon, TableView::CellPosition(i, 0));
      actorProps.SetCellAlignment(TableView::CellPosition(i, 0), HorizontalAlignment::CENTER, VerticalAlignment::CENTER);
      auto               slider = Slider::New();
      std::ostringstream sliderStyleName;
      sliderStyleName << "RotationSlider" << i + 1;
      slider.SetProperty(Actor::Property::NAME, sliderStyleName.str());
      slider.SetStyleName("ThinSlider");
      slider.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      slider.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
      slider.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
      slider.SetResizePolicy(ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT);
      slider.SetProperty(Slider::Property::LOWER_BOUND, -180.0f);
      slider.SetProperty(Slider::Property::UPPER_BOUND, 180.0f);
      slider.SetProperty(Slider::Property::VALUE_PRECISION, 0);
      slider.SetProperty(Slider::Property::VALUE, 0.0f);
      slider.SetProperty(Slider::Property::SHOW_POPUP, true);
      slider.RegisterProperty("actorId", i, Property::READ_ONLY); // imageViews[i].GetId()

      actorProps.AddChild(slider, TableView::CellPosition(i, 1));
      actorProps.SetCellAlignment(TableView::CellPosition(i, 1), HorizontalAlignment::RIGHT, VerticalAlignment::CENTER);
      slider.ValueChangedSignal().Connect(this, &CameraTestExample::OnRotationSliderChanged);
      i++;
    }

    auto fovLabel = TextLabel::New("FOV");
    fovLabel.SetProperty(TextLabel::Property::POINT_SIZE, 12);
    fovLabel.SetProperty(TextLabel::Property::TEXT_COLOR, Color::WHITE);
    fovLabel[TextLabel::Property::HORIZONTAL_ALIGNMENT] = HorizontalAlignment::CENTER;
    fovLabel[Actor::Property::PARENT_ORIGIN]            = ParentOrigin::CENTER;
    fovLabel[Actor::Property::ANCHOR_POINT]             = AnchorPoint::CENTER;
    actorProps.AddChild(fovLabel, TableView::CellPosition(i, 0));

    auto slider = Slider::New();
    slider.SetProperty(Actor::Property::NAME, "FOVSlider");
    slider.SetStyleName("ThinSlider");
    slider.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    slider.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    slider.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    slider.SetResizePolicy(ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT);
    slider.SetProperty(Slider::Property::LOWER_BOUND, 1.0f);
    slider.SetProperty(Slider::Property::UPPER_BOUND, 160.0f);
    slider.SetProperty(Slider::Property::VALUE_PRECISION, 0);
    slider.SetProperty(Slider::Property::VALUE, Degree(Radian(cameraActor.GetFieldOfView())).degree);
    slider.SetProperty(Slider::Property::SHOW_POPUP, true);
    slider.ValueChangedSignal().Connect(this, &CameraTestExample::OnFOVChanged);
    actorProps.AddChild(slider, TableView::CellPosition(i, 1));

    layoutContainer.Add(actorProps);

    cameraDetail                                            = TextLabel::New();
    cameraDetail[TextLabel::Property::POINT_SIZE]           = 10;
    cameraDetail[TextLabel::Property::HORIZONTAL_ALIGNMENT] = HorizontalAlignment::CENTER;
    cameraDetail[TextLabel::Property::MULTI_LINE]           = true;
    cameraDetail[TextLabel::Property::TEXT_COLOR]           = Color::WHITE;
    cameraDetail[Actor::Property::PARENT_ORIGIN]            = ParentOrigin::BOTTOM_CENTER;
    cameraDetail[Actor::Property::ANCHOR_POINT]             = AnchorPoint::BOTTOM_CENTER;

    cameraDetail[Actor::Property::POSITION] = Vector3(0.0f, -15.0f, 0.0f);
    cameraDetail.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
    cameraDetail.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);
    layoutContainer.Add(cameraDetail);

    SetCameraDetailString();
  }

  RenderTask CreateSceneRenderTask(Window window, Layer source, Vector2 sceneSize)
  {
    // Create a render task with moveable viewport and camera
    RenderTask renderTask = window.GetRenderTaskList().CreateTask();

    cameraActor = CameraActor::New();
    cameraActor.SetType(Camera::FREE_LOOK);
    cameraActor.SetPerspectiveProjection(sceneSize);
    window.Add(cameraActor);

    renderTask.SetCameraActor(cameraActor);
    renderTask.SetInputEnabled(false);
    renderTask.SetClearColor(Vector4(0.05f, 0.05f, 0.05f, 1.0f));
    renderTask.SetClearEnabled(true);
    renderTask.SetSourceActor(source);

    return renderTask;
  }

  void ConstrainSceneViewportCamera(RenderTask renderTask, Vector2 viewportOrigin)
  {
    Constraint constraint = Constraint::New<Vector3>(cameraActor, Actor::Property::POSITION, CameraPosConstraint());
    constraint.AddSource(Source(cameraFrame, Actor::Property::POSITION));
    constraint.Apply();

    constraint = Constraint::New<Vector2>(cameraActor, Actor::Property::SIZE, EqualToConstraint());
    constraint.AddSource(Source(cameraFrame, Actor::Property::SIZE));
    constraint.Apply();

    constraint = Constraint::New<Vector2>(renderTask, RenderTask::Property::VIEWPORT_POSITION, ViewportPositionConstraint(viewportOrigin));
    constraint.AddSource(Source(viewportFrame, Actor::Property::POSITION));
    constraint.Apply();

    constraint = Constraint::New<Vector2>(renderTask, RenderTask::Property::VIEWPORT_SIZE, ViewportSizeConstraint());
    constraint.AddSource(Source(viewportFrame, Actor::Property::SIZE));
    constraint.Apply();
  }

  RenderTask CreateOverlayRenderTask(Window window, Vector2 windowSize, Layer source)
  {
    // Create a separate camera / render task to visualize the input frames above the 2nd rendered layer above.
    CameraActor overlayCameraActor = CameraActor::New();
    overlayCameraActor.SetType(Camera::FREE_LOOK);
    overlayCameraActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    overlayCameraActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    overlayCameraActor.SetProperty(Actor::Property::POSITION, Vector2::ZERO);
    overlayCameraActor.SetProperty(Actor::Property::SIZE, windowSize);
    overlayCameraActor.SetPerspectiveProjection(windowSize);
    window.Add(overlayCameraActor);

    RenderTask renderTask = window.GetRenderTaskList().CreateTask();

    renderTask.SetCameraActor(overlayCameraActor);
    renderTask.SetInputEnabled(true);
    renderTask.SetClearEnabled(false);
    renderTask.SetSourceActor(source);
    renderTask.SetExclusive(true);

    renderTask.SetViewportPosition(Vector2(0, 0));
    renderTask.SetViewportSize(windowSize);

    renderTask.SetCullMode(false);

    return renderTask;
  }

  Actor CreateFrame(Vector2 position, Vector2 size, std::string label)
  {
    Actor frame = Toolkit::ImageView::New(BORDER_IMAGE);
    frame.SetProperty(Dali::Actor::Property::NAME, "ContentFrame");
    frame.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    frame.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    frame.SetProperty(Actor::Property::SIZE, size);
    frame.SetProperty(Actor::Property::POSITION, position);

    auto frameLabel                                       = TextLabel::New(label);
    frameLabel[TextLabel::Property::POINT_SIZE]           = 12;
    frameLabel[TextLabel::Property::HORIZONTAL_ALIGNMENT] = HorizontalAlignment::CENTER;
    frameLabel[TextLabel::Property::TEXT_COLOR]           = Color::YELLOW;
    frameLabel[Actor::Property::PARENT_ORIGIN]            = ParentOrigin::BOTTOM_CENTER;
    frameLabel[Actor::Property::ANCHOR_POINT]             = AnchorPoint::BOTTOM_CENTER;
    frameLabel[Actor::Property::POSITION]                 = Vector3(0.0f, 15.0f, 0.0f);
    frame.Add(frameLabel);

    return frame;
  }

  Actor CreateCornerHandle(Actor parent)
  {
    Actor cornerHandle = Toolkit::ImageView::New(RESIZE_HANDLE_IMAGE);
    cornerHandle.SetProperty(Dali::Actor::Property::NAME, "GrabHandle");
    cornerHandle.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);
    cornerHandle.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_RIGHT);
    cornerHandle.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_RIGHT);
    cornerHandle.SetProperty(Actor::Property::POSITION, Vector2(-BORDER_WIDTH, -BORDER_WIDTH));
    cornerHandle.SetProperty(Actor::Property::OPACITY, 0.6f);
    parent.Add(cornerHandle);
    return cornerHandle;
  }

  Actor CreateCenterHandle(Actor parent)
  {
    Actor grabCenter = Toolkit::ImageView::New(CIRCLE_STROKE);
    grabCenter.SetProperty(Dali::Actor::Property::NAME, "GrabCenter");
    grabCenter.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);
    grabCenter.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    grabCenter.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    Toolkit::ImageView grabCenter2 = Toolkit::ImageView::New(CIRCLE_POINT);
    grabCenter2.SetProperty(Dali::Actor::Property::NAME, "GrabCenter2");
    grabCenter2.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);
    grabCenter2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    grabCenter2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    grabCenter.Add(grabCenter2);
    grabCenter.SetProperty(Actor::Property::OPACITY, 0.6f);
    parent.Add(grabCenter);
    return grabCenter;
  }

  void SetCameraDetailString()
  {
    std::ostringstream oss;

    auto pos  = cameraActor.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
    auto size = cameraFrame.GetTargetSize();

    oss << "Camera Detail: Pos: (" << pos.x << ", " << pos.y << ")" << std::endl;
    oss << "  Frame size: (" << size.x << ", " << size.y << ")" << std::endl;

    // Really, just need a box that represents the screen plane at z=0 from MVP.

    cameraDetail.SetProperty(TextLabel::Property::TEXT, oss.str());
  }

  void OnPan(Actor actor, const PanGesture& gesture)
  {
    if(actor == viewportGrabHandle)
    {
      auto size    = viewportFrame.GetTargetSize();
      auto newSize = Vector2(size.GetVectorXY() + gesture.GetDisplacement() * 2.0f);
      viewportFrame.SetProperty(Actor::Property::SIZE, newSize);
    }
    else if(actor == viewportGrabCenter)
    {
      Vector3 pos;
      if(gesture.GetState() == GestureState::FINISHED)
      {
        pos = viewportFrame.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
      }
      else
      {
        pos = viewportFrame.GetProperty<Vector3>(Actor::Property::POSITION);
      }
      pos = Vector2(pos.GetVectorXY() + gesture.GetDisplacement());
      viewportFrame.SetProperty(Actor::Property::POSITION, pos);
    }
    else if(actor == cameraGrabHandle)
    {
      auto size    = cameraFrame.GetTargetSize();
      auto newSize = Vector2(size.GetVectorXY() + gesture.GetDisplacement() * 2.0f);
      cameraFrame.SetProperty(Actor::Property::SIZE, newSize);
      cameraActor.SetPerspectiveProjection(newSize);
    }
    else if(actor == cameraGrabCenter)
    {
      Vector3 pos;
      if(gesture.GetState() == GestureState::FINISHED)
      {
        pos = cameraFrame.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
      }
      else
      {
        pos = cameraFrame.GetProperty<Vector3>(Actor::Property::POSITION);
      }
      pos = Vector2(pos.GetVectorXY() + gesture.GetDisplacement());
      cameraFrame.SetProperty(Actor::Property::POSITION, pos);
    }

    SetCameraDetailString();
  }

  void OnDoubleTap(Actor actor, const TapGesture& gesture)
  {
    if(actor == viewportGrabHandle)
    {
      viewportFrame.SetProperty(Actor::Property::SIZE, initialViewportSize);
    }
    else if(actor == viewportGrabCenter)
    {
      viewportFrame.SetProperty(Actor::Property::POSITION, initialViewportPosition);
    }
    else if(actor == cameraGrabHandle)
    {
      cameraFrame.SetProperty(Actor::Property::SIZE, initialCameraSize);
      cameraActor.SetProperty(Actor::Property::SIZE, initialCameraSize);
      cameraActor.SetPerspectiveProjection(Vector2(initialCameraSize));
    }
    else if(actor == cameraGrabCenter)
    {
      cameraFrame.SetProperty(Actor::Property::POSITION, initialCameraPosition);
    }
  }

  bool OnFOVChanged(Slider slider, float value)
  {
    cameraActor.SetProperty(CameraActor::Property::FIELD_OF_VIEW, (float)Radian(Degree(value)));
    return true;
  }

  bool OnRotationSliderChanged(Slider slider, float value)
  {
    auto actorIdIndex = slider.GetPropertyIndex("actorId");
    if(actorIdIndex != Property::INVALID_INDEX)
    {
      int   actorIndex = slider["actorId"];
      float newValue   = slider["value"];
      imageViews[actorIndex].SetProperty(Actor::Property::ORIENTATION, AngleAxis(Degree(newValue), Vector3::ZAXIS));
    }
    return true;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
      {
        mApp.Quit();
      }
    }
  }

  RenderTask sceneRenderTask;
  RenderTask overlayRenderTask;

  PanGestureDetector mPanGestureDetector;
  TapGestureDetector mTapGestureDetector;

  Application            mApp;
  std::vector<ImageView> imageViews;

  Layer viewLayer; // The "scene"

  Vector3 initialCameraPosition;
  Vector3 initialCameraSize;
  Vector3 initialViewportPosition;
  Vector3 initialViewportSize;

  Actor viewportFrame;
  Actor viewportGrabCenter;
  Actor viewportGrabHandle;

  Actor cameraFrame;
  Actor cameraGrabCenter;
  Actor cameraGrabHandle;

  CameraActor cameraActor;
  TextLabel   cameraDetail;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application       application = Application::New(&argc, &argv, DEMO_THEME_TWO_PATH);
  CameraTestExample test(application);
  application.MainLoop();
  return 0;
}
