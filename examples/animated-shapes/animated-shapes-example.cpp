/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/dali.h>
#include "shared/view.h"
#include "generated/animated-shapes-vert.h"
#include "generated/animated-shapes-frag.h"

#include <sstream>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* APPLICATION_TITLE("Animated Shapes");

Shader CreateShader(unsigned int pointCount)
{
  std::ostringstream vertexShader;
  vertexShader << "#define MAX_POINT_COUNT " << pointCount << "\n"
               << SHADER_ANIMATED_SHAPES_VERT;

  std::ostringstream fragmentShader;
  fragmentShader << "#extension GL_OES_standard_derivatives : enable "
                 << "\n"
                 << SHADER_ANIMATED_SHAPES_FRAG;

  Shader shader = Shader::New(vertexShader.str(), fragmentShader.str());
  for(unsigned int i(0); i < pointCount; ++i)
  {
    std::ostringstream propertyName;
    propertyName << "uPosition[" << i << "]";
    shader.RegisterProperty(propertyName.str(), Vector3(0.0f, 0.0f, 0.0f));
  }

  return shader;
}

} //unnamed namespace

// This example shows resolution independent rendering and animation of curves using the gpu.
//
class AnimatedShapesExample : public ConnectionTracker
{
public:
  AnimatedShapesExample(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &AnimatedShapesExample::Create);
  }

  ~AnimatedShapesExample() = default;

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    Window        window     = application.GetWindow();
    const Vector2 windowSize = window.GetSize();

    // Creates the background gradient
    Toolkit::Control background                = Dali::Toolkit::Control::New();
    background[Actor::Property::ANCHOR_POINT]  = Dali::AnchorPoint::CENTER;
    background[Actor::Property::PARENT_ORIGIN] = Dali::ParentOrigin::CENTER;
    background.SetResizePolicy(Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::ALL_DIMENSIONS);

    Dali::Property::Map map;
    map.Insert(Toolkit::Visual::Property::TYPE, Visual::GRADIENT);
    Property::Array stopOffsets;
    stopOffsets.PushBack(0.0f);
    stopOffsets.PushBack(1.0f);
    map.Insert(GradientVisual::Property::STOP_OFFSET, stopOffsets);
    Property::Array stopColors;
    stopColors.PushBack(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    stopColors.PushBack(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    map.Insert(GradientVisual::Property::STOP_COLOR, stopColors);
    Vector2 halfWindowSize = windowSize * 0.5f;
    map.Insert(GradientVisual::Property::START_POSITION, Vector2(0.0f, -halfWindowSize.y));
    map.Insert(GradientVisual::Property::END_POSITION, Vector2(0.0f, halfWindowSize.y));
    map.Insert(GradientVisual::Property::UNITS, GradientVisual::Units::USER_SPACE);
    background[Dali::Toolkit::Control::Property::BACKGROUND] = map;
    window.Add(background);

    // Create a TextLabel for the application title.
    Toolkit::TextLabel label                                  = Toolkit::TextLabel::New(APPLICATION_TITLE);
    label[Actor::Property::ANCHOR_POINT]                      = AnchorPoint::TOP_CENTER;
    label[Actor::Property::PARENT_ORIGIN]                     = Vector3(0.5f, 0.0f, 0.5f);
    label[Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT] = "CENTER";
    label[Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT]   = "CENTER";
    label[Toolkit::TextLabel::Property::TEXT_COLOR]           = Color::WHITE;
    window.Add(label);

    CreateTriangleMorph(Vector3(windowSize.x * 0.5f, windowSize.y * 0.15f, 0.0f), 100.0f);
    CreateCircleMorph(Vector3(windowSize.x * 0.5f, windowSize.y * 0.5f, 0.0f), 55.0f);
    CreateQuadMorph(Vector3(windowSize.x * 0.5f, windowSize.y * 0.85f, 0.0f), 60.0f);

    window.KeyEventSignal().Connect(this, &AnimatedShapesExample::OnKeyEvent);
  }

  void CreateTriangleMorph(Vector3 center, float side)
  {
    float h = (side * 0.5f) / 0.866f;

    Vector3 v0 = Vector3(-h, h, 0.0f);
    Vector3 v1 = Vector3(0.0f, -side * 0.366f, 0.0f);
    Vector3 v2 = Vector3(h, h, 0.0f);

    Vector3 v3 = v0 + ((v1 - v0) * 0.5f);
    Vector3 v4 = v1 + ((v2 - v1) * 0.5f);
    Vector3 v5 = v2 + ((v0 - v2) * 0.5f);

    Shader shader          = CreateShader(12);
    shader["uPosition[0]"] = v0;
    shader["uPosition[1]"] = v3;
    shader["uPosition[2]"] = v1;

    shader["uPosition[3]"] = v1;
    shader["uPosition[4]"] = v4;
    shader["uPosition[5]"] = v2;

    shader["uPosition[6]"] = v2;
    shader["uPosition[7]"] = v5;
    shader["uPosition[8]"] = v0;

    shader["uPosition[9]"]  = v0;
    shader["uPosition[10]"] = v1;
    shader["uPosition[11]"] = v2;

    //Create geometry
    static const Vector3 vertexData[] = {Dali::Vector3(0.0f, 0.0f, 0.0f),
                                         Dali::Vector3(0.5f, 0.0f, 1.0f),
                                         Dali::Vector3(1.0f, 1.0f, 2.0f),

                                         Dali::Vector3(0.0f, 0.0f, 3.0f),
                                         Dali::Vector3(0.5f, 0.0f, 4.0f),
                                         Dali::Vector3(1.0f, 1.0f, 5.0f),

                                         Dali::Vector3(0.0f, 0.0f, 6.0f),
                                         Dali::Vector3(0.5f, 0.0f, 7.0f),
                                         Dali::Vector3(1.0f, 1.0f, 8.0f),

                                         Dali::Vector3(0.0f, 1.0f, 9.0f),
                                         Dali::Vector3(0.0f, 1.0f, 10.0f),
                                         Dali::Vector3(0.0f, 1.0f, 11.0f)};

    unsigned short indexData[] = {0, 2, 1, 3, 5, 4, 6, 8, 7, 9, 11, 10};

    //Create a vertex buffer for vertex positions and texture coordinates
    Dali::Property::Map vertexFormat;
    vertexFormat["aCoefficient"]    = Dali::Property::VECTOR3;
    Dali::VertexBuffer vertexBuffer = Dali::VertexBuffer::New(vertexFormat);
    vertexBuffer.SetData(vertexData, sizeof(vertexData) / sizeof(vertexData[0]));

    //Create the geometry
    Dali::Geometry geometry = Dali::Geometry::New();
    geometry.AddVertexBuffer(vertexBuffer);
    geometry.SetIndexBuffer(indexData, sizeof(indexData) / sizeof(indexData[0]));

    Renderer renderer                        = Renderer::New(geometry, shader);
    renderer[Renderer::Property::BLEND_MODE] = BlendMode::ON;

    Actor actor                          = Actor::New();
    actor[Actor::Property::SIZE]         = Vector2(400.0f, 400.0f);
    actor[Actor::Property::POSITION]     = center;
    actor[Actor::Property::ANCHOR_POINT] = AnchorPoint::CENTER;
    actor[Actor::Property::COLOR]        = Color::YELLOW;
    actor.AddRenderer(renderer);

    Window window = mApplication.GetWindow();
    window.Add(actor);

    //Animation
    Animation animation = Animation::New(5.0f);
    KeyFrames k0        = KeyFrames::New();
    k0.Add(0.0f, v3);
    k0.Add(0.5f, v3 + Vector3(-150.0f, -150.0f, 0.0f));
    k0.Add(1.0f, v3);
    animation.AnimateBetween(Property(shader, "uPosition[1]"), k0, AlphaFunction::EASE_IN_OUT_SINE);

    k0 = KeyFrames::New();
    k0.Add(0.0f, v4);
    k0.Add(0.5f, v4 + Vector3(150.0f, -150.0f, 0.0f));
    k0.Add(1.0f, v4);
    animation.AnimateBetween(Property(shader, "uPosition[4]"), k0, AlphaFunction::EASE_IN_OUT_SINE);

    k0 = KeyFrames::New();
    k0.Add(0.0f, v5);
    k0.Add(0.5f, v5 + Vector3(0.0, 150.0f, 0.0f));
    k0.Add(1.0f, v5);
    animation.AnimateBetween(Property(shader, "uPosition[7]"), k0, AlphaFunction::EASE_IN_OUT_SINE);
    animation.SetLooping(true);
    animation.Play();
  }

  void CreateCircleMorph(Vector3 center, float radius)
  {
    Shader shader          = CreateShader(16);
    shader["uPosition[0]"] = Vector3(-radius, -radius, 0.0f);
    shader["uPosition[1]"] = Vector3(0.0f, -radius, 0.0f);
    shader["uPosition[2]"] = Vector3(radius, -radius, 0.0f);

    shader["uPosition[3]"] = Vector3(radius, -radius, 0.0f);
    shader["uPosition[4]"] = Vector3(radius, 0.0f, 0.0f);
    shader["uPosition[5]"] = Vector3(radius, radius, 0.0f);

    shader["uPosition[6]"] = Vector3(radius, radius, 0.0f);
    shader["uPosition[7]"] = Vector3(0.0f, radius, 0.0f);
    shader["uPosition[8]"] = Vector3(-radius, radius, 0.0f);

    shader["uPosition[9]"]  = Vector3(-radius, radius, 0.0f);
    shader["uPosition[10]"] = Vector3(-radius, 0.0f, 0.0f);
    shader["uPosition[11]"] = Vector3(-radius, -radius, 0.0f);

    shader["uPosition[12]"] = Vector3(-radius, -radius, 0.0f);
    shader["uPosition[13]"] = Vector3(radius, -radius, 0.0f);
    shader["uPosition[14]"] = Vector3(radius, radius, 0.0f);
    shader["uPosition[15]"] = Vector3(-radius, radius, 0.0f);

    //shader["uLineWidth"), 2.0] ;

    static const Vector3 vertexData[] = {Vector3(0.0f, 0.0f, 0.0f),
                                         Vector3(0.5f, 0.0f, 1.0f),
                                         Vector3(1.0f, 1.0f, 2.0f),
                                         Vector3(0.0f, 0.0f, 3.0f),
                                         Vector3(0.5f, 0.0f, 4.0f),
                                         Vector3(1.0f, 1.0f, 5.0f),
                                         Vector3(0.0f, 0.0f, 6.0f),
                                         Vector3(0.5f, 0.0f, 7.0f),
                                         Vector3(1.0f, 1.0f, 8.0f),
                                         Vector3(0.0f, 0.0f, 9.0f),
                                         Vector3(0.5f, 0.0f, 10.0f),
                                         Vector3(1.0f, 1.0f, 11.0f),
                                         Vector3(0.0f, 1.0f, 12.0f),
                                         Vector3(0.0f, 1.0f, 13.0f),
                                         Vector3(0.0f, 1.0f, 14.0f),
                                         Vector3(0.0f, 1.0f, 15.0f)};

    short unsigned int indexData[] = {0, 2, 1, 3, 5, 4, 6, 8, 7, 9, 11, 10, 12, 13, 14, 12, 14, 15};

    //Create a vertex buffer for vertex positions and texture coordinates
    Dali::Property::Map vertexFormat;
    vertexFormat["aCoefficient"]    = Dali::Property::VECTOR3;
    Dali::VertexBuffer vertexBuffer = Dali::VertexBuffer::New(vertexFormat);
    vertexBuffer.SetData(vertexData, sizeof(vertexData) / sizeof(vertexData[0]));

    //Create the geometry
    Dali::Geometry geometry = Dali::Geometry::New();
    geometry.AddVertexBuffer(vertexBuffer);
    geometry.SetIndexBuffer(indexData, sizeof(indexData) / sizeof(indexData[0]));

    Renderer renderer                        = Renderer::New(geometry, shader);
    renderer[Renderer::Property::BLEND_MODE] = BlendMode::ON;

    Actor actor                          = Actor::New();
    actor[Actor::Property::SIZE]         = Vector2(400.0f, 400.0f);
    actor[Actor::Property::POSITION]     = center;
    actor[Actor::Property::ANCHOR_POINT] = AnchorPoint::CENTER;
    actor.AddRenderer(renderer);

    Window window = mApplication.GetWindow();
    window.Add(actor);

    //Animation
    Animation animation = Animation::New(5.0f);
    KeyFrames k0        = KeyFrames::New();
    k0.Add(0.0f, Vector3(0.0f, -radius * 1.85, 0.0f));
    k0.Add(0.5f, Vector3(-radius * 1.85, -radius * 3.0f, 0.0f));
    k0.Add(1.0f, Vector3(0.0f, -radius * 1.85, 0.0f));
    animation.AnimateBetween(Property(shader, shader.GetPropertyIndex("uPosition[1]")), k0, AlphaFunction::EASE_IN_OUT_SINE);

    k0 = KeyFrames::New();
    k0.Add(0.0f, Vector3(radius * 1.85, 0.0f, 0.0f));
    k0.Add(0.5f, Vector3(radius * 3.0f, -radius * 1.85, 0.0f));
    k0.Add(1.0f, Vector3(radius * 1.85, 0.0f, 0.0f));
    animation.AnimateBetween(Property(shader, shader.GetPropertyIndex("uPosition[4]")), k0, AlphaFunction::EASE_IN_OUT_SINE);

    k0 = KeyFrames::New();
    k0.Add(0.0f, Vector3(0.0f, radius * 1.85, 0.0f));
    k0.Add(0.5f, Vector3(radius * 1.85, radius * 3.0f, 0.0f));
    k0.Add(1.0f, Vector3(0.0f, radius * 1.85, 0.0f));
    animation.AnimateBetween(Property(shader, shader.GetPropertyIndex("uPosition[7]")), k0, AlphaFunction::EASE_IN_OUT_SINE);

    k0 = KeyFrames::New();
    k0.Add(0.0f, Vector3(-radius * 1.85, 0.0f, 0.0f));
    k0.Add(0.5f, Vector3(-radius * 3.0f, radius * 1.85, 0.0f));
    k0.Add(1.0f, Vector3(-radius * 1.85, 0.0f, 0.0f));
    animation.AnimateBetween(Property(shader, shader.GetPropertyIndex("uPosition[10]")), k0, AlphaFunction::EASE_IN_OUT_SINE);

    animation.AnimateBy(Property(actor, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(-90.0f)), Vector3::ZAXIS));
    animation.SetLooping(true);
    animation.Play();
  }

  void CreateQuadMorph(Vector3 center, float radius)
  {
    Shader shader          = CreateShader(16);
    shader["uPosition[0]"] = Vector3(-radius, -radius, 0.0f);
    shader["uPosition[1]"] = Vector3(0.0f, -radius, 0.0f);
    shader["uPosition[2]"] = Vector3(radius, -radius, 0.0f);

    shader["uPosition[3]"] = Vector3(radius, -radius, 0.0f);
    shader["uPosition[4]"] = Vector3(radius, 0.0f, 0.0f);
    shader["uPosition[5]"] = Vector3(radius, radius, 0.0f);

    shader["uPosition[6]"] = Vector3(radius, radius, 0.0f);
    shader["uPosition[7]"] = Vector3(0.0f, radius, 0.0f);
    shader["uPosition[8]"] = Vector3(-radius, radius, 0.0f);

    shader["uPosition[9]"]  = Vector3(-radius, radius, 0.0f);
    shader["uPosition[10]"] = Vector3(-radius, 0.0f, 0.0f);
    shader["uPosition[11]"] = Vector3(-radius, -radius, 0.0f);

    shader["uPosition[12]"] = Vector3(-radius, -radius, 0.0f);
    shader["uPosition[13]"] = Vector3(radius, -radius, 0.0f);
    shader["uPosition[14]"] = Vector3(radius, radius, 0.0f);
    shader["uPosition[15]"] = Vector3(-radius, radius, 0.0f);

    static const Vector3 vertexData[] = {Dali::Vector3(0.0f, 0.0f, 0.0f),
                                         Dali::Vector3(0.5f, 0.0f, 1.0f),
                                         Dali::Vector3(1.0f, 1.0f, 2.0f),

                                         Dali::Vector3(0.0f, 0.0f, 3.0f),
                                         Dali::Vector3(0.5f, 0.0f, 4.0f),
                                         Dali::Vector3(1.0f, 1.0f, 5.0f),

                                         Dali::Vector3(0.0f, 0.0f, 6.0f),
                                         Dali::Vector3(0.5f, 0.0f, 7.0f),
                                         Dali::Vector3(1.0f, 1.0f, 8.0f),

                                         Dali::Vector3(0.0f, 0.0f, 9.0f),
                                         Dali::Vector3(0.5f, 0.0f, 10.0f),
                                         Dali::Vector3(1.0f, 1.0f, 11.0f),

                                         Dali::Vector3(0.0f, 1.0f, 12.0f),
                                         Dali::Vector3(0.0f, 1.0f, 13.0f),
                                         Dali::Vector3(0.0f, 1.0f, 14.0f),
                                         Dali::Vector3(0.0f, 1.0f, 15.0f)};

    short unsigned int indexData[] = {0, 2, 1, 3, 5, 4, 6, 8, 7, 9, 11, 10, 12, 15, 14, 12, 14, 13};

    //Create a vertex buffer for vertex positions and texture coordinates
    Dali::Property::Map vertexFormat;
    vertexFormat["aCoefficient"]    = Dali::Property::VECTOR3;
    Dali::VertexBuffer vertexBuffer = Dali::VertexBuffer::New(vertexFormat);
    vertexBuffer.SetData(vertexData, sizeof(vertexData) / sizeof(vertexData[0]));

    //Create the geometry
    Dali::Geometry geometry = Dali::Geometry::New();
    geometry.AddVertexBuffer(vertexBuffer);
    geometry.SetIndexBuffer(indexData, sizeof(indexData) / sizeof(indexData[0]));

    Renderer renderer                        = Renderer::New(geometry, shader);
    renderer[Renderer::Property::BLEND_MODE] = BlendMode::ON;

    Actor actor                          = Actor::New();
    actor[Actor::Property::SIZE]         = Vector2(400.0f, 400.0f);
    actor[Actor::Property::POSITION]     = center;
    actor[Actor::Property::ANCHOR_POINT] = AnchorPoint::CENTER;
    actor[Actor::Property::COLOR]        = Color::RED;
    actor.AddRenderer(renderer);

    Window window = mApplication.GetWindow();
    window.Add(actor);

    //Animation
    Animation animation = Animation::New(5.0f);
    KeyFrames k0        = KeyFrames::New();
    k0.Add(0.0f, Vector3(0.0f, -radius, 0.0f));
    k0.Add(0.5f, Vector3(0.0f, -radius * 4.0f, 0.0f));
    k0.Add(1.0f, Vector3(0.0f, -radius, 0.0f));
    animation.AnimateBetween(Property(shader, shader.GetPropertyIndex("uPosition[1]")), k0, AlphaFunction::EASE_IN_OUT_SINE);

    k0 = KeyFrames::New();
    k0.Add(0.0f, Vector3(radius, 0.0f, 0.0f));
    k0.Add(0.5f, Vector3(radius * 4.0f, 0.0f, 0.0f));
    k0.Add(1.0f, Vector3(radius, 0.0f, 0.0f));
    animation.AnimateBetween(Property(shader, shader.GetPropertyIndex("uPosition[4]")), k0, AlphaFunction::EASE_IN_OUT_SINE);

    k0 = KeyFrames::New();
    k0.Add(0.0f, Vector3(0.0f, radius, 0.0f));
    k0.Add(0.5f, Vector3(0.0f, radius * 4.0f, 0.0f));
    k0.Add(1.0f, Vector3(0.0f, radius, 0.0f));
    animation.AnimateBetween(Property(shader, shader.GetPropertyIndex("uPosition[7]")), k0, AlphaFunction::EASE_IN_OUT_SINE);

    k0 = KeyFrames::New();
    k0.Add(0.0f, Vector3(-radius, 0.0f, 0.0f));
    k0.Add(0.5f, Vector3(-radius * 4.0f, 0.0f, 0.0f));
    k0.Add(1.0f, Vector3(-radius, 0.0f, 0.0f));
    animation.AnimateBetween(Property(shader, shader.GetPropertyIndex("uPosition[10]")), k0, AlphaFunction::EASE_IN_OUT_SINE);

    animation.AnimateBy(Property(actor, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(90.0f)), Vector3::ZAXIS));
    animation.SetLooping(true);
    animation.Play();
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN && (IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK)))
    {
      mApplication.Quit();
    }
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application           application = Application::New(&argc, &argv);
  AnimatedShapesExample test(application);
  application.MainLoop();
  return 0;
}
