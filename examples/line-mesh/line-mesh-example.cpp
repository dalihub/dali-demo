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

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali/devel-api/actors/actor-devel.h>

// INTERNAL INCLUDES
#include "generated/line-mesh-frag.h"
#include "generated/line-mesh-vert.h"
#include "shared/view.h"

#include <sstream>

using namespace Dali;

namespace
{
const unsigned short  INDEX_LINES[]   = {0, 1, 1, 2, 2, 3, 3, 4, 4, 0};
const unsigned short  INDEX_LOOP[]    = {0, 1, 2, 3, 4};
const unsigned short  INDEX_STRIP[]   = {0, 1, 2, 3, 4, 0};
const unsigned short* INDICES[3]      = {&INDEX_LINES[0], &INDEX_LOOP[0], &INDEX_STRIP[0]};
const unsigned int    INDICES_SIZE[3] = {sizeof(INDEX_LINES) / sizeof(INDEX_LINES[0]), sizeof(INDEX_LOOP) / sizeof(INDEX_LOOP[0]), sizeof(INDEX_STRIP) / sizeof(INDEX_STRIP[0])};

Geometry CreateGeometry()
{
  // Create vertices
  struct Vertex
  {
    Vector2 position1;
    Vector2 position2;
    Vector3 color;
  };

  // Create new geometry object
  Vertex pentagonVertexData[5] =
    {
      {Vector2(0.0f, 1.00f), Vector2(0.0f, -1.00f), Vector3(1.0f, 1.0f, 1.0f)},      // 0
      {Vector2(-0.95f, 0.31f), Vector2(0.59f, 0.81f), Vector3(1.0f, 0.0f, 0.0f)},    // 1
      {Vector2(-0.59f, -0.81f), Vector2(-0.95f, -0.31f), Vector3(0.0f, 1.0f, 0.0f)}, // 2
      {Vector2(0.59f, -0.81f), Vector2(0.95f, -0.31f), Vector3(0.0f, 0.0f, 1.0f)},   // 3
      {Vector2(0.95f, 0.31f), Vector2(-0.59f, 0.81f), Vector3(1.0f, 1.0f, 0.0f)},    // 4
    };

  Property::Map pentagonVertexFormat;
  pentagonVertexFormat["aPosition1"] = Property::VECTOR2;
  pentagonVertexFormat["aPosition2"] = Property::VECTOR2;
  pentagonVertexFormat["aColor"]     = Property::VECTOR3;
  VertexBuffer pentagonVertices      = VertexBuffer::New(pentagonVertexFormat);
  pentagonVertices.SetData(pentagonVertexData, 5);

  // Create the geometry object
  Geometry pentagonGeometry = Geometry::New();
  pentagonGeometry.AddVertexBuffer(pentagonVertices);
  pentagonGeometry.SetIndexBuffer(INDICES[0], INDICES_SIZE[0]);
  pentagonGeometry.SetType(Geometry::LINES);
  return pentagonGeometry;
}

} // anonymous namespace

