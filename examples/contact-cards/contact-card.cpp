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

// CLASS HEADER
#include "contact-card.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>

// INTERNAL INCLUDES
#include "contact-card-layout-info.h"
#include "clipped-image.h"
#include "masked-image.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
/*
 * The constants below are used to create the following Unfold Animation.
 *
 * 0ms  50  100  150  200  250  300  350  400 Total Animation time in Milliseconds
 * |    |    |    |    |    |    |    |    |
 * o-----------------------------------o   |  X Position Animation            ( 0ms To 360ms)
 * |   o-----------------------------------o  Y Position Animation            (40ms To 400ms)
 * o-----------------------------------o   |  Width Animation                 ( 0ms To 360ms)
 * |   o-----------------------------------o  Height Animation                (40ms To 400ms)
 * o-------o |    |    |    |    |    |    |  Fade out Name Text Animation    ( 0ms To  80ms)
 * |       o-------o   |    |    |    |    |  Fade in Details Text Animation  (80ms To 160ms)
 * o---------------o   |    |    |    |    |  Fade out other cards Animation  ( 0ms To 160ms)
 * o---------------------------------------o  Mesh Morph Animation            ( 0ms To 400ms)
 */
const TimePeriod TIME_PERIOD_UNFOLD_X( 0.0f,  0.36f ); ///< Start at 0ms, duration 360ms
const TimePeriod TIME_PERIOD_UNFOLD_Y( 0.04f, 0.36f ); ///< Start at 40ms, duration 360ms
const TimePeriod TIME_PERIOD_UNFOLD_WIDTH( 0.0f,  0.36f ); ///< Start at 0ms, duration 360ms
const TimePeriod TIME_PERIOD_UNFOLD_HEIGHT( 0.04f, 0.36f ); ///< Start at 40ms, duration 360ms
const TimePeriod TIME_PERIOD_UNFOLD_NAME_OPACITY( 0.0f, 0.08f ); ///< Start at 0ms, duration 80ms
const TimePeriod TIME_PERIOD_UNFOLD_DETAIL_OPACITY( 0.08f, 0.08f ); ///< Start at 80ms, duration 80ms
const TimePeriod TIME_PERIOD_UNFOLD_SIBLING_OPACITY( 0.0f, 0.08f ); ///< Start at 0ms, duration 80ms
const TimePeriod TIME_PERIOD_UNFOLD_MESH_MORPH( 0.0f, 0.4f ); ///< Start at 0ms, duration 400ms

/*
 * The constants below are used to create the following Fold Animation:
 *
 * 0ms  50  100  150  200  250  300  350  400 Total Animation time in Milliseconds
 * |    |    |    |    |    |    |    |    |
 * |    |o---------------------------------o  X Position Animation            ( 64ms To 400ms)
 * o---------------------------------o|    |  Y Position Animation            (  0ms To 336ms)
 * |    |o---------------------------------o  Width Animation                 ( 64ms To 400ms)
 * o---------------------------------o|    |  Height Animation                (  0ms To 336ms)
 * |       o-------o   |    |    |    |    |  Fade in Name Text animation     ( 80ms To 160ms)
 * o-------o |    |    |    |    |    |    |  Fade out Details Text animation (  0ms To  80ms)
 * |    |    |    |    |    |    | o-------o  Fade in other cards animation   (320ms To 400ms)
 * o---------------------------------------o  Morph Animation                 (  0ms To 400ms)
 */
