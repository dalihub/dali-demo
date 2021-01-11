/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include "utils.h"
#include "dali/dali.h"
#include "dali-toolkit/dali-toolkit.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const Vector4 BG_COLOR = Vector4(0.f, 0.f, 0.f, 0.f);

const char* const COMPOSITE_VSH = DALI_COMPOSE_SHADER(
precision mediump float;

// <DALI>
uniform mat4 uMvpMatrix;
uniform vec3 uSize;
// </DALI>

uniform float uFlow;
uniform vec4 uUvTransform; // rotation, scale (initial, target))

attribute vec2 aPosition;

varying vec2 vUv;
varying vec2 vUvFlow;

void main()
{
  vec4 position = uMvpMatrix * vec4(aPosition * uSize.xy, 0., 1.);

  gl_Position = position;

  vec2 uv = position.xy / (position.ww * 2.);
  vUv = uv + vec2(.5);

  float alpha = uFlow * .5 + .5;
  vec2 uvRotationScale = mix(uUvTransform.xy, uUvTransform.zw, alpha);
  float c = cos(uvRotationScale.x) * uvRotationScale.y;
  float s = sin(uvRotationScale.x) * uvRotationScale.y;
  vec4 uvMatrix = vec4(c, -s, s, c);
  uv = vec2(dot(uvMatrix.xy, uv), dot(uvMatrix.zw, uv));

  // N.B. +y is down which is well aligned with the inverted y of the off-screen render,
  // however we need to flip the y of the uvs for the flow map.
  vUvFlow = vec2(uv.x + .5, .5 - uv.y);
});

const char* const COMPOSITE_FSH = DALI_COMPOSE_SHADER(
precision mediump float;

const float kStepsilon = 1e-2;

uniform sampler2D sColor;
uniform sampler2D sFlowMap;

uniform float uFlow;
uniform vec3 uRgb[2];

varying vec2 vUv;
varying vec2 vUvFlow;

void main()
{
  vec4 colorAlpha = texture2D(sColor, vUv);
  float flow = smoothstep(.5 - kStepsilon, .5 + kStepsilon, clamp(uFlow + texture2D(sFlowMap, vUvFlow).r, 0., 1.));

  gl_FragColor = vec4(mix(colorAlpha.rgb, mix(uRgb[0], uRgb[1], flow), colorAlpha.a), 1.);
});

} // nonamespace

ColorTransitionController::ColorTransitionController(WeakHandle<RenderTaskList> window, Actor content, RenderTaskList tasks, Vector3 initialColor)
: mWeakRenderTasks(window)
{
  auto contentSize = content.GetProperty(Actor::Property::SIZE).Get<Vector2>();

  auto defaultTask = tasks.GetTask(0);

  // create rendertarget and rendertask
  auto rtt = Texture::New(TextureType::TEXTURE_2D, Pixel::Format::RGBA8888,
    static_cast<uint32_t>(contentSize.x), static_cast<uint32_t>(contentSize.y));

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

  mPropFlow = composite.RegisterProperty("uFlow", -1.f);
  mPropUvTransform = composite.RegisterProperty("uUvTransform", Vector4(0.f, 0.f, 1.f, 1.f));
  mPropRgb[0] = composite.RegisterProperty("uRgb[0]", initialColor);
  mPropRgb[1] = composite.RegisterProperty("uRgb[1]", Vector3::ONE);

  auto geomComposite = CreateQuadGeometry();

  auto tsComposite = TextureSet::New();
  tsComposite.SetTexture(0, rtt);

  Sampler flowSampler = Sampler::New();
  flowSampler.SetWrapMode(WrapMode::REPEAT, WrapMode::REPEAT);
  tsComposite.SetSampler(1, flowSampler);

  auto shdComposite = Shader::New(COMPOSITE_VSH, COMPOSITE_FSH);

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
  if (auto renderTasks = mWeakRenderTasks.GetHandle())
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
  auto texSet = renderer.GetTextures();
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
  mOnFinished = onFinished;
  mOnFinishedData = data;
}

void ColorTransitionController::OnTransitionFinished(Animation& anim)
{
  // shift the secondary color down
  Vector3 color1 = mComposite.GetProperty(mPropRgb[1]).Get<Vector3>();
  mComposite.SetProperty(mPropRgb[0], color1);

  if (mOnFinished)
  {
    mOnFinished(mOnFinishedData);
  }
}
