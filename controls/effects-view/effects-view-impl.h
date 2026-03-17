#ifndef DALI_DEMO_CONTROLS_INTERNAL_EFFECTS_VIEW_H
#define DALI_DEMO_CONTROLS_INTERNAL_EFFECTS_VIEW_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <controls/effects-view/effects-view.h>

namespace Dali::Demo
{
namespace Internal
{
class ImageFilter;

/**
 * EffectsView implementation class
 * @copydoc Dali::Demo::EffectsView
 */
class EffectsView : public Toolkit::ControlImpl
{
public:
  /// @copydoc Dali::Demo::EffectsView New()
  static Demo::EffectsView New();

  /**
   * Construct a new EffectsView.
   * @copydoc Demo::EffectsView New()
   */
  EffectsView();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~EffectsView();

public:
  /// @copydoc Dali::Demo::EffectsView::SetType
  void SetType(Demo::EffectsView::EffectType type);

  /// @copydoc Dali::Demo::EffectsView::GetType
  Demo::EffectsView::EffectType GetType() const;

  /// @copydoc Dali::Demo::EffectsView::Refresh
  void Refresh();

  /// @copydoc Dali::Demo::EffectsView::SetRefreshOnDemand
  void SetRefreshOnDemand(bool onDemand);

  /// @copydoc Dali::Demo::EffectsView::SetPixelFormat
  void SetPixelFormat(Pixel::Format pixelFormat);

  /// @copydoc Dali::Demo::EffectsView::SetBackgroundColor(const Vector4&)
  void SetBackgroundColor(const Vector4& color);

  /// @copydoc Dali::Demo::EffectsView::GetBackgroundColor
  Vector4 GetBackgroundColor() const;

  /**
   * Set the effect size which decides the size of filter kernel.
   * @param[in] effectSize The effect size.
   */
  void SetEffectSize(int effectSize);

  /**
   * Get the effect size.
   * @return The effect size.
   */
  int GetEffectSize();

  // Properties
  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index propertyIndex);

private: // From Control
  /**
   * @copydoc Toolkit::ControlImpl::OnInitialize()
   */
  void OnInitialize() override;

  /**
   * @copydoc CustomActorImpl::OnSizeSet( const Vector3& targetSize )
   */
  void OnSizeSet(const Vector3& targetSize) override;

  /**
   * @copydoc Toolkit::ControlImpl::OnSceneConnection
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc Toolkit::ControlImpl::OnSceneDisconnection
   */
  void OnSceneDisconnection() override;

  /**
   * @copydoc Toolkit::ControlImpl::OnChildAdd
   */
  void OnChildAdd(Actor& child) override;

  /**
   * @copydoc Toolkit::ControlImpl::OnChildRemove
   */
  void OnChildRemove(Actor& child) override;

private:
  /**
   * Enable the effect when the control is set on stage
   */
  void Enable();

  /**
   * Disable the effect when the control is set off stage
   */
  void Disable();

  /**
   * Setup image filters
   */
  void SetupFilters();

  /**
   * Allocate resources
   */
  void AllocateResources();

  /**
   * Setup cameras
   */
  void SetupCameras();

  /**
   * Create render tasks for internal jobs
   */
  void CreateRenderTasks();

  /**
   * Remove render tasks
   */
  void RemoveRenderTasks();

  /**
   * Refresh render tasks
   */
  void RefreshRenderTasks();

  /**
   * Remove ImageFilters
   */
  void RemoveFilters();

private:
  // Undefined
  EffectsView(const EffectsView&);

  // Undefined
  EffectsView& operator=(const EffectsView&);

private: // attributes/properties
  /////////////////////////////////////////////////////////////
  // for rendering all user added children to offscreen target
  FrameBuffer mFrameBufferForChildren;
  Renderer    mRendererForChildren;
  RenderTask  mRenderTaskForChildren;
  CameraActor mCameraForChildren;
  Actor       mChildrenRoot; // for creating a subtree for all user added child actors

  /////////////////////////////////////////////////////////////
  // background fill color
  Vector4 mBackgroundColor;

  /////////////////////////////////////////////////////////////
  // for checking if we need to reallocate render targets
  Vector2 mTargetSize;
  Vector2 mLastSize;
  /////////////////////////////////////////////////////////////
  // post blur image
  FrameBuffer mFrameBufferPostFilter;
  Renderer    mRendererPostFilter;

  Vector<ImageFilter*> mFilters;

  /////////////////////////////////////////////////////////////
  // downsampling is used for the separated blur passes to get increased blur with the same number of samples and also to make rendering quicker
  int mEffectSize;

  /////////////////////////////////////////////////////////////
  Demo::EffectsView::EffectType mEffectType;
  Pixel::Format                 mPixelFormat; ///< pixel format used by render targets

  bool mEnabled : 1;
  bool mRefreshOnDemand : 1;
}; // class EffectsView

} // namespace Internal

// Helpers for public-api forwarding methods

inline Demo::Internal::EffectsView& GetImpl(Demo::EffectsView& effectsView)
{
  DALI_ASSERT_ALWAYS(effectsView);

  Dali::RefObject& handle = effectsView.GetImplementation();

  return static_cast<Demo::Internal::EffectsView&>(handle);
}

inline const Demo::Internal::EffectsView& GetImpl(const Demo::EffectsView& effectsView)
{
  DALI_ASSERT_ALWAYS(effectsView);

  const Dali::RefObject& handle = effectsView.GetImplementation();

  return static_cast<const Demo::Internal::EffectsView&>(handle);
}

} // namespace Dali::Demo

#endif // DALI_DEMO_CONTROLS_INTERNAL_EFFECTS_VIEW_H