const TimePeriod TIME_PERIOD_FOLD_X( 0.064f, 0.336f ); ///< Start at 64ms, duration 336ms
const TimePeriod TIME_PERIOD_FOLD_Y( 0.0f, 0.336f ); ///< Start at 0ms, duration 336ms
const TimePeriod TIME_PERIOD_FOLD_WIDTH( 0.064f, 0.336f ); ///< Start at 64ms, duration 336ms
const TimePeriod TIME_PERIOD_FOLD_HEIGHT( 0.0f, 0.336f ); ///< Start at 0ms, duration 336ms
const TimePeriod TIME_PERIOD_FOLD_NAME_OPACITY( 0.08f, 0.08f ); ///< Start at 80ms, duration 80ms
const TimePeriod TIME_PERIOD_FOLD_DETAIL_OPACITY( 0.0f, 0.08f ); ///< Start at 0ms, duration 80ms
const TimePeriod TIME_PERIOD_FOLD_SIBLING_OPACITY( 0.32f, 0.08f ); ///< Start at 320ms, duration 80ms
const TimePeriod TIME_PERIOD_FOLD_MESH_MORPH( 0.0f, 0.4f ); ///< Start at 0ms, duration 400ms

AlphaFunction ALPHA_FUNCTION_UNFOLD( AlphaFunction::DEFAULT ); ///< Alpha function used for the Unfold Animation
AlphaFunction ALPHA_FUNCTION_FOLD( AlphaFunction::EASE_IN_OUT ); ///< Alpha function used for the Fold Animation

const Vector4 HEADER_COLOR( 231.0f/255.0f, 231.0f/255.0f, 231.0f/255.0f, 1.0f ); ///< The color of the header

} // unnamed namespace

ContactCard::ContactCard(
    Dali::Window window,
    const ContactCardLayoutInfo& contactCardLayoutInfo,
    const std::string& contactName,
    const std::string& contactAddress,
    const std::string& imagePath,
    const Vector2& position )
: mTapDetector(),
  mContactCard(),
  mHeader(),
  mClippedImage(),
  mMaskedImage(),
  mNameText(),
  mDetailText(),
  mAnimation(),
  mSlotDelegate( this ),
  mContactCardLayoutInfo( contactCardLayoutInfo ),
  foldedPosition( position ),
  mClippedImagePropertyIndex( Property::INVALID_INDEX ),
  mFolded( true )
{
  // Connect to the window's key signal to allow Back and Escape to fold a contact card if it is unfolded
  window.KeyEventSignal().Connect( mSlotDelegate, &ContactCard::OnKeyEvent );

  // Create a control which will be used for the background and to clip the contents
  mContactCard = Control::New();
  mContactCard.SetProperty( Control::Property::BACKGROUND,
                            Property::Map{ { Toolkit::Visual::Property::TYPE, Visual::COLOR },
                                           { ColorVisual::Property::MIX_COLOR, Color::WHITE } } );
  mContactCard.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN );
  mContactCard.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  mContactCard.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  mContactCard.SetProperty( Actor::Property::POSITION, Vector2( foldedPosition.x, foldedPosition.y ));
  mContactCard.SetProperty( Actor::Property::SIZE, mContactCardLayoutInfo.foldedSize );
  window.Add( mContactCard );

  // Create the header which will be shown only when the contact is unfolded
  mHeader = Control::New();
  mHeader.SetProperty( Actor::Property::SIZE, mContactCardLayoutInfo.headerSize );
  mHeader.SetProperty( Control::Property::BACKGROUND,
                       Property::Map{ { Toolkit::Visual::Property::TYPE, Visual::COLOR },
                                      { ColorVisual::Property::MIX_COLOR, HEADER_COLOR } } );
  mHeader.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  mHeader.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  mHeader.SetProperty( Actor::Property::POSITION, Vector2( mContactCardLayoutInfo.headerFoldedPosition.x, mContactCardLayoutInfo.headerFoldedPosition.y ));

  // Create a clipped image (whose clipping can be animated)
  mClippedImage = ClippedImage::Create( imagePath, mClippedImagePropertyIndex );
  mClippedImage.SetProperty( Actor::Property::SIZE, mContactCardLayoutInfo.imageSize );
  mClippedImage.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  mClippedImage.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  mClippedImage.SetProperty( Actor::Property::POSITION, Vector2( mContactCardLayoutInfo.imageFoldedPosition.x, mContactCardLayoutInfo.imageFoldedPosition.y ));
  mClippedImage.SetProperty( Actor::Property::VISIBLE, false ); // Hide image as we only want to display it if we are animating or unfolded
  mContactCard.Add( mClippedImage );

  // Create an image with a mask which is to be used when the contact is folded
  mMaskedImage = MaskedImage::Create( imagePath );
  mMaskedImage.SetProperty( Actor::Property::SIZE, mContactCardLayoutInfo.imageSize );
  mMaskedImage.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  mMaskedImage.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  mMaskedImage.SetProperty( Actor::Property::POSITION, Vector2( mContactCardLayoutInfo.imageFoldedPosition.x, mContactCardLayoutInfo.imageFoldedPosition.y ));
  mContactCard.Add( mMaskedImage );

  // Add the text label for just the name
  mNameText = TextLabel::New( contactName );
  mNameText.SetStyleName( "ContactNameTextLabel" );
  mNameText.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  mNameText.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  mNameText.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  mNameText.SetProperty( Actor::Property::POSITION, Vector2( mContactCardLayoutInfo.textFoldedPosition.x, mContactCardLayoutInfo.textFoldedPosition.y ));
  mContactCard.Add( mNameText );

  // Create the detail text-label
  std::string detailString( contactName );
  detailString += "\n\n";
  detailString += contactAddress;

  mDetailText = TextLabel::New( detailString );
  mDetailText.SetStyleName( "ContactDetailTextLabel" );
  mDetailText.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  mDetailText.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  mDetailText.SetProperty( Actor::Property::POSITION, Vector2( mContactCardLayoutInfo.textFoldedPosition.x, mContactCardLayoutInfo.textFoldedPosition.y ));
  mDetailText.SetProperty( Actor::Property::SIZE, Vector2( mContactCardLayoutInfo.unfoldedSize.width - mContactCardLayoutInfo.textFoldedPosition.x * 2.0f, 0.0f ) );
  mDetailText.SetProperty( Actor::Property::OPACITY, 0.0f );

  // Attach tap detection to the overall clip control
  mTapDetector = TapGestureDetector::New();
  mTapDetector.Attach( mContactCard );
  mTapDetector.DetectedSignal().Connect( mSlotDelegate, &ContactCard::OnTap );
}

