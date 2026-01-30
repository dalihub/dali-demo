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
#include "scene3d-example.h"
#include <dirent.h>
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <string_view>
#include "scene3d-extension.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Scene3D::Loader;

namespace
{
const float ROTATION_SCALE = 180.f; // the amount of rotation that a swipe whose length is the width of the screen, causes, in degrees.

const float ITEM_HEIGHT = 50.f;

const Vector3 CAMERA_DEFAULT_POSITION(0.0f, 0.0f, 3.5f);

const std::string_view DLI_EXTENSION  = ".dli";
const std::string_view GLTF_EXTENSION = ".gltf";

const std::string RESOURCE_TYPE_DIRS[]{
  "images/",
  "shaders/",
  "models/",
  "images/",
};

using StringVector = std::vector<std::string>;

StringVector ListFiles(
  const std::string& path, bool (*predicate)(const char*) = [](const char*)
{ return true; })
{
  StringVector results;

  if(auto dirp = opendir(path.c_str()))
  {
    std::unique_ptr<DIR, int (*)(DIR*)> dir(dirp, closedir);

    struct dirent* ent;
    while((ent = readdir(dir.get())) != nullptr)
    {
      if(ent->d_type == DT_REG && predicate(ent->d_name))
      {
        results.push_back(ent->d_name);
      }
    }
  }
  return results;
}

TextLabel MakeLabel(std::string msg)
{
  TextLabel label = TextLabel::New("  " + msg);
  label.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  label.SetProperty(TextLabel::Property::TEXT_COLOR, Color::WHITE);
  label.SetProperty(TextLabel::Property::PIXEL_SIZE, ITEM_HEIGHT * 4 / 7);
  label.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
  SetActorCentered(label);
  return label;
}

struct ItemFactoryImpl : Dali::Toolkit::ItemFactory
{
  const std::vector<std::string>& mSceneNames;
  TapGestureDetector              mTapDetector;

  ItemFactoryImpl(const std::vector<std::string>& sceneNames, TapGestureDetector tapDetector)
  : mSceneNames(sceneNames),
    mTapDetector(tapDetector)
  {
  }

  unsigned int GetNumberOfItems() override
  {
    return mSceneNames.size();
  }

  Actor NewItem(unsigned int itemId) override
  {
    auto label = MakeLabel(mSceneNames[itemId]);
    mTapDetector.Attach(label);
    label.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
    return label;
  }
};

Actor CreateErrorMessage(std::string msg)
{
  auto label = MakeLabel(msg);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  label.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, HorizontalAlignment::LEFT);
  label.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, VerticalAlignment::TOP);
  return label;
}

void ConfigureBlendShapeShaders(ResourceBundle& resources, const SceneDefinition& scene, Actor root, std::vector<BlendshapeShaderConfigurationRequest>&& requests)
{
  std::vector<std::string> errors;
  auto                     onError = [&errors](const std::string& msg)
  {
    errors.push_back(msg);
  };
  if(!scene.ConfigureBlendshapeShaders(resources, root, std::move(requests), onError))
  {
    ExceptionFlinger flinger(ASSERT_LOCATION);
    for(auto& msg : errors)
    {
      flinger << msg << '\n';
    }
  }
}

Actor LoadScene(std::string sceneName, CameraActor camera, std::vector<Dali::Animation>& generatedAnimations, Animation& animation)
{
  ResourceBundle::PathProvider pathProvider = [](ResourceType::Value type)
  {
    return Application::GetResourcePath() + RESOURCE_TYPE_DIRS[type];
  };

  auto path = pathProvider(ResourceType::Mesh) + sceneName;

  ResourceBundle                                          resources;
  SceneDefinition                                         scene;
  SceneMetadata                                           metaData;
  std::vector<Dali::Scene3D::Loader::AnimationDefinition> animations;
  std::vector<AnimationGroupDefinition>                   animGroups;
  std::vector<CameraParameters>                           cameraParameters;
  std::vector<LightParameters>                            lights;

  animations.clear();

  LoadResult output{
    resources,
    scene,
    metaData,
    animations,
    animGroups,
    cameraParameters,
    lights};

  Dali::Scene3D::Loader::ModelLoader modelLoader(path, pathProvider(ResourceType::Mesh) + "/", output);
  modelLoader.LoadModel(pathProvider);

  if(cameraParameters.empty())
  {
    cameraParameters.push_back(CameraParameters());
    cameraParameters[0].matrix.SetTranslation(CAMERA_DEFAULT_POSITION);
  }
  cameraParameters[0].ConfigureCamera(camera);
  SetActorCentered(camera);

  std::filesystem::path modelPath(path);
  std::string           extension = modelPath.extension();
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

  Scene3D::Loader::ShaderManagerPtr shaderManager  = (extension == GLTF_EXTENSION) ? new Scene3D::Loader::ShaderManager() : nullptr;
  ViewProjection                    viewProjection = cameraParameters[0].GetViewProjection();
  Transforms                        xforms{
    MatrixStack{},
    viewProjection};
  NodeDefinition::CreateParams nodeParams{
    resources,
    xforms,
    shaderManager,
    {},
    {},
    {}};

  Actor root = Actor::New();
  SetActorCentered(root);

  auto& resourceChoices = modelLoader.GetResourceChoices();
  for(auto iRoot : scene.GetRoots())
  {
    if(auto actor = scene.CreateNodes(iRoot, resourceChoices, nodeParams))
    {
      scene.ConfigureSkinningShaders(resources, actor, std::move(nodeParams.mSkinnables));
      ConfigureBlendShapeShaders(resources, scene, actor, std::move(nodeParams.mBlendshapeRequests));

      scene.ApplyConstraints(actor, std::move(nodeParams.mConstrainables));

      root.Add(actor);
    }
  }

  generatedAnimations.clear();
  if(!animations.empty())
  {
    generatedAnimations.reserve(animations.size());
    auto getActor = [&](const Scene3D::Loader::AnimatedProperty& property)
    {
      Dali::Actor actor;
      if(property.mNodeIndex != Scene3D::Loader::INVALID_INDEX)
      {
        auto* node = scene.GetNode(property.mNodeIndex);
        if(node != nullptr)
        {
          actor = root.FindChildById(node->mNodeId);
        }
      }
      else
      {
        actor = root.FindChildByName(property.mNodeName);
      }
      return actor;
    };

    for(auto& animationDefinition : animations)
    {
      generatedAnimations.push_back(animationDefinition.ReAnimate(getActor));
    }
    generatedAnimations[0].Play();
  }

  return root;
}

} // namespace

