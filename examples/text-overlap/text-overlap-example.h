#pragma once

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
 */

#include <dali-toolkit/dali-toolkit.h>

namespace Demo
{
class TextOverlapController : public Dali::ConnectionTracker
{
public:
  TextOverlapController(Dali::Application& app);

private:
  void Create(Dali::Application& app);
  void Create2();
  void Destroy(Dali::Application& app);
  void OnPan(Dali::Actor actor, const Dali::PanGesture& gesture);
  void OnKeyEvent(const Dali::KeyEvent& keyEvent);
  bool OnClicked(Dali::Toolkit::Button button);

private:
  Dali::Application&       mApplication;
  Dali::Toolkit::TextLabel mLabels[2];
  Dali::PanGestureDetector mPanDetector;
  Dali::Actor              mGrabbedActor;
  Dali::Toolkit::Button    mSwapButton;
  int                      mTopmostLabel;
};

} // namespace Demo
