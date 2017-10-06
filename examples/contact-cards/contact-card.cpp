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

// CLASS HEADER
#include "contact-card.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>

// INTERNAL INCLUDES
#include "contact-card-layout-info.h"
#include "clipped-image.h"

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
    const ContactCardLayoutInfo& contactCardLayoutInfo,
    const std::string& contactName,
    const std::string& contactAddress,
    const std::string& imagePath,
    const Vector2& position )
: mTapDetector(),
  mContactCard(),
  mHeader(),
  mClippedImage(),
  mNameText(),
  mDetailText(),
  mSlotDelegate( this ),
  mContactCardLayoutInfo( contactCardLayoutInfo ),
  foldedPosition( position ),
  mClippedImagePropertyIndex( Property::INVALID_INDEX ),
  mFolded( true )
{
  // Create a control which will be used for the background and to clip the contents
  mContactCard = Control::New();
  mContactCard.SetProperty( Control::Property::BACKGROUND,
                            Property::Map().Add( Toolkit::Visual::Property::TYPE, Visual::COLOR )
                                           .Add( ColorVisual::Property::MIX_COLOR, Color::WHITE ) );
  mContactCard.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN );
  mContactCard.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mContactCard.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mContactCard.SetPosition( foldedPosition.x, foldedPosition.y );
  mContactCard.SetSize( mContactCardLayoutInfo.foldedSize );
  Stage::GetCurrent().Add( mContactCard );

  // Create the header which will be shown only when the contact is unfolded
  mHeader = Control::New();
  mHeader.SetSize( mContactCardLayoutInfo.headerSize );
  mHeader.SetProperty( Control::Property::BACKGROUND,
                       Property::Map().Add( Toolkit::Visual::Property::TYPE, Visual::COLOR )
                                      .Add( ColorVisual::Property::MIX_COLOR, HEADER_COLOR ) );
  mHeader.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mHeader.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mHeader.SetPosition( mContactCardLayoutInfo.headerFoldedPosition.x, mContactCardLayoutInfo.headerFoldedPosition.y );

  // Create a clipped image (whose clipping can be animated)
  mClippedImage = ClippedImage::Create( imagePath, mClippedImagePropertyIndex );
  mClippedImage.SetSize( mContactCardLayoutInfo.imageSize );
  mClippedImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mClippedImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mClippedImage.SetPosition( mContactCardLayoutInfo.imageFoldedPosition.x, mContactCardLayoutInfo.imageFoldedPosition.y );
  mContactCard.Add( mClippedImage );

  // Add the text label for just the name
  mNameText = TextLabel::New( contactName );
  mNameText.SetStyleName( "ContactNameTextLabel" );
  mNameText.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mNameText.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mNameText.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  mNameText.SetPosition( mContactCardLayoutInfo.textFoldedPosition.x, mContactCardLayoutInfo.textFoldedPosition.y );
  mContactCard.Add( mNameText );

  // Create the detail text-label
  std::string detailString( contactName );
  detailString += "\n\n";
  detailString += contactAddress;

  mDetailText = TextLabel::New( detailString );
  mDetailText.SetStyleName( "ContactDetailTextLabel" );
  mDetailText.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mDetailText.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mDetailText.SetPosition( mContactCardLayoutInfo.textFoldedPosition.x, mContactCardLayoutInfo.textFoldedPosition.y );
  mDetailText.SetSize( Vector2( mContactCardLayoutInfo.unfoldedSize.width - mContactCardLayoutInfo.textFoldedPosition.x * 2.0f, 0.0f ) );
  mDetailText.SetOpacity( 0.0f );

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

