#ifndef DALI_DEMO_CONTROLS_COMMON_H
#define DALI_DEMO_CONTROLS_COMMON_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

/*
 * Definitions for dali-demo-controls static library support.
 *
 * Since dali-demo-controls is a static library (not shared), we use a simple
 * visibility macro. On gcc/clang, we set default visibility so symbols are
 * visible to the examples that link against this library. On other platforms,
 * the macro is left empty (no special visibility handling needed).
 */
#if __GNUC__ >= 4
#define DALI_DEMO_CONTROLS_API __attribute__((visibility("default")))
#else
#define DALI_DEMO_CONTROLS_API
#endif

#endif // DALI_DEMO_CONTROLS_COMMON_H
