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

#include <string>
#include "animation-example.h"
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/linear-layout.h>
#include <dali-toolkit/devel-api/layouting/grid.h>

#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* const TITLE = "Animation Example";

// Button file names
const char* LTR_IMAGE( DEMO_IMAGE_DIR "icon-play.png" );
const char* LTR_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-play-selected.png" );

const char* RTL_IMAGE( DEMO_IMAGE_DIR "icon-reverse.png" );
const char* RTL_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-reverse-selected.png" );

const char* ROTATE_CLOCKWISE_IMAGE( DEMO_IMAGE_DIR "icon-reset.png" );
const char* ROTATE_CLOCKWISE_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-reset-selected.png" );

const char* GRID_IMAGE( DEMO_IMAGE_DIR "icon-item-view-layout-grid.png" );
const char* GRID_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-item-view-layout-grid-selected.png" );

// Child image filenames
const char* IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "application-icon-101.png",
  DEMO_IMAGE_DIR "application-icon-102.png",
  DEMO_IMAGE_DIR "application-icon-103.png",
  DEMO_IMAGE_DIR "application-icon-104.png"
};

#if defined(DEBUG_ENABLED)
Debug::Filter* gLayoutFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_LAYOUT" );
#endif

const unsigned int NUMBER_OF_RESOURCES = sizeof(IMAGE_PATH) / sizeof(char*);

// Helper function to create ImageViews with given filename and size.
void CreateChildImageView( ImageView& imageView, int index, Size size )
{
  imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imagePropertyMap[ Toolkit::ImageVisual::Property::URL ] = IMAGE_PATH[ index ];
  imagePropertyMap[ ImageVisual::Property::DESIRED_WIDTH ] = size.width;
  imagePropertyMap[ ImageVisual::Property::DESIRED_HEIGHT ] = size.height;
  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imagePropertyMap );
  std::string name = "ImageView";
  name.append( 1, '0' + index );
  imageView.SetName( name );
  imageView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  imageView.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
}

LayoutTransitionData CreateOnSetLayoutTransition( Control& parent, std::vector< Toolkit::ImageView >& children )
{
  auto layoutTransitionData = LayoutTransitionData::New();
  Property::Map map;
  map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::COLOR_ALPHA;
  map[ LayoutTransitionData::AnimatorKey::INITIAL_VALUE ] = 0.5f;
  map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = 1.0f;
  map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
    .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "EASE_IN_OUT" )
    .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.25f )
      .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f ) );

  // Apply to parent only
  layoutTransitionData.AddPropertyAnimator( parent, map );

  for( size_t i = 0; i < children.size(); i++ )
  {
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Vector3( 100.0f * 1.2f, 100.0f * 1.2f, 0 );
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "SIN" )
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.5f + 0.1f * i)
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.25f ) );
    layoutTransitionData.AddPropertyAnimator( children[i], map );
  }

  return layoutTransitionData;
}

LayoutTransitionData CreateOnChildAddTransition( Control& parent, ImageView& child )
{
  auto layoutTransitionData = LayoutTransitionData::New();

  // Want the parent to resize itself instantly so children will position themselves correctly when the parent is added to stage first time
  Property::Map map;
  map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
  map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
    .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
    .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
      .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
  layoutTransitionData.AddPropertyAnimator( parent, map );

  // New child is growing
  {
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
    map["initialValue"] = Vector3( 0.0f, 0.0f, 0 );
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Vector3( 100.0f, 100.0f, 0 );
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f ) );
    layoutTransitionData.AddPropertyAnimator( child, map );
  }

  // Want new children instantly appear in their positions
  {
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY] = Actor::Property::POSITION;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
         .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( child, map );
  }

  return layoutTransitionData;
}

