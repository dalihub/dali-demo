/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include "cluster-impl.h"

// EXTERNAL INCLUDES
#include <algorithm>
#include <cstring> // for strcmp
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/property-array.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include "cluster-style.h"
#include "cluster-style-impl.h"

using namespace Dali;

namespace Dali
{

namespace Demo
{

namespace Internal
{

namespace
{

BaseHandle Create()
{
  Demo::ClusterStyleStandard s = Demo::ClusterStyleStandard::New( Demo::ClusterStyleStandard::ClusterStyle1 );
  return Demo::Cluster::New( s );
}

DALI_TYPE_REGISTRATION_BEGIN( Demo::Cluster, Toolkit::Control, Create )

DALI_ACTION_REGISTRATION( Demo, Cluster, "expand",    ACTION_EXPAND    )
DALI_ACTION_REGISTRATION( Demo, Cluster, "collapse",  ACTION_COLLAPSE  )
DALI_ACTION_REGISTRATION( Demo, Cluster, "transform", ACTION_TRANSFORM )

DALI_TYPE_REGISTRATION_END()

const float CLUSTER_STYLE_CONSTRAINT_DURATION = 1.0f;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Cluster
///////////////////////////////////////////////////////////////////////////////////////////////////

Dali::Demo::Cluster Cluster::New(Demo::ClusterStyle& style)
{
  // Create the implementation
  ClusterPtr cluster(new Cluster(style));

  // Pass ownership to CustomActor via derived handle
  Dali::Demo::Cluster handle(*cluster);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  cluster->Initialize();

  return handle;
}

Cluster::Cluster(Demo::ClusterStyle& style)
: Toolkit::Internal::Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS | DISABLE_SIZE_NEGOTIATION ) ),
  mClusterStyle(style),
  mExpandedCount(0)
{
}

void Cluster::OnInitialize()
{
}

void Cluster::OnSizeSet( const Vector3& targetSize )
{
  mClusterSize = targetSize;
  GetImpl(mClusterStyle).SetClusterSize(targetSize);

  for(ChildInfoIter iter = mChildren.begin(); iter != mChildren.end(); ++iter)
  {

    if((*iter).mActor)
    {
      mClusterStyle.ApplyStyle( (*iter).mActor,
                                (*iter).mPositionIndex,
                                AlphaFunction::EASE_OUT,
                                TimePeriod(0.f) );
    }
  }

  UpdateBackground(0.f);
  UpdateTitle(0.f);
}

Cluster::~Cluster()
{
}

void Cluster::AddChild( Actor child )
{
  // automatically add child with a position at end.
  AddChild( child, mChildren.size() );
}

void Cluster::AddChild( Actor child, unsigned int positionIndex )
{
  AddChildInfo( ChildInfo(child, positionIndex) );
}

void Cluster::AddChildAt( Actor child, unsigned int index )
{
  // automatically add child with a position at end.
  AddChild( child, mChildren.size() );
}

void Cluster::AddChildAt( Actor child, unsigned int positionIndex, unsigned int index )
{
  AddChildInfoAt( ChildInfo(child, positionIndex), index );
}

void Cluster::AddChildInfo( ChildInfo childInfo )
{
  AddChildInfoAt(childInfo, mChildren.size());
}

void Cluster::AddChildInfoAt( ChildInfo childInfo, unsigned int index )
{
  // check that the child is valid
  DALI_ASSERT_ALWAYS( childInfo.mActor );

  ChildInfoIter offset = index < mChildren.size() ? (mChildren.begin() + index) : mChildren.end();
  // now perform customization on this child.

  // adopt the child
  if(childInfo.mActor.GetParent() != Self())
  {
    Actor& child = childInfo.mActor;
    const float depth = std::distance(mChildren.begin(), offset);

    Property::Index depthProperty = child.GetPropertyIndex(Demo::Cluster::CLUSTER_ACTOR_DEPTH);
    if(depthProperty == Property::INVALID_INDEX)
    {
      child.RegisterProperty(Demo::Cluster::CLUSTER_ACTOR_DEPTH, depth);
    }

    // not added prior
    Self().Add( childInfo.mActor );
    mChildren.insert( offset, childInfo );

    // Use parent position plus relative position.
    child.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION_PLUS_LOCAL_POSITION );

    // remove old constraints
    child.RemoveConstraints();

    // apply new constraints to the child
    mClusterStyle.ApplyStyle(child, childInfo.mPositionIndex, AlphaFunction::EASE_OUT, TimePeriod(0.0f));
  }
  else
  {
    // already added.
    ChildInfoContainer mNewChildren;
    ChildInfoIter iter = mChildren.begin();
    float depth = 0.0f;

    for( ; iter != mChildren.end(); ++iter)
    {
      if(iter == offset)
      {
        SetDepth(childInfo, depth);
        depth++;
        // insert the new childInfo before offset.
        mNewChildren.push_back(childInfo);
      }
      // copy all children except the one that we wish to move.
      if((*iter).mActor != childInfo.mActor)
      {
        SetDepth(*iter, depth);
        depth++;
        mNewChildren.push_back(*iter);
      }
    } // end for.

    if(iter == offset)
    {
      SetDepth(childInfo, depth);
      // insert the new childInfo before offset (end).
      mNewChildren.push_back(childInfo);
    }

    mChildren = mNewChildren;

    // Todo somehow adjust their perceived depth.
  }
}

void Cluster::SetDepth( ChildInfo& childInfo, float depth )
{
  Property::Index depthProperty = childInfo.mActor.GetPropertyIndex(Demo::Cluster::CLUSTER_ACTOR_DEPTH);
  childInfo.mActor.SetProperty( depthProperty, depth );
}

ChildInfo Cluster::GetChildInfoAt( unsigned int index )
{
  // check if we have this position in the cluster
  if( index < mChildren.size() )
  {
    // return the child handle
    return mChildren[ index ];
  }

  // return an empty handle
  return ChildInfo();
}

Actor Cluster::GetChildAt( unsigned int index )
{
  // check if we have this position in the cluster
  if( index < mChildren.size() )
  {
    // return the child handle
    return mChildren[ index ].mActor;
  }

  // return an empty handle
  return Actor();
}

Actor Cluster::RemoveChildAt( unsigned int index )
{
  DALI_ASSERT_ALWAYS( index < mChildren.size() );

  ChildInfoIter iter = mChildren.begin() + index;
  Actor child = (*iter).mActor;
  mChildren.erase( iter );
  Self().Remove(child);
  // note: constraints will automatically be removed in OnControlChildRemove

  // update depths.
  float depth = 0.0f;

  for(ChildInfoIter iter = mChildren.begin(); iter != mChildren.end(); ++iter)
  {
    SetDepth(*iter, depth);
    depth++;
  } // end for.

  return child;
}

void Cluster::ExpandChild( unsigned int index )
{
  if( index < mChildren.size() )
  {
    ChildInfo& childInfo = mChildren[ index ];
    DALI_ASSERT_ALWAYS(childInfo.mActor);

    if(!childInfo.mExpanded)
    {
      // expand child to a random position/angle.
      const Vector3 clusterSize = Self().GetCurrentSize();
      const float length = clusterSize.Length() * 0.1f;
      const float zOffset = 50.0f;
      const float angle = (rand()%360) * Math::PI / 180.0f;
      Vector3 position(sin(angle) * length, -cos(angle) * length, zOffset);
      const float scale(1.2f);
      const Radian rotate( Degree( (rand()%30) - 15 ) );

      position += childInfo.mActor.GetCurrentPosition();

      TransformChild(index,
                     position,
                     Vector3::ONE * scale,
                     Quaternion(rotate, Vector3::ZAXIS),
                     AlphaFunction::EASE_OUT,
                     TimePeriod(0.5f));
    }
  }
}

void Cluster::ExpandAllChildren()
{
  for(unsigned int index = 0;index < mChildren.size(); index++)
  {
    ExpandChild( index );
  }
}

void Cluster::CollapseChild( unsigned int index, bool front )
{
  if( index < mChildren.size() )
  {
    RestoreChild(index,
                 AlphaFunction::EASE_OUT,
                 TimePeriod(0.25f),
                 front);
  }
}

void Cluster::CollapseAllChildren( bool front )
{
  for(unsigned int index = 0;index < mChildren.size(); index++)
  {
    RestoreChild(index,
                 AlphaFunction::EASE_OUT,
                 TimePeriod(0.25f),
                 front);
  }
}

void Cluster::TransformChild( unsigned int index, const Vector3& position, const Vector3& scale, const Quaternion& rotation, AlphaFunction alpha, const TimePeriod& period )
{
  if( index < mChildren.size() )
  {
    ChildInfo& childInfo = mChildren[ index ];
    DALI_ASSERT_ALWAYS(childInfo.mActor);

    if(!childInfo.mExpanded)
    {
      Actor child = childInfo.mActor;
      childInfo.mExpanded = true;
      mExpandedCount++;

      child.RemoveConstraints();
      Animation animation = Animation::New(period.delaySeconds + period.durationSeconds);
      animation.AnimateTo( Property(child, Actor::Property::POSITION), position, AlphaFunction::EASE_OUT, period);
      animation.AnimateTo( Property(child, Actor::Property::SCALE), scale, AlphaFunction::EASE_OUT, period);
      animation.AnimateTo( Property(child, Actor::Property::ORIENTATION), rotation, AlphaFunction::EASE_OUT, period);
      animation.Play();
    }
  }
}

void Cluster::RestoreChild( unsigned int index, AlphaFunction alpha, const TimePeriod& period, bool front )
{
  if( index < mChildren.size() )
  {
    ChildInfo& childInfo = mChildren[ index ];
    DALI_ASSERT_ALWAYS(childInfo.mActor);

    if(childInfo.mExpanded)
    {
      Actor child = childInfo.mActor;
      childInfo.mExpanded = false;
      mExpandedCount--;
      mClusterStyle.ApplyStyle( child, childInfo.mPositionIndex, alpha, period );

      const unsigned int hideIndex = front ? mChildren.size() : 0;
      AddChildInfoAt(childInfo, hideIndex); // move child info to the back or front of the pack.
    }
  }
}

void Cluster::SetBackgroundImage( Actor image )
{
  // Replaces the background image.
  if(mBackgroundImage && mBackgroundImage.GetParent())
  {
    mBackgroundImage.GetParent().Remove(mBackgroundImage);
  }

  mBackgroundImage = image;
  Self().Add(mBackgroundImage);

  mBackgroundImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mBackgroundImage.SetParentOrigin( ParentOrigin::TOP_LEFT );

  UpdateBackground(0.0f);
}

void Cluster::SetTitle( Actor text )
{
  // Replaces the title actor.
  if(mTitle && mTitle.GetParent())
  {
    mTitle.GetParent().Remove( mTitle );
  }

  mTitle = text;
  Self().Add( mTitle );

  mTitle.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mTitle.SetParentOrigin( ParentOrigin::TOP_LEFT );

  UpdateTitle(0.0f);
}

void Cluster::SetStyle(Demo::ClusterStyle style)
{
  unsigned int previousChildrenNum = mChildren.size();
  mClusterStyle = style;
  GetImpl(mClusterStyle).SetClusterSize(mClusterSize);
  unsigned int newChildrenNum = mClusterStyle.GetMaximumNumberOfChildren();

  // New style supports less children (remove those that no longer belong)
  if(newChildrenNum < previousChildrenNum)
  {
    ChildInfoIter removeStart = mChildren.begin() + newChildrenNum;

    for(ChildInfoIter iter = removeStart; iter != mChildren.end(); ++iter)
    {
      Actor child = (*iter).mActor;
      child.RemoveConstraints();
      Self().Remove(child);
    }

    mChildren.erase( removeStart, mChildren.end() );
  }

  for(ChildInfoIter iter = mChildren.begin(); iter != mChildren.end(); ++iter)
  {

    if((*iter).mActor)
    {
      mClusterStyle.ApplyStyle( (*iter).mActor,
                        (*iter).mPositionIndex,
                        AlphaFunction::EASE_OUT,
                        TimePeriod(CLUSTER_STYLE_CONSTRAINT_DURATION) );
    }
  }

  UpdateBackground(CLUSTER_STYLE_CONSTRAINT_DURATION);
  UpdateTitle(CLUSTER_STYLE_CONSTRAINT_DURATION);
}

Demo::ClusterStyle Cluster::GetStyle() const
{
  return mClusterStyle;
}

unsigned int Cluster::GetExpandedCount() const
{
  return mExpandedCount;
}

unsigned int Cluster::GetTotalCount() const
{
  return mChildren.size();
}

void Cluster::UpdateBackground(float duration)
{
  if (mBackgroundImage)
  {
    mClusterStyle.ApplyStyleToBackground(mBackgroundImage, AlphaFunction::EASE_OUT, TimePeriod(duration));
  }
}

void Cluster::UpdateTitle(float duration)
{
  if (mTitle)
  {
    mClusterStyle.ApplyStyleToTitle(mTitle, AlphaFunction::EASE_OUT, TimePeriod(duration));
  }
}

void Cluster::DoExpandAction(const Property::Map& attributes)
{
  Property::Value* value = attributes.Find( "indices" );

  if( value )
  {
    if( value->GetType() == Property::ARRAY )
    {
      Property::Array array = value->Get<Property::Array>();
      for( size_t i = 0; i < array.Size(); i++ )
      {
        Property::Value& item = array[i];
        DALI_ASSERT_ALWAYS(item.GetType() == Property::INTEGER);
        ExpandChild( item.Get<int>() );
      }
    }
  }
  else
  {
    ExpandAllChildren();
  }
}

void Cluster::DoCollapseAction(const Property::Map& attributes)
{
  Property::Value* value = attributes.Find( "indices" );

  if( value )
  {
    if( value->GetType() == Property::ARRAY )
    {
      Property::Array array = value->Get<Property::Array>();
      for( size_t i = 0; i < array.Size(); i++ )
      {
        Property::Value& item = array[i];
        DALI_ASSERT_ALWAYS(item.GetType() == Property::INTEGER);
        CollapseChild( item.Get<int>(), false );
      }
    }
  }
  else
  {
    CollapseAllChildren( false );
  }
}


void Cluster::DoTransformAction(const Property::Map& attributes)
{
  typedef Dali::StringValuePair StringValuePair;

  int index = 0;
  Vector3 position;
  Vector3 scale(Vector3::ONE);
  Quaternion rotation( Dali::ANGLE_0, Vector3::ZAXIS );

  for(size_t i = 0; i < attributes.Count(); i++)
  {
    StringValuePair& stringValue = attributes.GetPair(i);
    Property::Type type = stringValue.second.GetType();

    if( Property::VECTOR3 == type && "position" == stringValue.first )
    {
      stringValue.second.Get(position);
    }
    else if( Property::VECTOR3 == type && "scale" == stringValue.first )
    {
      stringValue.second.Get(scale);
    }
    else if( "rotation" == stringValue.first )
    {
      (void)Scripting::SetRotation(stringValue.second, rotation);
    }
  }

  // wrap index around -1 => size - 1
  index%= mChildren.size();

  TransformChild(index, position, scale, rotation, AlphaFunction::EASE_OUT, TimePeriod(0.5f));
}

void Cluster::OnControlChildRemove(Actor& child)
{
  child.RemoveConstraints();
}

bool Cluster::DoAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes)
{
  bool ret = false;

  Dali::BaseHandle handle( object );

  Demo::Cluster cluster = Demo::Cluster::DownCast( handle );

  DALI_ASSERT_ALWAYS( cluster );

  if( 0 == strcmp( actionName.c_str(), ACTION_EXPAND ) )
  {
    GetImpl( cluster ).DoExpandAction( attributes );
    ret = true;
  }
  else if( 0 == strcmp( actionName.c_str(), ACTION_COLLAPSE ) )
  {
    GetImpl( cluster ).DoCollapseAction( attributes );
    ret = true;
  }
  else if( 0 == strcmp( actionName.c_str(), ACTION_TRANSFORM ) )
  {
    GetImpl( cluster ).DoTransformAction( attributes );
    ret = true;
  }

  return ret;
}

} // namespace Internal

} // namespace Demo

} // namespace Dali
