#ifndef DALI_DEMO_CONTROLS_INTERNAL_ALIGNMENT_H
#define DALI_DEMO_CONTROLS_INTERNAL_ALIGNMENT_H

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

// INTERNAL INCLUDES
#include "alignment.h"

namespace Dali
{
namespace Demo
{
class Alignment;

namespace Internal
{
/**
 * Alignment is a control to position and resize actors inside other container actors.
 * @see Dali::Demo::Alignment for more details.
 */
class Alignment : public Toolkit::ControlImpl
{
public:
  /**
   * Create an initialized Alignment.
   * @param type Type of alignment.
   * @return A handle to a newly allocated Dali resource.
   */
  static Demo::Alignment New(Demo::Alignment::Type horizontal, Demo::Alignment::Type vertical);

  /**
   * @copydoc Dali::Demo::Alignment::SetAlignmentType()
   */
  void SetAlignmentType(Demo::Alignment::Type type);

  /**
   * @copydoc Dali::Demo::Alignment::GetAlignmentType()
   */
  Demo::Alignment::Type GetAlignmentType() const;

  /**
   * @copydoc Dali::Demo::Alignment::SetScaling()
   */
  void SetScaling(Demo::Alignment::Scaling scaling);

  /**
   * @copydoc Dali::Demo::Alignment::GetScaling()
   */
  Demo::Alignment::Scaling GetScaling() const;

  /**
   * @copydoc Dali::Demo::Alignment::SetPadding()
   */
  void SetPadding(const Demo::Alignment::Padding& padding);

  /**
   * @copydoc Dali::Demo::Alignment::GetPadding()
   */
  const Demo::Alignment::Padding& GetPadding() const;

private: // From Control
  /**
   * @copydoc Control::OnInitialize()
   */
  virtual void OnInitialize() override;

  /**
   * @copydoc Control::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

private:
  /**
   * Constructor.
   * It initializes Alignment members.
   */
  Alignment(Demo::Alignment::Type horizontal, Demo::Alignment::Type vertical);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Alignment();

private:
  // Undefined
  Alignment(const Alignment&);
  Alignment& operator=(const Alignment&);

private:
  Demo::Alignment::Type    mHorizontal; ///< Type of alignment.
  Demo::Alignment::Type    mVertical;   ///< Type of alignment.
  Demo::Alignment::Scaling mScaling;    ///< Stores the geometry scaling.
  Demo::Alignment::Padding mPadding;    ///< Stores the padding values.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Demo::Internal::Alignment& GetImpl(Demo::Alignment& alignment)
{
  DALI_ASSERT_ALWAYS(alignment);

  Dali::RefObject& handle = alignment.GetImplementation();

  return static_cast<Demo::Internal::Alignment&>(handle);
}

inline const Demo::Internal::Alignment& GetImpl(const Demo::Alignment& alignment)
{
  DALI_ASSERT_ALWAYS(alignment);

  const Dali::RefObject& handle = alignment.GetImplementation();

  return static_cast<const Demo::Internal::Alignment&>(handle);
}

} // namespace Demo

} // namespace Dali

#endif // DALI_DEMO_CONTROLS_INTERNAL_ALIGNMENT_H
