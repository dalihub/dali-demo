/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace DemoHelper;

namespace
{
const char* BACKGROUND_IMAGE( DEMO_IMAGE_DIR "background-blocks.jpg" );
const char* TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char* APPLICATION_TITLE( "DALi Blocks" );
const char* BALL_IMAGE = DEMO_IMAGE_DIR "blocks-ball.png";
const char* PADDLE_IMAGE = DEMO_IMAGE_DIR "blocks-paddle.png";
const char* PADDLE_HANDLE_IMAGE = DEMO_IMAGE_DIR "blocks-paddle-handle.png";

const char* BRICK_IMAGE_PATH[] =    { DEMO_IMAGE_DIR "blocks-brick-1.png",
                                      DEMO_IMAGE_DIR "blocks-brick-2.png",
                                      DEMO_IMAGE_DIR "blocks-brick-3.png",
                                      DEMO_IMAGE_DIR "blocks-brick-4.png" };

const int TOTAL_BRICKS(4);                                                  ///< Total bricks in game.
const Vector3 ICON_SIZE(100.0f, 100.0f, 0.0f);

const float SCREEN_MARGIN = 10.0f;                                          ///< Margin indentation around screen
const Vector3 MENU_BUTTON_SIZE = Vector3(0.15f, 0.05f, 1.0f);               ///< Standard Menu Buttons.

const float MAX_ANIMATION_DURATION = 60.0f;                                 ///< 60 seconds animations. Long enough for ball to hit an obstacle.
const float BALL_VELOCITY = 300.0f;                                         ///< Ball velocity in pixels/second.
const float MAX_VELOCITY = 500.0f;                                          ///< Max. velocity in pixels/second.
const Vector3 PADDLE_COLLISION_MARGIN(0.0f, 0.0f, 0.0f);                    ///< Collision margin for ball-paddle detection.
const Vector3 BRICK_COLLISION_MARGIN(0.0f, 0.0f, 0.0f);                     ///< Collision margin for ball-brick detection.
const Vector3 INITIAL_BALL_DIRECTION(1.0f, 1.0f, 0.0f);                     ///< Initial ball direction.

const std::string WOBBLE_PROPERTY_NAME("wobbleProperty");                  ///< Wobble property name.
const std::string COLLISION_PROPERTY_NAME("collisionProperty");            ///< Collision property name.

const Vector2 BRICK_SIZE(0.1f, 0.05f );                                     ///< Brick size relative to width of stage.
const Vector2 BALL_SIZE( 0.05f, 0.05f );                                    ///< Ball size relative to width of stage.
const Vector2 PADDLE_SIZE( 0.2f, 0.05f );                                   ///< Paddle size relative to width of stage.
const Vector2 PADDLE_HANDLE_SIZE( 0.3f, 0.3f );                             ///< Paddle handle size relative to width of stage.
const Vector2 BALL_START_POSITION(0.5f, 0.8f);                              ///< Ball start position relative to stage size.
const Vector2 PADDLE_START_POSITION(0.5f, 0.9f);                            ///< Paddler start position relative to stage size.
const Vector2 PADDLE_HIT_MARGIN( 0.1, 0.15f );                              ///< Extra hit Area for Paddle when touching.

const int TOTAL_LIVES(3);                                                   ///< Total lives in game before it's game over!
const int TOTAL_LEVELS(3);                                                  ///< 3 Levels total, then repeats.

// constraints ////////////////////////////////////////////////////////////////

/**
 * CollisionCircleRectangleConstraint generates a collision vector
 * between two actors a (circle) and b (rectangle)
 */
struct CollisionCircleRectangleConstraint
{
  /**
   * Collision Constraint constructor
   * The adjust (optional) parameter can be used to add a margin
   * to the actors. A +ve size will result in larger collisions,
   * while a -ve size will result in tighter collisions.
   *
   * @param[in] adjustPosition (optional) Adjusts the position offset of detection
   * @param[in] adjustSize (optional) Adjusts the rectangular size of detection
   */
  CollisionCircleRectangleConstraint(Vector3 adjustPosition = Vector3::ZERO,
                                     Vector3 adjustSize = Vector3::ZERO)
  : mAdjustPosition(adjustPosition),
    mAdjustSize(adjustSize)
  {
  }

