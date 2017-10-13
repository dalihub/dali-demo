/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * const APPLICATION_TITLE( "Color Gradients" );

const char * const TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char * const CHANGE_ICON( DEMO_IMAGE_DIR "icon-change.png" );
const char * const CHANGE_ICON_SELECTED( DEMO_IMAGE_DIR "icon-change-selected.png" );
}

// This example shows how to render color gradients
//
class GradientController : public ConnectionTracker
{
public:

  GradientController( Application& application )
  : mApplication( application ),
    mIndex( 0 )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &GradientController::Create );
  }

  ~GradientController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &GradientController::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Toolkit::ToolBar toolBar;
    Layer content = DemoHelper::CreateView( application,
                                            mView,
                                            toolBar,
                                            "",
                                            TOOLBAR_IMAGE,
                                            APPLICATION_TITLE );

    PushButton changeButton = Toolkit::PushButton::New();
    changeButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, CHANGE_ICON );
    changeButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, CHANGE_ICON_SELECTED );
    changeButton.ClickedSignal().Connect( this, &GradientController::OnChangeIconClicked );
    toolBar.AddControl( changeButton,
                        DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage,
                        Toolkit::Alignment::HorizontalRight,
                        DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

// ---- Gradient for background

    mGradientMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::GRADIENT );

    Property::Array stopOffsets;
    stopOffsets.PushBack( 0.0f );
    stopOffsets.PushBack( 0.3f );
    stopOffsets.PushBack( 0.6f );
    stopOffsets.PushBack( 0.8f );
    stopOffsets.PushBack( 1.0f );
    mGradientMap.Insert( GradientVisual::Property::STOP_OFFSET, stopOffsets );

    Property::Array stopColors;
    stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 255.f )/255.f );
    stopColors.PushBack( Vector4( 196.f, 198.f, 71.f, 122.f )/255.f );
    stopColors.PushBack( Vector4( 214.f, 37.f, 139.f, 191.f )/255.f );
    stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 150.f )/255.f );
    stopColors.PushBack( Color::YELLOW );
    mGradientMap.Insert( GradientVisual::Property::STOP_COLOR, stopColors );

    OnChangeIconClicked( changeButton );
  }

  bool OnChangeIconClicked( Toolkit::Button button )
  {
    Property::Map gradientMap;

    switch( mIndex%4 )
    {
      case 0: // linear gradient with units as objectBoundingBox
      {
        gradientMap.Insert( GradientVisual::Property::START_POSITION,   Vector2(  0.5f,  0.5f ) );
        gradientMap.Insert( GradientVisual::Property::END_POSITION,     Vector2( -0.5f, -0.5f ) );
        break;
      }
      case 1: // linear gradient with units as userSpaceOnUse
      {
        Vector2 halfStageSize = Stage::GetCurrent().GetSize()*0.5f;
        gradientMap.Insert( GradientVisual::Property::START_POSITION,  halfStageSize );
        gradientMap.Insert( GradientVisual::Property::END_POSITION,   -halfStageSize );
        gradientMap.Insert( GradientVisual::Property::UNITS, GradientVisual::Units::USER_SPACE );
        break;
      }
      case 2: // radial gradient with units as objectBoundingBox
      {
        gradientMap.Insert( GradientVisual::Property::CENTER, Vector2( 0.5f, 0.5f ) );
        gradientMap.Insert( GradientVisual::Property::RADIUS, 1.414f );
        break;
      }
      default: // radial gradient with units as userSpaceOnUse
      {
        Vector2 stageSize = Stage::GetCurrent().GetSize();
        gradientMap.Insert( GradientVisual::Property::CENTER, stageSize * 0.5f );
        gradientMap.Insert( GradientVisual::Property::RADIUS, stageSize.Length());
        gradientMap.Insert( GradientVisual::Property::UNITS,  GradientVisual::Units::USER_SPACE );
        break;
      }
    }

    gradientMap.Merge( mGradientMap );
    mView.SetProperty( Control::Property::BACKGROUND, gradientMap );

    mIndex++;
    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;

  Property::Map mGradientMap;
  Control mView;
  unsigned mIndex;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  GradientController test( application );
  application.MainLoop();
  return 0;
}
