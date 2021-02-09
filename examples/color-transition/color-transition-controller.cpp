/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include "color-transition-controller.h"
#include "dali-toolkit/dali-toolkit.h"
#include "dali/dali.h"
#include "generated/color-transition-controller-composite-frag.h"
#include "generated/color-transition-controller-composite-vert.h"
#include "utils.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const Vector4 BG_COLOR = Vector4(0.f, 0.f, 0.f, 0.f);

} // namespace

ColorTransitionController::ColorTransitionController(WeakHandle<RenderTaskList> window, Actor content, RenderTaskList tasks, Vector3 initialColor)
: mWeakRenderTasks(window)
{
  auto contentSize = content.GetProperty(Actor::Property::SIZE).Get<Vector2>();

  auto defaultTask = tasks.GetTask(0);

  // create rendertarget and rendertask
  auto rtt = Texture::New(TextureType::TEXTURE_2D, Pixel::Format::RGBA8888, static_cast<uint32_t>(contentSize.x), static_cast<uint32_t>(contentSize.y));

  auto fbo = FrameBuffer::New(rtt.GetWidth(), rtt.GetHeight(), FrameBuffer::Attachment::NONE);
  fbo.AttachColorTexture(rtt);

  RenderTask rtCompositor = tasks.CreateTask();
  rtCompositor.SetClearEnabled(true);
  rtCompositor.SetClearColor(BG_COLOR);
  rtCompositor.SetFrameBuffer(fbo);
  rtCompositor.SetSourceActor(content);
  rtCompositor.SetExclusive(true);
  mRtCompositor = rtCompositor;

  // renderer for the composite
  ACTOR_DECL(composite);
  CenterActor(composite);
  composite.SetProperty(Actor::Property::SIZE, contentSize);

  mPropFlow        = composite.RegisterProperty("uFlow", -1.f);
  mPropUvTransform = composite.RegisterProperty("uUvTransform", Vector4(0.f, 0.f, 1.f, 1.f));
  mPropRgb[0]      = composite.RegisterProperty("uRgb[0]", initialColor);
  mPropRgb[1]      = composite.RegisterProperty("uRgb[1]", Vector3::ONE);

  auto geomComposite = CreateQuadGeometry();

  auto tsComposite = TextureSet::New();
  tsComposite.SetTexture(0, rtt);

  Sampler flowSampler = Sampler::New();
  flowSampler.SetWrapMode(WrapMode::REPEAT, WrapMode::REPEAT);
  tsComposite.SetSampler(1, flowSampler);

  auto shdComposite = Shader::New(SHADER_COLOR_TRANSITION_CONTROLLER_COMPOSITE_VERT, SHADER_COLOR_TRANSITION_CONTROLLER_COMPOSITE_FRAG);

  auto compositeRenderer = CreateRenderer(tsComposite, geomComposite, shdComposite);
  composite.AddRenderer(compositeRenderer);

  mComposite = composite;

  // create transition animation
  Animation anim = Animation::New(1.f);
  anim.SetEndAction(Animation::EndAction::DISCARD);
  anim.AnimateTo(Property(composite, mPropFlow), 1.f);
  anim.FinishedSignal().Connect(this, &ColorTransitionController::OnTransitionFinished);
  mAnimation = anim;
}

ColorTransitionController::~ColorTransitionController()
{
  if(auto renderTasks = mWeakRenderTasks.GetHandle())
  {
    renderTasks.RemoveTask(mRtCompositor);
  }
}

Dali::Actor ColorTransitionController::GetComposite()
{
  return mComposite;
}

void ColorTransitionController::SetFlowMap(Texture flowMap)
{
  auto renderer = mComposite.GetRendererAt(0);
  auto texSet   = renderer.GetTextures();
  texSet.SetTexture(1, flowMap);
}

void ColorTransitionController::SetUvTransform(float initialRotation, float initialScale, float targetRotation, float targetScale)
{
  mComposite.SetProperty(mPropUvTransform, Vector4(initialRotation, initialScale, targetRotation, targetScale));
}

void ColorTransitionController::RequestTransition(float duration, const Dali::Vector3& targetColor)
{
  mComposite.SetProperty(mPropRgb[1], targetColor);

  mAnimation.SetDuration(duration);
  mAnimation.SetCurrentProgress(0.f);
  mAnimation.Play();
}

void ColorTransitionController::SetOnFinished(OnFinished onFinished, void* data)
{
  mOnFinished     = onFinished;
  mOnFinishedData = data;
}

void ColorTransitionController::OnTransitionFinished(Animation& anim)
{
  // shift the secondary color down
  Vector3 color1 = mComposite.GetProperty(mPropRgb[1]).Get<Vector3>();
  mComposite.SetProperty(mPropRgb[0], color1);

  if(mOnFinished)
  {
    mOnFinished(mOnFinishedData);
  }
}
