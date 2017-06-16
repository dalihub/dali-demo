#ifndef KTX_LOADER_H
#define KTX_LOADER_H

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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/pixel-data.h>

using namespace Dali;

namespace PbrDemo
{

/**
 * @brief Stores the pixel data objects for each face of the cube texture and their mipmaps.
 */
struct CubeData
{
  std::vector< std::vector<Dali::PixelData> > img;
};

/**
 * @brief Loads a cube map texture from a ktx file.
 *
 * @param[in] path The file path.
 * @param[out] cubedata The data structure with all pixel data objects.
 */
bool LoadCubeMapFromKtxFile( const std::string& path, CubeData& cubedata );

} // namespace PbrDemo

#endif //KTX_LOADER_H
