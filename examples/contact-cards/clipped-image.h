#ifndef CLIPPED_IMAGE_H
#define CLIPPED_IMAGE_H

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

// EXTERNAL INCLUDES
#include <string>
#include <dali-toolkit/public-api/controls/control.h>

/**
 * @brief This namespace provides a helper function to create a control that clips an image either as a quad or a circle.
 *
 * The CIRCLE_GEOMETRY and QUAD_GEOMETRY constants can be used to set or animate to the different clipping geometries.
 */
namespace ClippedImage
{

extern const float CIRCLE_GEOMETRY; ///< Setting or animating the returned propertyIndex in Create() to this value will provide a circle geometry on the image @see Create
extern const float QUAD_GEOMETRY; ///< Setting or animating the returned propertyIndex in Create() to this value will provide a quad geometry on the image @see Create

/**
 * @brief Creates a clipping image whose geometry (i.e. clip area) can be morphed between a circle and a quad by animating the propertyIndex out parameter.
 *
 * The propertyIndex parameter can be set or animated to CIRCLE_GEOMETRY or QUAD_GEOMETRY depending on the type of clipping required.
 * If set to a value between these two constants, then the resulting geometry will be somewhere in between a circle and a quad.
 *
 * @param[in]   imagePath      The path to the image to show.
 * @param[out]  propertyIndex  Gets set with the property index which the caller can animate using the CIRCLE_GEOMETRY & QUAD_GEOMETRY values.
 * @return The image-mesh control
 */
Dali::Toolkit::Control Create( const std::string& imagePath, Dali::Property::Index& propertyIndex );

} // namespace ClippedImage

#endif // CLIPPED_IMAGE_H
