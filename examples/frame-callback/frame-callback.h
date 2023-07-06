#ifndef DEMO_FRAME_CALLBACK_H
#define DEMO_FRAME_CALLBACK_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/update/frame-callback-interface.h>
#include <dali/devel-api/update/update-proxy.h>
#include <dali/public-api/common/dali-vector.h>

/**
 * @brief Implementation of the FrameCallbackInterface.
 *
 * When this is used, it will expand the size of the actors the closer they get to the horizontal edge
 * and make the actor transparent the closer it gets to the middle.
 */
class FrameCallback : public Dali::FrameCallbackInterface
{
public:
  /**
   * @brief Constructor.
   */
  FrameCallback();

  /**
   * @brief Sets the window width.
   * @param[in]  windowWidth  The window width.
   */
  void SetWindowWidth(float windowWidth);

  /**
   * @brief The actor with the specified ID will be changed when Update() is called.
   * @param[in]  id  Actor ID of actor which should be changed by the FrameCallback.
   */
  void AddId(uint32_t id);

private:
  /**
   * @brief Called when every frame is updated.
   * @param[in]  updateProxy     Used to set the world-matrix and sizes.
   * @param[in]  elapsedSeconds  Time elapsed time since the last frame (in seconds)
   * @return Whether we should keep rendering.
   */
  virtual bool Update(Dali::UpdateProxy& updateProxy, float elapsedSeconds);

private:
  Dali::Vector<uint32_t> mActorIdContainer; ///< Container of Actor IDs.
  float                  windowHalfWidth;   ///< Half the width of the window. Center is 0,0 in the world matrix.

  constexpr static float SIZE_MULTIPLIER = 2.0f; ///< Multiplier for the size to set as the actors hit the edge.
};

#endif // DEMO_FRAME_CALLBACK_H
