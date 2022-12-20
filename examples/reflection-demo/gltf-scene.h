#ifndef GLTF_SCENE_H
#define GLTF_SCENE_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/debug.h>
#include <string>

// INTERNAL INCLUDES
#include "third-party/pico-json.h"

#define GLTF_LOG(...)                                                              \
  {                                                                                \
    Dali::Integration::Log::LogMessage(Dali::Integration::Log::INFO, __VA_ARGS__); \
  }

enum class glTFAttributeType
{
  POSITION   = 0,
  NORMAL     = 1,
  TEXCOORD_0 = 2,
  UNDEFINED,
};

struct glTF_Camera
{
  std::string name;

  // perspective setup
  float yfov;
  float zfar;
  float znear;

  bool isPerspective;
};

struct glTF_BufferView
{
  uint32_t bufferIndex;
  uint32_t byteLength;
  uint32_t byteOffset;
  void*    data;
};

struct glTF_Accessor
{
  uint32_t    bufferView;
  uint32_t    componentType;
  uint32_t    count;
  uint32_t    componentSize;
  std::string type;
};

struct glTF_Mesh
{
  std::string                                         name;
  std::vector<std::pair<glTFAttributeType, uint32_t>> attributes;
  uint32_t                                            indices;
  uint32_t                                            material;
};

struct glTF_Texture
{
  std::string uri;
  std::string name;
};

struct glTF_Material
{
  bool        doubleSided;
  std::string name;
  struct pbrMetallicRoughness
  {
    bool enabled{false};
    struct baseTextureColor
    {
      uint32_t index;
      uint32_t texCoord;
    } baseTextureColor;
  } pbrMetallicRoughness;
};

struct glTF_Node
{
  uint32_t              index{0u};
  std::string           name{};
  uint32_t              meshId{0xffffffff};
  uint32_t              cameraId{0xffffffff};
  glTF_Node*            parent{nullptr};
  std::vector<uint32_t> children{};

  // Transform
  float rotationQuaternion[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  float translation[3]        = {0.0f, 0.0f, 0.0f};
  float scale[3]              = {1.0f, 1.0f, 1.0f};
};

using glTF_Buffer = std::vector<unsigned char>;

/**
 * Simple glTF parser
 *
 * This implementation requires 2 files (it doesn't decode Base64 embedded in json)
 */
struct glTF
{
  glTF(const std::string& filename);
  ~glTF() = default;

  std::vector<const glTF_Mesh*> GetMeshes() const;

  std::vector<const glTF_Camera*> GetCameras();

  const std::vector<glTF_Material>& GetMaterials() const;

  const std::vector<glTF_Texture>& GetTextures() const;

  const std::vector<glTF_Node>& GetNodes() const
  {
    return mNodes;
  }

  /**
   * MESH interface
   */
  /**
   * Returns a copy of attribute buffer
   * @return
   */
  std::vector<unsigned char> GetMeshAttributeBuffer(const glTF_Mesh& mesh, const std::vector<glTFAttributeType>& attrTypes);
  uint32_t                   GetMeshAttributeCount(const glTF_Mesh* mesh) const;
  const glTF_Mesh*           FindMeshByName(const std::string& name) const;

  /**
   * Returns a copy of index buffer
   * @return
   */
  std::vector<uint16_t> GetMeshIndexBuffer(const glTF_Mesh* mesh) const;

  const glTF_Node* FindNodeByName(const std::string& name) const;

private:
  void LoadFromFile(const std::string& filename);

  glTF_Buffer LoadFile(const std::string& filename);

  bool ParseJSON();

  std::vector<glTF_Mesh>       mMeshes;
  std::vector<glTF_Camera>     mCameras;
  std::vector<glTF_BufferView> mBufferViews;
  std::vector<glTF_Accessor>   mAccessors;
  std::vector<glTF_Node>       mNodes;
  std::vector<glTF_Material>   mMaterials;
  std::vector<glTF_Texture>    mTextures;
  glTF_Buffer                  mBuffer;
  glTF_Buffer                  jsonBuffer;

  // json nodes
  picojson::value jsonNode;
};

#endif //DALI_CMAKE_GLTF_SCENE_H
