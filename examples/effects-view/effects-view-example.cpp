//
// Copyright (c) 2017 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "shared/view.h"

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>
#include <dali-toolkit/devel-api/controls/effects-view/effects-view.h>
#include <sstream>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* const TITLE( "EffectsView: effect size = " );
const char* TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char* VIEW_SWAP_IMAGE( DEMO_IMAGE_DIR "icon-change.png" );
const char* VIEW_SWAP_SELECTED_IMAGE( DEMO_IMAGE_DIR "icon-change-selected.png" );
const char* TEST_IMAGE( DEMO_IMAGE_DIR "Kid1.svg" );
} // namespace

// This example illustrates the capabilities of the EffectsView container
//
class EffectsViewApp : public ConnectionTracker
{
public:

  /**
   * Constructor
   */
  EffectsViewApp( Application& application );
  /**
   * Destructor
   */
  ~EffectsViewApp();

private:

  /**
   * Initialisation. This method gets called once the main loop of application is up and running
   */
  void OnAppInitialize( Application& application );

  /**
   * Create a effect view of drop shadow
   *
   * @param[in] type The type of effect to be performed by the EffectView.
   * @param[in] viewSize Size of the effect view
   * @param[in] effectSize The effect size used in image filters.
   */
  EffectsView CreateEffectsView( EffectsView::EffectType type, const Vector2& viewSize, int effectSize );

  /**
   * Animate the effect offset and color properties.
   * @param[in] effectsView The view whose properties to be animated.
   */
  void AnimateEffectProperties( EffectsView& effectsView );

  /**
   * Set title onto the toolbar
   * @param[in] effectSize The effect size value to be indicated on the title
   */
  void SetTitle(int effectSize);

  /**
   * Callback function to change the effect size.
   * @param[in] button The button which triggered the callback.
   */
  bool ChangeEffectSize( Button button );

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event);

private:
  Application&           mApplication;
  Layer                  mContents;
  Toolkit::Control       mView;
  Toolkit::ToolBar       mToolBar;
  EffectsView            mDropShadowView;
  EffectsView            mEmbossView;
  Toolkit::TextLabel     mTitleActor; ///< The title on the toolbar
  Vector2                mStageSize;
  int                    mEffectSize;
};

EffectsViewApp::EffectsViewApp( Application& application )
: mApplication( application ),
  mEffectSize( 2 )
{
  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &EffectsViewApp::OnAppInitialize );
}

EffectsViewApp::~EffectsViewApp()
{
  // Nothing to do here;
}

void EffectsViewApp::OnAppInitialize( Application& application )
{
  // The Init signal is received once (only) during the Application lifetime

  Stage stage = Stage::GetCurrent();
  stage.KeyEventSignal().Connect(this, &EffectsViewApp::OnKeyEvent);
  stage.SetBackgroundColor( Color::WHITE );

  mStageSize = stage.GetSize();

  // Creates a default view with a default tool bar.
  // The view is added to the stage.
  mContents = DemoHelper::CreateView( application, mView, mToolBar, "", TOOLBAR_IMAGE, "" );

  // Creates view change button.
  Toolkit::PushButton viewButton = Toolkit::PushButton::New();
  viewButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, VIEW_SWAP_IMAGE );
  viewButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, VIEW_SWAP_SELECTED_IMAGE );
  // Connects the view change button clicked signal to the OnView method.
  viewButton.ClickedSignal().Connect( this, &EffectsViewApp::ChangeEffectSize );
  mToolBar.AddControl( viewButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

  Vector2 effectsViewSize( mStageSize.width, mStageSize.height * 0.25f );
  mDropShadowView = CreateEffectsView( EffectsView::DROP_SHADOW, effectsViewSize, mEffectSize );
  mDropShadowView.SetParentOrigin( ParentOrigin::CENTER );
  mDropShadowView.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mDropShadowView.SetZ( -mStageSize.height * 0.1f );
  mContents.Add( mDropShadowView );

  mEmbossView = CreateEffectsView( EffectsView::EMBOSS, effectsViewSize, mEffectSize );
  mEmbossView.SetParentOrigin( ParentOrigin::CENTER );
  mEmbossView.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  mEmbossView.SetZ( mStageSize.height * 0.1f );
  mContents.Add( mEmbossView );

  SetTitle( mEffectSize );
}


