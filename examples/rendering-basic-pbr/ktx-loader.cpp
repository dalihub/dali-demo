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

// FILE HEADER
#include "ktx-loader.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/file-stream.h>
#include <dali/integration-api/debug.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>

namespace PbrDemo
{
struct KtxFileHeader
{
  char     identifier[12];
  uint32_t endianness;
  uint32_t glType; //(UNSIGNED_BYTE, UNSIGNED_SHORT_5_6_5, etc.)
  uint32_t glTypeSize;
  uint32_t glFormat;         //(RGB, RGBA, BGRA, etc.)
  uint32_t glInternalFormat; //For uncompressed textures, specifies the internalformat parameter passed to glTexStorage*D or glTexImage*D
  uint32_t glBaseInternalFormat;
  uint32_t pixelWidth;
  uint32_t pixelHeight;
  uint32_t pixelDepth;
  uint32_t numberOfArrayElements;
  uint32_t numberOfFaces; //Cube map faces are stored in the order: +X, -X, +Y, -Y, +Z, -Z.
  uint32_t numberOfMipmapLevels;
  uint32_t bytesOfKeyValueData;
};

/**
 * Convert KTX format to Dali::Pixel::Format
 */
bool ConvertPixelFormat(const uint32_t ktxPixelFormat, Dali::Pixel::Format& format)
{
  switch(ktxPixelFormat)
  {
    case 0x93B0: // GL_COMPRESSED_RGBA_ASTC_4x4_KHR
    {
      format = Dali::Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR;
      break;
    }
    case 0x881B: // GL_RGB16F
    {
      format = Dali::Pixel::RGB16F;
      break;
    }
    case 0x8815: // GL_RGB32F
    {
      format = Dali::Pixel::RGB32F;
      break;
    }
    case 0x8C3A: // GL_R11F_G11F_B10F
    {
      format = Dali::Pixel::R11G11B10F;
      break;
    }
    case 0x8D7C: // GL_RGBA8UI
    {
      format = Dali::Pixel::RGBA8888;
      break;
    }
    case 0x8D7D: // GL_RGB8UI
    {
      format = Dali::Pixel::RGB888;
      break;
    }
    default:
    {
      return false;
    }
  }

  return true;
}

bool LoadCubeMapFromKtxFile(const std::string& path, CubeData& cubedata)
{
  Dali::FileStream daliFileStream(path);
  FILE*            fp(daliFileStream.GetFile());
  if(!fp)
  {
    return false;
  }

  KtxFileHeader header;
  int           result = fread(&header, sizeof(KtxFileHeader), 1u, fp);
  if(0 == result)
  {
    return false;
  }

  // Skip the key-values:
  if(fseek(fp, header.bytesOfKeyValueData, SEEK_CUR))
  {
    return false;
  }

  cubedata.img.resize(header.numberOfFaces);

  for(unsigned int face = 0; face < header.numberOfFaces; ++face) //array_element must be 0 or 1
  {
    cubedata.img[face].resize(header.numberOfMipmapLevels);
  }

  if(0 == header.numberOfMipmapLevels)
  {
    header.numberOfMipmapLevels = 1u;
  }

  if(0 == header.numberOfArrayElements)
  {
    header.numberOfArrayElements = 1u;
  }

  if(0 == header.pixelDepth)
  {
    header.pixelDepth = 1u;
  }

  if(0 == header.pixelHeight)
  {
    header.pixelHeight = 1u;
  }

  Dali::Pixel::Format daliformat = Pixel::RGB888;

  ConvertPixelFormat(header.glInternalFormat, daliformat);

  for(unsigned int mipmapLevel = 0; mipmapLevel < header.numberOfMipmapLevels; ++mipmapLevel)
  {
    uint32_t byteSize = 0;
    if(fread(&byteSize, sizeof(byteSize), 1u, fp) != 1)
    {
      return false;
    }

    if(0 != byteSize % 4u)
    {
      byteSize += 4u - byteSize % 4u;
    }

    for(unsigned int arrayElement = 0; arrayElement < header.numberOfArrayElements; ++arrayElement) // arrayElement must be 0 or 1
    {
      for(unsigned int face = 0; face < header.numberOfFaces; ++face)
      {
        std::unique_ptr<uint8_t, void (*)(void*)> img(static_cast<unsigned char*>(malloc(byteSize)), free); // resources will be freed when the PixelData is destroyed.
        if(fread(img.get(), byteSize, 1u, fp) != 1)
        {
          return false;
        }
        cubedata.img[face][mipmapLevel] = PixelData::New(img.release(), byteSize, header.pixelWidth, header.pixelHeight, daliformat, PixelData::FREE);
      }
    }

    header.pixelHeight /= 2u;
    header.pixelWidth /= 2u;
  }

  return true;
}

} // namespace PbrDemo
