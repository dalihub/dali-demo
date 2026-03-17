#ifndef DALI_DEMO_CONTROLS_INTERNAL_SPREAD_FILTER_H
#define DALI_DEMO_CONTROLS_INTERNAL_SPREAD_FILTER_H

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
#include <dali/public-api/render-tasks/render-task.h>

// INTERNAL INCLUDES
#include <controls/filters/image-filter.h>

namespace Dali::Demo::Internal
{
/**
 * A spread/thicken filter. Expands an image into transparent areas.
 */
class SpreadFilter : public ImageFilter
{
public:
  /**
   * Default constructor
   */
  SpreadFilter();

  /**
   * Destructor
   */
  virtual ~SpreadFilter();

  /**
   * Set the amount of spread in pixels.
   * @param[in] spread The amount of spread
   */
  void SetSpread(float spread);

public: // From ImageFilter
  /// @copydoc Dali::Toolkit::Internal::ImageFilter::Enable
  void Enable() override;

  /// @copydoc Dali::Toolkit::Internal::ImageFilter::Disable
  void Disable() override;

  /// @copydoc Dali::Toolkit::Internal::ImageFilter::Refresh
  void Refresh() override;

  /// @copydoc Dali::Toolkit::Internal::ImageFilter::SetSize
  void SetSize(const Vector2& size) override;

private:
  /**
   * Setup render tasks for blur
   */
  void CreateRenderTasks();

private:
  SpreadFilter(const SpreadFilter&);
  SpreadFilter& operator=(const SpreadFilter&);

private: // Attributes
  // To perform horizontal spread from mInputTexture to mFrameBufferForHorz
  RenderTask  mRenderTaskForHorz;
  Actor       mActorForInput;
  FrameBuffer mFrameBufferForHorz;

  // To perform vertical spread from mFrameBufferForHorz to mOutputFrameBuffer
  RenderTask mRenderTaskForVert;
  Actor      mActorForHorz;

  int mSpread;
}; // class SpreadFilter

} // namespace Dali::Demo::Internal

#endif // DALI_DEMO_CONTROLS_INTERNAL_SPREAD_FILTER_H
