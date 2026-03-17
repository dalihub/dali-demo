#ifndef DALI_DEMO_CONTROLS_INTERNAL_SHADOW_VIEW_H
#define DALI_DEMO_CONTROLS_INTERNAL_SHADOW_VIEW_H

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
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <cmath>
#include <sstream>

// INTERNAL INCLUDES
#include <controls/filters/blur-two-pass-filter.h>
#include <controls/shadow-view/shadow-view.h>

namespace Dali::Demo
{
class ShadowView;

namespace Internal
{
/**
 * ShadowView implementation class
 */
class ShadowView : public Toolkit::Internal::Control
{
public:
  /**
   * @copydoc Dali::Demo::ShadowView::ShadowView
   */
  ShadowView();

  /**
   * @copydoc Dali::Demo::ShadowView::ShadowView
   */
  ShadowView(float downsampleWidthScale, float downsampleHeightScale);

  /**
   * @copydoc Dali::Demo::ShadowView::~ShadowView
   */
  virtual ~ShadowView();

  /**
   * @copydoc Dali::Demo::ShadowView::New(float downsampleWidthScale, float downsampleHeightScale)
   */
  static Demo::ShadowView New(float downsampleWidthScale, float downsampleHeightScale);

  /**
   * @copydoc Dali::Demo::ShadowView::SetShadowPlaneBackground(Actor shadowPlaneBackground)
   */
  void SetShadowPlaneBackground(Actor shadowPlaneBackground);

  /**
   * @copydoc Dali::Demo::ShadowView::SetPointLight(Actor pointLight)
   */
  void SetPointLight(Actor pointLight);

  /**
   * @copydoc Dali::Demo::ShadowView::SetPointLightFieldOfView(float fieldOfView)
   */
  void SetPointLightFieldOfView(float fieldOfView);

  /**
   * @copydoc Dali::Demo::ShadowView::SetShadowColor(Vector4 color)
   */
  void SetShadowColor(Vector4 color);

  /**
   * @copydoc Dali::Demo::ShadowView::Activate()
   */
  void Activate();

  /**
   * @copydoc Dali::Demo::ShadowView::Deactivate()
   */
  void Deactivate();

  /**
   * @copydoc Dali::Demo::ShadowView::GetBlurStrengthPropertyIndex()
   */
  Property::Index GetBlurStrengthPropertyIndex() const
  {
    return mBlurStrengthPropertyIndex;
  }

  /**
   * @copydoc Dali::Demo::ShadowView::GetShadowColorPropertyIndex()
   */
  Property::Index GetShadowColorPropertyIndex() const
  {
    return mShadowColorPropertyIndex;
  }

  void SetShaderConstants();

private:
  void OnInitialize() override;

  /**
   * @copydoc Toolkit::Internal::Control::OnChildAdd()
   */
  void OnChildAdd(Actor& child) override;

  /**
   * @copydoc Toolkit::Internal::Control::OnChildRemove()
   */
  void OnChildRemove(Actor& child) override;

  /**
   * Constrain the camera actor to the position of the point light, pointing
   * at the center of the shadow plane.
   */
  void ConstrainCamera();

  void CreateRenderTasks();
  void RemoveRenderTasks();
  void CreateBlurFilter();

private:
  Actor mShadowPlane;   // Shadow renders into this actor
  Actor mShadowPlaneBg; // mShadowPlane renders directly in front of this actor
  Actor mPointLight;    // Shadow is cast from this point light

  /////////////////////////////////////////////////////////////
  FrameBuffer mSceneFromLightRenderTarget; // for rendering normal scene seen from light to texture instead of the screen

  FrameBuffer mOutputFrameBuffer;

  Actor      mChildrenRoot;  // Subtree for all user added child actors that should be rendered normally
  Actor      mBlurRootActor; // Root actor for blur filter processing
  RenderTask mRenderSceneTask;

  CameraActor mCameraActor; // Constrained to same position as mPointLight and pointing at mShadowPlane

  Property::Map     mShadowVisualMap;
  Demo::Internal::BlurTwoPassFilter mBlurFilter;

  Vector4 mCachedShadowColor;     ///< Cached Shadow color.
  Vector4 mCachedBackgroundColor; ///< Cached Shadow background color (same as shadow color but with alpha at 0.0)

  /////////////////////////////////////////////////////////////
  // Properties that can be animated
  Property::Index mBlurStrengthPropertyIndex;
  Property::Index mShadowColorPropertyIndex;
  float           mDownsampleWidthScale;
  float           mDownsampleHeightScale;

private:
  // Undefined copy constructor.
  ShadowView(const ShadowView&);

  // Undefined assignment operator.
  ShadowView& operator=(const ShadowView&);
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Demo::Internal::ShadowView& GetImpl(Demo::ShadowView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Demo::Internal::ShadowView&>(handle);
}

inline const Demo::Internal::ShadowView& GetImpl(const Demo::ShadowView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Demo::Internal::ShadowView&>(handle);
}

} // namespace Dali::Demo

#endif // DALI_DEMO_CONTROLS_INTERNAL_SHADOW_VIEW_H
