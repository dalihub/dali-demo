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
const char* APPLICATION_TITLE( "Path Animation Example" );
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

  /*
   * Create a control composed of a label and an slider
   * @param[in] label The text to be displayed ny the label
   * @param[in] size The size of the slider
   * @param[in] callback Pointer to the callback function to be called when user moves the slider
  */
  Actor CreateVectorComponentControl( const std::string& label, const Vector3& size, bool(PathController::*callback)(Slider,float) )
  {
    Dali::TextActor textActor = TextActor::New(label);
    textActor.SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
    textActor.SetSize(size.y,size.y,0.0f);

    Slider slider = Slider::New();
    slider.SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    slider.SetParentOrigin( ParentOrigin::CENTER_RIGHT);
    slider.SetProperty(Slider::Property::LOWER_BOUND, -1.0f );
    slider.SetProperty(Slider::Property::UPPER_BOUND, 1.0f );

    Property::Array marks;
    float mark = -1.0f;
    for(unsigned short i(0); i<21; ++i )
    {
      marks.push_back( mark );
      mark += 0.1f;
    }

    slider.SetProperty(Slider::Property::MARKS, marks);
    slider.SetProperty(Slider::Property::SNAP_TO_MARKS, true );
    slider.SetSize(size);
    slider.SetScale( 0.5f );
    slider.ValueChangedSignal().Connect(this,callback);
    textActor.Add( slider );
    return textActor;
  }

  /**
   * Crate all the GUI controls
   */
  void CreateControls()
  {
    Stage stage = Stage::GetCurrent();

    //TextInput
    Dali::Layer controlsLayer = Dali::Layer::New();
    controlsLayer.SetSize( stage.GetSize().x, stage.GetSize().y*0.3f, 0.0 );
    controlsLayer.SetPosition( 0.0f, stage.GetSize().y*0.8f, 0.0f );
    controlsLayer.SetAnchorPoint( AnchorPoint::TOP_LEFT);
    controlsLayer.SetParentOrigin( ParentOrigin::TOP_LEFT);
    controlsLayer.TouchedSignal().Connect(this, &PathController::OnTouchGuiLayer);
    stage.Add( controlsLayer );

    Vector3 textInputSize( stage.GetSize().x, stage.GetSize().y*0.04f, 0.0f );
    Actor control0 = CreateVectorComponentControl("x:", textInputSize, &PathController::OnSliderXValueChange );
    control0.SetY( stage.GetSize().y*0.05f );
    control0.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    controlsLayer.Add( control0 );

    Actor control1 = CreateVectorComponentControl("y:", textInputSize, &PathController::OnSliderYValueChange );
    control1.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    control1.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    control1.SetPosition(0.0f,stage.GetSize().y*0.01,0.0f);
    control0.Add( control1 );

    Actor control2 =CreateVectorComponentControl("z:", textInputSize, &PathController::OnSliderZValueChange );
    control2.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    control2.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    control2.SetPosition(0.0f,stage.GetSize().y*0.01,0.0f);
    control1.Add( control2 );
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
    index = 0;
    for( size_t i(0); i<pointCount; ++i )
    {
      if( !mControlPoint[index] )
      {
        mControlPoint[index] = Toolkit::CreateSolidColorActor(Vector4(1.0f,1.0f,1.0f,1.0f));
        mControlPoint[index].SetParentOrigin( ParentOrigin::TOP_LEFT);
        mControlPoint[index].SetAnchorPoint( AnchorPoint::CENTER );
        mControlPoint[index].SetSize( 20.0f, 20.0f );
        mContentLayer.Add(mControlPoint[index]);
      }

      std::string name( "Knot");
      name.push_back(i);
      mControlPoint[index].SetName( name );
      mControlPoint[index].SetPosition( mPath.GetPoint(i) );
      mControlPoint[index].SetColor( Vector4(0.0f,0.0f,0.0f,1.0f));
      ++index;
    }

    //Create actors representing control points
    size_t controlPointCount=2*(pointCount-1);
    for( size_t i(0); i<controlPointCount; ++i )
    {
      if( !mControlPoint[index])
      {
        mControlPoint[index] = Toolkit::CreateSolidColorActor(Vector4(1.0f,1.0f,1.0f,1.0f));
        mControlPoint[index].SetParentOrigin( ParentOrigin::TOP_LEFT);
        mControlPoint[index].SetAnchorPoint( AnchorPoint::CENTER );
        mControlPoint[index].SetSize( 20.0f, 20.0f );
        mContentLayer.Add(mControlPoint[index]);
      }
      std::string name( "ControlPoint");
      name.push_back(i);
      mControlPoint[index].SetName( name );
      mControlPoint[index].SetPosition( mPath.GetControlPoint(i) );
      mControlPoint[index].SetColor( Vector4(1.0f,0.0f,0.0f,1.0f));
      ++index;
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

  /**
   * Get closest control point.void
   * @param[in] point Point from where the distance is computed
   * @return The closest actor if the distance is beyond 0.5cm or an uninitialized actor otherwise
   */
  Actor GetClosestActor(const Vector3& point)
  {
    size_t pointCount = mPath.GetPointCount();
    size_t controlPointCount = 3*pointCount - 2;
    Actor result;
    float minDistance = 1.0/0.0;

    for( size_t i(0); i<controlPointCount; ++i )
    {
      Vector3 v = mControlPoint[i].GetCurrentPosition() - point;
      float distance = v.LengthSquared();
      if( distance < minDistance )
      {
        result = mControlPoint[i];
        minDistance = distance;
      }
    }

    Vector2 dpi = Dali::Stage::GetCurrent().GetDpi();
    float distanceTreshold = 0.2f * dpi.x * 0.2f * dpi.x;

    if( minDistance < distanceTreshold )
    {
      return result;
    }
    else
    {
      return Actor();
    }
  }

  /**
   * Callback called when the background layer is touched
   */
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
      else if(point.state==TouchPoint::Down)
      {
        Vector3 touchPoint = Vector3(event.GetPoint(0).screen.x, event.GetPoint(0).screen.y, 0.0f);
        if(!mDragActor )
        {
          mDragActor = GetClosestActor( touchPoint );
          if( !mDragActor && mPath.GetPointCount() < 10 )
          {
            // Add new point
            Vector3 lastPoint = mPath.GetPoint( mPath.GetPointCount()-1);
            mPath.AddPoint( touchPoint );
            Vector3 displacement = (touchPoint-lastPoint)/8;
            mPath.AddControlPoint( lastPoint + displacement );
            mPath.AddControlPoint( touchPoint - displacement);

            DrawPath( 200u );
            CreateAnimation();
          }
        }
      }
      else if( mDragActor && point.state==TouchPoint::Motion  )
      {
        Vector3 touchPoint = Vector3(event.GetPoint(0).screen.x, event.GetPoint(0).screen.y, 0.0f);
        std::string actorName(mDragActor.GetName());
        if( actorName.compare(0, 4, "Knot") == 0)
        {
          int index = actorName[4];
          mPath.GetPoint(index) = touchPoint;
        }
        else
        {
          int index = actorName[12];
          mPath.GetControlPoint(index) = touchPoint;
        }

        DrawPath( 200u );
        CreateAnimation();
      }
    }

    return true;
  }

  bool OnTouchGuiLayer(Actor actor, const TouchEvent& event)
 {
    mDragActor.Reset();
    return false;
 }
  /**
   * Callback called when user changes slider X
   * @param[in] slider The slider that has generated the signal
   * @param[in] value The new value
   */
  bool OnSliderXValueChange( Slider s, float value)
  {
    if( fabs( value ) - Math::MACHINE_EPSILON_1000 < 0.0f )
    {
      mForward.x = 0.0f;
    }
    else
    {
      mForward.x = value;
    }

    CreateAnimation();
    return true;
  }

  /**
   * Callback called when user changes slider Y
   * @param[in] slider The slider that has generated the signal
   * @param[in] value The new value
   */
  bool OnSliderYValueChange( Slider s, float value)
  {
    if( fabs( value ) - Math::MACHINE_EPSILON_1000 < 0.0f )
    {
      mForward.y = 0.0f;
    }
    else
    {
      mForward.y = value;
    }
    CreateAnimation();
    return true;
  }

  /**
   * Callback called when user changes slider Z
   * @param[in] slider The slider that has generated the signal
   * @param[in] value The new value
   */
  bool OnSliderZValueChange( Slider s, float value)
  {
    if( fabs( value ) - Math::MACHINE_EPSILON_1000 < 0.0f )
    {
      mForward.z = 0.0f;
    }
    else
    {
      mForward.z = value;
    }

    CreateAnimation();
    return true;
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

    //Title
    TextView title = TextView::New();
    toolBar.AddControl( title, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Alignment::HorizontalCenter );
    Font font = Font::New();
    title.SetText( APPLICATION_TITLE );
    title.SetSize( font.MeasureText( APPLICATION_TITLE ) );
    title.SetStyleToCurrentText(DemoHelper::GetDefaultTextStyle());

    //Path
    mPath = Dali::Path::New();
    mPath.AddPoint( Vector3( 10.0f, stage.GetSize().y*0.5f, 0.0f ));
    mPath.AddPoint( Vector3( stage.GetSize().x*0.5f, stage.GetSize().y*0.5f, 0.0f ));
    mPath.GenerateControlPoints(0.25f);
    DrawPath( 200u );

    //Actor
    ImageAttributes attributes;
    Image img = ResourceImage::New(ACTOR_IMAGE, attributes );
    mActor = ImageActor::New( img );
    mActor.SetAnchorPoint( AnchorPoint::CENTER );
    mActor.SetSize( 100, 50, 1 );
    stage.Add( mActor );

    CreateAnimation();
    CreateControls();
  }

private:
  Application&  mApplication;

  Layer      mContentLayer;       ///< The content layer

  Path       mPath;               ///< The path used in the animation
  ImageActor mActor;              ///< Actor being animated
  Vector3    mForward;            ///< Current forward vector
  Animation  mAnimation;          ///< Path animation

  MeshActor  mMeshPath;           ///< Mesh actor for the path
  MeshActor  mMeshHandlers;       ///< Mesh actor for the handlers of the path

  Actor      mControlPoint[28];   ///< ImageActors represeting control points of the path

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
