#ifndef DALI_DEMO_MODELPBR_H
#define DALI_DEMO_MODELPBR_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/rendering/renderer.h>

using namespace Dali;

class ModelPbr
{
public:
  /**
   * @brief Constructor.
   * Does nothing.
   */
  ModelPbr();

  /**
   * @brief Destructor
   * Does nothing.
   */
  ~ModelPbr();

  /**
   * @brief Initializes an Actor for the Physically Based Rendering.
   *
   * @note Call InitTexture() before calling this method.
   *
   * It creates a geometry for the renderer.
   * According to the parameter @p modelType it may create a @e quad, a @e sphere or load a model.
   * If @p modelType is @e MODEL, the @p modelUrl parameter needs to be set with a valid url pointing a file with a @e obj model.
   *
   * A renderer is created with the geometry and the shader and it's set to the an actor that can be retrieved by calling GetActor().
   * The @p position and @p size is set to the actor.
   *
   * @param[in] shader The Physically Based Rendering shader.
   * @param[in] modelType The type of geometry.
   * @param[in] modelUrl The url of the model if the paramterer @p modelType is @e MODEL.
   * @param[in] position The position of the actor.
   * @param[in] size The size of the actor.
   */
  void Init( Shader shader, const std::string& modelUrl, const Vector3& position, const Vector3& size );

  /**
   * @brief Initializes the @p mTextureSet member with the needed textures for Physically Based Rendering.
   *
   * @param[in] albedoMetalTexture The albedo metal texture.
   * @param[in] normalRoughTexture The normal rough texture.
   * @param[in] diffuseTexture The diffuse texture.
   * @param[in] specularTexture The specular texture.
   */
  void InitTexture( Texture albedoMetalTexture, Texture normalRoughTexture, Texture diffuseTexture, Texture specularTexture );

  /**
   * @brief Retrieves the actor created by calling the Init() method.
   *
   * @return The Actor for the Physically Based Rendering.
   */
  Actor &GetActor();

private:

  /**
   * @brief Creates a geometry from a @e obj model.
   *
   * @param[in] url A url pointing a file with a @e obj model.
   *
   * @return The geometry.
   */
  Geometry CreateGeometry( const std::string& url );

  Actor mActor;
  TextureSet mTextureSet;
};

#endif // DALI_DEMO_MODELPBR_H

