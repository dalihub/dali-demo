/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include "native-renderer.h"
#include <iostream>

/**
 * Set of math helper functions
 */
namespace
{

[[maybe_unused]] std::vector<std::string> split(const std::string& s, char seperator)
{
  std::vector<std::string> output;
  std::string::size_type prev_pos = 0, pos = 0;
  while((pos = s.find(seperator, pos)) != std::string::npos)
  {
    std::string substring( s.substr(prev_pos, pos-prev_pos) );
    output.push_back(substring);
    prev_pos = ++pos;
  }
  output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
  return output;
}

int GetEnvInt(const char* name, int def)
{
  auto v = getenv(name);
  return v ? atoi(v) : def;
}

[[maybe_unused]] std::string GetEnvString(const char* name, std::string def = "")
{
  auto v = getenv(name);
  return v ? std::string(v) : def;
}

const uint32_t MAX_CUBES = GetEnvInt("MAX_CUBES", 200);

#define GL(x) {glGetError();{x;};auto err = glGetError();if(err){ \
printf("%p:%d: ERROR: 0x%X\n", this, __LINE__, int(err));} }

constexpr GLfloat CUBE_VERTICES[] = {-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f};

constexpr GLfloat CUBE_COLOURS[] = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

constexpr GLushort CUBE_INDICES[] = {0, 2, 3, 0, 1, 3, 4, 6, 7, 4, 5, 7, 8, 9, 10, 11, 8, 10, 12, 13, 14, 15, 12, 14, 16, 17, 18, 16, 19, 18, 20, 21, 22, 20, 23, 22};

constexpr float QUAD_VERTS[] = {
  // positions          // colors           // texture coords
  1.0f,  1.0f,
  1.0f, -1.0f,
  -1.0f, -1.0f,
  -1.0f,  1.0f,
};

constexpr unsigned short QUAD_INDICES[] = {
  0, 1, 2,
  3, 0, 2
};

constexpr float QUAD_UV[] = {
  // positions          // colors           // texture coords
  1.0f, 1.0f,   // top right
  1.0f, 0.0f,   // bottom right
  0.0f, 0.0f,   // bottom left
  0.0f, 1.0f    // top left
};

float matrixDegreesToRadians(float degrees)
{
  return M_PI * degrees / 180.0f;
}

[[maybe_unused]] void matrixIdentityFunction(float* matrix)
{
  if(matrix == NULL)
  {
    return;
  }
  matrix[0]  = 1.0f;
  matrix[1]  = 0.0f;
  matrix[2]  = 0.0f;
  matrix[3]  = 0.0f;
  matrix[4]  = 0.0f;
  matrix[5]  = 1.0f;
  matrix[6]  = 0.0f;
  matrix[7]  = 0.0f;
  matrix[8]  = 0.0f;
  matrix[9]  = 0.0f;
  matrix[10] = 1.0f;
  matrix[11] = 0.0f;
  matrix[12] = 0.0f;
  matrix[13] = 0.0f;
  matrix[14] = 0.0f;
  matrix[15] = 1.0f;
}

[[maybe_unused]] void matrixMultiply(float* destination, float* operand1, float* operand2)
{
  float theResult[16];
  int   i, j = 0;
  for(i = 0; i < 4; i++)
  {
    for(j = 0; j < 4; j++)
    {
      theResult[4 * i + j] = operand1[j] * operand2[4 * i] + operand1[4 + j] * operand2[4 * i + 1] +
                             operand1[8 + j] * operand2[4 * i + 2] + operand1[12 + j] * operand2[4 * i + 3];
    }
  }
  for(int i = 0; i < 16; i++)
  {
    destination[i] = theResult[i];
  }
}

[[maybe_unused]] void matrixTranslate(float* matrix, float x, float y, float z)
{
  float temporaryMatrix[16];
  matrixIdentityFunction(temporaryMatrix);
  temporaryMatrix[12] = x;
  temporaryMatrix[13] = y;
  temporaryMatrix[14] = z;
  matrixMultiply(matrix, temporaryMatrix, matrix);
}

[[maybe_unused]] void matrixScale(float* matrix, float x, float y, float z)
{
  float tempMatrix[16];
  matrixIdentityFunction(tempMatrix);
  tempMatrix[0]  = x;
  tempMatrix[5]  = y;
  tempMatrix[10] = z;
  matrixMultiply(matrix, tempMatrix, matrix);
}

