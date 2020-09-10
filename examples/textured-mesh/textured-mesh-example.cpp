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
#include <dali-toolkit/dali-toolkit.h>

// INTERNAL INCLUDES
#include "shared/utility.h"
#include "shared/view.h"

using namespace Dali;

namespace
{
const char* MATERIAL_SAMPLE(DEMO_IMAGE_DIR "gallery-small-48.jpg");
const char* MATERIAL_SAMPLE2(DEMO_IMAGE_DIR "gallery-medium-19.jpg");

#define MAKE_SHADER(A) #A

const char* VERTEX_SHADER = MAKE_SHADER(
  attribute mediump vec2 aPosition;
  attribute highp vec2   aTexCoord;
  varying mediump vec2   vTexCoord;
  uniform mediump mat4   uMvpMatrix;
  uniform mediump vec3   uSize;
  uniform lowp vec4      uFadeColor;

  void main() {
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
    vertexPosition.xyz *= uSize;
    vertexPosition = uMvpMatrix * vertexPosition;
    vTexCoord      = aTexCoord;
    gl_Position    = vertexPosition;
  });

const char* FRAGMENT_SHADER = MAKE_SHADER(
  varying mediump vec2 vTexCoord;
  uniform lowp vec4    uColor;
  uniform sampler2D    sTexture;
  uniform lowp vec4    uFadeColor;

  void main() {
    gl_FragColor = texture2D(sTexture, vTexCoord) * uColor * uFadeColor;
  });

/**
 * Sinusoidal curve starting at zero with 2 cycles
 */
float AlphaFunctionSineX2(float progress)
{
  return 0.5f - cosf(progress * 4.0f * Math::PI) * 0.5f;
}

} // anonymous namespace

// This example shows how to use a simple mesh
//
class ExampleController : public ConnectionTracker
{
public:
  /**
   * The example controller constructor.
   * @param[in] application The application instance
   */
  ExampleController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ExampleController::Create);
  }

  /**
   * The example controller destructor
   */
  ~ExampleController()
  {
    // Nothing to do here;
  }

  /**
   * Invoked upon creation of application
   * @param[in] application The application instance
   */
  void Create(Application& application)
  {
    // The Init signal is received once (only) during the Application lifetime

    Window window = application.GetWindow();
    window.KeyEventSignal().Connect(this, &ExampleController::OnKeyEvent);

    mWindowSize = window.GetSize();

    Texture texture1 = DemoHelper::LoadTexture(MATERIAL_SAMPLE);
    Texture texture2 = DemoHelper::LoadTexture(MATERIAL_SAMPLE2);

    mShader      = Shader::New(VERTEX_SHADER, FRAGMENT_SHADER);
    mTextureSet1 = TextureSet::New();
    mTextureSet1.SetTexture(0u, texture1);

    mTextureSet2 = TextureSet::New();
    mTextureSet2.SetTexture(0u, texture2);

    mGeometry = DemoHelper::CreateTexturedQuad();

    mRenderer = Renderer::New(mGeometry, mShader);
    mRenderer.SetTextures(mTextureSet1);

    mMeshActor = Actor::New();
    mMeshActor.AddRenderer(mRenderer);
    mMeshActor.SetProperty(Actor::Property::SIZE, Vector2(400, 400));

    Property::Index fadeColorIndex = mRenderer.RegisterProperty("uFadeColor", Color::MAGENTA);
    mRenderer.SetProperty(Renderer::Property::DEPTH_INDEX, 0);

    mMeshActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_CENTER);
    mMeshActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_CENTER);
    window.Add(mMeshActor);

    mRenderer2 = Renderer::New(mGeometry, mShader);
    mRenderer2.SetTextures(mTextureSet2);

    mMeshActor2 = Actor::New();
    mMeshActor2.AddRenderer(mRenderer2);
    mMeshActor2.SetProperty(Actor::Property::SIZE, Vector2(400, 400));

    mMeshActor2.RegisterProperty("anotherProperty", Color::GREEN);

    mRenderer2.RegisterProperty("anotherProperty", Vector3::ZERO);
    mRenderer2.RegisterProperty("aCoefficient", 0.008f);
    Property::Index fadeColorIndex2 = mRenderer2.RegisterProperty("uFadeColor", Color::BLUE);
    mRenderer2.SetProperty(Renderer::Property::DEPTH_INDEX, 0);

    mMeshActor2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER);
    mMeshActor2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER);
    window.Add(mMeshActor2);

    Animation animation = Animation::New(5);
    KeyFrames keyFrames = KeyFrames::New();
    keyFrames.Add(0.0f, Vector4::ZERO);
    keyFrames.Add(1.0f, Vector4(Color::GREEN));

    KeyFrames keyFrames2 = KeyFrames::New();
    keyFrames2.Add(0.0f, Vector4::ZERO);
    keyFrames2.Add(1.0f, Color::MAGENTA);

    animation.AnimateBetween(Property(mRenderer, fadeColorIndex), keyFrames, AlphaFunction(AlphaFunction::SIN));
    animation.AnimateBetween(Property(mRenderer2, fadeColorIndex2), keyFrames2, AlphaFunction(AlphaFunctionSineX2));
    animation.SetLooping(true);
    animation.Play();

    window.SetBackgroundColor(Vector4(0.0f, 0.2f, 0.2f, 1.0f));
  }

  /**
   * Invoked whenever the quit button is clicked
   * @param[in] button the quit button
   */
  bool OnQuitButtonClicked(Toolkit::Button button)
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication; ///< Application instance
  Vector3      mWindowSize;  ///< The size of the window

  Shader     mShader;
  TextureSet mTextureSet1;
  TextureSet mTextureSet2;
  Geometry   mGeometry;
  Renderer   mRenderer;
  Actor      mMeshActor;
  Renderer   mRenderer2;
  Actor      mMeshActor2;
  Timer      mChangeImageTimer;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application       application = Application::New(&argc, &argv);
  ExampleController test(application);
  application.MainLoop();
  return 0;
}
