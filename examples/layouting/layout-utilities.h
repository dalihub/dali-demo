/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#ifndef DALI_DEMO_LAYOUT_UTILITIES_H
#define DALI_DEMO_LAYOUT_UTILITIES_H

#include <dali-toolkit/public-api/controls/control.h>

namespace LayoutUtilities
{
/**
 * @brief
 * Create a root layout, ideally Dali would have a default layout in the root layer.
 * Without this root layer the mAbsoluteLayout (or any other layout) will not
 * honour WIDTH_SPECIFICATION or HEIGHT_SPECIFICATION settings.
 * It uses the default stage size and ideally should have a layout added to it.
 * @return resulting root layout
 */
Dali::Toolkit::Control CreateRootContainer();
} // namespace LayoutUtilities

#endif // DALI_DEMO_LAYOUT_UTILITIES_H