void ContactCard::OnTap( Actor actor, const TapGesture& gesture )
{
  if( mFolded )
  {
    mContactCard.Add( mHeader );
    mContactCard.Add( mDetailText );

    // Animate the size of the control (and clipping area)
    Animation animation = Animation::New( 0.0f ); // Overall duration is unimportant as superseded by TimePeriods set later
    animation.AnimateTo( Property( mContactCard, Actor::Property::POSITION_X ),  mContactCardLayoutInfo.unfoldedPosition.x,  ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_X );
    animation.AnimateTo( Property( mContactCard, Actor::Property::POSITION_Y ),  mContactCardLayoutInfo.unfoldedPosition.y,  ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_Y );
    animation.AnimateTo( Property( mContactCard, Actor::Property::SIZE_WIDTH ),  mContactCardLayoutInfo.unfoldedSize.width,  ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_WIDTH );
    animation.AnimateTo( Property( mContactCard, Actor::Property::SIZE_HEIGHT ), mContactCardLayoutInfo.unfoldedSize.height, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_HEIGHT );

    // Animate the header area into position
    animation.AnimateTo( Property( mHeader, Actor::Property::POSITION_X ), mContactCardLayoutInfo.headerUnfoldedPosition.x, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_X );
    animation.AnimateTo( Property( mHeader, Actor::Property::POSITION_Y ), mContactCardLayoutInfo.headerUnfoldedPosition.y, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_Y );

    // Animate the clipped image into the unfolded position and into a quad
    animation.AnimateTo( Property( mClippedImage, Actor::Property::POSITION_X ),  mContactCardLayoutInfo.imageUnfoldedPosition.x, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_X );
    animation.AnimateTo( Property( mClippedImage, Actor::Property::POSITION_Y ),  mContactCardLayoutInfo.imageUnfoldedPosition.y, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_Y );
    animation.AnimateTo( Property( mClippedImage, mClippedImagePropertyIndex ), ClippedImage::QUAD_GEOMETRY, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_MESH_MORPH );

    // Fade out the opacity of the name, and animate into the unfolded position
    animation.AnimateTo( Property( mNameText, Actor::Property::COLOR_ALPHA ), 0.0f, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_NAME_OPACITY );
    animation.AnimateTo( Property( mNameText, Actor::Property::POSITION_X ),  mContactCardLayoutInfo.textUnfoldedPosition.x, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_X );
    animation.AnimateTo( Property( mNameText, Actor::Property::POSITION_Y ),  mContactCardLayoutInfo.textUnfoldedPosition.y, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_Y );

    // Fade in the opacity of the detail, and animate into the unfolded position
    animation.AnimateTo( Property( mDetailText, Actor::Property::COLOR_ALPHA ), 1.0f, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_DETAIL_OPACITY );
    animation.AnimateTo( Property( mDetailText, Actor::Property::POSITION_X ),  mContactCardLayoutInfo.textUnfoldedPosition.x, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_X );
    animation.AnimateTo( Property( mDetailText, Actor::Property::POSITION_Y ),  mContactCardLayoutInfo.textUnfoldedPosition.y, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_Y );

    // Fade out all the siblings
    Actor parent = actor.GetParent();
    for( size_t i = 0; i < parent.GetChildCount(); ++i )
    {
      Actor sibling = parent.GetChildAt( i );
      if( sibling != actor )
      {
        animation.AnimateTo( Property( sibling, Actor::Property::COLOR_ALPHA ), 0.0f, ALPHA_FUNCTION_UNFOLD, TIME_PERIOD_UNFOLD_SIBLING_OPACITY );
        sibling.SetSensitive( false );
      }
    }

    animation.FinishedSignal().Connect( mSlotDelegate, &ContactCard::OnAnimationFinished );
    animation.Play();
  }
  else
  {
    mContactCard.Add( mNameText );

    // Animate the size of the control (and clipping area)
    Animation animation = Animation::New( 0.0f ); // Overall duration is unimportant as superseded by TimePeriods set later
    animation.AnimateTo( Property( mContactCard, Actor::Property::POSITION_X ),  foldedPosition.x, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_X );
    animation.AnimateTo( Property( mContactCard, Actor::Property::POSITION_Y ),  foldedPosition.y, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_Y );
    animation.AnimateTo( Property( mContactCard, Actor::Property::SIZE_WIDTH ),  mContactCardLayoutInfo.foldedSize.width,  ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_WIDTH );
    animation.AnimateTo( Property( mContactCard, Actor::Property::SIZE_HEIGHT ), mContactCardLayoutInfo.foldedSize.height, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_HEIGHT );

    // Animate the header area out of position
    animation.AnimateTo( Property( mHeader, Actor::Property::POSITION_X ), mContactCardLayoutInfo.headerFoldedPosition.x, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_X );
    animation.AnimateTo( Property( mHeader, Actor::Property::POSITION_Y ), mContactCardLayoutInfo.headerFoldedPosition.y, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_Y );

    // Animate the clipped image into the folded position and into a circle
    animation.AnimateTo( Property( mClippedImage, Actor::Property::POSITION_X ),  mContactCardLayoutInfo.imageFoldedPosition.x, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_X );
    animation.AnimateTo( Property( mClippedImage, Actor::Property::POSITION_Y ),  mContactCardLayoutInfo.imageFoldedPosition.y, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_Y );
    animation.AnimateTo( Property( mClippedImage, mClippedImagePropertyIndex ), ClippedImage::CIRCLE_GEOMETRY, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_MESH_MORPH );

    // Fade in the opacity of the name, and animate into the folded position
    animation.AnimateTo( Property( mNameText, Actor::Property::COLOR_ALPHA ), 1.0f, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_NAME_OPACITY );
    animation.AnimateTo( Property( mNameText, Actor::Property::POSITION_X ),  mContactCardLayoutInfo.textFoldedPosition.x, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_X );
    animation.AnimateTo( Property( mNameText, Actor::Property::POSITION_Y ),  mContactCardLayoutInfo.textFoldedPosition.y, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_Y );

    // Fade out the opacity of the detail, and animate into the folded position
    animation.AnimateTo( Property( mDetailText, Actor::Property::COLOR_ALPHA ), 0.0f, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_DETAIL_OPACITY );
    animation.AnimateTo( Property( mDetailText, Actor::Property::POSITION_X ),  mContactCardLayoutInfo.textFoldedPosition.x, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_X );
    animation.AnimateTo( Property( mDetailText, Actor::Property::POSITION_Y ),  mContactCardLayoutInfo.textFoldedPosition.y, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_Y );

    // Slowly fade in all the siblings
    Actor parent = actor.GetParent();
    for( size_t i = 0; i < parent.GetChildCount(); ++i )
    {
      Actor sibling = parent.GetChildAt( i );
      if( sibling != actor )
      {
        animation.AnimateTo( Property( sibling, Actor::Property::COLOR_ALPHA ), 1.0f, ALPHA_FUNCTION_FOLD, TIME_PERIOD_FOLD_SIBLING_OPACITY );
        sibling.SetSensitive( true );
      }
    }

    animation.FinishedSignal().Connect( mSlotDelegate, &ContactCard::OnAnimationFinished );
    animation.Play();
  }

  mFolded = !mFolded;
}

void ContactCard::OnAnimationFinished( Dali::Animation& animation )
{
  if( mFolded )
  {
    mHeader.Unparent();
    mDetailText.Unparent();
  }
  else
  {
    mNameText.Unparent();
  }
}
