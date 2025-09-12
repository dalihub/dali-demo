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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>

// For Wait
#include <chrono>
#include <iostream>
#include <thread>

using namespace Dali;
using Dali::Toolkit::TextLabel;

const std::string_view SHADER_CUSTOM_VERT{
  R"(attribute mediump vec2 aPosition;
uniform mediump mat4 uMvpMatrix;
uniform vec3 uSize;

void main()
{
  gl_Position = uMvpMatrix * vec4(aPosition*uSize.xy, 0.0, 1.0);
}
)"};

const std::string_view SHADER_CUSTOM_FRAG{
  R"(uniform lowp vec4 uColor;

void main()
{
  gl_FragColor = vec4(0.0,0.0,0.0,1.0);
}
)"};

using namespace Dali;
using namespace Dali::Toolkit;

// This example shows how to create and display Hello World! using a simple TextActor
//
class PrecompileShaderController : public ConnectionTracker
{
public:
  PrecompileShaderController(Application& application)
  : mApplication(application)
  {
    setenv("DALI_SHADER_USE_PROGRAM_BINARY", "1", 1); // Enable program binary
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &PrecompileShaderController::Create);
  }

  ~PrecompileShaderController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    Property::Map imageShader;
    imageShader["shaderType"]   = "image";
    imageShader["shaderOption"] = Property::Map().Add("YUV_AND_RGB", true);
    imageShader["shaderName"]   = "IMAGE_SHADER_YUV_AND_RGB";

    Property::Map imageShader2;
    imageShader2["shaderType"]   = "image";
    imageShader2["shaderOption"] = Property::Map()
                                     .Add("ROUNDED_CORNER", true)
                                     .Add("MASKING", true);
    imageShader2["shaderName"] = "";

    Property::Map textShader;
    textShader["shaderType"]   = "text";
    textShader["shaderOption"] = Property::Map()
                                   .Add("MULTI_COLOR", true)
                                   .Add("STYLES", true);
    textShader["shaderName"] = "";

    Property::Map colorShader;
    colorShader["shaderType"]   = "color";
    colorShader["shaderOption"] = Property::Map()
                                    .Add("CUTOUT", true)
                                    .Add("BORDERLINE", true);
    colorShader["shaderName"] = "ColorBorderlineCutout";

    Property::Map customSHader;
    customSHader["shaderType"]     = "custom";
    customSHader["shaderName"]     = "myShader";
    customSHader["vertexShader"]   = SHADER_CUSTOM_VERT.data();
    customSHader["fragmentShader"] = SHADER_CUSTOM_FRAG.data();

    Property::Map npatchShader;
    npatchShader["shaderType"]    = "npatch";
    npatchShader["xStretchCount"] = 4;
    npatchShader["yStretchCount"] = 3;

    VisualFactory::Get().AddPrecompileShader(imageShader);
    VisualFactory::Get().AddPrecompileShader(imageShader2);
    VisualFactory::Get().AddPrecompileShader(textShader);
    VisualFactory::Get().AddPrecompileShader(colorShader);
    VisualFactory::Get().AddPrecompileShader(customSHader);
    VisualFactory::Get().AddPrecompileShader(npatchShader);
    VisualFactory::Get().UsePreCompiledShader();

    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    TextLabel textLabel = TextLabel::New("Precomile Shader");
    textLabel.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    textLabel.SetProperty(Dali::Actor::Property::NAME, "Precompile Shader");
    window.Add(textLabel);

    // Need to wait forcelly for precompiling shaders
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                application = Application::New(&argc, &argv);
  PrecompileShaderController test(application);
  application.MainLoop();
  return 0;
}
