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
 */

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>
#include <dali/public-api/animation/animation.h>

#include "expanding-buttons-impl.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace Demo
{
namespace Internal
{

namespace
{

const unsigned int GAP_BETWEEN_BUTTONS = 3;

const char* const STYLES_IMAGE = DEMO_IMAGE_DIR "FontStyleButton_Main.png";
const char* const TICK_IMAGE_IMAGE = DEMO_IMAGE_DIR "FontStyleButton_OK_02.png";

/**
 * Unparent the given number of registered controls from the supplied Vector of controls.
 */
void ResetControls( std::vector< WeakHandle< Control > > controls, unsigned int numberOfButtons )
{
  for( unsigned int index = 0; index < numberOfButtons; index++)
  {
    Dali::Toolkit::Control control = controls[index].GetHandle();
    UnparentAndReset( control );
  }
}

} // anonymous namespace


Internal::ExpandingButtons::ExpandingButtons()
: Control( ControlBehaviour( REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mStyleButtonsHidden( false )
{
}

Internal::ExpandingButtons::~ExpandingButtons()
{
}

Demo::ExpandingButtons Internal::ExpandingButtons::New()
{
  IntrusivePtr<Internal::ExpandingButtons> impl = new Internal::ExpandingButtons();
  Demo::ExpandingButtons handle = Demo::ExpandingButtons( *impl );
  impl->Initialize();
  return handle;
}

void ExpandingButtons::OnInitialize()
{
  mExpandButton = PushButton::New();

  mExpandButton.ClickedSignal().Connect( this, &ExpandingButtons::OnExpandButtonClicked );
  mExpandButton.SetProperty( Button::Property::TOGGLABLE, true );
  mExpandButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, STYLES_IMAGE ); // Default for Styles
  mExpandButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, TICK_IMAGE_IMAGE );
  mExpandButton.SetProperty( Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  Self().Add( mExpandButton );
}

void ExpandingButtons::OnRelayout( const Dali::Vector2& targetSize, Dali::RelayoutContainer& container )
{
  mButtonSize = targetSize;
  mExpandButton.SetSize( targetSize );
}

void ExpandingButtons::RegisterButton( Dali::Toolkit::Control& control )
{
  mExpandingControls.push_back( control );
}

void ExpandingButtons::Expand()
{
  if ( !mExpandCollapseAnimation )
  {
    mExpandCollapseAnimation = Animation::New( 0.2f );
    mExpandCollapseAnimation.FinishedSignal().Connect( this, &ExpandingButtons::OnExpandAnimationFinished );
  }

  unsigned int numberOfControls = mExpandingControls.size();

  for( unsigned int index = 0; index < numberOfControls; index++ )
  {
    Dali::Toolkit::Control control = mExpandingControls[index].GetHandle();
    if ( control )
    {
      Self().Add( control );
      AlphaFunction focusedAlphaFunction = AlphaFunction( Vector2 ( 0.32f, 0.08f ), Vector2( 0.38f, 1.72f ) );
      mExpandCollapseAnimation.AnimateTo( Property( control, Actor::Property::POSITION_X ), mButtonSize.width + ( mButtonSize.width + GAP_BETWEEN_BUTTONS ) * (index) , focusedAlphaFunction );
    }
  }
  Self().RaiseToTop();
  mStyleButtonsHidden = false;
  mExpandCollapseAnimation.Play();
}

void ExpandingButtons::OnExpandAnimationFinished( Animation& animation )
{
  if ( mStyleButtonsHidden )
  {
    unsigned int numberOfControls = mExpandingControls.size();
    ResetControls( mExpandingControls, numberOfControls );
    animation.Clear();
    animation.Reset();
  }
}

void ExpandingButtons::Collapse()
{
  Demo::ExpandingButtons handle( GetOwner() );
  mCollapsedSignal.Emit( handle );

  mStyleButtonsHidden = true;
  mExpandButton.SetProperty(Button::Property::SELECTED, false );

  if ( mExpandCollapseAnimation )
  {
    unsigned int numberOfControls = mExpandingControls.size();

    for ( unsigned int index = 0; index < numberOfControls; index++ )
    {
      Dali::Toolkit::Control control = mExpandingControls[index].GetHandle();
      if ( control )
      {
        mExpandCollapseAnimation.AnimateTo( Property( control, Actor::Property::POSITION_X ), 0.0f );
      }
    }
    mExpandCollapseAnimation.Play();
  }
}

// Hide or show (expand) buttons if expand button pressed
bool ExpandingButtons::OnExpandButtonClicked( Toolkit::Button button )
{
  if ( button.GetProperty( Toolkit::Button::Property::SELECTED ).Get<bool>() )
  {
    Expand();
  }
  else
  {
    Collapse();
  }

  return true;
}


Demo::ExpandingButtons::ExpandingButtonsSignalType& ExpandingButtons::CollapsingSignal()
{
  return mCollapsedSignal;
}

} // Internal
} // Demo