[[maybe_unused]] void matrixRotateX(float* matrix, float angle)
{
  float tempMatrix[16];
  matrixIdentityFunction(tempMatrix);
  tempMatrix[5]  = cos(matrixDegreesToRadians(angle));
  tempMatrix[9]  = -sin(matrixDegreesToRadians(angle));
  tempMatrix[6]  = sin(matrixDegreesToRadians(angle));
  tempMatrix[10] = cos(matrixDegreesToRadians(angle));
  matrixMultiply(matrix, tempMatrix, matrix);
}
[[maybe_unused]] void matrixRotateY(float* matrix, float angle)
{
  float tempMatrix[16];
  matrixIdentityFunction(tempMatrix);
  tempMatrix[0]  = cos(matrixDegreesToRadians(angle));
  tempMatrix[8]  = sin(matrixDegreesToRadians(angle));
  tempMatrix[2]  = -sin(matrixDegreesToRadians(angle));
  tempMatrix[10] = cos(matrixDegreesToRadians(angle));
  matrixMultiply(matrix, tempMatrix, matrix);
}
[[maybe_unused]] void matrixRotateZ(float* matrix, float angle)
{
  float tempMatrix[16];
  matrixIdentityFunction(tempMatrix);
  tempMatrix[0] = cos(matrixDegreesToRadians(angle));
  tempMatrix[4] = -sin(matrixDegreesToRadians(angle));
  tempMatrix[1] = sin(matrixDegreesToRadians(angle));
  tempMatrix[5] = cos(matrixDegreesToRadians(angle));
  matrixMultiply(matrix, tempMatrix, matrix);
}

void matrixFrustum(float* matrix, float left, float right, float bottom, float top, float zNear, float zFar)
{
  float temp, xDistance, yDistance, zDistance;
  temp      = 2.0 * zNear;
  xDistance = right - left;
  yDistance = top - bottom;
  zDistance = zFar - zNear;
  matrixIdentityFunction(matrix);
  matrix[0]  = temp / xDistance;
  matrix[5]  = temp / yDistance;
  matrix[8]  = (right + left) / xDistance;
  matrix[9]  = (top + bottom) / yDistance;
  matrix[10] = (-zFar - zNear) / zDistance;
  matrix[11] = -1.0f;
  matrix[14] = (-temp * zFar) / zDistance;
  matrix[15] = 0.0f;
}

[[maybe_unused]] void matrixPerspective(float* matrix, float fieldOfView, float aspectRatio, float zNear, float zFar)
{
  float ymax, xmax;
  ymax = zNear * tanf(fieldOfView * M_PI / 360.0);
  xmax = ymax * aspectRatio;
  matrixFrustum(matrix, -xmax, xmax, -ymax, ymax, zNear, zFar);
}

} // namespace

NativeRenderer::~NativeRenderer() = default;

NativeRenderer::NativeRenderer(const CreateInfo& info )
: mWidth(info.width),
  mHeight(info.height),
  mCreateInfo(info)
{
}

void NativeRenderer::PrepareShader()
{
  static const char glVertexShader[] =
    "attribute vec4 vertexPosition;\n"
    "attribute vec3 vertexColour;\n"
    "varying vec3 fragColour;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 modelView;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * modelView * vertexPosition;\n"
    "    fragColour = vertexColour;\n"
    "}\n";

  static const char glFragmentShader[] =
    "precision mediump float;\n"
    "varying vec3 fragColour;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = vec4(fragColour, 1.0);\n"
    "}\n";

  mProgramId = CreateProgram(glVertexShader, glFragmentShader);
}

void NativeRenderer::Setup(int width, int height)
{
  PrepareShader();

  mVertexLocation       = glGetAttribLocation(mProgramId, "vertexPosition");
  mVertexColourLocation = glGetAttribLocation(mProgramId, "vertexColour");
  mProjectionLocation   = glGetUniformLocation(mProgramId, "projection");
  mModelViewLocation    = glGetUniformLocation(mProgramId, "modelView");

  GL(glEnable(GL_DEPTH_TEST));
}

GLuint NativeRenderer::CreateProgram(const char* vertexSource, const char* fragmentSource)
{
  GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, vertexSource);
  if(!vertexShader)
  {
    return 0;
  }
  GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragmentSource);
  if(!fragmentShader)
  {
    return 0;
  }
  GLuint program = glCreateProgram();
  if(program)
  {
    GL(glAttachShader(program, vertexShader));
    GL(glAttachShader(program, fragmentShader));
    GL(glLinkProgram(program));
    GLint linkStatus = GL_FALSE;
    GL(glGetProgramiv(program, GL_LINK_STATUS, &linkStatus));
    if(linkStatus != GL_TRUE)
    {
      GLint bufLength = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
      if(bufLength)
      {
        char* buf = (char*)malloc(bufLength);
        if(buf)
        {
          glGetProgramInfoLog(program, bufLength, NULL, buf);
          free(buf);
        }
      }
      glDeleteProgram(program);
      program = 0;
    }
  }
  return program;
}