  /**
   * Generates collision vector indicating whether Actor's A and B
   * have overlapped eachother, and the relative position of Actor B to A.
   *
   * @param[in,out] current The current collision-property
   * @param[in] inputs Contains:
   *                    Actor A's Position property.
   *                    Actor B's Position property.
   *                    Actor A's Size property.
   *                    Actor B's Size property.
   * @return The collision vector is returned.
   */
  void operator()( Vector3& current, const PropertyInputContainer& inputs )
  {
    const Vector3& a = inputs[0]->GetVector3();
    const Vector3 b = inputs[1]->GetVector3() + mAdjustPosition;
    const Vector3& sizeA = inputs[2]->GetVector3();
    const Vector3& sizeB = inputs[3]->GetVector3();
    const Vector3 sizeA2 = sizeA * 0.5f; // circle radius
    const Vector3 sizeB2 = (sizeB + mAdjustSize) * 0.5f; // rectangle half rectangle.

    // get collision relative to a (rectangle).
    Vector3 delta = a - b;

    // reduce rectangle to 0.
    if (delta.x > sizeB2.x)
    {
      delta.x -= sizeB2.x;
    }
    else if (delta.x < -sizeB2.x)
    {
      delta.x += sizeB2.x;
    }
    else
    {
      delta.x = 0;
    }

    if (delta.y > sizeB2.y)
    {
      delta.y -= sizeB2.y;
    }
    else if (delta.y < -sizeB2.y)
    {
      delta.y += sizeB2.y;
    }
    else
    {
      delta.y = 0;
    }

    // now calculate collision vector vs origin. (assume A is a circle, not ellipse)
    if(delta.Length() < sizeA2.x)
    {
      delta.Normalize();
      current = delta;
    }
    else
    {
      current = Vector3::ZERO;
    }
  }

  const Vector3 mAdjustPosition;            ///< Position Adjustment value
  const Vector3 mAdjustSize;                ///< Size Adjustment value
};

/**
 * WobbleConstraint generates a decaying sinusoidial rotation.
 * The result when applied to an Actor, is the Actor rotating left/right
 * initially a large amount (deviation degrees, when wobble property is 0.0f)
 * then eventually coming to a stop (once wobble property reaches 1.0f)
 */
struct WobbleConstraint
{
  /**
   * Wobble Constraint constructor
   * Generates a sinusoidial rotation that starts with
   * high amplitude (deviation), and then decays to zero over input 0.0f to 1.0f
   *
   * @param[in] deviation The max. deviation of wobble effect in degrees.
   */
  WobbleConstraint( Degree deviation )
  : mDeviation( deviation )
  {

  }

  /**
   * @param[in,out] current The current rotation property
   * @param[in] inputs Contains the wobble property (value from 0.0f to 1.0f)
   * @return The rotation (quaternion) is generated.
   */
  void operator()( Quaternion& current, const PropertyInputContainer& inputs )
  {
    const float& wobble = inputs[0]->GetFloat();

    float f = sinf(wobble * 10.0f) * (1.0f-wobble);

    current = Quaternion(mDeviation * f, Vector3::ZAXIS);
  }

  Radian mDeviation;           ///< Deviation factor in radians.
};

} // unnamed namespace

/**
 * This example shows how to use PropertyNotifications
 */
