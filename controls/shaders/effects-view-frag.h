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

const std::string_view SHADER_EFFECTS_VIEW_FRAG
{
R"(//@name effects-view.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 effectColor;
};

void main()
{
  gl_FragColor = effectColor;
  gl_FragColor.a *= TEXTURE( sTexture, vTexCoord).a;
}
)"
};
