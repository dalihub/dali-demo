//
// Created by adam.b on 15/03/2022.
//
#include "native-renderer.h"

/**
 * Set of math helper functions
 */
namespace
{

constexpr GLfloat CUBE_VERTICES[] = {-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f};

constexpr GLfloat CUBE_COLOURS[] = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

constexpr GLushort CUBE_INDICES[] = {0, 2, 3, 0, 1, 3, 4, 6, 7, 4, 5, 7, 8, 9, 10, 11, 8, 10, 12, 13, 14, 15, 12, 14, 16, 17, 18, 16, 19, 18, 20, 21, 22, 20, 23, 22};

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

NativeRenderer::NativeRenderer(uint32_t width, uint32_t height)
: mWidth(width),
  mHeight(height)
{
}

bool NativeRenderer::OnRender(const Dali::RenderCallbackInput& input)
{
  if(mState == State::INIT)
  {
    Setup(mWidth, mHeight);
    mState = State::RENDER;
  }

  RenderCube(input);

  return false;
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

  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, width, height);
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
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
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
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
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
  static float angle = 0.0f;

  auto x = float(mWidth - input.size.width) * 0.5f;
  auto y = float(mHeight - input.size.height) * 0.5f;
  auto w = input.size.width;
  auto h = input.size.height;

  matrixPerspective(mProjectionMatrix, 45, (float)w / (float)h, 0.1f, 100);

  glEnable(GL_SCISSOR_TEST);
  glScissor(x, y, w, h);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  matrixIdentityFunction(mModelViewMatrix);
  matrixScale(mModelViewMatrix, 0.5f, 0.5f, 0.5f);
  matrixRotateX(mModelViewMatrix, angle);
  matrixRotateY(mModelViewMatrix, angle);
  matrixTranslate(mModelViewMatrix, 0.0f, 0.0f, -10.0f);
  glUseProgram(mProgramId);
  glVertexAttribPointer(mVertexLocation, 3, GL_FLOAT, GL_FALSE, 0, CUBE_VERTICES);
  glEnableVertexAttribArray(mVertexLocation);
  glVertexAttribPointer(mVertexColourLocation, 3, GL_FLOAT, GL_FALSE, 0, CUBE_COLOURS);
  glEnableVertexAttribArray(mVertexColourLocation);
  glUniformMatrix4fv(mProjectionLocation, 1, GL_FALSE, mProjectionMatrix);
  glUniformMatrix4fv(mModelViewLocation, 1, GL_FALSE, mModelViewMatrix);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, CUBE_INDICES);
  angle += 1;
  if(angle > 360)
  {
    angle -= 360;
  }
}
