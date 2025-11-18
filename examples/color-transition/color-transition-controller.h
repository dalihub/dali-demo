#ifndef DALI_DEMO_COLOR_TRANSITION_CONTROLLER_H
#define DALI_DEMO_COLOR_TRANSITION_CONTROLLER_H
/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include "dali/public-api/actors/actor.h"
#include "dali/public-api/adaptor-framework/window.h"
#include "dali/public-api/animation/animation.h"
#include "dali/public-api/object/weak-handle.h"
#include "dali/public-api/render-tasks/render-task-list.h"
#include "dali/public-api/render-tasks/render-task.h"
#include "dali/public-api/rendering/texture.h"

class ColorTransitionController : public Dali::ConnectionTracker
{
public:
  using OnFinished = void (*)(void*);

  ColorTransitionController(Dali::Actor root, Dali::WeakHandle<Dali::RenderTaskList> renderTasks, Dali::Actor content, Dali::RenderTaskList tasks, Dali::Vector3 initialColor);
  ~ColorTransitionController();

  Dali::Actor GetComposite();

  void SetFlowMap(Dali::Texture flowMap);

  void SetUvTransform(float initialRotation, float initialScale, float targetRotation, float targetScale);

  void SetOnFinished(OnFinished onFinished, void* onFinishedData);

  void RequestTransition(float duration, const Dali::Vector3& targetColor);

private:
  void OnTransitionFinished(Dali::Animation& anim);

  Dali::Actor           mComposite;
  Dali::Property::Index mPropFlow;
  Dali::Property::Index mPropUvTransform;
  Dali::Property::Index mPropRgb[2];

  Dali::RenderTask mRtCompositor;

  Dali::Animation mAnimation;

  OnFinished mOnFinished     = nullptr;
  void*      mOnFinishedData = nullptr;

  Dali::WeakHandle<Dali::RenderTaskList> mWeakRenderTasks;
};

#endif // DALI_DEMO_COLOR_TRANSITION_CONTROLLER_H