ContactCard::~ContactCard()
{
  if( mContactCard )
  {
    mContactCard.Unparent();
  }
}

void ContactCard::OnTap( Actor actor, const TapGesture& /* gesture */ )
{
  if( actor == mContactCard )
  {
    Animate();
  }
}

void ContactCard::Animate()
{
  KeyInputFocusManager keyInputFocusManager = KeyInputFocusManager::Get();

  mAnimation = Animation::New( 0.0f ); // Overall duration is unimportant as superseded by TimePeriods set later

  if( mFolded )
  {
    // Set key-input-focus to our contact-card so that we can fold the contact-card if we receive a Back or Esc key
    keyInputFocusManager.SetFocus( mContactCard );

    mContactCard.Add( mHeader );
    mContactCard.Add( mDetailText );

    // Show clipped-image to animate geometry and hide the masked-image
    mClippedImage.SetProperty( Actor::Property::VISIBLE, true );
    mMaskedImage.SetProperty( Actor::Property::VISIBLE, false );

    // Animate the size of the control (and clipping area)
    mAnimation.AnimateTo( Property( mContactCard, Actor::Property::POSITION_X ),  mContactCardLayoutInfo.unfoldedPosition.x,  ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_X );
    mAnimation.AnimateTo( Property( mContactCard, Actor::Property::POSITION_Y ),  mContactCardLayoutInfo.unfoldedPosition.y,  ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_Y );
    mAnimation.AnimateTo( Property( mContactCard, Actor::Property::SIZE_WIDTH ),  mContactCardLayoutInfo.unfoldedSize.width,  ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_WIDTH );
    mAnimation.AnimateTo( Property( mContactCard, Actor::Property::SIZE_HEIGHT ), mContactCardLayoutInfo.unfoldedSize.height, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_HEIGHT );

    // Animate the header area into position
    mAnimation.AnimateTo( Property( mHeader, Actor::Property::POSITION_X ), mContactCardLayoutInfo.headerUnfoldedPosition.x, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_X );
    mAnimation.AnimateTo( Property( mHeader, Actor::Property::POSITION_Y ), mContactCardLayoutInfo.headerUnfoldedPosition.y, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_Y );

    // Animate the clipped image into the unfolded position and into a quad
    mAnimation.AnimateTo( Property( mClippedImage, Actor::Property::POSITION_X ),  mContactCardLayoutInfo.imageUnfoldedPosition.x, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_X );
    mAnimation.AnimateTo( Property( mClippedImage, Actor::Property::POSITION_Y ),  mContactCardLayoutInfo.imageUnfoldedPosition.y, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_Y );
    mAnimation.AnimateTo( Property( mClippedImage, mClippedImagePropertyIndex ), ClippedImage::QUAD_GEOMETRY, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_MESH_MORPH );

    // Fade out the opacity of the name, and animate into the unfolded position
    mAnimation.AnimateTo( Property( mNameText, Actor::Property::COLOR_ALPHA ), 0.0f, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_NAME_OPACITY );
    mAnimation.AnimateTo( Property( mNameText, Actor::Property::POSITION_X ),  mContactCardLayoutInfo.textUnfoldedPosition.x, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_X );
    mAnimation.AnimateTo( Property( mNameText, Actor::Property::POSITION_Y ),  mContactCardLayoutInfo.textUnfoldedPosition.y, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_Y );

    // Fade in the opacity of the detail, and animate into the unfolded position
    mAnimation.AnimateTo( Property( mDetailText, Actor::Property::COLOR_ALPHA ), 1.0f, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_DETAIL_OPACITY );
    mAnimation.AnimateTo( Property( mDetailText, Actor::Property::POSITION_X ),  mContactCardLayoutInfo.textUnfoldedPosition.x, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_X );
    mAnimation.AnimateTo( Property( mDetailText, Actor::Property::POSITION_Y ),  mContactCardLayoutInfo.textUnfoldedPosition.y, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_Y );

    // Fade out all the siblings
    Actor parent = mContactCard.GetParent();
    for( size_t i = 0; i < parent.GetChildCount(); ++i )
    {
      Actor sibling = parent.GetChildAt( i );
      if( sibling != mContactCard )
      {
        mAnimation.AnimateTo( Property( sibling, Actor::Property::COLOR_ALPHA ), 0.0f, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_SIBLING_OPACITY );
        sibling.SetProperty( Actor::Property::SENSITIVE, false );
      }
    }

    mAnimation.FinishedSignal().Connect( mSlotDelegate, &ContactCard::OnAnimationFinished );
    mAnimation.Play();
  }
  else
  {
    // Remove key-input-focus from our contact-card when we are folded
    keyInputFocusManager.RemoveFocus( mContactCard );

    mContactCard.Add( mNameText );

    // Animate the size of the control (and clipping area)
    mAnimation.AnimateTo( Property( mContactCard, Actor::Property::POSITION_X ),  foldedPosition.x, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_X );
    mAnimation.AnimateTo( Property( mContactCard, Actor::Property::POSITION_Y ),  foldedPosition.y, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_Y );
    mAnimation.AnimateTo( Property( mContactCard, Actor::Property::SIZE_WIDTH ),  mContactCardLayoutInfo.foldedSize.width,  ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_WIDTH );
    mAnimation.AnimateTo( Property( mContactCard, Actor::Property::SIZE_HEIGHT ), mContactCardLayoutInfo.foldedSize.height, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_HEIGHT );

    // Animate the header area out of position
    mAnimation.AnimateTo( Property( mHeader, Actor::Property::POSITION_X ), mContactCardLayoutInfo.headerFoldedPosition.x, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_X );
    mAnimation.AnimateTo( Property( mHeader, Actor::Property::POSITION_Y ), mContactCardLayoutInfo.headerFoldedPosition.y, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_Y );

    // Animate the clipped image into the folded position and into a circle
    mAnimation.AnimateTo( Property( mClippedImage, Actor::Property::POSITION_X ),  mContactCardLayoutInfo.imageFoldedPosition.x, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_X );
    mAnimation.AnimateTo( Property( mClippedImage, Actor::Property::POSITION_Y ),  mContactCardLayoutInfo.imageFoldedPosition.y, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_Y );
    mAnimation.AnimateTo( Property( mClippedImage, mClippedImagePropertyIndex ), ClippedImage::CIRCLE_GEOMETRY, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_MESH_MORPH );

    // Fade in the opacity of the name, and animate into the folded position
    mAnimation.AnimateTo( Property( mNameText, Actor::Property::COLOR_ALPHA ), 1.0f, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_NAME_OPACITY );
    mAnimation.AnimateTo( Property( mNameText, Actor::Property::POSITION_X ),  mContactCardLayoutInfo.textFoldedPosition.x, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_X );
    mAnimation.AnimateTo( Property( mNameText, Actor::Property::POSITION_Y ),  mContactCardLayoutInfo.textFoldedPosition.y, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_Y );

    // Fade out the opacity of the detail, and animate into the folded position
    mAnimation.AnimateTo( Property( mDetailText, Actor::Property::COLOR_ALPHA ), 0.0f, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_DETAIL_OPACITY );
    mAnimation.AnimateTo( Property( mDetailText, Actor::Property::POSITION_X ),  mContactCardLayoutInfo.textFoldedPosition.x, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_X );
    mAnimation.AnimateTo( Property( mDetailText, Actor::Property::POSITION_Y ),  mContactCardLayoutInfo.textFoldedPosition.y, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_Y );

    // Slowly fade in all the siblings
    Actor parent = mContactCard.GetParent();
    for( size_t i = 0; i < parent.GetChildCount(); ++i )
    {
      Actor sibling = parent.GetChildAt( i );
      if( sibling != mContactCard )
      {
        mAnimation.AnimateTo( Property( sibling, Actor::Property::COLOR_ALPHA ), 1.0f, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_SIBLING_OPACITY );
        sibling.SetProperty( Actor::Property::SENSITIVE, true );
      }
    }

    mAnimation.FinishedSignal().Connect( mSlotDelegate, &ContactCard::OnAnimationFinished );
    mAnimation.Play();
  }

  mFolded = !mFolded;
}

void ContactCard::OnAnimationFinished( Animation& animation )
{
  // Ensure the finishing animation is the latest as we do not want to change state if a previous animation has finished
  if( mAnimation == animation )
  {
    if( mFolded )
    {
      mHeader.Unparent();
      mDetailText.Unparent();

      // Hide the clipped-image as we have finished animating the geometry and show the masked-image again
      mClippedImage.SetProperty( Actor::Property::VISIBLE, false );
      mMaskedImage.SetProperty( Actor::Property::VISIBLE, true );
    }
    else
    {
      mNameText.Unparent();
    }
    mAnimation.Reset();
  }
}

void ContactCard::OnKeyEvent( const KeyEvent& event )
{
  if( ( ! mFolded ) && // If we're folded then there's no need to do any more checking
      ( event.GetState() == KeyEvent::Down ) )
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
    {
      KeyInputFocusManager keyInputFocusManager = KeyInputFocusManager::Get();
      if( keyInputFocusManager.GetCurrentFocusControl() == mContactCard )
      {
        // Our contact-card is set to receive focus and we're unfolded so animate back to the folded state
        Animate();
      }
    }
  }
}