Scene3DExample::Scene3DExample(Dali::Application& app)
: mApp(app),
  mScene3DExtension(new Scene3DExtension())
{
  if(!std::getenv("DALI_APPLICATION_PACKAGE"))
  {
    if(auto desktopPrefix = std::getenv("DESKTOP_PREFIX"))
    {
      std::stringstream sstr;
      sstr << desktopPrefix << "/share/com.samsung.dali-demo/res/";

      auto daliApplicationPackage = sstr.str();
      setenv("DALI_APPLICATION_PACKAGE", daliApplicationPackage.c_str(), 0);
    }
  }

  app.InitSignal().Connect(this, &Scene3DExample::OnInit);
  app.TerminateSignal().Connect(this, &Scene3DExample::OnTerminate);
}

Scene3DExample::~Scene3DExample() = default;

void Scene3DExample::OnInit(Application& app)
{
  // get scenes
  auto resPath    = Application::GetResourcePath();
  auto scenePath  = resPath + RESOURCE_TYPE_DIRS[ResourceType::Mesh];
  auto sceneNames = ListFiles(scenePath, [](const char* name)
  {
    auto len = strlen(name);
    return (len > DLI_EXTENSION.size() && DLI_EXTENSION.compare(name + (len - DLI_EXTENSION.size())) == 0) ||
           (len > GLTF_EXTENSION.size() && GLTF_EXTENSION.compare(name + (len - GLTF_EXTENSION.size())) == 0); });
  mSceneNames     = sceneNames;

  // create Dali objects
  auto window = app.GetWindow();

  // navigation view
  auto navigationView = NavigationView::New();
  navigationView.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  SetActorCentered(navigationView);

  // Set up the background gradient.
  Property::Array stopOffsets;
  stopOffsets.PushBack(0.0f);
  stopOffsets.PushBack(1.0f);
  Property::Array stopColors;
  stopColors.PushBack(Color::BLACK);
  stopColors.PushBack(Vector4(0.45f, 0.7f, 0.8f, 1.f)); // Medium bright, pastel blue
  const float percentageWindowHeight = window.GetSize().GetHeight() * 0.8f;

  navigationView.SetProperty(Toolkit::Control::Property::BACKGROUND,
                             Dali::Property::Map()
                               .Add(Toolkit::Visual::Property::TYPE, Dali::Toolkit::Visual::GRADIENT)
                               .Add(Toolkit::GradientVisual::Property::STOP_OFFSET, stopOffsets)
                               .Add(Toolkit::GradientVisual::Property::STOP_COLOR, stopColors)
                               .Add(Toolkit::GradientVisual::Property::START_POSITION, Vector2(0.f, 0.0f))
                               .Add(Toolkit::GradientVisual::Property::END_POSITION, Vector2(0.f, percentageWindowHeight))
                               .Add(Toolkit::GradientVisual::Property::UNITS, Toolkit::GradientVisual::Units::USER_SPACE));
  window.Add(navigationView);
  mNavigationView = navigationView;

  // item view
  auto tapDetector = TapGestureDetector::New();
  mItemFactory.reset(new ::ItemFactoryImpl(mSceneNames, tapDetector));

  auto items = ItemView::New(*mItemFactory);
  SetActorCentered(items);
  items.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  items.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  Vector3 windowSize(window.GetSize());
  auto    itemLayout = DefaultItemLayout::New(DefaultItemLayout::LIST);
  itemLayout->SetItemSize(Vector3(windowSize.x * 0.9f, ITEM_HEIGHT, 1.f));
  items.AddLayout(*itemLayout);
  navigationView.Push(items);

  mItemLayout = itemLayout;
  mItemView   = items;

  mItemView.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  KeyboardFocusManager::Get().PreFocusChangeSignal().Connect(this, &Scene3DExample::OnKeyboardPreFocusChange);
  KeyboardFocusManager::Get().FocusedActorEnterKeySignal().Connect(this, &Scene3DExample::OnKeyboardFocusedActorActivated);
  KeyboardFocusManager::Get().FocusChangedSignal().Connect(this, &Scene3DExample::OnKeyboardFocusChanged);

  SetActorCentered(KeyboardFocusManager::Get().GetFocusIndicatorActor());

  // camera
  auto camera = CameraActor::New();
  camera.SetInvertYAxis(true);
  window.Add(camera);
  mSceneCamera = camera;

  // event handling
  window.KeyEventSignal().Connect(this, &Scene3DExample::OnKey);

  tapDetector.DetectedSignal().Connect(this, &Scene3DExample::OnTap);
  mTapDetector = tapDetector;

  // activate layout
  mItemView.ActivateLayout(0, windowSize, 0.f);

  mScene3DExtension->SetSceneLoader(this);
}

