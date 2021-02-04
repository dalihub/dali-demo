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

#include "game-renderer.h"
#include "game-model.h"
#include "game-texture.h"

#include "generated/game-renderer-frag.h"
#include "generated/game-renderer-vert.h"

#include <dali/dali.h>

GameRenderer::GameRenderer()
: mModel(NULL),
  mTexture(NULL)
{
}

GameRenderer::~GameRenderer()
{
}

void GameRenderer::SetModel(GameModel* model)
{
  mModel = model;
  Setup();
}

void GameRenderer::SetMainTexture(GameTexture* texture)
{
  mTexture = texture;
  Setup();
}

void GameRenderer::Setup()
{
  if(!mRenderer && mModel)
  {
    Dali::Shader shader = Dali::Shader::New(SHADER_GAME_RENDERER_VERT, SHADER_GAME_RENDERER_FRAG);
    mRenderer           = Dali::Renderer::New(mModel->GetGeometry(), shader);
    mRenderer.SetProperty(Dali::Renderer::Property::DEPTH_WRITE_MODE, Dali::DepthWriteMode::ON);
    mRenderer.SetProperty(Dali::Renderer::Property::DEPTH_FUNCTION, Dali::DepthFunction::LESS_EQUAL);
    mRenderer.SetProperty(Dali::Renderer::Property::DEPTH_TEST_MODE, Dali::DepthTestMode::ON);
  }

  Dali::TextureSet textureSet;
  Dali::Geometry   geometry;

  if(mModel)
  {
    geometry = mModel->GetGeometry();
  }

  if(mTexture && mTexture->GetTextureSet())
  {
    textureSet = mTexture->GetTextureSet();
  }

  if(textureSet && geometry)
  {
    mRenderer.SetGeometry(geometry);
    mRenderer.SetTextures(textureSet);
  }
}

Dali::Renderer& GameRenderer::GetRenderer()
{
  return mRenderer;
}
