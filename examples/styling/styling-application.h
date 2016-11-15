#ifndef DALI_DEMO_STYLING_APPLICATION_H
#define DALI_DEMO_STYLING_APPLICATION_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// External includes
#include <dali-toolkit/dali-toolkit.h>
//#include <dali-toolkit/devel-api/controls/slider/slider.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include "image-channel-control.h"
#include <cstdio>
#include <sstream>

// Internal includes

using namespace Dali;
using namespace Dali::Toolkit;

namespace Demo
{

class StylingApplication : public ConnectionTracker
{
public:
  // Constructor
  StylingApplication( Application& application );

  // Destructor
  ~StylingApplication();

  // Init signal handler
  void Create( Application& application );

  // Create the GUI components
  Actor CreateContentPane();
  Actor CreateResizableContentPane();
  Toolkit::Popup CreateResetPopup();
  Toolkit::TextLabel CreateTitle( std::string title );

  // Key event handler
  void OnKeyEvent( const KeyEvent& event );

  // Button event handlers
  bool OnButtonStateChange( Toolkit::Button button );
  bool OnCheckButtonChange( Toolkit::Button button );
  bool OnResetClicked( Toolkit::Button button );
  bool OnThemeButtonClicked( Toolkit::Button button );

  // Slider event handler
  bool OnSliderChanged( Toolkit::Slider slider, float value );

  // Popup event handlers
  void HidePopup();
  void PopupHidden();
  bool OnReset(Button button);
  bool OnResetCancelled(Button button);

  // Grab handle handler
  void OnPan( Actor actor, const PanGesture& gesture );

  static const char* DEMO_THEME_ONE_PATH;
  static const char* DEMO_THEME_TWO_PATH;
  static const char* DEMO_THEME_THREE_PATH;

private:
  Application& mApplication;
  int mCurrentTheme;
  Actor mContentPane;
  TextLabel mTitle;
  RadioButton mRadioButtons[3]; // 3 demo images
  CheckBoxButton mCheckButtons[3]; // rgb buttons
  Slider mChannelSliders[3]; // rgb sliders
  PushButton mThemeButtons[3];
  PushButton mResetButton;
  ImageChannelControl mImageChannelControl;
  ImageChannelControl mIcc1;
  ImageChannelControl mIcc2;
  ImageChannelControl mIcc3;
  Actor mImagePlacement;
  Popup mResetPopup;
  PanGestureDetector mPanGestureDetector;
};

} // Namespace Demo


#endif // DALI_DEMO_STYLING_APPLICATION_H
