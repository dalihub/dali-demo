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

#pragma once

const std::string_view SHADER_BLOOM_VIEW_COMPOSITE_SHADER_FRAG
{
R"(//@name bloom-view-composite-shader.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM sampler2D sEffect;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM float uBloomIntensity;
  UNIFORM float uImageIntensity;
  UNIFORM float uBloomSaturation;
  UNIFORM float uImageSaturation;
};

vec4 ChangeSaturation(vec4 col, float sat)
{
  float grey = dot(col.rgb, vec3(0.3, 0.6, 0.1));
  return mix(vec4(grey, grey, grey, 1.0), col, sat);
}

void main()
{
  highp vec4 image;
  highp vec4 bloom;
  image = TEXTURE(sTexture, vTexCoord);
  bloom = TEXTURE(sEffect, vTexCoord);
  image = ChangeSaturation(image, uImageSaturation) * uImageIntensity;
  bloom = ChangeSaturation(bloom, uBloomSaturation) * uBloomIntensity;
  image *= 1.0 - clamp(bloom, 0.0, 1.0); // darken base where bloom is strong, to prevent excessive burn-out of result
  gl_FragColor = image + bloom;
}
)"
};
