/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include "edit-layout-impl.h"

// INTERNAL INCLUDES
#include "edit-layout.h"

namespace
{

const float INNER_BORDER_TOP = 4.0f;
const float INNER_BORDER_LEFT = 20.0f;
const float INNER_BORDER_RIGHT = 20.0f;

}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

Toolkit::EditLayout EditLayout::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< EditLayout > impl = new EditLayout();

  // Pass ownership to CustomActor handle
  Toolkit::EditLayout handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void EditLayout::SetTopPanel( Dali::Toolkit::Control panel )
{
  mTopPanel = panel;
  mTopPanel.SetParentOrigin( ParentOrigin::TOP_CENTER );
  mTopPanel.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  mTopPanel.SetY( INNER_BORDER_TOP );

  Self().Add( panel );
}

EditLayout::EditLayout()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_NONE ) )
{
}

EditLayout::~EditLayout()
{
}

void EditLayout::OnInitialize()
{
  CustomActor self = Self();

  // Move background behind text label
  Dali::Toolkit::Control::DownCast( self ).SetBackgroundColor( Color::BLUE );
  self.GetChildAt(0).SetZ(-1.0f);
}

void EditLayout::OnRelayout( const Vector2& size, ActorSizeContainer& container )
{
  CustomActor self = Self();

  if( self.GetChildCount() > 0 )
  {
    if( mTopPanel )
    {
      float panelWidth = size.width - INNER_BORDER_LEFT - INNER_BORDER_RIGHT;

      float height = mTopPanel.GetHeightForWidth( panelWidth );

      container.push_back( ActorSizePair( mTopPanel, Vector2(panelWidth, height) ) );
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
