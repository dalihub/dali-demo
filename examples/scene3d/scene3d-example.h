#ifndef SCENE_LAUNCHER_H_
#define SCENE_LAUNCHER_H_
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

#include <dali-scene3d/dali-scene3d.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/navigation-view/navigation-view.h>
#include <dali/dali.h>
#include <memory>

class Scene3DExtension;

class Scene3DExample : public Dali::ConnectionTracker
{
public:
  Scene3DExample(Dali::Application& app);
  ~Scene3DExample();

private: // data
  Dali::Application& mApp;

  std::vector<std::string> mSceneNames;

  Dali::Toolkit::NavigationView mNavigationView;

  std::unique_ptr<Dali::Toolkit::ItemFactory> mItemFactory;
  Dali::Toolkit::ItemLayoutPtr                mItemLayout;
  Dali::Toolkit::ItemView                     mItemView;

  Dali::CameraActor mSceneCamera;
  Dali::RenderTask  mSceneRender;

  Dali::Quaternion mCameraOrientationInv;

  Dali::TapGestureDetector mTapDetector;
  Dali::PanGestureDetector mPanDetector;

  Dali::Actor mActivatedActor;

public:
  Dali::Actor mScene;

  std::vector<Dali::Animation> mSceneAnimations;
  Dali::Animation              mCurrentAnimation;

  std::unique_ptr<Scene3DExtension> mScene3DExtension;

private: // methods
  void OnInit(Dali::Application& app);
  void OnTerminate(Dali::Application& app);

  void OnKey(const Dali::KeyEvent& e);
  void OnPan(Dali::Actor actor, const Dali::PanGesture& pan);
  void OnTap(Dali::Actor actor, const Dali::TapGesture& tap);

  Dali::Actor OnKeyboardPreFocusChange(Dali::Actor current, Dali::Actor proposed, Dali::Toolkit::Control::KeyboardFocus::Direction direction);
  void        OnKeyboardFocusedActorActivated(Dali::Actor activatedActor);
  void        OnKeyboardFocusChanged(Dali::Actor originalFocusedActor, Dali::Actor currentFocusedActor);
};

#endif // SCENE_LAUNCHER_H_
