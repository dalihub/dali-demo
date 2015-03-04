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
#include "center-layout-impl.h"

// INTERNAL INCLUDES
#include "center-layout.h"

namespace
{
  int ConvertToEven(int value)
  {
    return (value % 2 == 0) ? value : (value + 1);
  }
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

Toolkit::CenterLayout CenterLayout::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< CenterLayout > impl = new CenterLayout();

  // Pass ownership to CustomActor handle
  Toolkit::CenterLayout handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

CenterLayout::CenterLayout()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_NONE ) )
{
}

CenterLayout::~CenterLayout()
{
}

void CenterLayout::OnInitialize()
{
  CustomActor self = Self();

  // Move background behind text label
  Dali::Toolkit::Control::DownCast( self ).SetBackgroundColor( Vector4(0.1f,0.1f,0.1f,1.0f) );
  self.GetChildAt(0).SetZ(-1.0f);

  Stage stage = Stage::GetCurrent();
  Vector2 stageSize = stage.GetSize();

  // Resize the center layout when the corner is grabbed
  mGrabCorner = CreateSolidColorActor( Color::GREEN );
  mGrabCorner.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
  mGrabCorner.SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );
  mGrabCorner.SetSize( stageSize.width*0.1f, stageSize.width*0.1f );
  mGrabCorner.SetZ(1.0f);
  self.Add( mGrabCorner );

  mPanGestureDetector = PanGestureDetector::New();
  mPanGestureDetector.Attach( mGrabCorner );
  mPanGestureDetector.DetectedSignal().Connect( this, &CenterLayout::OnPan );
}

void CenterLayout::OnRelayout( const Vector2& size, ActorSizeContainer& container )
{
  CustomActor self = Self();

  if( mLayoutSize.x <= 0.0f )
  {
    mLayoutSize = size;
  }

  for( unsigned int i=0; i<self.GetChildCount(); ++i )
  {
    Dali::Toolkit::Control child = Dali::Toolkit::Control::DownCast( self.GetChildAt(i) );

    if( child )
    {
      child.SetParentOrigin( ParentOrigin::TOP_CENTER );
      child.SetAnchorPoint( AnchorPoint::TOP_CENTER );

      float height = child.GetHeightForWidth( size.width );

      container.push_back( ActorSizePair( child, Vector2( size.width, std::min(height, size.height) ) ) );
    }
  }
}

void CenterLayout::OnPan( Actor actor, const PanGesture& gesture )
{
  mLayoutSize.x += gesture.displacement.x * 2.0f;
  mLayoutSize.y += gesture.displacement.y * 2.0f;

  // Avoid pixel mis-alignment issue
  Vector2 clampedSize = Vector2( ConvertToEven(static_cast<int>(mLayoutSize.x)),
                                 ConvertToEven(static_cast<int>(mLayoutSize.y)) );

  Self().SetSize( clampedSize );

  RelayoutRequest();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