GLuint NativeRenderer::LoadShader(GLenum shaderType, const char* shaderSource)
{
  GLuint shader = glCreateShader(shaderType);
  if(shader != 0)
  {
    GL(glShaderSource(shader, 1, &shaderSource, NULL));
    GL(glCompileShader(shader));
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(compiled != GL_TRUE)
    {
      GLint infoLen = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

      if(infoLen > 0)
      {
        char* logBuffer = (char*)malloc(infoLen);

        if(logBuffer != NULL)
        {
          glGetShaderInfoLog(shader, infoLen, NULL, logBuffer);
          free(logBuffer);
          logBuffer = NULL;
        }

        glDeleteShader(shader);
        shader = 0;
      }
    }
  }

  return shader;
}

void NativeRenderer::RenderCube(const Dali::RenderCallbackInput& input)
{
  uint32_t drawCount = 0;

  float& angle = mAngle;

  [[maybe_unused]] auto x = mCreateInfo.viewportX; // float(mWidth - input.size.width) * 0.5f;
  [[maybe_unused]] auto y = mCreateInfo.viewportY; // float(mHeight - input.size.height) * 0.5f;
  auto                  w = mCreateInfo.width;
  auto                  h = mCreateInfo.height;
  
  matrixPerspective(mProjectionMatrix, 45, (float)w / (float)h, 0.1f, 100);

  GL(glViewport(x, y, w, h));
  GL(glEnable(GL_DEPTH_TEST));
  if(!mCreateInfo.offscreen)
  {
    GL(glEnable(GL_SCISSOR_TEST));
    GL(glScissor(x, y, w, h));
  }
  else
  {
    GL(glDisable(GL_SCISSOR_TEST));
  }
  GL(glClearColor(mCreateInfo.clearColor[0],
                  mCreateInfo.clearColor[1],
                  mCreateInfo.clearColor[2],
                  mCreateInfo.clearColor[3]
                  ));
  {
    GL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
  }
  GL(glUseProgram(mProgramId));
  GL(glVertexAttribPointer(mVertexLocation, 3, GL_FLOAT, GL_FALSE, 0, CUBE_VERTICES));
  GL(glEnableVertexAttribArray(mVertexLocation));
  GL(glVertexAttribPointer(mVertexColourLocation, 3, GL_FLOAT, GL_FALSE, 0, CUBE_COLOURS));
  GL(glEnableVertexAttribArray(mVertexColourLocation));

  srand(10);

  const auto maxCubes = int(MAX_CUBES);

  for( int i = 0; i < int(maxCubes); ++i)
  {
    GL(matrixIdentityFunction(mModelViewMatrix));
    matrixScale(mModelViewMatrix, 0.2f, 0.2f, 0.2f);
    matrixRotateX(mModelViewMatrix, angle);
    matrixRotateY(mModelViewMatrix, angle);
    auto max = 7000;
    if(mPosX.size() == uint32_t(i))
    {
      auto x = float((rand() % max) - (max / 2)) / 1000.0f;
      auto y = float((rand() % max) - (max / 2)) / 1000.0f;
      mPosX.emplace_back(x);
      mPosY.emplace_back(y);
    }
    matrixTranslate(mModelViewMatrix, mPosX[i], mPosY[i], -5.0f);
    GL(glUniformMatrix4fv(mProjectionLocation, 1, GL_FALSE, mProjectionMatrix));
    GL(glUniformMatrix4fv(mModelViewLocation, 1, GL_FALSE, mModelViewMatrix));
    GL(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, CUBE_INDICES));
    drawCount++;
  }
  angle += 1;
  if(angle > 360)
  {
    angle -= 360;
  }
}

void NativeRenderer::GlViewInitCallback(const Dali::RenderCallbackInput& input)
{
  Setup(mWidth, mHeight);
  mState = State::RENDER;
}

int NativeRenderer::GlViewRenderCallback(const Dali::RenderCallbackInput& input)
{
  RenderCube(input);
  return true;
}

void NativeRenderer::GlViewTerminateCallback(const Dali::RenderCallbackInput& input)
{
  // Nothing to do here
}
