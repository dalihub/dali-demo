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

const std::string_view SHADER_BUBBLE_EFFECT_FRAG
{
R"(//@name bubble-effect.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sBackground;
UNIFORM sampler2D sBubbleShape;
INPUT highp float vPercentage;
INPUT highp vec2 vEffectTexCoord;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};

void main()
{
  // Get the emit pisition color, and Mix with the actor color
  mediump vec4 fragColor = TEXTURE(sBackground, vEffectTexCoord)*uColor;

  // Apply the shape defined by the texture contained sBubbleShape
  // And make the opacity being 0.7, and animate from 0.7 to 0 during the last 1/3 of movement
  fragColor.a*= TEXTURE(sBubbleShape, vTexCoord).a * ( 2.1 - max( vPercentage*2.1, 1.4 ) );

  gl_FragColor = fragColor;
}
)"
};
