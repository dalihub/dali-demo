#ifndef GAME_MODEL_H
#define GAME_MODEL_H

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

#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/property-buffer.h>

#include <inttypes.h>

/**
 * @brief The ModelHeader struct
 * Model file header structure
 */
struct ModelHeader
{
  uint32_t tag;                   /// 'MODV' tag
  uint32_t version;               /// File version
  uint32_t vertexBufferSize;      /// total size of the vertex buffer to allocate
  uint32_t attributeCount;        /// number of stored attributes
  uint32_t attributeFormat[16];   /// format encoded as ((type << 16)|(count)); 'type' represents primitive type, 'count' represents number of components ( 1-4 )
  uint32_t attributeOffset[16];   /// attribute offsets
  uint32_t attributeSize[16];     /// attribute size in bytes
  uint32_t vertexStride;          /// vertex stride
  uint32_t reserved;              /// reserved, may point at additional structure
  uint32_t dataBeginOffset;       /// start of actual vertex data
};

/**
 * @brief The GameModel class
 * GameModel represents model geometry. It loads model data from external model file ( .mod file ).
 * Such data is ready to be used as GL buffer so it can be copied directly into the PropertyBuffer
 * object.
 *
 * Model file is multi-architecture so can be loaded on little and big endian architectures
 */
class GameModel
{
public:

  /**
   * Creates an instance of GameModel and loads the '.mod' file
   * @param[in] filename Name of file to load
   */
  GameModel( const char* filename );

  /**
   * Destroys an instance of GameModel
   */
  ~GameModel();

  /**
   * Returns DALi geometry object
   * @return Returns DALi geometry object
   */
  Dali::Geometry& GetGeometry();

  /**
   * Checks status of model, returns false if failed to load
   * @return true if model has been loaded, false otherwise
   */
  bool IsReady();

  /**
   * Returns unique Id of the texture
   * @return Unique Id
   */
  uint32_t GetUniqueId();

private:

  Dali::Geometry        mGeometry;
  Dali::PropertyBuffer  mVertexBuffer;

  ModelHeader           mHeader;

  uint32_t              mUniqueId;
  bool                  mIsReady;
};

#endif
