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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

#include "shared/view.h"

using namespace Dali;

namespace
{
const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-gradient.jpg" );

}

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
    DemoHelper::RequestThemeChange();

    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    //Create a view
    mView = Dali::Toolkit::Control::New();
    mView.SetAnchorPoint( Dali::AnchorPoint::CENTER );
    mView.SetParentOrigin( Dali::ParentOrigin::CENTER );
    mView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    stage.Add( mView );

    //Set background image for the view
    mView.SetBackgroundImage( ResourceImage::New( BACKGROUND_IMAGE ) );

    CreateTriangleMorph(Vector3( stage.GetSize().x*0.5f,stage.GetSize().y*0.15f,0.0f), 100.0f );
    CreateCircleMorph( Vector3( stage.GetSize().x*0.5f,stage.GetSize().y*0.85f,0.0f), 60.0f );
    CreatePathMorph( Vector3( stage.GetSize().x*0.5f,stage.GetSize().y*0.5f,0.0f), 55.0f );


    stage.KeyEventSignal().Connect(this, &AnimatedShapesExample::OnKeyEvent);
  }

  void CreateCircleMorph( Vector3 center, float radius )
  {
    Toolkit::QuadraticBezier shader = Toolkit::QuadraticBezier::New(16, true);

    shader.SetPoint(0, Vector3(-radius,-radius,0.0f));
    shader.SetPoint(1, Vector3( 0.0f,-radius,0.0f));
    shader.SetPoint(2, Vector3(radius,-radius,0.0f));

    shader.SetPoint(3, Vector3(radius,-radius,0.0f));
    shader.SetPoint(4, Vector3( radius,0.0f,0.0f));
    shader.SetPoint(5, Vector3(radius,radius,0.0f));

    shader.SetPoint(6, Vector3(radius,radius,0.0f));
    shader.SetPoint(7, Vector3( 0.0f,radius,0.0f));
    shader.SetPoint(8, Vector3( -radius,radius,0.0f));

    shader.SetPoint(9,  Vector3( -radius,radius,0.0f));
    shader.SetPoint(10, Vector3( -radius,0.0f,0.0f));
    shader.SetPoint(11, Vector3(-radius,-radius,0.0f));

    shader.SetPoint(12, Vector3(-radius,-radius,0.0f));
    shader.SetPoint(13, Vector3(radius,-radius,0.0f));
    shader.SetPoint(14, Vector3(radius,radius,0.0f));
    shader.SetPoint(15, Vector3( -radius,radius,0.0f));

    shader.SetColor(Vector4(1.0f,0.0f,0.0f,1.0f) );
    shader.SetLineWidth(2.0f);

    ////Generate the mesh
    Dali::MeshData::VertexContainer vertices;
    for( unsigned int i(0); i<12; i+=3 )
    {
      vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,0.0f,i)  ));
      vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.5f,0.0f,i+1)));
      vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(1.0f,1.0f,i+2)));
    }
    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,1.0f,12)  ));
    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,1.0f,13)));
    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,1.0f,14)));
    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,1.0f,15)));

    short unsigned int indexArray[] = { 0,2,1, 3,5,4,6,8,7, 9, 11, 10, 12,15,14,12,14,13};
    Dali::MeshData::FaceIndices index( indexArray, indexArray + sizeof(indexArray)/sizeof(short unsigned int) );

    //Material
    Dali::Material material = Material::New("Material");
    material.SetDiffuseColor( Vector4(1.0f,1.0f,1.0f,1.0f));

    //Create the Mesh object
    Dali::MeshData data;
    data.SetVertices(vertices);
    data.SetFaceIndices( index );
    data.SetMaterial( material );
    data.SetHasNormals( true );
    Mesh mesh = Mesh::New( data );

    //Create the mesh actor
    MeshActor meshActor = MeshActor::New(mesh);
    meshActor.SetAnchorPoint( AnchorPoint::CENTER );
    meshActor.SetShaderEffect(shader);
    meshActor.SetPosition( center );
    meshActor.SetBlendMode(BlendingMode::ON );
    mView.Add( meshActor );


    //Animation
    Animation animation = Animation::New(5.0f);
    KeyFrames k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( 0.0f,-radius, 0.0f) );
    k0.Add( 0.5f, Vector3(0.0f, -radius*4.0f, 0.0f));
    k0.Add( 1.0f, Vector3( 0.0f,-radius, 0.0f) );
    animation.AnimateBetween( Property(shader, shader.GetPointPropertyName(1)),k0,AlphaFunction::EASE_IN_OUT );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( radius, 0.0f, 0.0f) );
    k0.Add( 0.5f, Vector3(radius*4.0f,0.0f, 0.0f));
    k0.Add( 1.0f, Vector3( radius,0.0f, 0.0f));
    animation.AnimateBetween( Property(shader, shader.GetPointPropertyName(4)),k0,AlphaFunction::EASE_IN_OUT );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3(0.0f,radius, 0.0f) );
    k0.Add( 0.5f, Vector3(0.0f,radius*4.0f, 0.0f));
    k0.Add( 1.0f, Vector3(0.0f,radius, 0.0f) );
    animation.AnimateBetween( Property(shader, shader.GetPointPropertyName(7)),k0,AlphaFunction::EASE_IN_OUT );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( -radius,  0.0f, 0.0f) );
    k0.Add( 0.5f, Vector3(-radius*4.0f,0.0f, 0.0f));
    k0.Add( 1.0f, Vector3( -radius,  0.0f, 0.0f) );
    animation.AnimateBetween( Property(shader, shader.GetPointPropertyName(10)),k0,AlphaFunction::EASE_IN_OUT );

    animation.AnimateBy( Property( meshActor, Actor::Property::ORIENTATION ), Quaternion( Radian( Degree(90.0f) ), Vector3::ZAXIS ) );
    animation.SetLooping( true );
    animation.Play();
  }

  void CreateTriangleMorph( Vector3 center, float side )
  {
    float h = (side *0.5f)/0.866f;

    Vector3 v0 = Vector3(-h,h,0.0f);
    Vector3 v1 = Vector3(0.0f,-(side*0.366f),0.0f );
    Vector3 v2 = Vector3(h,h,0.0f);

    Vector3 v3 = v0 + ((v1-v0) * 0.5f);
    Vector3 v4 = v1 + ((v2-v1) * 0.5f);
    Vector3 v5 = v2 + ((v0-v2) * 0.5f);

    Toolkit::QuadraticBezier shader = Toolkit::QuadraticBezier::New(12, true);

    shader.SetPoint(0,v0);
    shader.SetPoint(1,v3);
    shader.SetPoint(2,v1);

    shader.SetPoint(3,v1);
    shader.SetPoint(4,v4);
    shader.SetPoint(5,v2);

    shader.SetPoint(6,v2);
    shader.SetPoint(7,v5);
    shader.SetPoint(8,v0);

    shader.SetPoint(9, v0);
    shader.SetPoint(10,v1);
    shader.SetPoint(11,v2);

    shader.SetColor(Vector4(0.0f,1.0f,0.0f,1.0f));
    shader.SetLineWidth(2.0f);

    ////Generate the mesh
    Dali::MeshData::VertexContainer vertices;
    for( unsigned int i(0);i<9;i+=3 )
    {
      vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,0.0f,i)) );
      vertices.push_back(  MeshData::Vertex( Vector3::ZERO, Vector2::ZERO,Vector3(0.5f,0.0f,i+1) ) );
      vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(1.0f,1.0f,i+2)  ) );
    }

    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,1.0f,9)) );
    vertices.push_back(  MeshData::Vertex( Vector3::ZERO, Vector2::ZERO,Vector3(0.0f,1.0f,10) ) );
    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,1.0f,11)  ) );

    short unsigned int indexArray[] = { 0,2,1,3,5,4,6,8,7,9,11,10 };
    Dali::MeshData::FaceIndices index( indexArray, indexArray + sizeof(indexArray)/sizeof(short unsigned int) );

    //Material
    Dali::Material material = Material::New("Material");
    material.SetDiffuseColor( Vector4(1.0f,1.0f,1.0f,1.0f));

    //Create the Mesh object
    Dali::MeshData data;
    data.SetVertices(vertices);
    data.SetFaceIndices( index );
    data.SetMaterial( material );
    data.SetHasNormals( true );
    Mesh mesh = Mesh::New( data );

