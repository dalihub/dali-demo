/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/file-stream.h>

#include "gltf-scene.h"

#include "pico-json.h"

namespace
{
// string contains enum type index encoded matching glTFAttributeType
const std::vector<std::string> GLTF_STR_ATTRIBUTE_TYPE = {
  "POSITION",
  "NORMAL",
  "TEXCOORD_0"};

const std::vector<std::pair<std::string, uint32_t>> GLTF_STR_COMPONENT_TYPE = {
  std::make_pair("VEC2", 2),
  std::make_pair("VEC3", 3),
  std::make_pair("VEC4", 4),
  std::make_pair("SCALAR", 1)};

glTFAttributeType glTFAttributeTypeStrToEnum(const std::string& name)
{
  int  index = -1;
  auto iter  = std::find_if(GLTF_STR_ATTRIBUTE_TYPE.begin(), GLTF_STR_ATTRIBUTE_TYPE.end(), [name, &index](const std::string& val) {
    index++;
    return val == name;
  });
  if(iter == GLTF_STR_ATTRIBUTE_TYPE.end())
  {
    return glTFAttributeType::UNDEFINED;
  }

  return static_cast<glTFAttributeType>(index);
}

uint32_t glTFComponentTypeStrToNum(const std::string& name)
{
  auto iter = std::find_if(GLTF_STR_COMPONENT_TYPE.begin(), GLTF_STR_COMPONENT_TYPE.end(), [name](const std::pair<std::string, uint32_t>& val) {
    return val.first == name;
  });
  if(iter == GLTF_STR_COMPONENT_TYPE.end())
  {
    return 0;
  }

  return iter->second;
}

template<class T>
struct JsonResult
{
  bool success;
  T    result;
  operator T() const
  {
    return static_cast<T>(result);
  }
};

template<>
struct JsonResult<bool>
{
  bool success;
  bool result;

  operator bool() const
  {
    return result;
  }
};

template<class Expected, class Converted = Expected>
JsonResult<picojson::value> JsonFindValue(const picojson::object& object, const std::string& name)
{
  auto iter = find_if(object.begin(), object.end(), [name](decltype(*object.begin())& item) {
    return item.first == name;
  });

  if(iter == object.end())
  {
    return {false};
  }

  return {true, iter->second};
};

template<class Expected, class Converted = Expected>
JsonResult<Converted> JsonGetValue(const picojson::value& val, const std::string& name, const Converted& onFail)
{
  if(val.contains(name))
  {
    return {true, static_cast<Converted>(val.get(name).get<Expected>())};
  }
  return {false, onFail};
}

template<class Expected, class Converted = Expected>
bool JsonGetValueOut(const picojson::value& val, const std::string& name, Converted& writeOut)
{
  if(val.contains(name))
  {
    writeOut = static_cast<Converted>(val.get(name).get<Expected>());
    return true;
  }
  return false;
}

/**
 * Safe json value accessor
 */
template<class Expected, class Converted = Expected>
JsonResult<std::vector<Converted>> JsonGetArray(const picojson::value& val, const std::string& name, std::vector<Converted> valueOnFail = {})
{
  if(val.contains(name))
  {
    const auto&            array = val.get(name).get<picojson::array>();
    std::vector<Converted> result{};
    for(const auto& item : array)
    {
      result.emplace_back(static_cast<Converted>(item.get<Expected>()));
    }
    return {true, result};
  }
  return {false, valueOnFail};
}

} // namespace

glTF::glTF(const std::string& filename)
{
  LoadFromFile(filename);
  ParseJSON();
}

void glTF::LoadFromFile(const std::string& filename)
{
  std::string jsonFile(filename);
  jsonFile += ".gltf";
  std::string binFile(filename);
  binFile += ".bin";

  // load binary

  GLTF_LOG("LoadFromFile: %s", binFile.c_str());
  mBuffer    = LoadFile(binFile);
  jsonBuffer = LoadFile(jsonFile);

  // Log errors
  if(mBuffer.empty())
  {
    GLTF_LOG("Error, buffer empty!");
  }
  else
  {
    GLTF_LOG("GLTF[BIN]: %s loaded, size = %d", binFile.c_str(), int(mBuffer.size()));
  }
  if(jsonBuffer.empty())
  {
    GLTF_LOG("Error, buffer GLTF empty!");
  }
  else
  {
    GLTF_LOG("GLTF: %s loaded, size = %d", binFile.c_str(), int(jsonBuffer.size()));
  }

  // Abort if errors
  if(jsonBuffer.empty() || mBuffer.empty())
  {
    return;
  }

  // parse json
  auto err = picojson::parse(jsonNode, std::string(reinterpret_cast<char*>(jsonBuffer.data())));
  if(!err.empty())
  {
    GLTF_LOG("GLTF: Error parsing %s, error: %s", jsonFile.c_str(), err.c_str());
    return;
  }
  else
  {
    GLTF_LOG("GLTF: %s loaded, size = %d", jsonFile.c_str(), int(jsonBuffer.size()));
  }
}

