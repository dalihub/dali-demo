#ifndef MASKED_IMAGE_H
#define MASKED_IMAGE_H

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

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <string>

/**
 * @brief This namespace provides a helper function which creates an ImageView with a mask that matches the Circle geometry provided by ClippedImage.
 *
 * Using a mask yields much better quality than when using an image with a circle geometry.
 * @see ClippedImage
 */
namespace MaskedImage
{
/**
 * @brief Creates an image with a circular mask.
 *
 * @param[in]  imagePath  The path to the image to show.
 * @return The ImageView with a mask control.
 */
Dali::Toolkit::Control Create(const std::string& imagePath);

} // namespace MaskedImage

#endif // MASKED_IMAGE_H