//    //Create the mesh actor
    MeshActor meshActor = MeshActor::New(mesh);
    meshActor.SetAnchorPoint( AnchorPoint::CENTER );
    meshActor.SetShaderEffect(shader);
    meshActor.SetPosition( center );
    meshActor.SetBlendMode(BlendingMode::ON );
    mView.Add( meshActor );

    //Animation
    Animation animation = Animation::New(5.0f);

    KeyFrames k0 = KeyFrames::New();
    k0.Add( 0.0f,v3  );
    k0.Add( 0.5f, v3 + Vector3(-200.0f,-200.0f,0.0f));
    k0.Add( 1.0f, v3 );
    animation.AnimateBetween( Property(shader, shader.GetPointPropertyName(1)),k0,AlphaFunction::EASE_IN_OUT );

    k0 = KeyFrames::New();
    k0.Add( 0.0f,v4  );
    k0.Add( 0.5f, v4 + Vector3(200.0f,-200.0f,0.0f));
    k0.Add( 1.0f, v4 );
    animation.AnimateBetween( Property(shader, shader.GetPointPropertyName(4)),k0,AlphaFunction::EASE_IN_OUT );

    k0 = KeyFrames::New();
    k0.Add( 0.0f,v5  );
    k0.Add( 0.5f, v5 + Vector3(0.0,200.0f,0.0f));
    k0.Add( 1.0f, v5 );
    animation.AnimateBetween( Property(shader, shader.GetPointPropertyName(7)),k0,AlphaFunction::EASE_IN_OUT );
    animation.SetLooping( true );
    animation.Play();
  }

  void CreatePathMorph( Vector3 center, float radius )
  {
    Toolkit::QuadraticBezier shader = Toolkit::QuadraticBezier::New(12, false);

    shader.SetPoint(0, Vector3(-radius,-radius,0.0f));
    shader.SetPoint(1, Vector3( 0.0f,-radius,0.0f));
    shader.SetPoint(2, Vector3(radius,-radius,0.0f));

    shader.SetPoint(3, Vector3(radius,-radius,0.0f));
    shader.SetPoint(4, Vector3( radius,0.0f,0.0f));
    shader.SetPoint(5, Vector3(radius,radius,0.0f));

    shader.SetPoint(6, Vector3(radius,radius,0.0f));
    shader.SetPoint(7, Vector3( 0.0f,radius,0.0f));
    shader.SetPoint(8, Vector3( -radius,radius,0.0f));

    shader.SetPoint(9,  Vector3( -radius,radius,0.0f));
    shader.SetPoint(10, Vector3( -radius,0.0f,0.0f));
    shader.SetPoint(11, Vector3(-radius,-radius,0.0f));

    shader.SetColor(Vector4(1.0f,1.0f,0.0f,1.0f) );
    shader.SetLineWidth(1.5f);

    ////Generate the mesh/S
    Dali::MeshData::VertexContainer vertices;
    for( unsigned int i(0); i<12; i+=3 )
    {
      vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,0.0f,i)  ));
      vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.5f,0.0f,i+1)));
      vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(1.0f,1.0f,i+2)));
    }


    short unsigned int indexArray[] = { 0,2,1, 3,5,4,6,8,7, 9, 11, 10 };
    Dali::MeshData::FaceIndices index( indexArray, indexArray + sizeof(indexArray)/sizeof(short unsigned int) );

    //Material
    Dali::Material material = Material::New("Material");
    material.SetDiffuseColor( Vector4(1.0f,1.0f,1.0f,1.0f));

    //Create the Mesh object
    Dali::MeshData data;
    data.SetVertices(vertices);
    data.SetFaceIndices( index );
    data.SetMaterial( material );
    data.SetHasNormals( true );
    Mesh mesh = Mesh::New( data );

    //Create the mesh actor
    MeshActor meshActor = MeshActor::New(mesh);
    meshActor.SetAnchorPoint( AnchorPoint::CENTER );
    meshActor.SetShaderEffect(shader);
    meshActor.SetPosition( center );
    meshActor.SetBlendMode(BlendingMode::ON );
    mView.Add( meshActor );


    //Animation
    Animation animation = Animation::New(5.0f);
    KeyFrames k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( 0.0f,-radius*2.0, 0.0f) );
    k0.Add( 0.5f, Vector3(-radius*2.0, -radius*3.0f, 0.0f));
    k0.Add( 1.0f, Vector3( 0.0f,-radius*2.0, 0.0f) );
    animation.AnimateBetween( Property(shader, shader.GetPointPropertyName(1)),k0,AlphaFunction::EASE_IN_OUT );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( radius*2.0, 0.0f, 0.0f) );
    k0.Add( 0.5f, Vector3(radius*3.0f,-radius*2.0, 0.0f));
    k0.Add( 1.0f, Vector3( radius*2.0,0.0f, 0.0f));
    animation.AnimateBetween( Property(shader, shader.GetPointPropertyName(4)),k0,AlphaFunction::EASE_IN_OUT );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3(0.0f,radius*2.0, 0.0f) );
    k0.Add( 0.5f, Vector3(radius*2.0,radius*3.0f, 0.0f));
    k0.Add( 1.0f, Vector3(0.0f,radius*2.0, 0.0f) );
    animation.AnimateBetween( Property(shader, shader.GetPointPropertyName(7)),k0,AlphaFunction::EASE_IN_OUT );

    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3( -radius*2.0,  0.0f, 0.0f) );
    k0.Add( 0.5f, Vector3(-radius*3.0f,radius*2.0, 0.0f));
    k0.Add( 1.0f, Vector3( -radius*2.0,  0.0f, 0.0f) );
    animation.AnimateBetween( Property(shader, shader.GetPointPropertyName(10)),k0,AlphaFunction::EASE_IN_OUT );

    animation.AnimateBy( Property( meshActor, Actor::Property::ORIENTATION ), Quaternion( Radian( Degree(-90.0f) ), Vector3::ZAXIS ) );
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
  Toolkit::Control            mView;
};

void RunTest( Application& application )
{
  AnimatedShapesExample test( application );
  application.MainLoop();
}

int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  RunTest( application );

  return 0;
}