bool glTF::ParseJSON()
{
  if(!jsonNode.is<picojson::object>())
  {
    return false;
  }

  // Add dummy first node to nodes (scene node)
  mNodes.emplace_back();

  // Sources for textures to be resolved later
  std::vector<uint32_t>     textureSources{};
  std::vector<glTF_Texture> images{};

  for(auto& val : jsonNode.get<picojson::object>())
  {
    GLTF_LOG("node: %s", val.first.c_str());

    // Parse bufferviews
    if(val.first == "bufferViews")
    {
      auto bufferViews = val.second;
      for(auto& view : bufferViews.get<picojson::array>())
      {
        auto bufferIndex = uint32_t(view.get("buffer").get<double>());
        auto byteLength  = uint32_t(view.get("byteLength").get<double>());
        auto byteOffset  = uint32_t(view.get("byteOffset").get<double>());

        glTF_BufferView bufferView{};
        bufferView.bufferIndex = bufferIndex;
        bufferView.byteLength  = byteLength;
        bufferView.byteOffset  = byteOffset;

        mBufferViews.emplace_back(bufferView);
      }
    }

    // parse accessors
    else if(val.first == "accessors")
    {
      for(const auto& accessor : val.second.get<picojson::array>())
      {
        auto gltfAccessor          = glTF_Accessor{};
        gltfAccessor.bufferView    = uint32_t(accessor.get("bufferView").get<double>());
        gltfAccessor.componentType = uint32_t(accessor.get("componentType").get<double>());
        gltfAccessor.count         = uint32_t(accessor.get("count").get<double>());
        gltfAccessor.type          = accessor.get("type").get<std::string>();
        gltfAccessor.componentSize = glTFComponentTypeStrToNum(gltfAccessor.type);
        mAccessors.emplace_back(gltfAccessor);
      }
    }

    // parse meshes
    else if(val.first == "meshes")
    {
      for(const auto& mesh : val.second.get<picojson::array>())
      {
        glTF_Mesh gltfMesh{};
        gltfMesh.name = mesh.get("name").get<std::string>();

        // get primitives (in this implementation assuming single mesh consists of
        // one and only one primitive)
        const auto& primitive = mesh.get("primitives").get<picojson::array>()[0];
        const auto& attrs     = primitive.get("attributes").get<picojson::object>();
        for(const auto& attr : attrs)
        {
          auto type    = glTFAttributeTypeStrToEnum(attr.first);
          auto bvIndex = uint32_t(attr.second.get<double>());
          gltfMesh.attributes.emplace_back(std::make_pair(type, bvIndex));
          GLTF_LOG("GLTF: ATTR: type: %d, index: %d", int(type), int(bvIndex));
        }

        gltfMesh.indices  = uint32_t(primitive.get("indices").get<double>());
        gltfMesh.material = uint32_t(primitive.get("material").get<double>());
        mMeshes.emplace_back(gltfMesh);
      }
    }
    // parse cameras
    else if(val.first == "cameras")
    {
      glTF_Camera tgifCamera{};
      for(const auto& camera : val.second.get<picojson::array>())
      {
        tgifCamera.name          = camera.get("name").to_str();
        tgifCamera.isPerspective = (camera.get("type").to_str() == "perspective");
        if(tgifCamera.isPerspective)
        {
          const auto& perspective = camera.get("perspective");
          tgifCamera.yfov         = static_cast<float>(perspective.get("yfov").get<double>());
          tgifCamera.zfar         = static_cast<float>(perspective.get("zfar").get<double>());
          tgifCamera.znear        = static_cast<float>(perspective.get("znear").get<double>());
        }

        mCameras.emplace_back(tgifCamera);
      }
    }
    // parse nodes
    else if(val.first == "nodes")
    {
      auto nodeIndex = 1u;
      for(const auto& node : val.second.get<picojson::array>())
      {
        glTF_Node gltfNode{};
        gltfNode.name    = node.get("name").to_str();
        auto rotation    = JsonGetArray<double, float>(node, "rotation", {0.0f, 0.0f, 0.0f, 1.0f});
        auto translation = JsonGetArray<double, float>(node, "translation", {0.0f, 0.0f, 0.0f});
        auto scale       = JsonGetArray<double, float>(node, "scale", {1.0f, 1.0f, 1.0f});
        std::copy(rotation.result.begin(), rotation.result.end(), gltfNode.rotationQuaternion);
        std::copy(translation.result.begin(), translation.result.end(), gltfNode.translation);
        std::copy(scale.result.begin(), scale.result.end(), gltfNode.scale);

        auto children = JsonGetArray<double, uint32_t>(node, "children");
        if(children.success)
        {
          gltfNode.children = std::move(children.result);
        }
        gltfNode.index    = nodeIndex;
        gltfNode.cameraId = 0xffffffff;
        gltfNode.meshId   = 0xffffffff;

        auto cameraId = JsonGetValue<double, uint32_t>(node, "camera", 0xffffffff);
        if(cameraId.success)
        {
          gltfNode.cameraId = cameraId.result;
        }
        auto meshId = JsonGetValue<double, uint32_t>(node, "mesh", 0xffffffff);
        if(meshId.success)
        {
          gltfNode.meshId = meshId.result;
        }
        mNodes.emplace_back(gltfNode);
        ++nodeIndex;
      }
    }
    // parse scenes, note: only first scene is being parsed
    else if(val.first == "scenes")
    {
      auto&       sceneNode = mNodes[0];
      const auto& scene     = val.second.get<picojson::array>()[0];
      sceneNode.name        = JsonGetValue<std::string>(scene, "name", std::string());
      auto result           = JsonGetArray<double, uint32_t>(scene, "nodes");
      sceneNode.children    = result.result;
      sceneNode.index       = 0;
    }
    else if(val.first == "materials")
    {
      for(const auto& node : val.second.get<picojson::array>())
      {
        // Get pbr material, base color texture
        glTF_Material material{};
        material.doubleSided = JsonGetValue<bool>(node, "doubleSided", false).result;
        material.name        = JsonGetValue<std::string>(node, "name", std::string()).result;
        if(node.contains("pbrMetallicRoughness"))
        {
          const auto& node0 = node.get("pbrMetallicRoughness");
          if(node0.contains("baseColorTexture"))
          {
            const auto& node1                                       = node0.get("baseColorTexture");
            auto        index                                       = uint32_t(node1.get("index").get<double>());
            auto        texCoord                                    = uint32_t(node1.get("texCoord").get<double>());
            material.pbrMetallicRoughness.enabled                   = true;
            material.pbrMetallicRoughness.baseTextureColor.index    = index;
            material.pbrMetallicRoughness.baseTextureColor.texCoord = texCoord;
          }
        }
        mMaterials.emplace_back(material);
      }
    }
    else if(val.first == "textures")
    {
      for(const auto& item : val.second.get<picojson::array>())
      {
        auto source = JsonGetValue<double, uint32_t>(item, "source", 0xffffffff);
        textureSources.emplace_back(source.result);
      }
    }
    else if(val.first == "images")
    {
      for(const auto& item : val.second.get<picojson::array>())
      {
        glTF_Texture tex{};
        JsonGetValueOut<std::string>(item, "name", tex.name);
        JsonGetValueOut<std::string>(item, "uri", tex.uri);
        images.emplace_back(tex);
      }
    }
  }

  // Resolve cross-referencing
  for(const auto& source : textureSources)
  {
    mTextures.emplace_back(images[source]);
  }

  return true;
}