LayoutTransitionData CreateOnChildRemoveTransition(std::vector< Toolkit::ImageView >& images)
{
  auto layoutTransitionData = LayoutTransitionData::New();
  // Apply animation to remaining children
  for (unsigned int i = 0; i < images.size(); i++)
  {
    {
      Property::Map map;
      map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = "position";
      map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "SIN")
        .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
           .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
           .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
      layoutTransitionData.AddPropertyAnimator( images[i], map );
    }
  }

  return layoutTransitionData;
}

void CreateChildImageViewAndAdd( Control& container, int index, std::vector< Toolkit::ImageView >& images )
{
  Toolkit::ImageView imageView;
  CreateChildImageView( imageView, index, Size( 100.0f, 100.0f ) );
  auto layout = DevelControl::GetLayout( container );
  layout.SetTransitionData( LayoutTransitionData::ON_CHILD_ADD, CreateOnChildAddTransition( container, imageView ) );

  container.Add( imageView );
  images.push_back( imageView );
}

}  // namespace

namespace Demo
{

AnimationExample::AnimationExample()
: Example( TITLE ),
  mGridSet( false )
{
}

void AnimationExample::Create()
{
  DALI_LOG_INFO( gLayoutFilter, Debug::Verbose, "AnimationExample::Create\n");
  auto stage = Stage::GetCurrent();

  mRemoveButton = PushButton::New();
  mRemoveButton.SetProperty( PushButton::Property::UNSELECTED_ICON, RTL_IMAGE );
  mRemoveButton.SetProperty( PushButton::Property::SELECTED_ICON, RTL_SELECTED_IMAGE );
  mRemoveButton.ClickedSignal().Connect( this, &AnimationExample::OnRemoveClicked );
  mRemoveButton.SetParentOrigin( Vector3( 0.2f, 1.0f, 0.5f ) );
  mRemoveButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mRemoveButton.SetSize( 75, 75 );
  stage.Add( mRemoveButton );

  mAddButton = PushButton::New();
  mAddButton.SetProperty( PushButton::Property::UNSELECTED_ICON, LTR_IMAGE );
  mAddButton.SetProperty( PushButton::Property::SELECTED_ICON, LTR_SELECTED_IMAGE );
  mAddButton.ClickedSignal().Connect( this, &AnimationExample::OnAddClicked );
  mAddButton.SetParentOrigin( Vector3( 0.4f, 1.0f, 0.5f ) );
  mAddButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mAddButton.SetSize( 75, 75 );
  stage.Add( mAddButton );

  mSelectGridButton = Toolkit::PushButton::New();
  mSelectGridButton.SetProperty( PushButton::Property::UNSELECTED_ICON, GRID_IMAGE );
  mSelectGridButton.SetProperty( PushButton::Property::SELECTED_ICON, GRID_SELECTED_IMAGE );
  mSelectGridButton.ClickedSignal().Connect( this, &AnimationExample::OnSelectGridClicked );
  mSelectGridButton.SetParentOrigin( Vector3( 0.6f, 1.0f, 0.5f ) );
  mSelectGridButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mSelectGridButton.SetSize( 75, 75 );
  stage.Add( mSelectGridButton );

  mShakeButton = PushButton::New();
  mShakeButton.SetProperty( PushButton::Property::UNSELECTED_ICON, ROTATE_CLOCKWISE_IMAGE );
  mShakeButton.SetProperty( PushButton::Property::SELECTED_ICON, ROTATE_CLOCKWISE_SELECTED_IMAGE );
  mShakeButton.ClickedSignal().Connect( this, &AnimationExample::OnChangeClicked );
  mShakeButton.SetParentOrigin( Vector3( 0.8f, 1.0f, 0.5f ) );
  mShakeButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mShakeButton.SetSize( 75, 75 );
  stage.Add( mShakeButton );

  // Create a linear layout
  mAnimationContainer = Control::New();
  mAnimationContainer.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  mAnimationContainer.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  mAnimationContainer.SetProperty( Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT );
  mAnimationContainer.SetParentOrigin( ParentOrigin::CENTER );
  mAnimationContainer.SetAnchorPoint( AnchorPoint::CENTER );
  mAnimationContainer.SetName( "AnimationExample" );
  mAnimationContainer.SetProperty( Toolkit::Control::Property::PADDING, Extents( 0.0f, 0.0f, 45.0f, 75.0f) );

  mHorizontalLayout = LinearLayout::New();
  mHorizontalLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );
  mHorizontalLayout.SetAlignment( LinearLayout::Alignment::CENTER_HORIZONTAL | LinearLayout::Alignment::CENTER_VERTICAL );
  mHorizontalLayout.SetAnimateLayout(true);
  DevelControl::SetLayout( mAnimationContainer, mHorizontalLayout );

