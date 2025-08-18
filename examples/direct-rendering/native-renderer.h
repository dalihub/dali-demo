#ifndef DALI_DIRECT_RENDERING_NATIVE_RENDERER_H
#define DALI_DIRECT_RENDERING_NATIVE_RENDERER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/signals/render-callback.h>

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <array>
#include <atomic>
#include <cmath>
#include <deque>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

/**
 * Class NativeRenderer uses GL directly in order to render
 * custom GL code within DirectRendering callbacks.
 */
class NativeRenderer
{
public:
  /**
   * Structure encapsulates initialization parameters
   */
  struct CreateInfo
  {
    std::string          name;
    uint32_t             width;
    uint32_t             height;
    int32_t              viewportX;
    int32_t              viewportY;
    std::array<float, 4> clearColor{0, 0, 0, 1};
    bool                 offscreen{false};
    bool                 threaded{false};
  };

  /**
   * Constructor
   */
  explicit NativeRenderer(const CreateInfo& info);

  /**
   * Destructor
   */
  ~NativeRenderer();

  /**
   * Prepare main shader
   */
  void PrepareShader();

  /**
   * Setup custom scene
   */
  void Setup(int width, int height);

  /**
   * Render cube with given callback input
   */
  void RenderCube(const Dali::RenderCallbackInput& input);

  /**
   * Creates GL program from shader sources
   */
  GLuint CreateProgram(const char* vertexSource, const char* fragmentSource);

  /**
   * Loads shader
   */
  GLuint LoadShader(GLenum shaderType, const char* shaderSource);

  enum class State
  {
    INIT,
    RENDER
  };

  // GLView-compatible callbacks
  void GlViewInitCallback(const Dali::RenderCallbackInput& input);
  int  GlViewRenderCallback(const Dali::RenderCallbackInput& input);
  void GlViewTerminateCallback(const Dali::RenderCallbackInput& input);

private:
  State mState{State::INIT};

  GLuint mProgramId{0u};

  GLint mVertexLocation{};
  GLint mVertexColourLocation{};
  GLint mProjectionLocation{};
  GLint mModelViewLocation{};

  float mModelViewMatrix[16];
  float mProjectionMatrix[16];

  uint32_t mWidth;
  uint32_t mHeight;

  CreateInfo mCreateInfo;

  float mAngle{0.f};

  std::vector<float> mPosX;
  std::vector<float> mPosY;
};

#endif // DALI_DIRECT_RENDERING_NATIVE_RENDERER_H
