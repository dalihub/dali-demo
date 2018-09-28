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

// CLASS HEADER
#include "custom-layout-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>

namespace Demo
{

namespace Internal
{

using Dali::Actor;
using Dali::Toolkit::MeasuredSize;

CustomLayoutPtr CustomLayout::New()
{
  CustomLayoutPtr layout( new CustomLayout() );
  return layout;
}

void CustomLayout::OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
  LayoutLength accumulatedWidth = 0;
  LayoutLength maxHeight = 0;

  // In this layout we will:
  //  Measuring the layout with the children in a horizontal configuration, one after another
  //  Set the required width to be the accumulated width of our children
  //  Set the required height to be the maximum height of any of our children

  for( unsigned int i=0; i<GetChildCount(); ++i )
  {
    auto childLayout = GetChildAt( i );
    if( childLayout )
    {
      MeasureChild( childLayout, widthMeasureSpec, heightMeasureSpec );
      accumulatedWidth += childLayout->GetMeasuredWidth();
      std::max( childLayout->GetMeasuredHeight(), maxHeight );
    }
  }

  // Finally, call this method to set the dimensions we would like
  SetMeasuredDimensions( MeasuredSize( accumulatedWidth ), MeasuredSize( maxHeight ) );
}

void CustomLayout::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
  LayoutLength childTop( 0 );
  LayoutLength childLeft( 0 );

  // We want to vertically align the children to the middle
  LayoutLength middle = (bottom - top) / 2;

  // Horizontally align the children to the middle of the space they are given too
  LayoutLength width = right - left;
  int count = GetChildCount();
  LayoutLength childIncrement = width / count;
  LayoutLength center = childIncrement / 2;

  // Check layout direction
  auto owner = GetOwner();
  auto actor = Actor::DownCast(owner);
  const bool isLayoutRtl = actor ? actor.GetProperty< bool >( Actor::Property::LAYOUT_DIRECTION ) : false;

  for( int i = 0; i < count; i++)
  {
    auto itemIndex = isLayoutRtl ? count - 1 - i : i; // If RTL, then layout the last item first

    Dali::Toolkit::Internal::LayoutItemPtr childLayout = GetChildAt( itemIndex );
    if( childLayout != nullptr )
    {
      LayoutLength childWidth = childLayout->GetMeasuredWidth();
      LayoutLength childHeight = childLayout->GetMeasuredHeight();

      childTop = middle - childHeight / 2;

      LayoutLength left = childLeft + center - childWidth / 2;

      childLayout->Layout( left, childTop, left + childWidth, childTop + childHeight );
      childLeft += childIncrement;
    }
  }
}

} // namespace Internal

} // namespace Demo
