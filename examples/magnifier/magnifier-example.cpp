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

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "shared/view.h"

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/magnifier/magnifier.h>
using namespace Dali;

namespace
{
const char* BACKGROUND_IMAGE( DEMO_IMAGE_DIR "background-magnifier.jpg" );
const char* TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char* APPLICATION_TITLE( "Magnifier Example" );
const Vector3 MAGNIFIER_SIZE(0.25f, 0.25f, 0.0f);       ///< Magnifier sides should be 25% of the width of the stage
const float ANIMATION_DURATION(60.0f);                  ///< Run animation for a minute before repeating.
const float MAGNIFIER_DISPLAY_DURATION(0.125f);         ///< Duration in seconds for show/hide manual magnifier animation

const float MAGNIFICATION_FACTOR(2.0f);                 ///< Amount to magnify by.
const float MAGNIFIER_INDENT(10.0f);                    ///< Indentation around edge of stage to define where magnifiers may move.
const float FINGER_RADIUS_INCHES(0.25f);                ///< Average finger radius in inches from the center of index finger to edge.

/**
 * MagnifierPathConstraint
 * This constraint governs the position of the
 * animating magnifier in a swirly pattern around
 * the stage.
 */
struct MagnifierPathConstraint
{
  /**
   * Constraint constructor
   * @param[in] stageSize The stage size so that the constraint can create a path
   * within stage bounds.
   */
  MagnifierPathConstraint(const Vector3& stageSize,
                          Vector3 offset = Vector3::ZERO)
  : mStageSize(stageSize),
    mOffset(offset)
  {
  }

  void operator()( Vector3& current, const PropertyInputContainer& inputs )
  {
    float time = inputs[1]->GetFloat();
    const Vector3& size = inputs[0]->GetVector3();

    current = mOffset;

    Vector3 range( mStageSize - size - Vector3::ONE * MAGNIFIER_INDENT * 2.0f );
    current.x += 0.5f * sinf(time * 0.471f) * range.width;
    current.y += 0.5f * sinf(time * 0.8739f) * range.height;
  }

  Vector3 mStageSize;     ///< Keep track of the stage size for determining path within stage bounds
  Vector3 mOffset;        ///< Amount to offset magnifier path
};

/**
 * Confine Actor to boundaries of reference actor (e.g. Parent)
 * Actor bounds (top-left position + size) are confined to reference Actor's
 * bounds.
 */
struct ConfinementConstraint
{
  /**
   * Confinement constraint constructor.
   * @param[in] offsetOrigin (optional) Whether to offset the parent origin or not.
   * @param[in] topLeftMargin (optional) Top-Left margins (defaults to 0.0f, 0.0f)
   * @param[in] bottomRightMargin (optional) Bottom-Right margins (defaults to 0.0f, 0.0f)
   * @param[in] flipHorizontal (optional) whether to flip Actor to the other side X if near edge, and by
   * how much (defaults to 0.0f i.e. no flip)
   * @param[in] flipVertical (optional) whether to flip Actor to the other side Y if near edge, and by
   * how much (defaults to 0.0f i.e. no flip)
   */
  ConfinementConstraint(Vector3 offsetOrigin = Vector3::ZERO, Vector2 topLeftMargin = Vector2::ZERO, Vector2 bottomRightMargin = Vector2::ZERO, bool flipHorizontal = false, bool flipVertical = false)
  : mOffsetOrigin(offsetOrigin),
    mMinIndent(topLeftMargin),
    mMaxIndent(bottomRightMargin),
    mFlipHorizontal(flipHorizontal),
    mFlipVertical(flipVertical)
  {
  }

  void operator()( Vector3& current, const PropertyInputContainer& inputs )
  {
    const Vector3& size = inputs[0]->GetVector3();
    const Vector3 origin = inputs[1]->GetVector3();
    const Vector3& anchor = inputs[2]->GetVector3();
    const Vector3& referenceSize = inputs[3]->GetVector3();

    Vector3 offset(mOffsetOrigin * referenceSize);

    // Get actual position of Actor relative to parent's Top-Left.
    Vector3 position(current + offset + origin * referenceSize);

    current += offset;

    // if top-left corner is outside of Top-Left bounds, then push back in screen.
    Vector3 corner(position - size * anchor - mMinIndent);

    if(mFlipHorizontal && corner.x < 0.0f)
    {
      corner.x = 0.0f;
      current.x += size.width;
    }

    if(mFlipVertical && corner.y < 0.0f)
    {
      corner.y = 0.0f;
      current.y += size.height;
    }

    current.x -= std::min(corner.x, 0.0f);
    current.y -= std::min(corner.y, 0.0f);

    // if bottom-right corner is outside of Bottom-Right bounds, then push back in screen.
    corner += size - referenceSize + mMinIndent + mMaxIndent;

    if(mFlipHorizontal && corner.x > 0.0f)
    {
      corner.x = 0.0f;
      current.x -= size.width;
    }

    if(mFlipVertical && corner.y > 0.0f)
    {
      corner.y = 0.0f;
      current.y -= size.height;
    }

    current.x -= std::max(corner.x, 0.0f);
    current.y -= std::max(corner.y, 0.0f);
  }

