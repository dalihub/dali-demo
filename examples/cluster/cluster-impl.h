#ifndef __DALI_DEMO_INTERNAL_CLUSTER_H__
#define __DALI_DEMO_INTERNAL_CLUSTER_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include "cluster.h"
#include "cluster-style.h"
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{

namespace Demo
{

namespace Internal
{

class Cluster;

typedef IntrusivePtr<Cluster> ClusterPtr;

class ChildInfo
{

public:

  ChildInfo()
  : mExpanded(false)
  {
  }

  ChildInfo(Actor actor, unsigned int positionIndex)
  : mActor(actor),
    mExpanded(false),
    mPositionIndex(positionIndex)
  {
  }

  Actor mActor;
  bool mExpanded;
  unsigned int mPositionIndex;
};

typedef std::vector<ChildInfo> ChildInfoContainer;
typedef ChildInfoContainer::iterator ChildInfoIter;
typedef ChildInfoContainer::const_iterator ChildInfoConstIter;

/**
 * Cluster is a container of grouped actors positioned in different cluster styles.
 * Multiple cluster styles may be provided, to determine the position, size, rotation, scale, color and visibility
 * of the child actors in the cluster.
 */
class Cluster : public Toolkit::Internal::Control
{
public:

  /**
   * Create a new Cluster.
   * @param[in] style of the cluster
   * @return A public handle to the newly allocated Cluster.
   */
  static Dali::Demo::Cluster New(Demo::ClusterStyle& style);

  /**
   * @copydoc Demo::Cluster::AddChild( Actor child )
   */
  void AddChild( Actor child );

  /**
   * @copydoc Demo::Cluster::AddChild( Actor child, unsigned int positionIndex )
   */
  void AddChild( Actor child, unsigned int positionIndex );

  /**
   * @copydoc Demo::Cluster::AddChildAt( Actor child, unsigned int index );
   */
  void AddChildAt( Actor child, unsigned int index );

  /**
   * @copydoc Demo::Cluster::AddChildAt( Actor child, unsigned int positionIndex, unsigned int index );
   */
  void AddChildAt( Actor child, unsigned int positionIndex, unsigned int index );

  /**
   * Adds a ChildInfo struct to the end of the children list.
   * @param[in] childInfo the child info to that to children list.
   */
  void AddChildInfo( ChildInfo childInfo );

  /**
   * Adds a ChildInfo struct before the specified index.
   * @param[in] childInfo the child info to that to children list.
   * @param[in] index the index within the children list to insert
   * ChildInfo
   */
  void AddChildInfoAt( ChildInfo childInfo, unsigned int index );

  /**
   * @copydoc Demo::Cluster::GetChildAt
   */
  Actor GetChildAt( unsigned int index );

  /**
   * @copydoc Demo::Cluster::RemoveChildAt
   */
  Actor RemoveChildAt( unsigned int index );

  /**
   * @copydoc Demo::Cluster::ExpandChild
   */
  void ExpandChild( unsigned int index );

  /**
   * @copydoc Demo::Cluster::ExpandAllChildren
   */
  void ExpandAllChildren();

  /**
   * @copydoc Demo::Cluster::CollapseChild
   */
  void CollapseChild( unsigned int index, bool front );

  /**
   * @copydoc Demo::Cluster::CollapseAllChildren
   */
  void CollapseAllChildren( bool front );

  /**
   * @copydoc Demo::Cluster::TransformChild
   */
  void TransformChild( unsigned int index, const Vector3& position, const Vector3& scale, const Quaternion& rotation, AlphaFunction alpha, const TimePeriod& period );

  /**
   * @copydoc Demo::Cluster::RestoreChild
   */
  void RestoreChild( unsigned int index, AlphaFunction alpha, const TimePeriod& period, bool front );

  /**
   * @copydoc Demo::Cluster::SetBackgroundImage
   */
  void SetBackgroundImage( Actor image );

  /**
   * @copydoc Demo::Cluster::SetTitle
   */
  void SetTitle( Actor text );

  /**
   * @copydoc Demo::Cluster::SetStyle
   */
  void SetStyle(Demo::ClusterStyle style);

  /**
   * @copydoc Demo::Cluster::GetStyle
   */
  Demo::ClusterStyle GetStyle() const;

  /**
   * @copydoc Demo::Cluster::GetExpandedCount
   */
  unsigned int GetExpandedCount() const;

  /**
   * @copydoc Demo::Cluster::GetTotalCount
   */
  unsigned int GetTotalCount() const;

private:

  ChildInfo GetChildInfoAt( unsigned int index );

  void SetDepth( ChildInfo& childInfo, float depth );

  /**
   * Updates the style of the Background
   * (occurs when either background changes or style changes)
   * @param[in] duration apply duration for style
   */
  void UpdateBackground(float duration);

  /**
   * Updates the style of the Title
   * (occurs when either background changes or style changes)
   * @param[in] duration apply duration for style
   */
  void UpdateTitle(float duration);

  /**
   * Action: Expand
   * Expands one or more actors.
   *
   * @param[in] attributes list of indices of actors to expand.
   * (if no attributes specifies, then all actors expand)
   */
  void DoExpandAction(const PropertyValueContainer& attributes);

  /**
   * Action: Collapse
   * Collapses one or more actors.
   *
   * @param[in] attributes list of indices of actors to collapse.
   * (if no attributes specifies, then all actors collapse)
   */
  void DoCollapseAction(const PropertyValueContainer& attributes);

  /**
   * Action: Transform
   * Transforms one actor (index) to a specified position (Vector3),
   * scale (Vector3), and rotation (Quaternion).
   *
   * @param[in] attributes index and transform values.
   */
  void DoTransformAction(const PropertyValueContainer& attributes);

private: // From Control
  /**
   * From Control; called shortly before a child is removed from the owning actor.
   * @param[in] child The child being removed.Ptr
   */
  virtual void OnControlChildRemove(Actor& child);

public:

  /**
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return true if action has been accepted by this control
   */
  static bool DoAction(BaseObject* object, const std::string& actionName, const PropertyValueContainer& attributes);

private: // From Control

  /**
   * @copydoc Demo::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   *
   * @copydoc Demo::Control::OnControlSizeSet( const Vector3& targetSize )
   */
  virtual void OnControlSizeSet( const Vector3& targetSize );

protected:

  /**
   * Construct a new Cluster.
   * @param[in] style of the cluster
   */
  Cluster(Demo::ClusterStyle& style);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Cluster();

private:

  // Undefined
  Cluster(const Cluster&);

  // Undefined
  Cluster& operator=(const Cluster& rhs);

private:

  Demo::ClusterStyle mClusterStyle;
  ChildInfoContainer mChildren;
  Vector3 mClusterSize;

  Actor mBackgroundImage;           ///< Stores the background image.
  Actor mTitle;                     ///< Stores the text title.
  unsigned int mExpandedCount;      ///< A count of how many children have been expanded.

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Demo::Internal::Cluster& GetImpl(Demo::Cluster& cluster)
{
  DALI_ASSERT_ALWAYS(cluster);

  Dali::RefObject& handle = cluster.GetImplementation();

  return static_cast<Demo::Internal::Cluster&>(handle);
}

inline const Demo::Internal::Cluster& GetImpl(const Demo::Cluster& cluster)
{
  DALI_ASSERT_ALWAYS(cluster);

  const Dali::RefObject& handle = cluster.GetImplementation();

  return static_cast<const Demo::Internal::Cluster&>(handle);
}

} // namespace Demo

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_CLUSTER_H__
