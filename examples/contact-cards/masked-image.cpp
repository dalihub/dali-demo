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

// HEADER
#include "masked-image.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>

namespace MaskedImage
{
using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* const IMAGE_MASK(DEMO_IMAGE_DIR "contact-cards-mask.png");
} // unnamed namespace

Dali::Toolkit::Control Create(const std::string& imagePath)
{
  Control maskedImage = ImageView::New();
  maskedImage.SetProperty(
    Toolkit::ImageView::Property::IMAGE,
    Property::Map{{Visual::Property::TYPE, Toolkit::Visual::Type::IMAGE},
                  {ImageVisual::Property::URL, imagePath},
                  {ImageVisual::Property::ALPHA_MASK_URL, IMAGE_MASK}});
  return maskedImage;
}

} // namespace MaskedImage