class ExampleController : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application Application class, stored as reference
   */
  ExampleController( Application& application )
  : mApplication( application ),
    mView(),
    mContentLayer(),
    mBall(),
    mBallStartPosition(),
    mBallVelocity(),
    mBallAnimation(),
    mPaddle(),
    mPaddleImage(),
    mPaddleHandle(),
    mPaddleHitMargin(),
    mWobbleAnimation(),
    mWobbleProperty( Property::INVALID_INDEX ),
    mLevelContainer(),
    mBrickImageMap(),
    mDragAnimation(),
    mDragActor(),
    mRelativeDragPoint(),
    mDestroyAnimationMap(),
    mPaddleFullSize(),
    mLevel( 0 ),
    mLives( TOTAL_LIVES ),
    mBrickCount( 0 )

  {
    // Connect to the Application's Init and orientation changed signal
    mApplication.InitSignal().Connect(this, &ExampleController::Create);
  }

  /**
   * This method gets called once the main loop of application is up and running
   * @param[in] application Reference to the application instance
   */
  void Create(Application& application)
  {
    Stage::GetCurrent().KeyEventSignal().Connect(this, &ExampleController::OnKeyEvent);

    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Toolkit::ToolBar toolBar;
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            toolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            APPLICATION_TITLE );

    // Add an extra space on the right to center the title text.
    toolBar.AddControl( Actor::New(), DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight );

    // Create the content layer, which is where game actors appear.
    AddContentLayer();
  }