glTF_Buffer glTF::LoadFile(const std::string& filename)
{
  Dali::FileStream           fileStream(filename.c_str(), Dali::FileStream::READ | Dali::FileStream::BINARY);
  FILE*                      fin = fileStream.GetFile();
  std::vector<unsigned char> buffer;
  if(fin)
  {
    if(fseek(fin, 0, SEEK_END))
    {
      return {};
    }
    auto size = ftell(fin);
    if(fseek(fin, 0, SEEK_SET))
    {
      return {};
    }
    buffer.resize(unsigned(size));
    auto result = fread(buffer.data(), 1, size_t(size), fin);
    if(result != size_t(size))
    {
      GLTF_LOG("LoadFile: Result: %d", int(result));
      // return empty buffer
      return {};
    }
  }
  else
  {
    GLTF_LOG("LoadFile: Can't open file: errno = %d", errno);
  }

  return buffer;
}

std::vector<const glTF_Mesh*> glTF::GetMeshes() const
{
  std::vector<const glTF_Mesh*> retval;
  for(auto& mesh : mMeshes)
  {
    retval.emplace_back(&mesh);
  }
  return retval;
}

std::vector<const glTF_Camera*> glTF::GetCameras()
{
  std::vector<const glTF_Camera*> cameras;
  for(const auto& cam : mCameras)
  {
    cameras.emplace_back(&cam);
  }
  return cameras;
}

