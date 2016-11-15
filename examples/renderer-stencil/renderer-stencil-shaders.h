#ifndef DALI_DEMO_RENDERER_STENCIL_SHADERS_H
#define DALI_DEMO_RENDERER_STENCIL_SHADERS_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/rendering/shader.h>

// Shader uniforms:
const char * const COLOR_UNIFORM_NAME( "uColor" );
const char * const OBJECT_DIMENSIONS_UNIFORM_NAME( "uObjectDimensions" );
const char * const STAGE_SIZE_UNIFORM_NAME( "uStageSize" );
const char * const LIGHT_POSITION_UNIFORM_NAME = "uLightPosition";
const char * const POSITION( "aPosition");
const char * const NORMAL( "aNormal" );
const char * const TEXTURE( "aTexCoord" );

// Shader for basic, per-vertex lighting (vertex):
const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec3  aPosition;
  attribute highp   vec3  aNormal;
  attribute highp   vec2  aTexCoord;

  varying   mediump vec2  vTexCoord;
  uniform   mediump mat4  uMvpMatrix;
  uniform   mediump vec3  uSize;
  uniform   mediump vec3  uObjectDimensions;
  varying   mediump vec3  vIllumination;
  uniform   mediump mat4  uModelView;
  uniform   mediump mat4  uViewMatrix;
  uniform   mediump mat3  uNormalMatrix;
  uniform   mediump vec3  uLightPosition;

  void main()
  {
    mediump vec4 vertexPosition = vec4( aPosition * uObjectDimensions, 1.0 );
    vertexPosition = uMvpMatrix * vertexPosition;

    vec4 mvVertexPosition = uModelView * vertexPosition;

    vec3 vectorToLight = normalize( mat3( uViewMatrix ) * uLightPosition - mvVertexPosition.xyz );

    vec3 normal = uNormalMatrix * aNormal;
    float lightDiffuse = max( dot( vectorToLight, normal ), 0.0 );
    vIllumination = vec3( lightDiffuse * 0.5 + 0.5 );

    gl_Position = vertexPosition;
  }
);

// Fragment shader.
const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2  vTexCoord;
  varying mediump vec3  vIllumination;
  uniform lowp    vec4  uColor;
  uniform sampler2D     sTexture;

  void main()
  {
    gl_FragColor = vec4( vIllumination.rgb * uColor.rgb, uColor.a );
  }
);

// Shader for basic, per-vertex lighting with texture (vertex):
const char* VERTEX_SHADER_TEXTURED = DALI_COMPOSE_SHADER(
  attribute mediump vec3  aPosition;
  attribute highp   vec3  aNormal;
  attribute highp   vec2  aTexCoord;

  varying   mediump vec2  vTexCoord;
  uniform   mediump mat4  uMvpMatrix;
  uniform   mediump vec3  uSize;
  uniform   mediump vec3  uObjectDimensions;
  varying   mediump vec3  vIllumination;
  uniform   mediump mat4  uModelView;
  uniform   mediump mat4  uViewMatrix;
  uniform   mediump mat3  uNormalMatrix;
  uniform   mediump vec3  uLightPosition;

  void main()
  {
    mediump vec4 vertexPosition = vec4( aPosition * uObjectDimensions, 1.0 );
    vertexPosition = uMvpMatrix * vertexPosition;

    vec4 mvVertexPosition = uModelView * vertexPosition;

    vec3 vectorToLight = normalize( mat3( uViewMatrix ) * uLightPosition - mvVertexPosition.xyz );

    vec3 normal = uNormalMatrix * aNormal;
    float lightDiffuse = max( dot( vectorToLight, normal ), 0.0 );
    vIllumination = vec3( lightDiffuse * 0.5 + 0.5 );

    vTexCoord = aTexCoord;
    gl_Position = vertexPosition;
  }
);

// Fragment shader.
const char* FRAGMENT_SHADER_TEXTURED = DALI_COMPOSE_SHADER(
  varying mediump vec2  vTexCoord;
  varying mediump vec3  vIllumination;
  uniform lowp    vec4  uColor;
  uniform sampler2D     sTexture;

  void main()
  {
    gl_FragColor = vec4( texture2D( sTexture, vTexCoord ).rgb * vIllumination.rgb * uColor.rgb, uColor.a );
  }
);

#endif // DALI_DEMO_RENDERER_STENCIL_SHADERS_H
