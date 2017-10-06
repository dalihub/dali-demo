/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include "shared/view.h"

#include <sstream>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* APPLICATION_TITLE("Animated Shapes");

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER
(
  attribute mediump vec3 aCoefficient;
  uniform mediump mat4 uMvpMatrix;
  uniform mediump vec3 uPosition[MAX_POINT_COUNT];
  varying lowp vec2 vCoefficient;
  void main()
  {
    int vertexId = int(aCoefficient.z);
    gl_Position = uMvpMatrix * vec4(uPosition[vertexId], 1.0);

    vCoefficient = aCoefficient.xy;
  }
);

// Fragment shader.
const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER
(
  uniform lowp vec4 uColor;
  varying lowp vec2 vCoefficient;
  void main()
  {
    lowp float C = (vCoefficient.x*vCoefficient.x-vCoefficient.y);
    lowp float Cdx = dFdx(C);
    lowp float Cdy = dFdy(C);

    lowp float distance = float(C / sqrt(Cdx*Cdx + Cdy*Cdy));
    lowp float alpha = 0.5 - distance;
    gl_FragColor = vec4( uColor.rgb, uColor.a * alpha );
  }
);

Shader CreateShader( unsigned int pointCount )
{
  std::ostringstream vertexShader;
  vertexShader << "#define MAX_POINT_COUNT "<< pointCount << "\n"<<VERTEX_SHADER;

  std::ostringstream fragmentShader;
  fragmentShader << "#extension GL_OES_standard_derivatives : enable "<< "\n"<<FRAGMENT_SHADER;

  Shader shader = Shader::New( vertexShader.str(), fragmentShader.str() );
  for( unsigned int i(0); i<pointCount; ++i )
  {
    std::ostringstream propertyName;
    propertyName << "uPosition["<<i<<"]";
    shader.RegisterProperty(propertyName.str(),Vector3(0.0f,0.0f,0.0f) );
  }

  return shader;
}

} //unnamed namespace

// This example shows resolution independent rendering and animation of curves using the gpu.
//
class AnimatedShapesExample : public ConnectionTracker
{
public:

  AnimatedShapesExample( Application& application )
: mApplication( application )
{
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &AnimatedShapesExample::Create );
}

  ~AnimatedShapesExample()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    Stage stage = Stage::GetCurrent();

    // Creates the background gradient
    Toolkit::Control background = Dali::Toolkit::Control::New();
    background.SetAnchorPoint( Dali::AnchorPoint::CENTER );
    background.SetParentOrigin( Dali::ParentOrigin::CENTER );
    background.SetResizePolicy( Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::ALL_DIMENSIONS );
    Dali::Property::Map map;
    map.Insert( Toolkit::Visual::Property::TYPE,  Visual::GRADIENT );
    Property::Array stopOffsets;
    stopOffsets.PushBack( 0.0f );
    stopOffsets.PushBack( 1.0f );
    map.Insert( GradientVisual::Property::STOP_OFFSET, stopOffsets );
    Property::Array stopColors;
    stopColors.PushBack( Vector4( 0.0f,0.0f,1.0f,1.0f ) );
    stopColors.PushBack( Vector4( 1.0f,1.0f,1.0f,1.0f ) );
    map.Insert( GradientVisual::Property::STOP_COLOR, stopColors );
    Vector2 halfStageSize = Stage::GetCurrent().GetSize()*0.5f;
    map.Insert( GradientVisual::Property::START_POSITION,  Vector2(0.0f,-halfStageSize.y) );
    map.Insert( GradientVisual::Property::END_POSITION,  Vector2(0.0f,halfStageSize.y)  );
    map.Insert( GradientVisual::Property::UNITS, GradientVisual::Units::USER_SPACE );
    background.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
    stage.Add( background );

    // Create a TextLabel for the application title.
    Toolkit::TextLabel label = Toolkit::TextLabel::New( APPLICATION_TITLE );
    label.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    label.SetParentOrigin( Vector3( 0.5f, 0.0f, 0.5f ) );
    label.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    label.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    label.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
    stage.Add( label );

    CreateTriangleMorph(Vector3( stage.GetSize().x*0.5f, stage.GetSize().y*0.15f, 0.0f), 100.0f );
    CreateCircleMorph( Vector3( stage.GetSize().x*0.5f, stage.GetSize().y*0.5f, 0.0f), 55.0f );
    CreateQuadMorph( Vector3( stage.GetSize().x*0.5f, stage.GetSize().y*0.85f, 0.0f), 60.0f );

    stage.KeyEventSignal().Connect( this, &AnimatedShapesExample::OnKeyEvent );
  }

  void CreateTriangleMorph( Vector3 center, float side )
  {
    float h = ( side *0.5f ) / 0.866f;

    Vector3 v0 = Vector3( -h, h, 0.0f );
    Vector3 v1 = Vector3( 0.0f, -side * 0.366f, 0.0f );
    Vector3 v2 = Vector3( h, h, 0.0f );

    Vector3 v3 = v0 + ( ( v1 - v0 ) * 0.5f );
    Vector3 v4 = v1 + ( ( v2 - v1 ) * 0.5f );
    Vector3 v5 = v2 + ( ( v0 - v2 ) * 0.5f );

    Shader shader = CreateShader( 12 );
    shader.SetProperty( shader.GetPropertyIndex( "uPosition[0]"), v0 );
    shader.SetProperty( shader.GetPropertyIndex( "uPosition[1]"), v3 );
    shader.SetProperty( shader.GetPropertyIndex( "uPosition[2]"), v1 );

    shader.SetProperty( shader.GetPropertyIndex( "uPosition[3]"), v1 );
    shader.SetProperty( shader.GetPropertyIndex( "uPosition[4]"), v4 );
    shader.SetProperty( shader.GetPropertyIndex( "uPosition[5]"), v2 );

    shader.SetProperty( shader.GetPropertyIndex( "uPosition[6]"), v2 );
    shader.SetProperty( shader.GetPropertyIndex( "uPosition[7]"), v5 );
    shader.SetProperty( shader.GetPropertyIndex( "uPosition[8]"), v0 );

    shader.SetProperty( shader.GetPropertyIndex( "uPosition[9]"),  v0 );
    shader.SetProperty( shader.GetPropertyIndex( "uPosition[10]"), v1 );
    shader.SetProperty( shader.GetPropertyIndex( "uPosition[11]"), v2 );


    //Create geometry
    static const Vector3 vertexData[] = { Dali::Vector3( 0.0f, 0.0f, 0.0f ),
                                          Dali::Vector3( 0.5f, 0.0f, 1.0f ),
                                          Dali::Vector3( 1.0f, 1.0f, 2.0f ),

                                          Dali::Vector3( 0.0f, 0.0f, 3.0f ),
                                          Dali::Vector3( 0.5f, 0.0f, 4.0f ),
                                          Dali::Vector3( 1.0f, 1.0f, 5.0f ),

                                          Dali::Vector3( 0.0f, 0.0f, 6.0f ),
                                          Dali::Vector3( 0.5f, 0.0f, 7.0f ),
                                          Dali::Vector3( 1.0f, 1.0f, 8.0f ),

                                          Dali::Vector3( 0.0f, 1.0f, 9.0f ),
                                          Dali::Vector3( 0.0f, 1.0f, 10.0f ),
                                          Dali::Vector3( 0.0f, 1.0f, 11.0f )
    };

    unsigned short indexData[] = { 0, 2, 1, 3, 5, 4, 6, 8, 7, 9, 11, 10 };

    //Create a vertex buffer for vertex positions and texture coordinates
    Dali::Property::Map vertexFormat;
    vertexFormat["aCoefficient"] = Dali::Property::VECTOR3;
    Dali::PropertyBuffer vertexBuffer = Dali::PropertyBuffer::New( vertexFormat );
    vertexBuffer.SetData( vertexData, sizeof(vertexData)/sizeof(vertexData[0]));

    //Create the geometry
    Dali::Geometry geometry = Dali::Geometry::New();
    geometry.AddVertexBuffer( vertexBuffer );
    geometry.SetIndexBuffer( indexData, sizeof(indexData)/sizeof(indexData[0]) );

    Renderer renderer = Renderer::New( geometry, shader );
    renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );

    Actor actor = Actor::New();
    actor.SetSize( 400.0f, 400.0f );
    actor.SetPosition( center );
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetColor(Vector4(1.0f,1.0f,0.0f,1.0f) );
    actor.AddRenderer( renderer );

    Stage stage = Stage::GetCurrent();
    stage.Add( actor );

    //Animation
    Animation animation = Animation::New(5.0f);
    KeyFrames k0 = KeyFrames::New();
    k0.Add( 0.0f,v3  );
    k0.Add( 0.5f, v3 + Vector3(-150.0f,-150.0f,0.0f));
    k0.Add( 1.0f, v3 );
    animation.AnimateBetween( Property(shader, "uPosition[1]"),k0, AlphaFunction::EASE_IN_OUT_SINE );

    k0 = KeyFrames::New();
    k0.Add( 0.0f,v4  );
    k0.Add( 0.5f, v4 + Vector3(150.0f,-150.0f,0.0f));
    k0.Add( 1.0f, v4 );
    animation.AnimateBetween( Property(shader,"uPosition[4]"),k0, AlphaFunction::EASE_IN_OUT_SINE );

    k0 = KeyFrames::New();
    k0.Add( 0.0f,v5  );
    k0.Add( 0.5f, v5 + Vector3(0.0,150.0f,0.0f));
    k0.Add( 1.0f, v5 );
    animation.AnimateBetween( Property(shader, "uPosition[7]"),k0, AlphaFunction::EASE_IN_OUT_SINE );
    animation.SetLooping( true );
    animation.Play();
  }

  void CreateCircleMorph( Vector3 center, float radius )
  {
    Shader shader = CreateShader( 16 );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[0]"), Vector3( -radius, -radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[1]"), Vector3( 0.0f, -radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[2]"), Vector3( radius, -radius, 0.0f ) );

    shader.SetProperty( shader.GetPropertyIndex("uPosition[3]"), Vector3( radius, -radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[4]"), Vector3( radius, 0.0f, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[5]"), Vector3( radius, radius, 0.0f ) );

    shader.SetProperty( shader.GetPropertyIndex("uPosition[6]"), Vector3( radius, radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[7]"), Vector3( 0.0f, radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[8]"), Vector3( -radius, radius, 0.0f ) );

    shader.SetProperty( shader.GetPropertyIndex("uPosition[9]"),  Vector3( -radius, radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[10]"), Vector3( -radius, 0.0f, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[11]"), Vector3( -radius, -radius, 0.0f ) );

    shader.SetProperty( shader.GetPropertyIndex("uPosition[12]"), Vector3( -radius, -radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[13]"),  Vector3( radius, -radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[14]"), Vector3( radius, radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[15]"), Vector3( -radius, radius, 0.0f ) );

    //shader.SetProperty( shader.GetPropertyIndex("uLineWidth"), 2.0f );

    static const Vector3 vertexData[] = { Vector3( 0.0f, 0.0f, 0.0f ),
                                          Vector3( 0.5f, 0.0f, 1.0f ),
                                          Vector3( 1.0f, 1.0f, 2.0f ),
                                          Vector3( 0.0f, 0.0f, 3.0f ),
                                          Vector3( 0.5f, 0.0f, 4.0f ),
                                          Vector3( 1.0f, 1.0f, 5.0f ),
                                          Vector3( 0.0f, 0.0f, 6.0f ),
                                          Vector3( 0.5f, 0.0f, 7.0f ),
                                          Vector3( 1.0f, 1.0f, 8.0f ),
                                          Vector3( 0.0f, 0.0f, 9.0f ),
                                          Vector3( 0.5f, 0.0f, 10.0f ),
                                          Vector3( 1.0f, 1.0f, 11.0f ),
                                          Vector3( 0.0f, 1.0f, 12.0f ),
                                          Vector3( 0.0f, 1.0f, 13.0f ),
                                          Vector3( 0.0f, 1.0f, 14.0f ),
                                          Vector3( 0.0f, 1.0f, 15.0f )};

    short unsigned int indexData[] = { 0, 2, 1, 3, 5, 4, 6, 8, 7, 9, 11, 10, 12, 13, 14, 12, 14, 15 };

    //Create a vertex buffer for vertex positions and texture coordinates
    Dali::Property::Map vertexFormat;
    vertexFormat["aCoefficient"] = Dali::Property::VECTOR3;
    Dali::PropertyBuffer vertexBuffer = Dali::PropertyBuffer::New( vertexFormat );
    vertexBuffer.SetData( vertexData, sizeof(vertexData)/sizeof(vertexData[0]));

    //Create the geometry
    Dali::Geometry geometry = Dali::Geometry::New();
    geometry.AddVertexBuffer( vertexBuffer );
    geometry.SetIndexBuffer( indexData, sizeof(indexData)/sizeof(indexData[0]) );

    Renderer renderer = Renderer::New( geometry, shader );
    renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );

    Actor actor = Actor::New();
    actor.SetSize( 400.0f, 400.0f );
    actor.SetPosition( center );
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.AddRenderer( renderer );

    Stage stage = Stage::GetCurrent();
    stage.Add( actor );

    //Animation
    Animation animation = Animation::New(5.0f);
    KeyFrames k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( 0.0f,-radius*1.85, 0.0f ) );
    k0.Add( 0.5f, Vector3( -radius*1.85, -radius*3.0f, 0.0f ) );
    k0.Add( 1.0f, Vector3( 0.0f,-radius*1.85, 0.0f ) );
    animation.AnimateBetween( Property( shader, shader.GetPropertyIndex("uPosition[1]") ),k0, AlphaFunction::EASE_IN_OUT_SINE );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( radius*1.85, 0.0f, 0.0f ) );
    k0.Add( 0.5f, Vector3( radius*3.0f,-radius*1.85, 0.0f ) );
    k0.Add( 1.0f, Vector3( radius*1.85,0.0f, 0.0f ) );
    animation.AnimateBetween( Property(shader, shader.GetPropertyIndex("uPosition[4]")),k0, AlphaFunction::EASE_IN_OUT_SINE );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( 0.0f, radius*1.85, 0.0f ) );
    k0.Add( 0.5f, Vector3( radius*1.85, radius*3.0f, 0.0f) );
    k0.Add( 1.0f, Vector3( 0.0f, radius*1.85, 0.0f) );
    animation.AnimateBetween( Property( shader, shader.GetPropertyIndex("uPosition[7]") ),k0, AlphaFunction::EASE_IN_OUT_SINE );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( -radius*1.85, 0.0f, 0.0f) );
    k0.Add( 0.5f, Vector3(-radius*3.0f, radius*1.85, 0.0f) );
    k0.Add( 1.0f, Vector3( -radius*1.85, 0.0f, 0.0f) );
    animation.AnimateBetween( Property( shader, shader.GetPropertyIndex("uPosition[10]") ),k0, AlphaFunction::EASE_IN_OUT_SINE );

    animation.AnimateBy( Property( actor, Actor::Property::ORIENTATION ), Quaternion( Radian( Degree(-90.0f) ), Vector3::ZAXIS ) );
    animation.SetLooping( true );
    animation.Play();
  }

  void CreateQuadMorph( Vector3 center, float radius )
  {
    Shader shader = CreateShader( 16 );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[0]"), Vector3( -radius, -radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[1]"), Vector3( 0.0f, -radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[2]"), Vector3( radius, -radius, 0.0f ) );

    shader.SetProperty( shader.GetPropertyIndex("uPosition[3]"), Vector3( radius, -radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[4]"), Vector3( radius, 0.0f, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[5]"), Vector3( radius, radius, 0.0f ) );

    shader.SetProperty( shader.GetPropertyIndex("uPosition[6]"), Vector3( radius, radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[7]"), Vector3( 0.0f, radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[8]"), Vector3( -radius, radius, 0.0f ) );

    shader.SetProperty( shader.GetPropertyIndex("uPosition[9]"), Vector3( -radius, radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[10]"), Vector3( -radius, 0.0f, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[11]"), Vector3( -radius, -radius, 0.0f ) );

    shader.SetProperty( shader.GetPropertyIndex("uPosition[12]"), Vector3( -radius, -radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[13]"), Vector3( radius, -radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[14]"), Vector3( radius, radius, 0.0f ) );
    shader.SetProperty( shader.GetPropertyIndex("uPosition[15]"), Vector3( -radius, radius, 0.0f ) );

    static const Vector3 vertexData[] = { Dali::Vector3( 0.0f, 0.0f, 0.0f ),
                                          Dali::Vector3( 0.5f, 0.0f, 1.0f ),
                                          Dali::Vector3( 1.0f, 1.0f, 2.0f ),

                                          Dali::Vector3( 0.0f, 0.0f, 3.0f ),
                                          Dali::Vector3( 0.5f, 0.0f, 4.0f ),
                                          Dali::Vector3( 1.0f, 1.0f, 5.0f ),

                                          Dali::Vector3( 0.0f, 0.0f, 6.0f ),
                                          Dali::Vector3( 0.5f, 0.0f, 7.0f ),
                                          Dali::Vector3( 1.0f, 1.0f, 8.0f ),

                                          Dali::Vector3( 0.0f, 0.0f, 9.0f ),
                                          Dali::Vector3( 0.5f, 0.0f, 10.0f ),
                                          Dali::Vector3( 1.0f, 1.0f, 11.0f ),

                                          Dali::Vector3( 0.0f, 1.0f, 12.0f ),
                                          Dali::Vector3( 0.0f, 1.0f, 13.0f ),
                                          Dali::Vector3( 0.0f, 1.0f, 14.0f ),
                                          Dali::Vector3( 0.0f, 1.0f, 15.0f ) };

    short unsigned int indexData[] = { 0, 2, 1, 3, 5, 4, 6, 8, 7, 9, 11, 10, 12, 15, 14, 12, 14, 13 };

    //Create a vertex buffer for vertex positions and texture coordinates
    Dali::Property::Map vertexFormat;
    vertexFormat["aCoefficient"] = Dali::Property::VECTOR3;
    Dali::PropertyBuffer vertexBuffer = Dali::PropertyBuffer::New( vertexFormat );
    vertexBuffer.SetData( vertexData, sizeof(vertexData)/sizeof(vertexData[0]));

    //Create the geometry
    Dali::Geometry geometry = Dali::Geometry::New();
    geometry.AddVertexBuffer( vertexBuffer );
    geometry.SetIndexBuffer( indexData, sizeof(indexData)/sizeof(indexData[0]) );

    Renderer renderer = Renderer::New( geometry, shader );
    renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );

    Actor actor = Actor::New();
    actor.SetSize( 400.0f, 400.0f );
    actor.SetPosition( center );
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetColor(Vector4(1.0f,0.0f,0.0f,1.0f) );
    actor.AddRenderer( renderer );

    Stage stage = Stage::GetCurrent();
    stage.Add( actor );

    //Animation
    Animation animation = Animation::New( 5.0f );
    KeyFrames k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( 0.0f, -radius, 0.0f ) );
    k0.Add( 0.5f, Vector3( 0.0f, -radius*4.0f, 0.0f ) );
    k0.Add( 1.0f, Vector3( 0.0f, -radius, 0.0f ) );
    animation.AnimateBetween( Property(shader, shader.GetPropertyIndex("uPosition[1]")),k0, AlphaFunction::EASE_IN_OUT_SINE );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( radius, 0.0f, 0.0f ) );
    k0.Add( 0.5f, Vector3( radius*4.0f, 0.0f, 0.0f ) );
    k0.Add( 1.0f, Vector3( radius, 0.0f, 0.0f ) );
    animation.AnimateBetween( Property(shader, shader.GetPropertyIndex("uPosition[4]")),k0, AlphaFunction::EASE_IN_OUT_SINE );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( 0.0f, radius, 0.0f ) );
    k0.Add( 0.5f, Vector3( 0.0f, radius*4.0f, 0.0f ) );
    k0.Add( 1.0f, Vector3( 0.0f, radius, 0.0f ) );
    animation.AnimateBetween( Property(shader, shader.GetPropertyIndex("uPosition[7]")),k0, AlphaFunction::EASE_IN_OUT_SINE );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( -radius, 0.0f, 0.0f ) );
    k0.Add( 0.5f, Vector3( -radius*4.0f,0.0f, 0.0f ) );
    k0.Add( 1.0f, Vector3( -radius,  0.0f, 0.0f ) );
    animation.AnimateBetween( Property(shader, shader.GetPropertyIndex("uPosition[10]")),k0, AlphaFunction::EASE_IN_OUT_SINE );

    animation.AnimateBy( Property( actor, Actor::Property::ORIENTATION ), Quaternion( Radian( Degree(90.0f) ), Vector3::ZAXIS ) );
    animation.SetLooping( true );
    animation.Play();
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if( event.state == KeyEvent::Down && (IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ))  )
    {
      mApplication.Quit();
    }
  }

private:
  Application&                mApplication;

};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  AnimatedShapesExample test( application );
  application.MainLoop();
  return 0;
}