EffectsView EffectsViewApp::CreateEffectsView( EffectsView::EffectType type, const Vector2& viewSize, int effectSize  )
{
  Toolkit::EffectsView effectsView = Toolkit::EffectsView::New(type);
  // set control size
   effectsView.SetSize( viewSize.width, viewSize.height );
  // set effect size property
  effectsView.SetProperty( EffectsView::Property::EFFECT_SIZE, effectSize );

  // Create some content
  // text
  std::string text = ( type == EffectsView::DROP_SHADOW) ? "Drop Shadow" : "Emboss";
  TextLabel textActor( TextLabel::New( text ) );
  textActor.SetParentOrigin( ParentOrigin::CENTER_LEFT );
  textActor.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  textActor.SetSize( viewSize );
  textActor.SetPosition( viewSize.width*0.4f, viewSize.height*0.3f );
  textActor.SetProperty(  TextLabel::Property::POINT_SIZE, DemoHelper::ScalePointSize(14.f) );
  effectsView.Add( textActor );

  // image
  ImageView icon = ImageView::New( TEST_IMAGE );
  icon.SetParentOrigin( ParentOrigin::CENTER_LEFT );
  icon.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  icon.SetX( viewSize.width*0.1f );
  icon.SetSize( viewSize.height*0.8f, viewSize.height*0.8f );
  effectsView.Add( icon );

  AnimateEffectProperties( effectsView );

  return effectsView;
}

void EffectsViewApp::AnimateEffectProperties( EffectsView& effectsView )
{
  const float animationTime( 5.0f );
  Animation animation( Animation::New(animationTime) );

  animation.AnimateTo( Property( effectsView, EffectsView::Property::EFFECT_OFFSET ), Vector3( 2.f,-2.f, 0.0f), TimePeriod(animationTime * 0.0f, animationTime * 0.2f) );
  animation.AnimateTo( Property( effectsView, EffectsView::Property::EFFECT_OFFSET ), Vector3(-2.f,-2.f, 0.0f), TimePeriod(animationTime * 0.2f, animationTime * 0.2f) );
  animation.AnimateTo( Property( effectsView, EffectsView::Property::EFFECT_OFFSET ), Vector3(-2.f, 2.f, 0.0f), TimePeriod(animationTime * 0.4f, animationTime * 0.2f) );
  animation.AnimateTo( Property( effectsView, EffectsView::Property::EFFECT_OFFSET ), Vector3( 4.f, 4.f, 0.0f), TimePeriod(animationTime * 0.6f, animationTime * 0.2f) );
  animation.AnimateTo( Property( effectsView, EffectsView::Property::EFFECT_OFFSET ), Vector3::ZERO, TimePeriod(animationTime * 0.8f, animationTime * 0.2f) );

  effectsView.SetProperty( EffectsView::Property::EFFECT_COLOR, Color::BLACK );
  animation.AnimateTo( Property( effectsView, EffectsView::Property::EFFECT_COLOR ), Color::BLUE, TimePeriod(animationTime * 0.0f, animationTime * 0.33f) );
  animation.AnimateTo( Property( effectsView, EffectsView::Property::EFFECT_COLOR ), Color::RED, TimePeriod(animationTime * 0.33f, animationTime * 0.33f) );
  animation.AnimateTo( Property( effectsView, EffectsView::Property::EFFECT_COLOR ), Color::BLACK, TimePeriod(animationTime * 0.66f, animationTime * 0.34f));

  animation.SetLooping( true );
  animation.Play();
}

void EffectsViewApp::SetTitle(int effectSize)
{
  std::ostringstream title;
  title<<TITLE<< effectSize;

  if(!mTitleActor)
  {
    mTitleActor = DemoHelper::CreateToolBarLabel( title.str() );
    // Add title to the tool bar.
    mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Toolkit::Alignment::HorizontalCenter );
  }
  mTitleActor.SetProperty( Toolkit::TextLabel::Property::TEXT, title.str() );
}

bool EffectsViewApp::ChangeEffectSize( Button button )
{
  mEffectSize = ( mEffectSize+1 )%5;
  mDropShadowView.SetProperty( EffectsView::Property::EFFECT_SIZE, mEffectSize );
  mEmbossView.SetProperty( EffectsView::Property::EFFECT_SIZE, mEffectSize );
  SetTitle( mEffectSize );

  return true;
}


void EffectsViewApp::OnKeyEvent(const KeyEvent& event)
{
  if(event.state == KeyEvent::Down)
  {
    if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
      mApplication.Quit();
    }
  }
}

/*****************************************************************************/

int DALI_EXPORT_API main(int argc, char **argv)
{
  Application application = Application::New(&argc, &argv, DEMO_THEME_PATH);
  EffectsViewApp test( application );
  application.MainLoop();
  return 0;
}