std::vector<unsigned char> glTF::GetMeshAttributeBuffer(const glTF_Mesh& mesh, const std::vector<glTFAttributeType>& attrTypes)
{
  // find buffer views
  struct Data
  {
    uint32_t accessorIndex{0u};
    uint32_t byteStride{0u};
    char*    srcPtr{nullptr};
  };
  std::vector<Data> data{};
  for(const auto& attrType : attrTypes)
  {
    std::find_if(mesh.attributes.begin(), mesh.attributes.end(), [&data, &attrType](const std::pair<glTFAttributeType, uint32_t>& item) {
      if(item.first == attrType)
      {
        data.emplace_back();
        data.back().accessorIndex = item.second;
      }
      return false;
    });
  }

  if(data.empty())
  {
    return {};
  }

  // number of attributes is same for the whole mesh so using very first
  // accessor

  glTF_Buffer retval{};

  // data is interleaved
  if(data.size() > 1)
  {
    auto     attributeCount  = mAccessors[data[0].accessorIndex].count; // / mAccessors[data[0].accessorIndex].componentSize;
    uint32_t attributeStride = 0;
    // now find buffer view stride for particular accessor
    for(auto& item : data)
    {
      auto& accessor = mAccessors[item.accessorIndex];

      // Update byte stride for this buffer view
      auto& bufferView = mBufferViews[accessor.bufferView];
      item.byteStride  = bufferView.byteLength / attributeCount;
      attributeStride += item.byteStride;
      item.srcPtr = reinterpret_cast<char*>(mBuffer.data()) + bufferView.byteOffset;
    }

    // now allocate final buffer and interleave data
    retval.resize(attributeStride * attributeCount);
    auto* dstPtr = retval.data();
    for(auto i = 0u; i < attributeCount; ++i)
    {
      for(auto& item : data)
      {
        std::copy(item.srcPtr,
                  item.srcPtr + item.byteStride,
                  reinterpret_cast<char*>(dstPtr));
        dstPtr += item.byteStride;
        item.srcPtr += item.byteStride;
      }
    }
  }
  else // copy data directly as single buffer
  {
    auto& bufferView = mBufferViews[mAccessors[data[0].accessorIndex].bufferView];
    retval.resize(bufferView.byteLength);
    std::copy(mBuffer.begin() + bufferView.byteOffset,
              mBuffer.begin() + bufferView.byteOffset + bufferView.byteLength,
              retval.begin());
  }
  return retval;
}

const glTF_Mesh* glTF::FindMeshByName(const std::string& name) const
{
  for(const auto& mesh : mMeshes)
  {
    if(mesh.name == name)
      return &mesh;
  }
  return nullptr;
}

uint32_t glTF::GetMeshAttributeCount(const glTF_Mesh* mesh) const
{
  const auto& accessor = mAccessors[mesh->attributes[0].second];
  return accessor.count; // / accessor.componentSize;
}

std::vector<uint16_t> glTF::GetMeshIndexBuffer(const glTF_Mesh* mesh) const
{
  // check GL component type
  const auto& accessor = mAccessors[mesh->indices];
  if(accessor.componentType == 0x1403) // GL_UNSIGNED_SHORT
  {
    std::vector<uint16_t> retval{};
    retval.resize(accessor.count);
    const auto& bufferView = mBufferViews[accessor.bufferView];
    const auto* srcPtr     = reinterpret_cast<const uint16_t*>(reinterpret_cast<const char*>(mBuffer.data()) + bufferView.byteOffset);
    std::copy(srcPtr, srcPtr + accessor.count, retval.data());
    return retval;
  }
  return {};
}

const glTF_Node* glTF::FindNodeByName(const std::string& name) const
{
  auto iter = std::find_if(mNodes.begin(), mNodes.end(), [name](const glTF_Node& node) {
    return !name.compare(node.name);
  });

  if(iter == mNodes.end())
  {
    return nullptr;
  }

  return &*iter;
}

const std::vector<glTF_Material>& glTF::GetMaterials() const
{
  return mMaterials;
}

const std::vector<glTF_Texture>& glTF::GetTextures() const
{
  return mTextures;
}