  Vector3 mOffsetOrigin;                                ///< Manual Parent Offset Origin.
  Vector3 mMinIndent;                                   ///< Top-Left Margin
  Vector3 mMaxIndent;                                   ///< Bottom-Right Margin.
  bool mFlipHorizontal;                                 ///< Whether to flip actor's position if exceeds horizontal screen bounds
  bool mFlipVertical;                                   ///< Whether to flip actor's position if exceeds vertical screen bounds
};

}

// This example shows how to use the Magnifier component.
//
class ExampleController : public ConnectionTracker
{
public:

  /**
   * The example controller constructor.
   * @param[in] application The application instance
   */
  ExampleController( Application& application )
  : mApplication( application ),
    mView(),
    mAnimationTime(0.0f),
    mAnimationTimeProperty( Property::INVALID_INDEX ),
    mMagnifierShown(false)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ExampleController::Create );
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
  void Create( Application& application )
  {
    Stage::GetCurrent().KeyEventSignal().Connect(this, &ExampleController::OnKeyEvent);

    mStageSize = Stage::GetCurrent().GetSize();

    // The Init signal is received once (only) during the Application lifetime

    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Toolkit::ToolBar toolBar;
    mContent = DemoHelper::CreateView( application,
                                       mView,
                                       toolBar,
                                       BACKGROUND_IMAGE,
                                       TOOLBAR_IMAGE,
                                       APPLICATION_TITLE );

    mContent.SetLeaveRequired(true);
    mContent.TouchSignal().Connect( this, &ExampleController::OnTouched );

    // Create magnifier (controlled by human touch)
    Layer overlay = Layer::New();
    overlay.SetSensitive(false);
    overlay.SetParentOrigin( ParentOrigin::CENTER );
    overlay.SetSize(mStageSize);
    Stage::GetCurrent().Add(overlay);

    mMagnifier = Toolkit::Magnifier::New();
    mMagnifier.SetSourceActor( mView );
    mMagnifier.SetSize( MAGNIFIER_SIZE * mStageSize.width );  // Size of magnifier is in relation to stage width
    mMagnifier.SetProperty( Toolkit::Magnifier::Property::MAGNIFICATION_FACTOR, MAGNIFICATION_FACTOR );
    mMagnifier.SetScale(Vector3::ZERO);
    overlay.Add( mMagnifier );

    // Apply constraint to animate the position of the magnifier.
    Constraint constraint = Constraint::New<Vector3>( mMagnifier, Actor::Property::POSITION, ConfinementConstraint(Vector3( 0.5f, 0.5f, 0.0f ), Vector2::ONE * MAGNIFIER_INDENT, Vector2::ONE * MAGNIFIER_INDENT) );
    constraint.AddSource( LocalSource(Actor::Property::SIZE) );
    constraint.AddSource( LocalSource(Actor::Property::PARENT_ORIGIN) );
    constraint.AddSource( LocalSource(Actor::Property::ANCHOR_POINT) );
    constraint.AddSource( ParentSource(Actor::Property::SIZE) );
    constraint.SetRemoveAction(Constraint::Discard);
    constraint.Apply();

    // Create bouncing magnifier automatically bounces around screen.
    mBouncingMagnifier = Toolkit::Magnifier::New();
    mBouncingMagnifier.SetSourceActor( mView );
    mBouncingMagnifier.SetSize( MAGNIFIER_SIZE * mStageSize.width ); // Size of magnifier is in relation to stage width
    mBouncingMagnifier.SetProperty( Toolkit::Magnifier::Property::MAGNIFICATION_FACTOR, MAGNIFICATION_FACTOR );
    overlay.Add( mBouncingMagnifier );

    mAnimationTimeProperty = mBouncingMagnifier.RegisterProperty("animationTime",  0.0f);
    ContinueAnimation();

    // Apply constraint to animate the position of the magnifier.
    constraint = Constraint::New<Vector3>( mBouncingMagnifier, Actor::Property::POSITION, MagnifierPathConstraint(mStageSize, mStageSize * 0.5f) );
    constraint.AddSource( LocalSource(Actor::Property::SIZE) );
    constraint.AddSource( LocalSource(mAnimationTimeProperty) );
    constraint.Apply();

    // Apply constraint to animate the source of the magnifier.
    constraint = Constraint::New<Vector3>( mBouncingMagnifier, Toolkit::Magnifier::Property::SOURCE_POSITION, MagnifierPathConstraint(mStageSize) );
    constraint.AddSource( LocalSource(Actor::Property::SIZE) );
    constraint.AddSource( LocalSource(mAnimationTimeProperty) );
    constraint.Apply();
  }

  /**
   * Invoked whenever the animation finishes (every 60 seconds)
   * @param[in] animation The animation
   */
  void OnAnimationFinished( Animation& animation )
  {
    animation.FinishedSignal().Disconnect(this, &ExampleController::OnAnimationFinished);
    animation.Clear();
    ContinueAnimation();
  }

  /**
   * Resumes animation for another ANIMATION_DURATION seconds.
   */
  void ContinueAnimation()
  {
    Animation animation = Animation::New(ANIMATION_DURATION);
    mAnimationTime += ANIMATION_DURATION;
    animation.AnimateTo( Property(mBouncingMagnifier, mAnimationTimeProperty), mAnimationTime );
    animation.Play();
    animation.FinishedSignal().Connect(this, &ExampleController::OnAnimationFinished);
  }

  /**
   * Invoked whenever the quit button is clicked
   * @param[in] button the quit button
   */
  bool OnQuitButtonClicked( Toolkit::Button button )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  /**
   * Invoked whenever the content (screen) is touched
   * @param[in] actor The actor that received the touch
   * @param[in] event The touch-event information
   */
  bool OnTouched( Actor actor, const TouchData& event )
  {
    if(event.GetPointCount() > 0)
    {
      switch( event.GetState( 0 ) )
      {
        case PointState::DOWN:
        case PointState::MOTION:
        {
          ShowMagnifier();
          break;
        }
        case PointState::UP:
        case PointState::LEAVE:
        case PointState::INTERRUPTED:
        {
          HideMagnifier();
          break;
        }
        case PointState::STATIONARY:
        {
          break;
        }
      } // end switch

      Vector3 touchPoint( event.GetScreenPosition( 0 ) );

      SetMagnifierPosition(touchPoint - mStageSize * 0.5f);
    }

    return false;
  }

  /**
   * Shows the magnifier
   */
  void ShowMagnifier()
  {
    if(!mMagnifierShown)
    {
      Animation animation = Animation::New(MAGNIFIER_DISPLAY_DURATION);
      animation.AnimateTo(Property(mMagnifier, Actor::Property::SCALE), Vector3::ONE, AlphaFunction::EASE_IN);
      animation.Play();
      mMagnifierShown = true;
    }
  }

  /**
   * Hides the magnifier
   */
  void HideMagnifier()
  {
    if(mMagnifierShown)
    {
      Animation animation = Animation::New(MAGNIFIER_DISPLAY_DURATION);
      animation.AnimateTo(Property(mMagnifier, Actor::Property::SCALE), Vector3::ZERO, AlphaFunction::EASE_OUT);
      animation.Play();
      mMagnifierShown = false;
    }
  }

  /**
   * Manually sets the magnifier position
   * @param[in] position The magnifier's position relative to center of stage
   */
  void SetMagnifierPosition(const Vector3 position)
  {
    mMagnifier.SetProperty( Toolkit::Magnifier::Property::SOURCE_POSITION, position );

    // position magnifier glass such that bottom edge is touching/near top of finger.
    Vector3 glassPosition(position);
    glassPosition.y -= mStageSize.width * MAGNIFIER_SIZE.height * 0.5f + Stage::GetCurrent().GetDpi().height * FINGER_RADIUS_INCHES;

    mMagnifier.SetPosition( glassPosition );
  }

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

  Application&  mApplication;                             ///< Application instance
  Toolkit::Control mView;                                 ///< The view
  Layer mContent;                                         ///< The content layer
  Toolkit::Magnifier mMagnifier;                          ///< The manually controlled magnifier
  Toolkit::Magnifier mBouncingMagnifier;                  ///< The animating magnifier (swirly animation)
  Vector3 mStageSize;                                     ///< The size of the stage
  float mAnimationTime;                                   ///< Keep track of start animation time.
  Property::Index mAnimationTimeProperty;                 ///< Animation time property (responsible for swirly animation)
  bool mMagnifierShown;                                   ///< Flag indicating whether the magnifier is being shown or not.

};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  ExampleController test( application );
  application.MainLoop();
  return 0;
}