private:

  /**
   * Adds a new layer to the stage, containing game actors.
   */
  void AddContentLayer()
  {
    Stage stage = Stage::GetCurrent();
    const Vector3 stageSize(stage.GetSize());

    // Ball setup
    mBallStartPosition = stageSize * Vector3( BALL_START_POSITION );
    mBall = CreateImage(BALL_IMAGE);
    mBall.SetPosition( mBallStartPosition );
    mBall.SetSize( BALL_SIZE * stageSize.width );
    mContentLayer.Add(mBall);
    mBallVelocity = Vector3::ZERO;

    // Paddle setup
    mPaddleHitMargin = Vector2(stageSize) * PADDLE_HIT_MARGIN;
    mPaddle = Actor::New();
    mPaddleHandle = CreateImage(PADDLE_HANDLE_IMAGE);
    mPaddleImage = CreateImage(PADDLE_IMAGE);
    mPaddle.Add( mPaddleHandle );
    mPaddle.Add( mPaddleImage );
    mPaddleHandle.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mPaddleHandle.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mPaddleHandle.SetPosition( 0.0f, stageSize.width * 0.0125f );
    mPaddleImage.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mPaddleImage.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mPaddle.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mPaddle.SetAnchorPoint( AnchorPoint::CENTER );
    mPaddleFullSize = PADDLE_SIZE * stageSize.width;
    mPaddle.SetSize( mPaddleFullSize + mPaddleHitMargin );
    mPaddleHandle.SetSize( PADDLE_HANDLE_SIZE * stageSize.width );
    mPaddleImage.SetSize( mPaddleFullSize );

    mWobbleProperty = mPaddle.RegisterProperty(WOBBLE_PROPERTY_NAME, 0.0f);
    Constraint wobbleConstraint = Constraint::New<Quaternion>( mPaddle, Actor::Property::ORIENTATION, WobbleConstraint(Degree( 10.0f )));
    wobbleConstraint.AddSource( LocalSource(mWobbleProperty) );
    wobbleConstraint.Apply();

    mPaddle.SetPosition( stageSize * Vector3( PADDLE_START_POSITION ) );
    mContentLayer.Add(mPaddle);
    mPaddle.TouchSignal().Connect(this, &ExampleController::OnTouchPaddle);
    mContentLayer.TouchSignal().Connect(this, &ExampleController::OnTouchLayer);

    const float margin(BALL_SIZE.width * stageSize.width * 0.5f);

    // Set up notifications for ball's collisions against walls.
    PropertyNotification leftNotification = mBall.AddPropertyNotification( Actor::Property::POSITION_X, LessThanCondition(margin) );
    leftNotification.NotifySignal().Connect( this, &ExampleController::OnHitLeftWall );

    PropertyNotification rightNotification = mBall.AddPropertyNotification( Actor::Property::POSITION_X, GreaterThanCondition(stageSize.width - margin) );
    rightNotification.NotifySignal().Connect( this, &ExampleController::OnHitRightWall );

    PropertyNotification topNotification = mBall.AddPropertyNotification( Actor::Property::POSITION_Y, LessThanCondition(margin) );
    topNotification.NotifySignal().Connect( this, &ExampleController::OnHitTopWall );

    PropertyNotification bottomNotification = mBall.AddPropertyNotification( Actor::Property::POSITION_Y, GreaterThanCondition(stageSize.height + margin) );
    bottomNotification.NotifySignal().Connect( this, &ExampleController::OnHitBottomWall );

    // Set up notification for ball colliding against paddle.
    Actor delegate = Actor::New();
    stage.Add(delegate);
    Property::Index property = delegate.RegisterProperty(COLLISION_PROPERTY_NAME, Vector3::ZERO);
    Constraint constraint = Constraint::New<Vector3>( delegate, property, CollisionCircleRectangleConstraint( -Vector3(0.0f, mPaddleHitMargin.height * 0.575f, 0.0f),-Vector3(mPaddleHitMargin) ) );
    constraint.AddSource( Source(mBall, Actor::Property::POSITION) );
    constraint.AddSource( Source(mPaddle, Actor::Property::POSITION) );
    constraint.AddSource( Source(mBall, Actor::Property::SIZE) );
    constraint.AddSource( Source(mPaddle, Actor::Property::SIZE) );
    constraint.Apply();

    PropertyNotification paddleNotification = delegate.AddPropertyNotification( property, GreaterThanCondition(0.0f) );
    paddleNotification.NotifySignal().Connect( this, &ExampleController::OnHitPaddle );

    RestartGame();
  }

  /**
   * Restarts Game
   * Resets Lives count and other stats, and loads level
   */
  void RestartGame()
  {
    mLives = TOTAL_LIVES;
    mLevel = 0;
    mBall.SetPosition( mBallStartPosition );
    mBallVelocity = Vector3::ZERO;
    mPaddle.SetSize( mPaddleFullSize + mPaddleHitMargin );
    mPaddleImage.SetSize( mPaddleFullSize );

    LoadLevel(mLevel);
  }

  /**
   * Loads level
   * All existing level content is removed, and new bricks
   * are added.
   * @param[in] level Level index to load.
   */
  void LoadLevel(int level)
  {
    if(mLevelContainer && mLevelContainer.GetParent() == mContentLayer)
    {
      mContentLayer.Remove( mLevelContainer );
    }

    mLevelContainer = Actor::New();
    mLevelContainer.SetAnchorPoint( AnchorPoint::CENTER );
    mLevelContainer.SetParentOrigin( ParentOrigin::CENTER );
    mLevelContainer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    mContentLayer.Add( mLevelContainer );

    mBrickCount = 0;

    if( mBrickImageMap.Empty() )
    {
      Vector2 stageSize(Stage::GetCurrent().GetSize());
      const Vector2 brickSize(BRICK_SIZE * Vector2(stageSize.x, stageSize.x));

      mBrickImageMap["desiredWidth"] = static_cast<int>( brickSize.width );
      mBrickImageMap["desiredHeight"] = static_cast<int>( brickSize.height );
      mBrickImageMap["fittingMode"] = "SCALE_TO_FILL";
      mBrickImageMap["samplingMode"] = "BOX_THEN_LINEAR";
    }

    switch(level%TOTAL_LEVELS)
    {
      case 0:
      {
        GenerateLevel0();
        break;
      }
      case 1:
      {
        GenerateLevel1();
        break;
      }
      case 2:
      {
        GenerateLevel2();
        break;
      }
      default:
      {
        break;
      }
    } // end switch
  }

  /**
   * Generates level 0
   */
  void GenerateLevel0()
  {
    Vector2 stageSize(Stage::GetCurrent().GetSize());
    const Vector2 brickSize(BRICK_SIZE * stageSize.width);

    const int columns = (0.85f * stageSize.width) / brickSize.width; // 85 percent of the width of the screen covered with bricks.
    const int rows = (0.3f * stageSize.height) / brickSize.height;   // 30 percent of the height of the screen covered with bricks.
    const Vector2 offset( (stageSize.x - (columns * brickSize.width)) * 0.5f,
                           stageSize.y * 0.125f );

    for(int j = 0; j < rows; j++)
    {
      for(int i = 0; i < columns; i++)
      {
        Actor brick = CreateBrick(Vector2(i * brickSize.width + offset.x, j * brickSize.height + offset.y) + (brickSize * 0.5f), j % TOTAL_BRICKS );
        mLevelContainer.Add(brick);
        mBrickCount++;
      }
    }
  }

  /**
   * Generates level 1
   */
  void GenerateLevel1()
  {
    Vector2 stageSize(Stage::GetCurrent().GetSize());
    const Vector2 brickSize(BRICK_SIZE * stageSize.width);

    const int columns = (0.85f * stageSize.width) / brickSize.width; // 85 percent of the width of the screen covered with bricks.
    const int rows = (0.3f * stageSize.height) / brickSize.height;   // 30 percent of the height of the screen covered with bricks.
    const Vector2 offset( (stageSize.x - (columns * brickSize.width)) * 0.5f,
                           stageSize.y * 0.125f );

    for(int j = 0; j < rows; j++)
    {
      for(int i = 0; i < columns; i++)
      {
        int i2 = columns - i - 1;
        int j2 = rows - j - 1;
        int brickIndex = std::min( std::min(i, j), std::min(i2, j2) ) % TOTAL_BRICKS;

        Actor brick = CreateBrick(Vector2(i * brickSize.width + offset.x, j * brickSize.height + offset.y) + (brickSize * 0.5f), brickIndex );

        mLevelContainer.Add(brick);
        mBrickCount++;
      }
    }
  }

  /**
   * Generates level 2
   */
  void GenerateLevel2()
  {
    Vector2 stageSize(Stage::GetCurrent().GetSize());
    const Vector2 brickSize(BRICK_SIZE * stageSize.width);

    const int columns = (0.85f * stageSize.width) / brickSize.width; // 85 percent of the width of the screen covered with bricks.
    const int rows = (0.3f * stageSize.height) / brickSize.height;   // 30 percent of the height of the screen covered with bricks.
    const Vector2 offset( (stageSize.x - (columns * brickSize.width)) * 0.5f,
                           stageSize.y * 0.125f );

    // lays down bricks in a spiral formation starting at i,j = (0,0) top left corner
    // travelling right di,dj = (1,0) initially
    int i = 0;
    int j = 0;
    int di = 1;
    int dj = 0;

    // contracting boundaries
    int left = 0;
    int right = columns - 1;
    int top = 2;
    int bottom = rows - 1;

    // length of current line. we stop laying down bricks when the length is 1 brick or less.
    int length = 0;
    while(true)
    {
      Actor brick = CreateBrick(Vector2(i * brickSize.width + offset.x, j * brickSize.height + offset.y) + (brickSize * 0.5f), 0 );
      mLevelContainer.Add(brick);
      i+=di;
      j+=dj;
      bool turn(false);
      if((i==right) && (di==1))
      {
        right -= 2;
        turn = true;
      }
      if((j==bottom) && (dj==1))
      {
        bottom -= 2;
        turn = true;
      }
      if((i==left) && (di==-1))
      {
        left += 2;
        turn = true;
      }
      if((j==top) && (dj==-1))
      {
        top += 2;
        turn = true;
      }
      if(turn)
      {
        // turn 90 degrees clockwise.
        std::swap(di, dj);
        di = -di;
        if (length<=1)
        {
          break;
        }
        length = 0;
      }
      length++;
      mBrickCount++;
    }
  }


  /**
   * Creates a brick at a specified position on the stage
   * @param[in] position the position for the brick
   * @param[in] type the type of brick
   * @return The Brick Actor is returned.
   */
  Actor CreateBrick( const Vector2& position, int type )
  {
    mBrickImageMap["url"] = BRICK_IMAGE_PATH[type];
    ImageView brick = ImageView::New();
    brick.SetProperty( ImageView::Property::IMAGE, mBrickImageMap );
    brick.SetParentOrigin(ParentOrigin::TOP_LEFT);
    brick.SetAnchorPoint(AnchorPoint::CENTER);
    brick.SetPosition( Vector3( position ) );

    // Add a constraint on the brick between it and the ball generating a collision-property
    Property::Index property = brick.RegisterProperty(COLLISION_PROPERTY_NAME, Vector3::ZERO);
    Constraint constraint = Constraint::New<Vector3>( brick, property, CollisionCircleRectangleConstraint(BRICK_COLLISION_MARGIN) );
    constraint.AddSource( Source(mBall, Actor::Property::POSITION) );
    constraint.AddSource( Source(brick, Actor::Property::POSITION) );
    constraint.AddSource( Source(mBall, Actor::Property::SIZE) );
    constraint.AddSource( Source(brick, Actor::Property::SIZE) );
    constraint.Apply();

    // Now add a notification on this collision-property

    PropertyNotification brickNotification = brick.AddPropertyNotification( property, GreaterThanCondition(0.0f) );
    brickNotification.NotifySignal().Connect( this, &ExampleController::OnHitBrick );

    return brick;
  }

  /**
   * Creates an Image (Helper)
   *
   * @param[in] filename the path of the image.
   */
  ImageView CreateImage(const std::string& filename)
  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, filename);
    propertyMap.Insert(DevelVisual::Property::VISUAL_FITTING_MODE, DevelVisual::FILL);
    ImageView actor = ImageView::New();
    actor.SetProperty(Toolkit::ImageView::Property::IMAGE, propertyMap);
    actor.SetParentOrigin(ParentOrigin::TOP_LEFT);
    actor.SetAnchorPoint(AnchorPoint::CENTER);
    return actor;
  }

  /**
   * Continue animation (based on current velocity)
   */
  void ContinueAnimation()
  {
    if(mBallAnimation)
    {
      mBallAnimation.Clear();
    }

    mBallAnimation = Animation::New(MAX_ANIMATION_DURATION);
    mBallAnimation.AnimateBy( Property( mBall, Actor::Property::POSITION ), mBallVelocity * MAX_ANIMATION_DURATION);
    mBallAnimation.Play();
  }

  /**
   * Signal invoked whenever user touches the Paddle.
   * @param[in] actor The actor touched
   * @param[in] event The touch event
   */
  bool OnTouchPaddle(Actor actor, const TouchData& event)
  {
    if(event.GetPointCount()>0)
    {
      if( event.GetState( 0 ) == PointState::DOWN ) // Commence dragging
      {
        // Get point where user touched paddle (relative to paddle's center)
        Vector2 screenPoint = event.GetScreenPosition( 0 );
        mRelativeDragPoint = screenPoint;
        mRelativeDragPoint -= actor.GetCurrentPosition();

        mDragActor = actor;
        mDragAnimation = Animation::New(0.25f);
        mDragAnimation.AnimateTo( Property(mDragActor, Actor::Property::SCALE), Vector3(1.1f, 1.1f, 1.0f), AlphaFunction::EASE_OUT);
        mDragAnimation.AnimateTo( Property(mPaddleHandle, Actor::Property::COLOR), Vector4(1.0f, 1.0f, 1.0f, 0.0f), AlphaFunction::EASE_OUT);
        mDragAnimation.Play();
      }
    }
    return false;
  }

  /**
   * Signal invoked whenever user touches anywhere on the screen.
   * @param[in] actor The actor touched
   * @param[in] event The touch event
   */
  bool OnTouchLayer(Actor actor, const TouchData& event)
  {
    if(event.GetPointCount()>0)
    {
      if(mDragActor)
      {
        Vector3 position( event.GetScreenPosition( 0 ) );
        mPaddle.SetPosition( position - mRelativeDragPoint );

        if( event.GetState( 0 ) == PointState::UP ) // Stop dragging
        {
          mDragAnimation = Animation::New(0.25f);
          mDragAnimation.AnimateTo( Property(mDragActor, Actor::Property::SCALE), Vector3(1.0f, 1.0f, 1.0f), AlphaFunction::EASE_IN);
          mDragAnimation.AnimateTo( Property(mPaddleHandle, Actor::Property::COLOR), Vector4(1.0f, 1.0f, 1.0f, 1.0f), AlphaFunction::EASE_OUT);
          mDragAnimation.Play();
          mDragActor.Reset();
        }
      }
    }
    return false;
  }

  /**
   * Notification: Ball hit left wall
   * @param source The notification
   */
  void OnHitLeftWall(PropertyNotification& source)
  {
    mBallVelocity.x = fabsf(mBallVelocity.x);
    ContinueAnimation();
  }

  /**
   * Notification: Ball hit right wall
   * @param source The notification
   */
  void OnHitRightWall(PropertyNotification& source)
  {
    mBallVelocity.x = -fabsf(mBallVelocity.x);
    ContinueAnimation();
  }

  /**
   * Notification: Ball hit top wall
   * @param source The notification
   */
  void OnHitTopWall(PropertyNotification& source)
  {
    mBallVelocity.y = fabsf(mBallVelocity.y);
    ContinueAnimation();
  }

  /**
   * Notification: Ball hit bottom wall
   * @param source The notification
   */
  void OnHitBottomWall(PropertyNotification& source)
  {
    if(mBallAnimation)
    {
      mBallAnimation.Clear();
    }

    if(mLives>0)
    {
      mLives--;
      const float f(static_cast<float>(mLives) / TOTAL_LIVES);
      mBallVelocity = Vector3::ZERO;

      Animation shrink = Animation::New(0.5f);
      shrink.AnimateTo( Property(mPaddle, Actor::Property::SIZE_WIDTH), mPaddleFullSize.x * f + mPaddleHitMargin.x);
      shrink.AnimateTo( Property(mPaddleImage, Actor::Property::SIZE_WIDTH), mPaddleFullSize.x * f );

      shrink.FinishedSignal().Connect( this, &ExampleController::OnPaddleShrunk );
      shrink.Play();
    }
  }

  /**
   * Paddle Shrink Animation complete.
   * @param[in] source The animation responsible for shrinking the paddle.
   */
  void OnPaddleShrunk( Animation &source )
  {
    // Reposition Ball in start position, and make ball appear.
    mBall.SetPosition( mBallStartPosition );
    mBall.SetColor( Vector4(1.0f, 1.0f, 1.0f, 0.1f) );
    Animation appear = Animation::New(0.5f);
    appear.AnimateTo( Property(mBall, Actor::Property::COLOR), Vector4(1.0f, 1.0f, 1.0f, 1.0f) );
    appear.Play();

    if(!mLives)
    {
      RestartGame();
    }
  }

  /**
   * Notification: Ball hit paddle
   * @param source The notification
   */
  void OnHitPaddle(PropertyNotification& source)
  {
    Actor delegate = Actor::DownCast(source.GetTarget());
    Vector3 collisionVector = delegate.GetCurrentProperty< Vector3 >( source.GetTargetProperty() );
    Vector3 ballRelativePosition(mBall.GetCurrentPosition() - mPaddle.GetCurrentPosition());
    ballRelativePosition.Normalize();

    collisionVector.x += ballRelativePosition.x * 0.5f;

    if(mBallVelocity.LengthSquared() < Math::MACHINE_EPSILON_1)
    {
      mBallVelocity += collisionVector * BALL_VELOCITY;
    }
    else
    {
      const float normalVelocity = fabsf(mBallVelocity.Dot(collisionVector));
      mBallVelocity += collisionVector * normalVelocity * 2.0f;
      const float currentSpeed = mBallVelocity.Length();
      const float limitedSpeed = std::min( currentSpeed, MAX_VELOCITY );
      mBallVelocity = mBallVelocity * limitedSpeed / currentSpeed;
    }

    ContinueAnimation();

    // wobble paddle
    mWobbleAnimation = Animation::New(0.5f);
    mWobbleAnimation.AnimateTo( Property( mPaddle, mWobbleProperty ), 1.0f );
    mWobbleAnimation.Play();
    mPaddle.SetProperty(mWobbleProperty, 0.0f);
  }

  /**
   * Notification: Ball hit brick
   * @param source The notification
   */
  void OnHitBrick(PropertyNotification& source)
  {
    Actor brick = Actor::DownCast(source.GetTarget());
    Vector3 collisionVector = brick.GetCurrentProperty< Vector3 >( source.GetTargetProperty() );

    const float normalVelocity = fabsf(mBallVelocity.Dot(collisionVector));
    mBallVelocity += collisionVector * normalVelocity * 2.0f;
    const float currentSpeed = mBallVelocity.Length();
    const float limitedSpeed = std::min( currentSpeed, MAX_VELOCITY );
    mBallVelocity = mBallVelocity * limitedSpeed / currentSpeed;

    ContinueAnimation();

    // remove collision-constraint and notification.
    brick.RemovePropertyNotification(source);
    brick.RemoveConstraints();

    // fade brick (destroy)
    Animation destroyAnimation = Animation::New(0.5f);
    destroyAnimation.AnimateTo( Property( brick, Actor::Property::COLOR_ALPHA ), 0.0f, AlphaFunction::EASE_IN );
    destroyAnimation.Play();
    destroyAnimation.FinishedSignal().Connect( this, &ExampleController::OnBrickDestroyed );
    mDestroyAnimationMap[destroyAnimation] = brick;
  }

  /**
   * Brick Destruction Animation complete.
   * @param[in] source The animation responsible for destroying the brick
   */
  void OnBrickDestroyed( Animation& source )
  {
    // Remove brick from stage, it's constraint and property notification should also remove themselves.
    Actor brick = mDestroyAnimationMap[source];
    mDestroyAnimationMap.erase(source);
    brick.GetParent().Remove(brick);
    mBrickCount--;

    if(!mBrickCount)
    {
      mLevel++;
      LoadLevel(mLevel);
    }
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  Application& mApplication;                            ///< Application instance
  Toolkit::Control mView;                               ///< The View instance.
  Layer mContentLayer;                                  ///< The content layer (contains game actors)
  ImageView mBall;                                      ///< The Moving ball image.
  Vector3 mBallStartPosition;                           ///< Ball Start position
  Vector3 mBallVelocity;                                ///< Ball's current direction.
  Animation mBallAnimation;                             ///< Ball's animation
  Actor mPaddle;                                        ///< The paddle including hit area.
  ImageView mPaddleImage;                               ///< The paddle's image.
  ImageView mPaddleHandle;                              ///< The paddle's handle (where the user touches)
  Vector2 mPaddleHitMargin;                             ///< The paddle hit margin.
  Animation mWobbleAnimation;                           ///< Paddle's animation when hit (wobbles)
  Property::Index mWobbleProperty;                      ///< The wobble property (generated from animation)
  Actor mLevelContainer;                                ///< The level container (contains bricks)
  Property::Map mBrickImageMap;                       ///< The property map used to load the brick

  // actor - dragging functionality

  Animation mDragAnimation;                             ///< Animation for dragging. (grows - affects ACTOR::SCALE)
  Actor mDragActor;                                     ///< The actor which is being dragged (if any)
  Vector3 mRelativeDragPoint;                           ///< The point the user touched, relative to the actor.
  std::map<Animation, Actor> mDestroyAnimationMap;      ///< Keep track of which actors are to be destroyed.
  Vector2 mPaddleFullSize;                              ///< Initial 100% size of the paddle.
  int mLevel;                                           ///< Current level
  int mLives;                                           ///< Total lives.
  int mBrickCount;                                      ///< Total bricks on screen.
};

int DALI_EXPORT_API main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv, DEMO_THEME_PATH);
  ExampleController test(app);
  app.MainLoop();
  return 0;
}
