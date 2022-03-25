#ifndef DALI_PROJECT_NATIVE_RENDERER_H
#define DALI_PROJECT_NATIVE_RENDERER_H

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

/**
 * This is demo that doesn't use DALi directly as if it's
 * a separate non-DALi app (so not using DALi math etc.)
 */

#include <dali/public-api/signals/render-callback.h>

#include <cmath>
#include <GLES3/gl3.h>

class NativeRenderer
{
public:

  NativeRenderer( uint32_t width, uint32_t height);

  bool OnRender( const Dali::RenderCallbackInput& input );

  void PrepareShader();

  void Setup( int width, int height);

  void RenderCube( const Dali::RenderCallbackInput& input );

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

private:

  State mState {State::INIT};

  GLuint mProgramId{0u};

  GLint mVertexLocation{};
  GLint mVertexColourLocation{};
  GLint mProjectionLocation{};
  GLint mModelViewLocation{};

  float mModelViewMatrix[16];
  float mProjectionMatrix[16];

  uint32_t mWidth;
  uint32_t mHeight;
};

#endif // DALI_PROJECT_NATIVE_RENDERER_H