// This example shows how to morph between 2 meshes with the same number of
// vertices.
class ExampleController : public ConnectionTracker
{
public:
  /**
   * The example controller constructor.
   * @param[in] application The application instance
   */
  ExampleController(Application& application)
  : mApplication(application),
    mWindowSize(),
    mShader(),
    mGeometry(),
    mRenderer(),
    mMeshActor(),
    mButtons(),
    mMinusButton(),
    mPlusButton(),
    mIndicesCountLabel(),
    mPrimitiveType(Geometry::LINES),
    mCurrentIndexCount(0),
    mMaxIndexCount(0)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ExampleController::Create);
  }

  /**
   * The example controller destructor
   */
  ~ExampleController()
  {
    // Nothing to do here;
  }

  /**
   * Invoked upon creation of application
   * @param[in] application The application instance
   */
  void Create(Application& application)
  {
    Window window = application.GetWindow();

    // initial settings
    mPrimitiveType     = Geometry::LINES;
    mCurrentIndexCount = 10;
    mMaxIndexCount     = 10;

    CreateRadioButtons();

    window.KeyEventSignal().Connect(this, &ExampleController::OnKeyEvent);

    mWindowSize = window.GetSize();

    Initialise();

    window.SetBackgroundColor(Vector4(0.0f, 0.2f, 0.2f, 1.0f));
  }

  /**
   * Invoked whenever application changes the type of geometry drawn
   */
  void Initialise()
  {
    Window window = mApplication.GetWindow();

    // destroy mesh actor and its resources if already exists
    if(mMeshActor)
    {
      window.Remove(mMeshActor);
      mMeshActor.Reset();
    }

    mShader   = Shader::New(SHADER_LINE_MESH_VERT, SHADER_LINE_MESH_FRAG);
    mGeometry = CreateGeometry();
    mRenderer = Renderer::New(mGeometry, mShader);

    mRenderer.SetIndexRange(0, 10); // lines
    mPrimitiveType = Geometry::LINES;

    mMeshActor = Actor::New();
    mMeshActor.AddRenderer(mRenderer);
    mMeshActor.SetProperty(Actor::Property::SIZE, Vector2(200, 200));
    mMeshActor.SetProperty(DevelActor::Property::UPDATE_SIZE_HINT, Vector2(400, 400));

    Property::Index morphAmountIndex = mMeshActor.RegisterProperty("uMorphAmount", 0.0f);

    mRenderer.SetProperty(Renderer::Property::DEPTH_INDEX, 0);

    mMeshActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mMeshActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    window.Add(mMeshActor);

    Animation animation = Animation::New(5);
    KeyFrames keyFrames = KeyFrames::New();
    keyFrames.Add(0.0f, 0.0f);
    keyFrames.Add(1.0f, 1.0f);

    animation.AnimateBetween(Property(mMeshActor, morphAmountIndex), keyFrames, AlphaFunction(AlphaFunction::SIN));
    animation.SetLooping(true);
    animation.Play();
  }

  /**
   * Invoked on create
   */
  void CreateRadioButtons()
  {
    Window window = mApplication.GetWindow();

    Toolkit::TableView modeSelectTableView = Toolkit::TableView::New(4, 1);
    modeSelectTableView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    modeSelectTableView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    modeSelectTableView.SetFitHeight(0);
    modeSelectTableView.SetFitHeight(1);
    modeSelectTableView.SetFitHeight(2);
    modeSelectTableView.SetCellPadding(Vector2(6.0f, 0.0f));
    modeSelectTableView.SetProperty(Actor::Property::SCALE, Vector3(0.8f, 0.8f, 0.8f));

    const char* labels[] =
      {
        "LINES",
        "LINE_LOOP",
        "LINE_STRIP"};

    for(int i = 0; i < 3; ++i)
    {
      Dali::Toolkit::RadioButton radio = Dali::Toolkit::RadioButton::New();

      radio.SetProperty(Toolkit::Button::Property::LABEL,
                        Property::Map()
                          .Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::TEXT)
                          .Add(Toolkit::TextVisual::Property::TEXT, labels[i])
                          .Add(Toolkit::TextVisual::Property::TEXT_COLOR, Vector4(0.8f, 0.8f, 0.8f, 1.0f)));

      radio.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      radio.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
      radio.SetProperty(Toolkit::Button::Property::SELECTED, i == 0);
      radio.PressedSignal().Connect(this, &ExampleController::OnButtonPressed);
      mButtons[i] = radio;
      modeSelectTableView.AddChild(radio, Toolkit::TableView::CellPosition(i, 0));
    }

    Toolkit::TableView elementCountTableView = Toolkit::TableView::New(1, 3);
    elementCountTableView.SetCellPadding(Vector2(6.0f, 0.0f));
    elementCountTableView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
    elementCountTableView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_LEFT);
    elementCountTableView.SetFitHeight(0);
    elementCountTableView.SetFitWidth(0);
    elementCountTableView.SetFitWidth(1);
    elementCountTableView.SetFitWidth(2);
    mMinusButton = Toolkit::PushButton::New();
    mMinusButton.SetProperty(Toolkit::Button::Property::LABEL, "<<");
    mMinusButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mMinusButton.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);

    Toolkit::PushButton mPlusButton = Toolkit::PushButton::New();
    mPlusButton.SetProperty(Toolkit::Button::Property::LABEL, ">>");
    mPlusButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mPlusButton.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_RIGHT);

    mMinusButton.ClickedSignal().Connect(this, &ExampleController::OnButtonClicked);
    mPlusButton.ClickedSignal().Connect(this, &ExampleController::OnButtonClicked);

    mIndicesCountLabel = Toolkit::TextLabel::New();
    mIndicesCountLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mIndicesCountLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

    std::stringstream str;
    str << mCurrentIndexCount;
    mIndicesCountLabel.SetProperty(Toolkit::TextLabel::Property::TEXT, str.str());
    mIndicesCountLabel.SetProperty(Toolkit::TextLabel::Property::TEXT_COLOR, Vector4(1.0, 1.0, 1.0, 1.0));
    mIndicesCountLabel.SetProperty(Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "BOTTOM");
    mIndicesCountLabel.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::WIDTH);
    mIndicesCountLabel.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT);

    elementCountTableView.AddChild(mMinusButton, Toolkit::TableView::CellPosition(0, 0));
    elementCountTableView.AddChild(mIndicesCountLabel, Toolkit::TableView::CellPosition(0, 1));
    elementCountTableView.AddChild(mPlusButton, Toolkit::TableView::CellPosition(0, 2));

    window.Add(modeSelectTableView);
    window.Add(elementCountTableView);
  }

  /**
   * Invoked whenever the quit button is clicked
   * @param[in] button the quit button
   */
  bool OnQuitButtonClicked(Toolkit::Button button)
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

  bool OnButtonPressed(Toolkit::Button button)
  {
    int indicesArray;
    if(button == mButtons[0])
    {
      mCurrentIndexCount = 10;
      mMaxIndexCount     = 10;
      mPrimitiveType     = Geometry::LINES;
      indicesArray       = 0;
    }
    else if(button == mButtons[1])
    {
      mCurrentIndexCount = 5;
      mMaxIndexCount     = 5;
      mPrimitiveType     = Geometry::LINE_LOOP;
      indicesArray       = 1;
    }
    else
    {
      mCurrentIndexCount = 6;
      mMaxIndexCount     = 6;
      mPrimitiveType     = Geometry::LINE_STRIP;
      indicesArray       = 2;
    }

    std::stringstream str;
    str << mCurrentIndexCount;
    mIndicesCountLabel.SetProperty(Toolkit::TextLabel::Property::TEXT, str.str());
    mGeometry.SetType(mPrimitiveType);
    mGeometry.SetIndexBuffer(INDICES[indicesArray], INDICES_SIZE[indicesArray]);
    mRenderer.SetIndexRange(0, mCurrentIndexCount);
    return true;
  }

  bool OnButtonClicked(Toolkit::Button button)
  {
    if(button == mMinusButton)
    {
      if(--mCurrentIndexCount < 2)
        mCurrentIndexCount = 2;
    }
    else
    {
      if(++mCurrentIndexCount > mMaxIndexCount)
        mCurrentIndexCount = mMaxIndexCount;
    }

    std::stringstream str;
    str << mCurrentIndexCount;
    mIndicesCountLabel.SetProperty(Toolkit::TextLabel::Property::TEXT, str.str());
    mRenderer.SetIndexRange(0, mCurrentIndexCount);
    return true;
  }

private:
  Application& mApplication; ///< Application instance
  Vector3      mWindowSize;  ///< The size of the window

  Shader               mShader;
  Geometry             mGeometry;
  Renderer             mRenderer;
  Actor                mMeshActor;
  Toolkit::RadioButton mButtons[3];
  Toolkit::PushButton  mMinusButton;
  Toolkit::PushButton  mPlusButton;
  Toolkit::TextLabel   mIndicesCountLabel;
  Geometry::Type       mPrimitiveType;
  int                  mCurrentIndexCount;
  int                  mMaxIndexCount;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application       application = Application::New(&argc, &argv);
  ExampleController test(application);
  application.MainLoop();
  return 0;
}