  mGridLayout = Grid::New();
  mGridLayout.SetAnimateLayout(true);
  mGridLayout.SetNumberOfColumns(2);

  CreateChildImageViewAndAdd( mAnimationContainer, 0, mImages );
  stage.Add( mAnimationContainer );
}

// Remove controls added by this example from stage
void AnimationExample::Remove()
{
  if ( mAnimationContainer )
  {
    UnparentAndReset( mRemoveButton );
    UnparentAndReset( mAddButton );
    UnparentAndReset( mSelectGridButton );
    UnparentAndReset( mShakeButton );
    UnparentAndReset( mAnimationContainer);
    mImages.clear();
  }
}

bool AnimationExample::OnRemoveClicked( Button button )
{
  DALI_LOG_INFO( gLayoutFilter, Debug::Verbose, "AnimationExample::OnRemoveClicked\n");

  if (mImages.size() > 1)
  {
    auto layout = DevelControl::GetLayout( mAnimationContainer );
    layout.SetTransitionData(LayoutTransitionData::ON_CHILD_REMOVE, CreateOnChildRemoveTransition( mImages ) );

    ImageView imageView = mImages.back();
    mAnimationContainer.Remove( imageView );
    mImages.pop_back();
  }
  return true;
}

// Change layout by setting new layout, triggers set layout transition
bool AnimationExample::OnSelectGridClicked( Button button )
{
  DALI_LOG_INFO( gLayoutFilter, Debug::Verbose, "AnimationExample::OnRotateClicked\n");

  if ( !mGridSet )
  {
    mGridLayout.SetTransitionData( LayoutTransitionData::ON_OWNER_SET, CreateOnSetLayoutTransition( mAnimationContainer, mImages ) );
    DevelControl::SetLayout( mAnimationContainer, mGridLayout );
  }
  else
  {
    mHorizontalLayout.SetTransitionData( LayoutTransitionData::ON_OWNER_SET, CreateOnSetLayoutTransition( mAnimationContainer, mImages ) );
    DevelControl::SetLayout( mAnimationContainer, mHorizontalLayout );
  }

  mGridSet = !mGridSet;
  return true;
}

bool AnimationExample::OnAddClicked( Button button )
{
  if (mImages.size() < 4)
  {
    CreateChildImageViewAndAdd( mAnimationContainer, mImages.size(), mImages );
  }
  return true;
}

bool AnimationExample::OnChangeClicked( Button button )
{
  if ( !mGridSet )
  {
    auto layout = LinearLayout::DownCast( DevelControl::GetLayout( mAnimationContainer ) );
    layout.SetAlignment( LinearLayout::Alignment::CENTER_HORIZONTAL | LinearLayout::Alignment::CENTER_VERTICAL );
    if ( layout.GetOrientation() == LinearLayout::Orientation::VERTICAL )
    {
      layout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );
    }
    else
    {
      layout.SetOrientation( LinearLayout::Orientation::VERTICAL );
    }
  }
  else
  {
    auto layout = Grid::DownCast( DevelControl::GetLayout( mAnimationContainer ) );
    if ( layout.GetNumberOfColumns() == 2 )
    {
      layout.SetNumberOfColumns(3);
    }
    else
    {
      layout.SetNumberOfColumns(2);
    }
  }
  return true;
}

} // namespace Demo