Actor Scene3DExample::OnKeyboardPreFocusChange(Actor current, Actor proposed, Control::KeyboardFocus::Direction direction)
{
  if(!current && !proposed)
  {
    return mItemView;
  }

  return proposed;
}

void Scene3DExample::OnKeyboardFocusedActorActivated(Actor activatedActor)
{
  if(activatedActor)
  {
    OnTap(activatedActor, Dali::TapGesture());
  }
}

void Scene3DExample::OnKeyboardFocusChanged(Actor originalFocusedActor, Actor currentFocusedActor)
{
  if(currentFocusedActor)
  {
    auto itemId = mItemView.GetItemId(currentFocusedActor);
    mItemView.ScrollToItem(itemId, 0.1f);
  }
}

void Scene3DExample::OnTerminate(Application& app)
{
  mTapDetector.Reset();
  mPanDetector.Reset();

  auto window      = app.GetWindow();
  auto renderTasks = window.GetRenderTaskList();
  renderTasks.RemoveTask(mSceneRender);
  mSceneRender.Reset();

  UnparentAndReset(mNavigationView);
  UnparentAndReset(mSceneCamera);

  mItemFactory.reset();
}

void Scene3DExample::OnKey(const KeyEvent& e)
{
  if(e.GetState() == KeyEvent::UP)
  {
    if(IsKey(e, DALI_KEY_ESCAPE) || IsKey(e, DALI_KEY_BACK))
    {
      if(mScene)
      {
        mPanDetector.Reset();

        mNavigationView.Pop();
        mScene.Reset();

        if(mActivatedActor)
        {
          KeyboardFocusManager::Get().SetCurrentFocusActor(mActivatedActor);
        }
        auto window = mApp.GetWindow();
        window.GetRootLayer().SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_UI);
      }
      else
      {
        mApp.Quit();
      }
    }
    else
    {
      mScene3DExtension->OnKey(e);
    }
  }
}

void Scene3DExample::OnPan(Actor actor, const PanGesture& pan)
{
  auto    windowSize = mApp.GetWindow().GetSize();
  Vector2 size{float(windowSize.GetWidth()), float(windowSize.GetHeight())};
  float   aspect = size.y / size.x;

  size /= ROTATION_SCALE;

  Vector2 rotation{pan.GetDisplacement().x / size.x, pan.GetDisplacement().y / size.y * aspect};

  Quaternion q  = Quaternion(Radian(Degree(rotation.y)), Radian(Degree(rotation.x)), Radian(0.f));
  Quaternion q0 = mScene.GetProperty(Actor::Property::ORIENTATION).Get<Quaternion>();

  mScene.SetProperty(Actor::Property::ORIENTATION, q * q0);
}

void Scene3DExample::OnTap(Dali::Actor actor, const Dali::TapGesture& tap)
{
  mActivatedActor = actor;

  auto id = mItemView.GetItemId(actor);

  try
  {
    auto window = mApp.GetWindow();
    window.GetRootLayer().SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_3D);
    auto renderTasks = window.GetRenderTaskList();
    renderTasks.RemoveTask(mSceneRender);

    auto scene = LoadScene(mSceneNames[id], mSceneCamera, mSceneAnimations, mCurrentAnimation);

    auto sceneRender = renderTasks.CreateTask();
    sceneRender.SetCameraActor(mSceneCamera);
    sceneRender.SetSourceActor(scene);
    sceneRender.SetExclusive(true);

    mScene       = scene;
    mSceneRender = sceneRender;

    mPanDetector = PanGestureDetector::New();
    mPanDetector.DetectedSignal().Connect(this, &Scene3DExample::OnPan);
    mPanDetector.Attach(mNavigationView);
  }
  catch(const DaliException& e)
  {
    mScene = CreateErrorMessage(e.condition);
  }

  mNavigationView.Push(mScene);

  mScene3DExtension->ConnectTouchSignals();
}
