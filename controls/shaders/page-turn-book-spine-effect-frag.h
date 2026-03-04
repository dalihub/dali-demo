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

const std::string_view SHADER_PAGE_TURN_BOOK_SPINE_EFFECT_FRAG
{
R"(//@name page-turn-book-spine-effect.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM vec2 uSpineShadowParameter;
};

UNIFORM_BLOCK SharedBlock
{
UNIFORM vec3 uSize;
};

void main()
{
  if( gl_FrontFacing ) // display front side
  {
    gl_FragColor = TEXTURE( sTexture, vTexCoord ) * uColor;
  }
  else // display back side, flip the image horizontally by changing the x component of the texture coordinate
  {
    gl_FragColor = TEXTURE( sTexture, vec2( 1.0 - vTexCoord.x, vTexCoord.y ) ) * uColor;
  }
  // display book spine, a stripe of shadowed texture
  float pixelPos = vTexCoord.x * uSize.x;
  if( pixelPos < uSpineShadowParameter.x )
  {
    float x = pixelPos - uSpineShadowParameter.x;
    float y = sqrt( uSpineShadowParameter.x*uSpineShadowParameter.x - x*x );
    vec2 spineNormal = normalize(vec2(uSpineShadowParameter.y*x/uSpineShadowParameter.x, y));
    gl_FragColor.rgb *= spineNormal.y;
  }
}
)"
};
