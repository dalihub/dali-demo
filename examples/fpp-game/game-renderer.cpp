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
 *
 */

#include "game-model.h"
#include "game-texture.h"
#include "game-renderer.h"

#include <dali/dali.h>

namespace
{

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
    attribute highp vec3 aPosition;\n
    attribute highp vec3 aNormal;\n
    attribute highp vec2 aTexCoord;\n
    uniform highp mat4 uMvpMatrix;\n
    varying highp vec2 vTexCoord;\n
    void main()\n
    {\n
      gl_Position = uMvpMatrix * vec4(aPosition, 1.0 );\n
      vTexCoord = aTexCoord;\n
      vTexCoord.y = 1.0 - vTexCoord.y;\n
    }\n
)
    ;
const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
    uniform sampler2D sTexture;\n
    varying highp vec2 vTexCoord;\n
    void main()\n
    {\n
      gl_FragColor = texture2D( sTexture, vTexCoord ) * vec4(1.2, 1.2, 1.2, 1.0);\n
    }\n
);

}

GameRenderer::GameRenderer()
  : mModel( NULL ),
    mTexture( NULL )
{
}

GameRenderer::~GameRenderer()
{
}

void GameRenderer::SetModel( GameModel* model )
{
  mModel = model;
  Setup();
}

void GameRenderer::SetMainTexture( GameTexture* texture )
{
  mTexture = texture;
  Setup();
}

void GameRenderer::Setup()
{
  if( !mRenderer && mModel )
  {
    Dali::Shader shader = Dali::Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    mRenderer = Dali::Renderer::New( mModel->GetGeometry(), shader );
    mRenderer.SetProperty( Dali::Renderer::Property::DEPTH_WRITE_MODE, Dali::DepthWriteMode::ON );
    mRenderer.SetProperty( Dali::Renderer::Property::DEPTH_FUNCTION, Dali::DepthFunction::LESS_EQUAL );
    mRenderer.SetProperty( Dali::Renderer::Property::DEPTH_TEST_MODE, Dali::DepthTestMode::ON );
  }

  Dali::TextureSet textureSet;
  Dali::Geometry geometry;

  if( mModel )
  {
    geometry = mModel->GetGeometry();
  }

  if( mTexture && mTexture->GetTextureSet() )
  {
    textureSet = mTexture->GetTextureSet();
  }

  if( textureSet && geometry )
  {
    mRenderer.SetGeometry( geometry );
    mRenderer.SetTextures( textureSet );
  }
}

Dali::Renderer& GameRenderer::GetRenderer()
{
  return mRenderer;
}



