/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

const Vector4 GRID_BACKGROUND_COLOR(0.85f, 0.85f, 0.85f, 1.0f);
const Vector4 CONTROL_POINT1_COLOR(Color::MAGENTA);
const Vector4 CONTROL_POINT2_COLOR(0.0, 0.9, 0.9, 1.0);
const Vector3 CONTROL_POINT1_ORIGIN(-100,  200, 0);
const Vector3 CONTROL_POINT2_ORIGIN( 100, -200, 0);
const char* const CIRCLE1_IMAGE( DEMO_IMAGE_DIR "circle1.png" );
const char* const CIRCLE2_IMAGE( DEMO_IMAGE_DIR "circle2.png" );
const char* const ANIMATION_BACKGROUND( DEMO_IMAGE_DIR "slider-skin.9.png" );
const char* APPLICATION_TITLE("Bezier curve animation");
const float ANIM_LEFT_FACTOR(0.2f);
const float ANIM_RIGHT_FACTOR(0.8f);


const char* CURVE_VERTEX_SHADER = DALI_COMPOSE_SHADER
  (
    attribute mediump vec2 aPosition;
    uniform mediump mat4 uMvpMatrix;
    uniform vec3 uSize;
    void main()
    {
      gl_Position = uMvpMatrix * vec4(aPosition*uSize.xy, 0.0, 1.0);
    }
   );

const char* CURVE_FRAGMENT_SHADER = DALI_COMPOSE_SHADER
  (
    uniform lowp vec4 uColor;
    void main()
    {
      gl_FragColor = vec4(0.0,0.0,0.0,1.0);
    }
   );


inline float Clamp(float v, float min, float max)
{
  if(v<min) return min;
  if(v>max) return max;
  return v;
}

struct HandlePositionConstraint
{
  HandlePositionConstraint( float minRelX, float maxRelX, float minRelY, float maxRelY )
  : minRelX(minRelX), maxRelX(maxRelX), minRelY(minRelY), maxRelY(maxRelY)
  {
  }

  void operator()( Vector3& current, const PropertyInputContainer& inputs )
  {
    Vector3 size( inputs[0]->GetVector3() );
    current.x = Clamp(current.x, minRelX*size.x, maxRelX*size.x );
    current.y = Clamp(current.y, minRelY*size.y, maxRelY*size.y );
  }

  float minRelX;
  float maxRelX;
  float minRelY;
  float maxRelY;
};

void AnimatingPositionConstraint( Vector3& current, const PropertyInputContainer& inputs )
{
  float positionFactor( inputs[0]->GetFloat() ); // -1 - 2
  Vector3 size( inputs[1]->GetVector3() );

  current.x = size.x * (positionFactor-0.5f); // size * (-1.5 - 1.5)
}

} //unnamed namespace



class BezierCurveExample : public ConnectionTracker
{
public:

  BezierCurveExample( Application& application )
  : mApplication( application ),
    mDuration(2.0f),
    mGoingRight(true)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &BezierCurveExample::Create );
  }

  ~BezierCurveExample()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    Stage stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect( this, &BezierCurveExample::OnKeyEvent );

    CreateBackground(stage);

    mControlPointScale = 0.5f;
    mControlPointZoomScale = mControlPointScale * 2.0f;

    mContentLayer = Layer::New();
    mContentLayer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mContentLayer.TouchSignal().Connect(this, &BezierCurveExample::OnTouchLayer);
    mContentLayer.SetParentOrigin( ParentOrigin::CENTER );
    stage.Add( mContentLayer );

    // 6 rows: title, grid, coords, play, anim1, anim2
    TableView contentLayout = TableView::New(5, 1);
    contentLayout.SetName("contentLayout");
    contentLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    contentLayout.SetCellPadding( Size( 30, 30 ) );
    contentLayout.SetParentOrigin(ParentOrigin::TOP_CENTER);
    contentLayout.SetAnchorPoint(AnchorPoint::TOP_CENTER);
    mContentLayer.Add( contentLayout );

    // Create a TextLabel for the application title.
    Toolkit::TextLabel label = Toolkit::TextLabel::New( APPLICATION_TITLE );
    label.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    label.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    label.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::BLACK );
    contentLayout.Add( label );
    contentLayout.SetFitHeight(0);

    mGrid = Control::New();

    mGrid.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::WIDTH );
    mGrid.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );

    mGrid.SetParentOrigin(ParentOrigin::CENTER);
    mGrid.SetAnchorPoint(AnchorPoint::CENTER);
    mGrid.SetBackgroundColor(GRID_BACKGROUND_COLOR);

    contentLayout.Add( mGrid );
    contentLayout.SetCellAlignment(1, HorizontalAlignment::CENTER, VerticalAlignment::CENTER );
    CreateCubic(mGrid);
    CreateControlPoints( mGrid ); // Control points constrained to double height of grid

    mCoefficientLabel = TextLabel::New();
    mCoefficientLabel.SetProperty( TextLabel::Property::ENABLE_MARKUP, true );
    mCoefficientLabel.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mCoefficientLabel.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mCoefficientLabel.SetParentOrigin(ParentOrigin::CENTER);

    contentLayout.Add( mCoefficientLabel );
    SetLabel( Vector2(0,0), Vector2(1,1));
    contentLayout.SetCellAlignment(2, HorizontalAlignment::CENTER, VerticalAlignment::CENTER );
    contentLayout.SetFitHeight(2);

    // Setup Play button and 2 icons to show off current anim and linear anim

    PushButton play = PushButton::New();
    play.SetName("Play");
    play.SetParentOrigin(ParentOrigin::CENTER);
    play.SetLabelText("Play");
    play.ClickedSignal().Connect( this, &BezierCurveExample::OnPlayClicked );

    contentLayout.Add( play );
    contentLayout.SetCellAlignment(3, HorizontalAlignment::CENTER, VerticalAlignment::CENTER );
    contentLayout.SetFitHeight(3);

    auto animContainer = Control::New();
    animContainer.SetName("AnimationContainer");
    animContainer.SetParentOrigin( ParentOrigin::CENTER );
    animContainer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    auto animRail = Control::New();
    animRail.SetProperty( Control::Property::BACKGROUND, Property::Map()
                          .Add( Visual::Property::TYPE, Visual::IMAGE )
                          .Add( ImageVisual::Property::URL, ANIMATION_BACKGROUND ) );
    animRail.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    animRail.SetSizeModeFactor( Vector3( 0.666f, 0.2f, 1.0f ) );
    animRail.SetParentOrigin( ParentOrigin::CENTER );
    animContainer.Add( animRail );

    contentLayout.Add( animContainer );
    contentLayout.SetFixedHeight(4, 150 );

    mAnimIcon1 = ImageView::New( CIRCLE1_IMAGE );
    mAnimIcon1.SetParentOrigin( ParentOrigin::CENTER );
    mAnimIcon1.SetAnchorPoint( AnchorPoint::CENTER );

    // Would like some means of setting and animating position as a percentage of
    // parent size without using constraints, but this will have to suffice for the moment.
    mPositionFactorIndex = mAnimIcon1.RegisterProperty( "positionFactor", ANIM_LEFT_FACTOR); // range: 0-1 (+/- 1)
    Constraint constraint = Constraint::New<Vector3>( mAnimIcon1, Actor::Property::POSITION, AnimatingPositionConstraint );
    constraint.AddSource( Source( mAnimIcon1, mPositionFactorIndex ) );
    constraint.AddSource( Source( animContainer, Actor::Property::SIZE ) );
    constraint.Apply();

    animContainer.Add( mAnimIcon1 );

    // First UpdateCurve needs to run after size negotiation and after images have loaded
    mGrid.OnRelayoutSignal().Connect( this, &BezierCurveExample::InitialUpdateCurve );

    auto controlPoint1 = Control::DownCast( mControlPoint1 );
    if( controlPoint1 )
    {
      controlPoint1.ResourceReadySignal().Connect( this, &BezierCurveExample::ControlPointReady );
    }

    auto controlPoint2 = Control::DownCast( mControlPoint2 );
    if( controlPoint2 )
    {
      controlPoint2.ResourceReadySignal().Connect( this, &BezierCurveExample::ControlPointReady );
    }
  }

  void ControlPointReady( Control control )
  {
    UpdateCurve();
  }

  void InitialUpdateCurve(Actor actor)
  {
    UpdateCurve();
  }

  void CreateBackground( Stage stage )
  {
    Toolkit::Control background = Dali::Toolkit::Control::New();
    background.SetAnchorPoint( Dali::AnchorPoint::CENTER );
    background.SetParentOrigin( Dali::ParentOrigin::CENTER );
    background.SetResizePolicy( Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::ALL_DIMENSIONS );

    Property::Map map;
    map.Insert( Visual::Property::TYPE,  Visual::COLOR );
    map.Insert( ColorVisual::Property::MIX_COLOR, Vector4( 253/255.0f, 245/255.0f, 230/255.0f, 1.0f ) );
    background.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
    stage.Add( background );
  }

  void CreateCubic(Actor parent)
  {
    // Create a mesh to draw the cubic as a single line
    mCurve = Actor::New();
    mCurve.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mCurve.SetParentOrigin( ParentOrigin::CENTER );

    Shader shader = Shader::New( CURVE_VERTEX_SHADER, CURVE_FRAGMENT_SHADER );

    Property::Map curveVertexFormat;
    curveVertexFormat["aPosition"] = Property::VECTOR2;
    mCurveVertices = PropertyBuffer::New( curveVertexFormat );
    Vector2 vertexData[2] = { Vector2(-0.5f, 0.5f), Vector2( 0.5f, -0.5f ) };
    mCurveVertices.SetData( vertexData, 2 );

    Geometry geometry = Geometry::New();
    geometry.AddVertexBuffer( mCurveVertices );
    geometry.SetType( Geometry::LINE_STRIP );

    Renderer renderer = Renderer::New( geometry, shader );
    mCurve.AddRenderer( renderer );
    parent.Add(mCurve);
  }

  Actor CreateControlPoint( Actor parent, const char* url, Vector3 position)
  {
    Actor actor = ImageView::New( url );
    actor.SetScale( mControlPointScale);
    actor.SetParentOrigin( ParentOrigin::CENTER );
    // Curve and line drawing works off current value (i.e. last update frame's value). Need to animate to ensure
    // initial position is baked to both frames before initially drawing the curve.
    auto positionAnimation = Animation::New( 0.01f );
    positionAnimation.AnimateTo( Property( actor, Actor::Property::POSITION ), position, AlphaFunction::EASE_IN_OUT );
    positionAnimation.Play();
    positionAnimation.FinishedSignal().Connect( this, &BezierCurveExample::OnAnimationFinished );

    // Set up constraints for drag/drop
    Constraint constraint = Constraint::New<Vector3>( actor, Actor::Property::POSITION, HandlePositionConstraint( -0.5, 0.5, -1, 1));
    constraint.AddSource( Source( parent, Actor::Property::SIZE ) );
    constraint.Apply();

    actor.TouchSignal().Connect(this, &BezierCurveExample::OnTouchControlPoint);
    return actor;
  }

  Actor CreateControlLine( PropertyBuffer vertexBuffer )
  {
    Actor line = Actor::New();
    line.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    line.SetParentOrigin( ParentOrigin::CENTER );

    Shader shader = Shader::New( CURVE_VERTEX_SHADER, CURVE_FRAGMENT_SHADER );
    Geometry geometry = Geometry::New();
    geometry.AddVertexBuffer( vertexBuffer );
    geometry.SetType( Geometry::LINE_STRIP );

    Renderer renderer = Renderer::New( geometry, shader );
    line.AddRenderer( renderer );
    return line;
  }

  void CreateControlPoints( Actor parent )
  {
    mControlPoint1 = CreateControlPoint( parent,
                                         CIRCLE1_IMAGE,
                                         CONTROL_POINT1_ORIGIN );
    mControlPoint1Id = mControlPoint1.GetId();

    mControlPoint2 = CreateControlPoint( parent,
                                         CIRCLE2_IMAGE,
                                         CONTROL_POINT2_ORIGIN );
    mControlPoint2Id = mControlPoint2.GetId();

    Property::Map lineVertexFormat;
    lineVertexFormat["aPosition"] = Property::VECTOR2;
    mLine1Vertices = PropertyBuffer::New( lineVertexFormat );
    mLine2Vertices = PropertyBuffer::New( lineVertexFormat );

    mControlLine1 = CreateControlLine( mLine1Vertices );
    mControlLine2 = CreateControlLine( mLine2Vertices );

    parent.Add( mControlLine1 );
    parent.Add( mControlLine2 );
    parent.Add( mControlPoint1 );
    parent.Add( mControlPoint2 );
  }

  void SetLabel( Vector2 pos1, Vector2 pos2 )
  {
    std::ostringstream oss;
    oss.setf(std::ios::fixed, std::ios::floatfield);
    oss.precision(2);
    oss << "( <color value='#971586'>" << pos1.x << ", " << pos1.y << ", </color>";
    oss << "<color value='#e7640d'>" << pos2.x << ", " << pos2.y << "</color>";
    oss << "<color value='black'> )</color>";

    mCoefficientLabel.SetProperty( TextLabel::Property::TEXT, oss.str() );
  }

  Vector2 AlignToGrid( Vector3 actorPos, Vector3 gridSize )
  {
    actorPos /= gridSize; // => -0.5 - 0.5
    actorPos.x = Clamp( actorPos.x, -0.5f, 0.5f );
    return Vector2( actorPos.x + 0.5f, 0.5f - actorPos.y );
  }

  void GetControlPoints(Vector2& pt1, Vector2& pt2)
  {
    Vector3 gridSize = mGrid.GetProperty<Vector3>( Actor::Property::SIZE ); // Get target value

    pt1 = AlignToGrid( mControlPoint1.GetCurrentPosition(), gridSize );
    pt2 = AlignToGrid( mControlPoint2.GetCurrentPosition(), gridSize );
  }

  /**
   * @param[in] actor The actor to get the position from
   * @param[out] point The point in the grid in the range -0.5 -> 0.5 in x and y, with y up.
   * @param[out] position The actor position, floored to the nearest pixel
   */
  void GetPoint( Actor actor, Vector2& point, Vector2& position)
  {
    auto gridSize = mGrid.GetProperty<Vector3>( Actor::Property::SIZE ); // Get target value
    auto currentPosition = actor.GetCurrentPosition(); // Get constrained current value

    position = Vector2( std::floor( currentPosition.x ), std::floor( currentPosition.y ) );

    point.x = Clamp( position.x / gridSize.x, -0.5f, 0.5f ) + 0.5f;
    point.y = 0.5f - position.y / gridSize.y;
  }

  void UpdateCurve()
  {
    Vector2 point1, point2;
    Vector2 position1, position2;
    const int NUMBER_OF_SEGMENTS(40);

    GetPoint( mControlPoint1, point1, position1 );
    GetPoint( mControlPoint2, point2, position2 );

    if( position1 != mLastControlPointPosition1 ||
        position2 != mLastControlPointPosition2 )
    {
      mLastControlPointPosition1 = position1;
      mLastControlPointPosition2 = position2;

      SetLabel( point1, point2 );

      Path path = Path::New();
      path.AddPoint(Vector3::ZERO);
      path.AddPoint(Vector3(1.0f, 1.0f, 1.0f));
      path.AddControlPoint( Vector3( point1.x, point1.y, 0 ) );
      path.AddControlPoint( Vector3( point2.x, point2.y, 0 ) );

      Dali::Vector<float> verts;

      verts.Resize(2*(NUMBER_OF_SEGMENTS+1)); // 1 more point than segment
      for( int i=0; i<=NUMBER_OF_SEGMENTS; ++i)
      {
        Vector3 position, tangent;
        path.Sample( i/float(NUMBER_OF_SEGMENTS), position, tangent );
        verts[i*2] = position.x-0.5;
        verts[i*2+1] = 0.5-position.y;
      }
      mCurveVertices.SetData(&verts[0], NUMBER_OF_SEGMENTS+1);

      Vector4 line1( -0.5f, 0.5f, point1.x-0.5f, 0.5f-point1.y );
      mLine1Vertices.SetData( line1.AsFloat(), 2 );

      Vector4 line2( 0.5f, -0.5f, point2.x-0.5f, 0.5f-point2.y );
      mLine2Vertices.SetData( line2.AsFloat(), 2 );
    }
  }

  bool OnTouchControlPoint( Actor controlPoint, const TouchData& event )
  {
    if( event.GetPointCount() > 0 )
    {
      if( event.GetState( 0 ) == PointState::DOWN )
      {
        Vector2 screenPoint = event.GetScreenPosition( 0 );
        mRelativeDragPoint = screenPoint;
        mRelativeDragPoint -= Vector2(controlPoint.GetCurrentPosition());
        mDragActor = controlPoint;
        mDragAnimation = Animation::New(0.25f);
        mDragAnimation.AnimateTo( Property(mDragActor, Actor::Property::SCALE), Vector3( mControlPointZoomScale, mControlPointZoomScale, 1.0f), AlphaFunction::EASE_OUT);
        mDragAnimation.Play();
      }
    }
    return false; // Don't mark this as consumed - let the layer get the touch
  }

  bool OnTouchLayer( Actor actor, const TouchData& event )
  {
    if( event.GetPointCount() > 0 )
    {
      if( mDragActor )
      {
        Vector3 position( event.GetScreenPosition( 0 ) );

        mDragActor.SetPosition( position - Vector3( mRelativeDragPoint ) );

        if( event.GetState( 0 ) == PointState::UP ) // Stop dragging
        {
          mDragAnimation = Animation::New(0.25f);
          mDragAnimation.AnimateTo( Property( mDragActor, Actor::Property::SCALE ), Vector3( mControlPointScale, mControlPointScale, 1.0f), AlphaFunction::EASE_IN);
          mDragAnimation.FinishedSignal().Connect( this, &BezierCurveExample::OnAnimationFinished );
          mDragAnimation.Play();
          mDragActor.Reset();
        }
      }
      UpdateCurve();
    }
    return false;
  }

  void OnAnimationFinished( Animation& animation )
  {
    UpdateCurve();
  }

  bool OnPlayClicked( Button button )
  {
    if( ! mBezierAnimation )
    {
      mBezierAnimation = Animation::New( mDuration );
    }
    mBezierAnimation.Stop();
    mBezierAnimation.Clear();

    float positionFactor = ANIM_LEFT_FACTOR;
    if( mGoingRight )
    {
      positionFactor = ANIM_RIGHT_FACTOR;
      mGoingRight = false;
    }
    else
    {
      mGoingRight = true;
    }

    Vector2 pt1, pt2;
    GetControlPoints(pt1, pt2);

    mBezierAnimation.AnimateTo( Property(mAnimIcon1, mPositionFactorIndex), positionFactor, AlphaFunction( pt1, pt2 ) );
    mBezierAnimation.Play();
    return true;
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
  Application& mApplication;
  Actor mControlPoint1;
  Actor mControlPoint2;
  Actor mControlLine1;
  Actor mControlLine2;
  ImageView mAnimIcon1;
  ImageView mAnimIcon2;
  Actor mDragActor;
  Actor mCurve;
  TextLabel mCoefficientLabel;
  Layer mContentLayer;
  Control mGrid;
  Timer mTimer;
  Animation mDragAnimation;
  Animation mBezierAnimation;
  PropertyBuffer mCurveVertices;
  PropertyBuffer mLine1Vertices;
  PropertyBuffer mLine2Vertices;
  Vector2 mRelativeDragPoint;
  Vector2 mLastControlPointPosition1;
  Vector2 mLastControlPointPosition2;
  Property::Index mPositionFactorIndex;
  float mDuration;
  unsigned int mControlPoint1Id;
  unsigned int mControlPoint2Id;
  float mControlPointScale;
  float mControlPointZoomScale;
  bool mGoingRight;
};


int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  BezierCurveExample test( application );
  application.MainLoop();
  return 0;
}
