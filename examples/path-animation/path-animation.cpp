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

/**
 * This example shows how to use path animations in DALi
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>

// INTERNAL INCLUDES
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;


namespace
{
const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-default.png" );
const char* ACTOR_IMAGE( DALI_IMAGE_DIR "dali-logo.png" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
}; //Unnamed namespace

/**
 * @brief The main class of the demo.
 */
class PathController : public ConnectionTracker
{
public:

  PathController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &PathController::Create );
  }

  ~PathController()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    // Get a handle to the stage:
    Stage stage = Stage::GetCurrent();

    // Connect to input event signals:
    stage.KeyEventSignal().Connect(this, &PathController::OnKeyEvent);

    // Create a default view with a default tool bar:
    Toolkit::View view;                ///< The View instance.
    Toolkit::ToolBar toolBar;          ///< The View's Toolbar.
    mContentLayer = DemoHelper::CreateView( mApplication,
                                            view,
                                            toolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            "" );

    mContentLayer.TouchedSignal().Connect(this, &PathController::OnTouchLayer);

    //Path
    mPath = Dali::Path::New();
    mPath.AddPoint( Vector3( 10.0f, stage.GetSize().y*0.5f, 0.0f ));
    mPath.AddPoint( Vector3( stage.GetSize().x*0.5f, stage.GetSize().y*0.3f, 0.0f ));
    mPath.GenerateControlPoints(0.25f);
    DrawPath( 200u );

    //Actor
    ImageAttributes attributes;
    Image img = ResourceImage::New(ACTOR_IMAGE, attributes );
    mActor = ImageActor::New( img );
    mActor.SetPosition( Vector3( 10.0f, stage.GetSize().y*0.5f, 0.0f ) );
    mActor.SetAnchorPoint( AnchorPoint::CENTER );
    mActor.SetSize( 100, 50, 1 );
    stage.Add( mActor );

    mForward = Vector3::XAXIS;
    CreateAnimation();
  }

  /**
   * Create an actor representing a control point of the curve
   * @param[in] name Name of the actor
   * @param[in] size Size of the containing actor
   * @param[in] imageSize Size of the imageActor
   * @param[in] color Color of the imageActor
   */
  Actor CreateControlPoint(const std::string& name, const Vector3& size, const Vector3& imageSize, const Vector4& color )
  {
    Actor actor = Actor::New();
    actor.SetParentOrigin( ParentOrigin::TOP_LEFT);
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetSize( size );
    actor.SetName( name );
    actor.TouchedSignal().Connect(this, &PathController::OnTouchPoint);

    ImageActor imageActor = Toolkit::CreateSolidColorActor(color);
    imageActor.SetColor(Vector4(1.0f,0.0f,0.0f,1.0f));
    imageActor.SetParentOrigin( ParentOrigin::CENTER);
    imageActor.SetAnchorPoint( AnchorPoint::CENTER );
    imageActor.SetSize( imageSize );
    actor.Add(imageActor );

    return actor;
  }

  /**
   * Draws the path and the control points for the path
   * @param[in] resolution Number of segments for the path.
   */
  void DrawPath( unsigned int resolution )
  {
    Stage stage = Dali::Stage::GetCurrent();

    //Create path mesh actor
    Dali::MeshData meshData = MeshFactory::NewPath( mPath, resolution );
    Dali::Material material = Material::New("LineMaterial");
    material.SetDiffuseColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    meshData.SetMaterial(material);
    Dali::Mesh mesh = Dali::Mesh::New( meshData );
    if( mMeshPath )
    {
      stage.Remove( mMeshPath );
    }
    mMeshPath = Dali::MeshActor::New( mesh );
    mMeshPath.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mMeshPath.SetParentOrigin( ParentOrigin::TOP_LEFT );
    stage.Add( mMeshPath );


    ////Create mesh connecting interpolation points and control points
    std::vector<Dali::MeshData::Vertex> vVertex;
    std::vector<unsigned short> vIndex;
    size_t pointCount = mPath.GetPointCount();
    size_t controlPointIndex = 0;
    for( size_t i(0); i<pointCount; ++i )
    {
      vVertex.push_back( MeshData::Vertex(mPath.GetPoint(i),Vector2::ZERO, Vector3::ZERO ) );
      if( i<pointCount-1)
      {
        vVertex.push_back( MeshData::Vertex(mPath.GetControlPoint(controlPointIndex),Vector2::ZERO, Vector3::ZERO ));
        vVertex.push_back( MeshData::Vertex(mPath.GetControlPoint(controlPointIndex+1),Vector2::ZERO, Vector3::ZERO ));
      }
      controlPointIndex += 2;
    }

    size_t segmentCount = 2*(pointCount-2)+2;
    unsigned short index=0;
    for( size_t i(0); i<segmentCount; ++i, ++index )
    {
      vIndex.push_back(index);
      vIndex.push_back(index+1);

      if( ~i & 1 )
      {
        index++;
      }
    }

    meshData.SetLineData( vVertex, vIndex, material );
    meshData.SetMaterial(material);
    mesh = Dali::Mesh::New( meshData );
    if( mMeshHandlers )
    {
      stage.Remove( mMeshHandlers );
    }
    mMeshHandlers = Dali::MeshActor::New( mesh );
    mMeshHandlers.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mMeshHandlers.SetParentOrigin( ParentOrigin::TOP_LEFT );
    stage.Add( mMeshHandlers );


    //Create actors representing interpolation points
    for( size_t i(0); i<pointCount; ++i )
    {
      if( !mKnot[i] )
      {
        std::string name( "Knot");
        name.push_back(i);
        mKnot[i] = CreateControlPoint( name, Vector3(150.0f,150.0f,0.0f), Vector3(20.0f,20.0f,0.0f), Vector4(0.0f,0.0f,0.0f,1.0f) );
        mContentLayer.Add(mKnot[i] );
      }

      mKnot[i].SetPosition( mPath.GetPoint(i) );
    }

    //Create actors representing control points
    size_t controlPointCount=2*(pointCount-1);
    for( size_t i(0); i<controlPointCount; ++i )
    {
      if( !mControlPoint[i])
      {
        std::string name( "ControlPoint");
        name.push_back(i);
        mControlPoint[i] = CreateControlPoint( name, Vector3(150.0f,150.0f,0.0f), Vector3(20.0f,20.0f,0.0f), Vector4(1.0f,0.0f,0.0f,1.0f) );
        mContentLayer.Add(mControlPoint[i] );
      }

      mControlPoint[i].SetPosition( mPath.GetControlPoint(i) );
    }
  }

  bool OnTouchPoint(Actor actor, const TouchEvent& event)
  {
    if(event.GetPointCount()>0)
    {
      const TouchPoint& point = event.GetPoint(0);

      if(point.state==TouchPoint::Down)
      {
        // Start dragging
        mDragActor = actor;
      }
    }
    return false;
  }

  bool OnTouchLayer(Actor actor, const TouchEvent& event)
  {
    if(event.GetPointCount()>0)
    {
      const TouchPoint& point = event.GetPoint(0);

      if(point.state==TouchPoint::Up)
      {
        //Stop dragging
        mDragActor.Reset();
      }
      else if(!mDragActor && point.state==TouchPoint::Down && mPath.GetPointCount()<10 )
      {
        // Add new point
        const TouchPoint& point = event.GetPoint(0);
        Vector3 newPoint = Vector3(point.screen.x, point.screen.y, 0.0f);

        size_t pointCount = mPath.GetPointCount();
        Vector3 lastPoint = mPath.GetPoint( pointCount-1);
        mPath.AddPoint( newPoint );

        Vector3 displacement = (newPoint-lastPoint)/8;

        mPath.AddControlPoint( lastPoint + displacement );
        mPath.AddControlPoint( newPoint - displacement);

        DrawPath( 200u );
        CreateAnimation();
      }
      else
      {
        if( mDragActor )
        {
          const TouchPoint& point = event.GetPoint(0);
          Vector3 newPosition = Vector3(point.screen.x, point.screen.y, 0.0f);

          std::string actorName(mDragActor.GetName());

          if( actorName.compare(0, 4, "Knot") == 0)
          {
             int index = actorName[4];
             mPath.GetPoint(index) = newPosition;
          }
          else
          {
            int index = actorName[12];
            mPath.GetControlPoint(index) = newPosition;
          }

          DrawPath( 200u );
          CreateAnimation();
        }
      }
    }
    return false;
  }

 /**
  * Main key event handler.
  * Quit on escape key.
  */
  void OnKeyEvent(const KeyEvent& event)
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE ) ||
          IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

  /**
   * Create the path animation.
   */
  void CreateAnimation()
  {
    if( !mAnimation )
    {
      mAnimation = Animation::New( 2.0f );
    }
    else
    {
      mAnimation.Pause();
      mAnimation.Clear();
      mActor.SetRotation( Quaternion() );
    }

    mAnimation.Animate( mActor, mPath, mForward );
    mAnimation.SetLooping( true );
    mAnimation.Play();
  }

private:
  Application&  mApplication;

  Layer      mContentLayer;       ///< The content layer

  Path       mPath;               ///< The path used in the animation
  ImageActor mActor;              ///< Actor being animated
  Vector3    mForward;            ///< Current forward vector
  Animation  mAnimation;          ///< Path animation

  MeshActor  mMeshPath;           ///< Mesh actor for the path
  MeshActor  mMeshHandlers;       ///< Mesh actor for the segments connecting points and control points
  Actor mKnot[10];           ///< ImageActors for the interpolation points
  Actor mControlPoint[18];   ///< ImageActors for the control points

  Actor      mDragActor;          ///< Reference to the actor currently being dragged
};

void RunTest( Application& application )
{
  PathController test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
